#include "ServerConnector.h"
#include "common/misc.h"
#include "common/MyQApp.h"

#include <QTimer>
#include <QImage>
#include <QDebug>

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

bool ServerConnector::initialize()
{
    try {
        // Initialize PureWeb libraries
        CSI::Library::Initialize();
        // this thread is the UI thread
        CSI::Threading::UiDispatcher::InitMessageThread();

        // Create PureWeb object instances
        m_server = new CSI::PureWeb::Server::StateManagerServer();
        m_stateManager = new CSI::PureWeb::Server::StateManager("pingpong");

        m_stateManager->PluginManager().RegisterPlugin(
                    "QtMessageTickler", new QtMessageTickler());
        m_server->Start(m_stateManager.get());
        m_server->ShutdownRequested() += OnPureWebShutdown;

        // extract URL encoded arguments
        qDebug() << "PureWeb startup parameters3:";
        for( auto kv : m_server-> StartupParameters()) {
            QString key = kv.first.ToAscii().begin();
            QString val = kv.second.ToAscii().begin();
            m_urlParams[ key ] = val;
            qDebug() << key << "=" << val;
        }

        // register generic command listener
        CSI::PureWeb::Server::StateManager::Instance()->CommandManager().AddUiHandler(
                "generic", CSI::Bind( this, &ServerConnector::genericCommandListener));

    }
    catch ( ... ) {
        qCritical() << "Could not initialize PureWeb";
        return false;
    }

    qDebug() << "Command line args2: " << MyQApp::arguments();

    m_initialized = true;
    return true;
} // initialize

void ServerConnector::setState(const QString &path, const QString &value)
{
    Q_ASSERT( m_initialized);

    std::string pwpath = path.toStdString();
    std::string pwval = value.toStdString();
    m_stateManager->XmlStateManager().SetValue( pwpath, pwval);
}

QString ServerConnector::getState(const QString &path)
{
    Q_ASSERT( m_initialized);

    std::string pwpath = path.toStdString();
    auto pwval = m_stateManager->XmlStateManager().GetValue( pwpath);
    if( !pwval.HasValue()) {
        return QString();
    }
    std::string val = pwval.ValueOr( "").As<std::string>();
    return val.c_str();
}

IConnector::CallbackID ServerConnector::addCommandCallback(const QString &cmd, const IConnector::CommandCallback &cb)
{
    m_commandCallbackMap[cmd].push_back( cb);
    return m_callbackNextId++;
}

void ServerConnector::OnPureWebShutdown(CSI::PureWeb::Server::StateManagerServer &, CSI::EmptyEventArgs &)
{
    std::cerr << "OnPureWebShutdown() called\n";
    QApplication::exit();
}

void ServerConnector::genericCommandListener(CSI::Guid sessionid, const CSI::Typeless &command, CSI::Typeless &responses)
{
    std::string cmd = command["cmd"].As<std::string>();
    std::string params = command["params"].As<std::string>();

    std::cerr << "Generic command: " << cmd << " " << params << "\n";

    auto & allCallbacks = m_commandCallbackMap[ cmd.c_str()];
    QStringList results;
    for( auto & cb : allCallbacks) {
        QString p1 = cmd.c_str();
        QString p2 = params.c_str();
        QString p3 = sessionid.ToString().As<std::string>().c_str();
        results += cb( p1, p2, p3);
    }
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

// this is getting called directly by PureWeb, which means it could potentially
// be called inside SetValue()... which is not what we want. So instead we do
// the real work inside the delayed version (below), which we invoke by sending
// ourselves a signal connected via queued connection...
void ServerConnector::pureWebValueChangedCB(const CSI::ValueChangedEventArgs &val)
{

    // here we call the actual callbacks
    QString path = val.Path().As< QString >();
    QString newVal = val.NewValue().As< QString >();

    qDebug() << "internalValueChangedCB\n"
             << "  path = " << path << "\n"
             << "  newValue = " << newVal << "\n";

    defer( [ path, newVal, this ] () {
        auto & callbacks = m_stateCallbackList[ path];
        for( auto & cb : callbacks) {
            cb( path, newVal);
        }

    });


    // invoke the delayed version asap
//    emit delayedInternalValueChangedSignal( val);
}

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
        if( qimage.format() != QImage::Format_RGB888) {
            QImage tmpImage = qimage.convertToFormat( QImage::Format_RGB888);
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

// registerView
void ServerConnector::registerView(IView *view)
{
    CSI::PureWeb::Server::ViewImageFormat viewImageFormat;
    viewImageFormat.PixelFormat = CSI::PureWeb::PixelFormat::Rgb24;
    viewImageFormat.ScanLineOrder = CSI::PureWeb::ScanLineOrder::TopDown;
    viewImageFormat.Alignment = 4;

    std::string vn = view->name().toStdString();
    std::cerr << "vn = " << vn << "\n";
    std::cerr << "view =" << reinterpret_cast<uint64_t>(view) << "\n";
    std::cerr << "registering view '" << view->name() << "'\n";
    // TODO: resource leak (only if we destroy a connector...)
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

// this is dealyed pureweb callback, here we do the actual work of calling
// registered handlers
//void ServerConnector::delayedInternalValueChangedCB( CSI::ValueChangedEventArgs val)
//{
//    std::cerr << "delayedInternalValueChangedCB\n"
//              << "  path = " << val.Path().As<std::string>() << "\n"
//              << "  newValue = " << val.NewValue() << "\n";

//    // here we call the actual callbacks
//    QString path = val.Path().As< QString >();
//    QString newVal = val.NewValue().As< QString >();

//    auto & callbacks = m_stateCallbackList[ path];
//    for( auto & cb : callbacks) {
//        cb( path, newVal);
//    }

//}

// required for queued connections
//Q_DECLARE_METATYPE( CSI::ValueChangedEventArgs )
