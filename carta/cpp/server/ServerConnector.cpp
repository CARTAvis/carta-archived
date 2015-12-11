#include "ServerConnector.h"
#include "core/MyQApp.h"
#include "core/Globals.h"
#include "CartaLib/Hooks/GetInitialFileList.h"
#include "core/SimpleRemoteVGView.h"

#include <QTimer>
#include <QImage>
#include <QXmlInputSource>
#include <QDebug>
#include <functional>



static
void OnPWStateInitialized(CSI::PureWeb::Server::StateManager &, CSI::EmptyEventArgs &)
{
    qDebug() << "PureWeb StateManager is now initialized";
}


/// internal class used by ServerConnector to bridge between PureWeb's view and Carta's
/// connector view
class PWIViewConverter : public CSI::PureWeb::Server::IRenderedView
{
public:
    PWIViewConverter( IView * iview,  CSI::CountedPtr<CSI::PureWeb::Server::StateManager> sm) {
        m_iview = iview;
        m_sm = sm;
    }

    virtual void SetClientSize(CSI::PureWeb::Size clientSize) Q_DECL_OVERRIDE
    {
        m_iview->handleResizeRequest( QSize( clientSize.Width, clientSize.Height));
    }
    virtual CSI::PureWeb::Size GetActualSize() Q_DECL_OVERRIDE
    {
        auto qtsize = m_iview->size();
        return CSI::PureWeb::Size( qtsize.width(), qtsize.height());
    }
    virtual void RenderView(CSI::PureWeb::Server::RenderTarget target) Q_DECL_OVERRIDE
    {
        CSI::ByteArray bits = target.RenderTargetImage().ImageBytes();

        const QImage & qimage = m_iview->getBuffer();
        if( qimage.format() != QImage::Format_ARGB32_Premultiplied) {
            // @todo could we do SSSE3 byte shuffle here as we are copying?
            // e.g. __m128i _mm_shuffle_epi8
            QImage tmpImage = qimage.convertToFormat( QImage::Format_ARGB32_Premultiplied);
            CSI::ByteArray::Copy(tmpImage.scanLine(0), bits, 0, bits.Count());
        }
        else {
            CSI::ByteArray::Copy(qimage.scanLine(0), bits, 0, bits.Count());
        }

        // tell the clients the ID of this refresh
        auto map = target.Parameters();
//        qint64 id = refreshId();
        map["refreshId"] = QString::number( m_refreshId).toStdString().c_str();
    }
    virtual void PostKeyEvent(const CSI::PureWeb::Ui::PureWebKeyboardEventArgs & /*keyEvent*/) Q_DECL_OVERRIDE
    {}
    virtual void PostMouseEvent(const CSI::PureWeb::Ui::PureWebMouseEventArgs & mouseEvent) Q_DECL_OVERRIDE
    {
        QEvent::Type action = QEvent::None;
        Qt::MouseButton button;
        Qt::MouseButtons buttons;
        Qt::KeyboardModifiers keys = 0;

        switch(mouseEvent.EventType)
        {
        case CSI::PureWeb::Ui::MouseEventType::MouseDown:
            action = QEvent::MouseButtonPress;
            break;
        case CSI::PureWeb::Ui::MouseEventType::MouseUp:
            action = QEvent::MouseButtonRelease;
            break;
        case CSI::PureWeb::Ui::MouseEventType::MouseMove:
            action = QEvent::MouseMove;
            break;
        default:
            return;
            break;
        }

        switch (mouseEvent.ChangedButton)
        {
        case CSI::PureWeb::Ui::MouseButtons::Left:
            button = Qt::LeftButton;
            break;
        case CSI::PureWeb::Ui::MouseButtons::Right:
            button = Qt::RightButton;
            break;
        default:
            button = Qt::NoButton;
            break;
        }

        if (0 != (mouseEvent.Modifiers & CSI::PureWeb::Ui::Modifiers::Shift)) {keys |= Qt::ShiftModifier; }
        if (0 != (mouseEvent.Modifiers & CSI::PureWeb::Ui::Modifiers::Control)) {keys |= Qt::ControlModifier; }
        if (0 != (mouseEvent.Buttons & CSI::PureWeb::Ui::MouseButtons::Left)) {buttons |= Qt::LeftButton; }
        if (0 != (mouseEvent.Buttons & CSI::PureWeb::Ui::MouseButtons::Right)) {buttons |= Qt::RightButton; }
        if (0 != (mouseEvent.Buttons & CSI::PureWeb::Ui::MouseButtons::Middle)) {buttons |= Qt::MidButton; }
        if (0 != (mouseEvent.Buttons & CSI::PureWeb::Ui::MouseButtons::XButton1)) {buttons |= Qt::XButton1; }
        if (0 != (mouseEvent.Buttons & CSI::PureWeb::Ui::MouseButtons::XButton2)) {buttons |= Qt::XButton2; }

        QMouseEvent * m = new QMouseEvent(action, QPoint(mouseEvent.X,mouseEvent.Y), button, buttons, keys);
        m_iview->handleMouseEvent( *m );
        delete m;
    } // PostMouseEvent

