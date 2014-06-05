/**
 *
 **/


#include "MyQApp.h"
#include <QMutexLocker>
#include <QMutex>
#include <QEvent>
#include <QThreadStorage>
#include <QDebug>

IPlatform * MyQApp::m_platform = nullptr;

MyQApp::MyQApp(int & argc, char ** argv) :
    QApplication( argc, argv)
{
}

void MyQApp::setPlatform(IPlatform *platform)
{
    if( m_platform) {
        qCritical() << "You are redefining platform...";
    }
    m_platform = platform;
}

IPlatform *MyQApp::platform()
{
    if( ! m_platform) {
        qCritical() << "You are trying to use NULL platform";
    }
    return m_platform;
}

//bool MyQApp::notify(QObject *obj, QEvent *event)
//{
//    return QApplication::notify( obj, event);
//}


typedef DeferHelper2::VoidFunc VoidFunc;

Q_DECLARE_METATYPE( VoidFunc)


//class DeferredEvent : public QEvent
//{
//public:
//    DeferredEvent( Type type, const VoidFunc & func)
//        : QEvent( type)
//        , m_func( func)
//    {
//    }
//    const VoidFunc & func() { return m_func; }
//protected:
//    VoidFunc m_func;
//};

//void defer(const VoidFunc & function)
//{
//    static bool registered = false;
//    static int eventType = 0;

//    // make sure to register event, but only once
//    static QMutex onlyMe;
//    {
//        QMutexLocker locker(& onlyMe);

//        if( ! registered) {
//            registered = true;
//            eventType = QEvent::registerEventType();
//        }
//    }

//    // now we send ourselves an event with the function to execute
//    auto event = new DeferredEvent( static_cast<QEvent::Type>(eventType), function);

//    MyQApp::postEvent( MyQApp::instance(), event);

//}

//class DeferHelper : public QObject
//{
//    Q_OBJECT
//public:
//    DeferHelper( const VoidFunc & function )
//        : QObject()
//        , m_func( function)
//    {
//        connect( this, & DeferHelper::go, this, & DeferHelper::going, Qt::QueuedConnection);
//    }
//    virtual ~DeferHelper() {}

//signals:
//    void go();
//public slots:
//    void going();


//    const VoidFunc & m_func;

//};


///
/// \brief deferHelpers contains a list of helpers, one for each thread
///
static QThreadStorage< DeferHelper2 *> deferHelpers;

void defer(const VoidFunc & function)
{
    // create a helper in this thread if not yet created
    if( ! deferHelpers.hasLocalData()) {
        deferHelpers.setLocalData( new DeferHelper2);
    }

    // queue up the function execution in this same thread
    // (because the object was created in this same thread)
    deferHelpers.localData()-> queue( function);
}


//DeferHelper2::DeferHelper2()
//    : QObject()
//{
////    connect( this, & DeferHelper2::go, this, & DeferHelper2::going, Qt::QueuedConnection);
//}

//DeferHelper2::~DeferHelper2() {}


///
/// \brief queues up the execution of a function by calling the execute slot
///        via queued connection
/// \param func
///
void DeferHelper2::queue(const DeferHelper2::VoidFunc &func) {

    qRegisterMetaType<VoidFunc>("VoidFunc");

    QMetaObject::invokeMethod( this, "execute", Qt::QueuedConnection, Q_ARG( VoidFunc, func));
//    emit go( func);
}

///
/// \brief executes the function \param func immediately
/// \param func function to execute
///
void DeferHelper2::execute(const DeferHelper2::VoidFunc &func)
{
    func();

}


