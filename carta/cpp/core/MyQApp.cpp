/**
 *
 **/

#include "MyQApp.h"
#include <QMutexLocker>
#include <QMutex>
#include <QEvent>
#include <QThreadStorage>
#include <QDebug>

MyQApp::MyQApp( int & argc, char * * argv ) :
    QApplication( argc, argv )
{
    setApplicationName( "carta" );
    setApplicationVersion( "0.9" );
}

#if CARTA_RUNTIME_CHECKS > 0

bool
MyQApp::notify( QObject * obj, QEvent * ev )
{
    QStringList err;
    try {
        return QApplication::notify( obj, ev );
    }
    catch ( std::exception & e ) {
        err << QString( "Exception (std::exception): %1" ).arg( e.what() );
    }
    catch ( const char * str ) {
        err << QString( "Exception (char *): %1" ).arg( str );
    }
    catch ( const std::string & str ) {
        err << QString( "Exception (std::string): %1" ).arg( str.c_str() );
    }
    catch ( const QString & str ) {
        err << QString( "Exception (QString): %1" ).arg( str );
    }
    catch ( ... ) {
        err << QString( "Exception (...)" );
    }

    err << "...caught in MyQapp::nofify()";
    qFatal( "%s", err.join( '\n' ).toStdString().c_str() );

    return false;
} // notify

#endif // if CARTA_RUNTIME_CHECKS > 0

typedef DeferHelper::VoidFunc VoidFunc;

// we need this for signal/slot to send/receive VoidFunc arguments
Q_DECLARE_METATYPE( VoidFunc )

///
/// \brief contains one helper per thread
///
static QThreadStorage < DeferHelper * > deferHelpers;

void
defer( const VoidFunc & function )
{
    // create a helper in this thread if not yet created
    if ( ! deferHelpers.hasLocalData() ) {
        deferHelpers.setLocalData( new DeferHelper );
    }

    // queue up the function execution in this same thread
    // (because the object was created in this same thread)
    deferHelpers.localData()-> queue( function );
}

///
/// \brief queues up the execution of a function by calling the execute slot
///        via queued connection
/// \param func
///
void
DeferHelper::queue( const DeferHelper::VoidFunc & func )
{
    qRegisterMetaType < VoidFunc > ( "VoidFunc" );
    QMetaObject::invokeMethod( this, "execute", Qt::QueuedConnection, Q_ARG( VoidFunc, func ) );
}

///
/// \brief executes the function \param func immediately
/// \param func function to execute
///
void
DeferHelper::execute( const DeferHelper::VoidFunc & func )
{
    func();
}