    /// schedule refresh and return ID of this refresh
    qint64 refresh() {
        m_refreshId ++;
        m_sm->ViewManager().RenderViewDeferred( m_iview->name().toStdString());
        return m_refreshId;
    }

    void viewRefreshed( qint64 id) {
        CARTA_ASSERT( m_iview);
        m_iview-> viewRefreshed( id);
    }

    IView * m_iview = nullptr;
    qint64 m_refreshId = -1;
    CSI::CountedPtr<CSI::PureWeb::Server::StateManager> m_sm;
};

ServerConnector::ServerConnector()
    : QObject( nullptr)
{
    m_callbackNextId = 0;
    m_initialized = false;
}

void ServerConnector::initialize(const InitializeCallback & cb)
{
    try {
        // start with unintialized state
        Q_ASSERT_X( ! m_initialized, "ServerConnector::initialize()", "Calling initialize twice?");

        // Initialize PureWeb libraries
        CSI::Library::Initialize();

        // this thread is the UI thread
        CSI::Threading::UiDispatcher::InitMessageThread();

        // Create PureWeb object instances
        m_server = new CSI::PureWeb::Server::StateManagerServer();
        m_stateManager = new CSI::PureWeb::Server::StateManager("pingpong");
        m_stateManager-> Initialized() += OnPWStateInitialized;

        m_stateManager->PluginManager().RegisterPlugin(
                    "QtMessageTickler", new QtMessageTickler());
        m_server->Start(m_stateManager.get());
        m_server->ShutdownRequested() += OnPureWebShutdown;

        // register generic command listener
        CSI::PureWeb::Server::StateManager::Instance()->CommandManager().AddUiHandler(
                "generic", CSI::Bind( this, &ServerConnector::genericCommandListener));

        // register view refresh command listener
        CSI::PureWeb::Server::StateManager::Instance()->CommandManager().AddUiHandler(
                "viewrefreshed", CSI::Bind( this, &ServerConnector::viewRefreshedCommandCB));

        // extract URL encoded arguments
        for( auto kv : m_server-> StartupParameters()) {
            QString key = kv.first.ToAscii().begin();
            QString val = kv.second.ToAscii().begin();
            m_urlParams[ key ] = val;
        }

        // make a list of initial files from this
        auto & pm = * Globals::instance()->pluginManager();
        auto list = pm.prepare < Carta::Lib::Hooks::GetInitialFileList > ( m_urlParams).first();
        if( list.isSet()) {
            m_initialFileList = list.val();
        }

        m_initialized = true;
    }
    catch ( ... ) {
        qCritical() << "Could not initialize PureWeb";
    }

    // schedule the callback immediately, as we already know if we succeeded or not
    defer( std::bind( cb, m_initialized));
} // initialize

void
ServerConnector::setState(const QString & path, const QString & value)
{
    Q_ASSERT( m_initialized);
    std::string pwpath = path.toStdString();
    std::string pwval = value.toStdString();
    m_stateManager-> XmlStateManager().SetValue( pwpath, pwval);
}

// 1.) why is this not a static function?
// 2.) why is it part of a connector at all?
QString ServerConnector::getStateLocation( const QString & saveName ) const {
	//TODO: generalize this.
	return "/tmp/"+saveName + ".json";
}

QString ServerConnector::getState(const QString& path)
{
    Q_ASSERT( m_initialized);
    std::string pwpath = path.toStdString();
    auto pwval = m_stateManager->XmlStateManager().GetValue( pwpath);
    if( !pwval.HasValue()) {
        return QString();
    }
    // warning: this is NOT how to convert CSI::string to std::string as it will use
    // the stream operator, which will treat spaces as separators... ie. you won't
    // be able to receive values with spaces in them ;(
    //    std::string val = pwval.ValueOr( "").As<std::string>();

    // this is how to convert to std::string:
    QString val( pwval.ValueOr( "").ToAscii().begin());
    return val;
}

IConnector::CallbackID ServerConnector::addCommandCallback(const QString &cmd, const IConnector::CommandCallback &cb)
{
    m_commandCallbackMap[cmd].push_back( cb);
    return m_callbackNextId ++;
}

void ServerConnector::OnPureWebShutdown(CSI::PureWeb::Server::StateManagerServer &, CSI::EmptyEventArgs &)
{
    qDebug() << "PureWeb is shutting down...";
    QApplication::exit();
}

void ServerConnector::genericCommandListener(CSI::Guid sessionid, const CSI::Typeless &command, CSI::Typeless &responses)
{
    QString cmd = command["cmd"].ValueOr("").ToAscii().begin();
    QString params = command["params"].ValueOr("").ToAscii().begin();
    QString sid = sessionid.ToString().ToAscii().begin();

    auto & allCallbacks = m_commandCallbackMap[ cmd];
    QStringList results;
    for( auto & cb : allCallbacks) {
        results += cb( cmd, params, sid);
    }

    // \todo do we want to allow more than one command listener? If we do, do we want to
    // concatenate the results like this? Or do we want to pass to the callbacks the current
    // value of the result and let it decide whether to append to it or overwrite it, or
    // whatever else it might want to do with it? In that case do we care about the order
    // of the callbacks, i.e. would we want to be able to control it with some priority system?
    responses["result"] = results.join("|").toStdString();
}

