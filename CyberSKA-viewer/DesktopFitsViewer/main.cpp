#include <iostream>
#include <fstream>

#include <QApplication>
#include <QStringList>
#include <QTimer>
#include <QDebug>
#include <QTime>
#include <QHostInfo>
#include <gsl/gsl_errno.h>
#include <thread>
#include <QThreadPool>
#include <functional>

//#include "MainWindow.h"
#include "FvController.h"
#include "FitsViewerLib/common.h"
#include "PureWeb.h"
#include "FitsViewerLib/ColormapFunction.h"
#include "FitsViewerLib/common.h"
#include "StartupParameters.h"
#include "GlobalSettings.h"

//using namespace std;
//ofstream logfile;

/*

void SimpleLoggingHandler(QtMsgType type, const char *msg) {
    switch (type) {
        case QtDebugMsg:
            logfile << QTime::currentTime().toString().toAscii().data() << " Debug: " << msg << "\n";
            break;
        case QtCriticalMsg:
            logfile << QTime::currentTime().toString().toAscii().data() << " Critical: " << msg << "\n";
            break;
        case QtWarningMsg:
            logfile << QTime::currentTime().toString().toAscii().data() << " Warning: " << msg << "\n";
            break;
        case QtFatalMsg:
            logfile << QTime::currentTime().toString().toAscii().data() <<  " Fatal: " << msg << "\n";
            abort();
        }
    }
    */

//static void
//usage( int exitCode)
//{
//    std::cerr << "Usage: DesktopFitsViewer filename\n";
//    exit( exitCode);
//}

class QTMessageTickler : public QObject, public CSI::PureWeb::Server::IResponseProvider, public CSI::PureWeb::Server::IStateManagerPlugin
{
private:
    CSI::Collections::List<CSI::PureWeb::Server::ResponseInfo> m_empty;
    CSI::PureWeb::Server::StateManager * m_pStateManager;
    int m_eventCounter;
public:
    QTMessageTickler()
    {
        m_eventCounter = 0;
    }


    bool event(QEvent * event)
    {
        if (event->type() == QEvent::User)
        {
            CSI::Threading::Interlocked::Exchange(&m_eventCounter, 0);
            CSI::Threading::UiDispatcher::PumpMessages();
            return true;
        }

        return false;
    }

    void SessionConnected(CSI::Guid sessionId, CSI::Typeless const& /*command*/)
    {
        m_pStateManager->SessionManager().AddResponseProvider(sessionId, this);

    }

    void SessionDisconnected(CSI::Guid sessionId, CSI::Typeless const& /*command*/)
    {
        m_pStateManager->SessionManager().RemoveResponseProvider(sessionId, this);
    }

    CSI::Collections::List<CSI::PureWeb::Server::ResponseInfo> GetNextResponses(
            CSI::Guid /*sessionId*/)
    {
        if (CSI::Threading::Interlocked::CompareExchange(&m_eventCounter, 1, 0))
        {
            QEvent * event = new QEvent(QEvent::User);
            QApplication::postEvent(this,event,INT_MAX);

        }
        return m_empty;
    }

    void Initialize(CSI::PureWeb::Server::StateManager * pStateManager)
    {

        if (pStateManager == NULL)
            CSI_THROW(CSI::ArgumentNullException, "pStateManager");
        m_pStateManager = pStateManager;
    }

    void Uninitialize() {}
};

class MyDebugReceiver : public Debug::DebugReceiver {
public:
    MyDebugReceiver( QString fname, bool doStdErr = false)
        : m_fname(fname),
          m_doStdErr( doStdErr)

    {}
    void appendDebugString (const QString & s) {

        static QMutex mutex;
        QMutexLocker locker( & mutex);

        if( m_doStdErr) {
            std::cerr << s;
        }

        if( m_fname.isEmpty ()) return;
        QFile f( m_fname);
        if( ! f.open( QFile::Append )) return;
        f.write ( s.toAscii ());
        f.flush();
        f.close ();
    }

protected:
    QString m_fname;
    bool m_doStdErr;

};

