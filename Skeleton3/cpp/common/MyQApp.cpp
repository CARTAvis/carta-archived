/**
 *
 **/


#include "MyQApp.h"
#include <QMutexLocker>
#include <QMutex>
#include <QEvent>
#include <QThreadStorage>
#include <QDebug>

//IPlatform * MyQApp::m_platform = nullptr;

MyQApp::MyQApp(int & argc, char ** argv) :
    QApplication( argc, argv)
{
    setApplicationName( "Skeleton3 something");
    setApplicationVersion( "0.0.1");
}

typedef DeferHelper2::VoidFunc VoidFunc;

Q_DECLARE_METATYPE( VoidFunc)

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


///
/// \brief queues up the execution of a function by calling the execute slot
///        via queued connection
/// \param func
///
void DeferHelper2::queue(const DeferHelper2::VoidFunc &func)
{
    qRegisterMetaType<VoidFunc>("VoidFunc");
    QMetaObject::invokeMethod( this, "execute", Qt::QueuedConnection, Q_ARG( VoidFunc, func));
}

///
/// \brief executes the function \param func immediately
/// \param func function to execute
///
void DeferHelper2::execute(const DeferHelper2::VoidFunc &func)
{
    func();
}