void
ServerConnector::viewRefreshedCommandCB(
        CSI::Guid sessionid,
        const CSI::Typeless & command,
        CSI::Typeless & /*responses*/)
{

    QString viewName = command["viewName"].ValueOr("").ToAscii().begin();
    QString sid = sessionid.ToString().ToAscii().begin();
    QString idStr = command["id"].ValueOr("").ToAscii().begin();
    bool ok;
    qint64 id = idStr.toInt( & ok);
    if( ! ok) {
        qCritical() << "Invalid refresh ID" << idStr << "for view" << viewName;
    }

    qDebug() << "viewRefreshedCommandCB" << sid << viewName << id;

    auto pwview = m_pwviews.find( viewName);
    if( pwview == m_pwviews.end()) {
        qCritical() << "Got refresh for view that does not exist" << viewName;
        return;
    }
    CARTA_ASSERT( pwview-> second);
    pwview-> second-> viewRefreshed( id);

}

IConnector::CallbackID ServerConnector::addStateCallback(IConnector::CSR path, const IConnector::StateChangedCallback &cb)
{
    // do we have a pureweb callback with this path registered already?
    // if not, register one
    if( m_pwStateCBset.count( path.toStdString()) == 0) {
        m_stateManager->XmlStateManager().AddValueChangedHandler(
                path.toStdString(),
                CSI::Bind(this, &ServerConnector::pureWebValueChangedCB));
        m_pwStateCBset.insert( path.toStdString());
    }

    // add our own callback to the list
    m_stateCallbackList[path].push_back( cb);

    // return an incremented callback id
    return m_callbackNextId++;
}

//const std::map<QString, QString> & ServerConnector::urlParams()
//{
//    Q_ASSERT( m_initialized);

//    return m_urlParams;
//}

void ServerConnector::pureWebValueChangedCB(const CSI::ValueChangedEventArgs &val)
{

    // here we call the actual callbacks
    QString path = val.Path().ToAscii().begin();
    QString newVal;
    if( val.NewValue().HasValue()) {
        newVal = val.NewValue().Value().ToAscii().begin();
    }

    // Note: DON'T DO THIS:
    //    QString path = val.Path().As< QString >();
    // As it would use ostream operator << overloading, breaking at spaces...

    // Pureweb could be calling this method directly from inside SetValue()...
    // which is not what we want. So instead we do the real work using defer()
    defer( [ path, newVal, this ] () {
        auto & callbacks = m_stateCallbackList[ path];
        for( auto & cb : callbacks) {
            cb( path, newVal);
        }
    });
}

// unregister the view
void ServerConnector::unregisterView( const QString & viewName )
{
    m_stateManager->ViewManager().UnregisterView( viewName.toStdString() );
    auto pwview = m_pwviews.find( viewName);
    if( pwview != m_pwviews.end()) {
        if( pwview-> second) {
            delete pwview-> second;
        }
        m_pwviews.erase( pwview);
    }
}

// registerView
void ServerConnector::registerView(IView *view)
{
    CSI::PureWeb::Server::ViewImageFormat viewImageFormat;
    viewImageFormat.PixelFormat = CSI::PureWeb::PixelFormat::Bgrx32;
    viewImageFormat.ScanLineOrder = CSI::PureWeb::ScanLineOrder::TopDown;
    viewImageFormat.Alignment = 4;

    std::string vn = view->name().toStdString();
    /// \bug resource leak
    /// \todo this should be cleaned up when we (a) destory connector (b) unregister view
    /// \note these should now be resolved
    PWIViewConverter * cvt = new PWIViewConverter( view, m_stateManager);
    // store this in our map so we can look it up later
    m_pwviews[ view-> name()] = cvt;

    m_stateManager->ViewManager().RegisterView( view->name().toStdString(), cvt);
    m_stateManager->ViewManager().SetViewImageFormat( view->name().toStdString(), viewImageFormat);

    // register the view with this connector
    view->registration( this);
}

qint64 ServerConnector::refreshView(IView *view)
{
    auto pwview = m_pwviews.find( view-> name());
    if( pwview == m_pwviews.end()) {
        qCritical() << "ServerConnector::refreshView::could not find this view";
        m_stateManager->ViewManager().RenderViewDeferred( view->name().toStdString());
        return -1;
    }
    auto id = pwview-> second-> refresh();
    return id;
}

void ServerConnector::removeStateCallback(const IConnector::CallbackID & /*id*/)
{
    qFatal( "Not implemented");
}

const QStringList & ServerConnector::initialFileList()
{
    return m_initialFileList;
}

Carta::Lib::IRemoteVGView * ServerConnector::makeRemoteVGView(QString viewName)
{
    return new Carta::Core::SimpleRemoteVGView( this, viewName, this);
}

ServerConnector::~ServerConnector()
{
    for( auto & pwview : m_pwviews) {
        if( pwview.second) {
            delete pwview.second;
        }
    }
}