// qt message handler (warnings, errors, etc)
static void myMessageOutput(QtMsgType type, const char *msg)
{
    switch (type) {
    case QtDebugMsg:
        dbg(1) << "Qt::Debug:" <<  msg << "\n";
        break;
    case QtWarningMsg:
        dbg(1) << ConsoleColors::error()
               << "Qt::Warning:" << msg
               << ConsoleColors::resetln();
        break;
    case QtCriticalMsg:
        dbg(1) << ConsoleColors::error()
               << "Qt::Critical:" <<  msg
               << ConsoleColors::resetln();
        break;
    case QtFatalMsg:
        dbg(1) << ConsoleColors::error()
               << "Qt::Fatal:" <<  msg
               << ConsoleColors::resetln();
        abort();
        break;
    }
}

/// global gsl error handler
static void gslErrorHandler (
        const char * reason,
        const char * file,
        int line,
        int gsl_errno)
{
    dbg(-1) << "GSL-ERROR:\n";
    dbg(-1) << "  reason: " << reason << "\n"
            << "  loc: " << file << ":" << line << "\n"
            << "  errno: " << gsl_errno << "\n"
            << "  strerr: " << gsl_strerror( gsl_errno) << "\n";
    throw gsl_exception( reason, file, line, gsl_errno);
    //    throw std::runtime_error( "GSL-error");
}

static void OnPureWebShutdown(CSI::PureWeb::Server::StateManagerServer& /*server*/,
                              CSI::EmptyEventArgs& /*args*/)
{
    //    dbg(1) << ConsoleColors::info()
    //           << "OnPurewebShutdown"
    //           << ConsoleColors::reset() + "\n";
    dbg(1) << "----======= PurewebShutdown =======--------\n";
    QApplication::exit();
}


