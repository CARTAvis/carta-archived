#include "ServerConnector.h"
#include "core/MyQApp.h"

#include <QTimer>
#include <QImage>
#include <QXmlInputSource>
#include <QDebug>

#include <functional>

// define convenience conversion between CSI::String and QString
// accomplished by going to std::string as an intermediate step
//namespace CSI {
//template<>
//struct cast_helper<QString, CSI::String >
//{
//    static bool try_cast(String const& value, QString & result)
//    {
//        std::string stdstr = value.As<std::string>();
//        result = stdstr.c_str();
//        return true;
//    }
//};
//}

ServerConnector::ServerConnector()
    : QObject( nullptr)
{
    m_callbackNextId = 0;
    m_initialized = false;
}

void OnPWStateInitialized(CSI::PureWeb::Server::StateManager &, CSI::EmptyEventArgs &)
{
    qDebug() << "State manager is now initialized";
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

        // extract URL encoded arguments
        for( auto kv : m_server-> StartupParameters()) {
            QString key = kv.first.ToAscii().begin();
            QString val = kv.second.ToAscii().begin();
            m_urlParams[ key ] = val;
            //qDebug() << key << "=" << val;
        }

        // register generic command listener
        CSI::PureWeb::Server::StateManager::Instance()->CommandManager().AddUiHandler(
                "generic", CSI::Bind( this, &ServerConnector::genericCommandListener));

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

// Commenting this out because (a) it was not used, (b) it was probably not converting
// CSI::String to std::string correctly. (Pavol)
/*
QString ServerConnector::toQString( const CSI::String source) const {
    std::string treeValueStr = source.As<std::string>();
    QString treeValueQ = treeValueStr.c_str();
    return treeValueQ;
}
*/


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

const std::map<QString, QString> & ServerConnector::urlParams()
{
    Q_ASSERT( m_initialized);

    return m_urlParams;
}

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

/// internal class used by ServerConnector to bridge between PureWeb's view and Carta's
/// connector view
class PWIViewConverter : public CSI::PureWeb::Server::IRenderedView
{
public:
    PWIViewConverter( IView * iview) {
        m_iview = iview;
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

    IView * m_iview;
};

// unregister the view
void ServerConnector::unregisterView( const QString& viewName ){
    std::string vn = viewName.toStdString();
    m_stateManager->ViewManager().UnregisterView( vn );
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
    PWIViewConverter * cvt = new PWIViewConverter( view);

    m_stateManager->ViewManager().RegisterView( view->name().toStdString(), cvt);
    m_stateManager->ViewManager().SetViewImageFormat( view->name().toStdString(), viewImageFormat);

    // register the view with this connector
    view->registration( this);
}

void ServerConnector::refreshView(IView *view)
{
    m_stateManager->ViewManager().RenderViewDeferred( view->name().toStdString());
}

void ServerConnector::removeStateCallback(const IConnector::CallbackID & /*id*/)
{
    qFatal( "Not implemented");
}