int main(int argc, char *argv[])
{

    // initialize QT
    QApplication app(argc, argv);

    QString globalConfigFile = QDir::homePath () + "/.sfviewer.config";

    // setup debugging
    if( GlobalSettings::isDebug()) {
        Debug::verbocity = 10;
        Debug::displayLevel = true;
        Debug::displaySource = true;
        Debug::setReceiver ( new MyDebugReceiver( "/tmp/fitsviewer.dbg.txt"));
    } else {
        Debug::verbocity = 0;
        Debug::displayLevel = false;
        Debug::displaySource = false;
    }

    dbg(1) << "*******************************************************************\n";
    dbg(1) << "                    FitsViewer starting\n";
    dbg(1) << "*******************************************************************\n";
    dbg(1) << "Ideal thread count (qt): " << QThread::idealThreadCount() << "\n";
    dbg(1) << "Ideal thread count (c++11): " << std::thread::hardware_concurrency() << "\n";
    dbg(1) << "Global thread pool: " << QThreadPool::globalInstance()->maxThreadCount() << "\n";
    {
        int nt = 0;
#pragma omp parallel
        {
#pragma omp critical
            {
                nt ++;
                dbg(1) << "thread " << nt << " reporting\n";
            }
        }
        dbg(1) << "detected " << nt << "OMP threads\n";

    }

    // install QT error handler
    qInstallMsgHandler(myMessageOutput);

    // install GSL error handler
    gsl_set_error_handler( gslErrorHandler);

    try {
        // initialize PureWeb
        CSI::Library::Initialize();
        CSI::Threading::UiDispatcher::InitMessageThread();
        CSI::PureWeb::Server::StateManagerServer server;
        CSI::PureWeb::Server::StateManager stateManager("fitsviewer");
        // add a callback to listen to PureWeb shutdown event
        server.ShutdownRequested() += OnPureWebShutdown;
        // register tickler
        stateManager.PluginManager().RegisterPlugin("QTMessageTickler", new QTMessageTickler());
        // connect
        server.Start(&stateManager);

        // DEBUG
        if(1){
            ScopedDebug dbg__( "Startup paramters:", 1);
            for( auto kv : server.StartupParameters()) {
                std::string key = kv.first.ToAscii().begin();
                std::string val = kv.second.ToAscii().begin();
                dbg(1) << "key/val = " << key << " / " << val << "\n";
            }
        }

        // figure out which file to load
        FitsFileLocation flocToLoad;
        VisualizationStartupParameters vsp;
        {
            // convert PureWeb params to std::map, so that we don't propagate dependencies
            // on PureWeb all across the system
            std::map< QString, QString > myMap;
            for( auto kv : server.StartupParameters())
                myMap[ kv.first.ToAscii().begin() ] = kv.second.ToAscii().begin();
            vsp = determineStartupParameters( myMap);
        }

        // check for errors
        if( vsp.error) {
            dbg(0) << ConsoleColors::error()
                   << "Error: could not figure out startup parameters."
                   << ConsoleColors::resetln()
                   << "  -> " << vsp.errorString << "\n"
                   << "Will try to load the default file\n";
            flocToLoad = FitsFileLocation::fromLocal( GlobalSettings::defaultImage());
        } else {
            dbg(1) << "Startup parameters ===================================\n"
                   << "  title: " << vsp.title << "\n"
                   << "  path:  " << vsp.path << "\n"
                   << "  id:    " << vsp.id << "\n"
                   << "  stamp: " << vsp.stamp << "\n"
                   << "======================================================\n"
                      ;
            flocToLoad = FitsFileLocation::fromElgg( vsp.id, vsp.path, vsp.stamp);
        }

        if( vsp.title.isEmpty()) vsp.title = "N/A";

//        const Collections::Map<CSI::String, CSI::String> & pars =
//                server.StartupParameters();

//        {
//            QString fileId, stamp, path;
//            if( pars.ContainsKey( "fileid"))
//                fileId = pars["fileid"].ToAscii().begin();
//            if( pars.ContainsKey( "timestamp"))
//                stamp = pars["timestamp"].ToAscii().begin();
//            if( pars.ContainsKey( "filepath"))
//                path = pars["filepath"].ToAscii().begin();
//            //            dbg(1) << "fileId = " << fileId << "\n"
//            //                   << "stamp = " << stamp << "\n"
//            //                   << "path = " << path << "\n";
//            if( !( fileId.isEmpty() || stamp.isEmpty() || path.isEmpty())) {
//                flocToLoad = FitsFileLocation::fromElgg( fileId, path, stamp);
//                dbg(1) << "floc2load = " << flocToLoad.toStr() << "\n";
//            }
//        }

        dbg(1) << "will try to load file: " << flocToLoad.toStr() << "\n";
        dbg(1) << " ... i.e: " << flocToLoad.getLocalFname() << "\n";
        dbg(1) << "if that fails, will load: " << GlobalSettings::defaultImage() << "\n";
        dbg(1) << "if that fails too, i'll probably crash :(\n";

        // start up the controller
        FvController fvc ( app.arguments ());
        fvc.setDefaultFileToOpen( FitsFileLocation::fromLocal( GlobalSettings::defaultImage()));
        fvc.start ();
        fvc.setTitle( vsp.title);
        fvc.loadFileAuto( flocToLoad);

        // give up control to Qt event loop
        int ret = app.exec();
        s_stop.Set();
        return ret;
    }
    catch (QString s) {
        fatalError( s);
    } catch (const char * s) {
        fatalError( s);
    } catch (const std::string & s) {
        fatalError(s);
    } catch (const std::runtime_error & err) {
        fatalError ( err.what());
    } catch (const std::exception & err) {
        fatalError ( err.what());
    } catch (...) {
        fatalError( "Unknown exception occured");
    }

    // test
    std::function< void(double) > f;
    class ABC {
    public:
        void operator()(double val) { dbg(1) << "Yay " << val; }
    };
    ABC abc;
    abc(7.1);
    f = abc;
    f(1.7);
}
