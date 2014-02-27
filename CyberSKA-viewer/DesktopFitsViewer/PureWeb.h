#ifndef PUREWEB_H
#define PUREWEB_H

#undef min
#undef max

#include <map>
#include <qglobal.h>
#include <QMutexLocker>

// disable some warnings when compiling PureWeb header files...
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wreorder"
#pragma GCC diagnostic ignored "-Wmissing-braces"

#include "CSI/Standard/CsiStandard.h"
#include "CSI/Standard/CsiThreading.h"
#include "CSI/PureWeb/StateManager/StateManager.h"

#pragma GCC diagnostic pop

#include "../FitsViewerLib/common.h"

#ifdef new
#undef new
#endif

#include <QApplication>

static CSI::Threading::Thread s_ioThread;
static CSI::PureWeb::Server::StateManager& GetStateManager()
{
    return *CSI::PureWeb::Server::StateManager::Instance();
}

static CSI::Threading::Signal s_stop;
//static void OnPureWebShutdown(CSI::PureWeb::Server::StateManagerServer& /*server*/,
//                              CSI::EmptyEventArgs& /*args*/)
//{
////    dbg(1) << ConsoleColors::info()
////           << "OnPurewebShutdown"
////           << ConsoleColors::reset() + "\n";
//    dbg(1) << "----======= PurewebShutdown =======--------\n";
//    QApplication::exit();
//}

// convenience function for setting a state
template < typename T >
static void pwset( const QString & key, const T & val)
{
    GetStateManager().XmlStateManager().SetValueAs<T>( key.toStdString(), val);
}

// convenience function for setting a state + appending some cache destroyer number
template < typename T >
static void pwsetdc( const QString & key, const T & val)
{
    /*
    static std::map< std::string, quint32 > dcmap;
    static QMutex mutex;

    QMutexLocker mutexLocker( & mutex);

    std::string key = qkey.toStdString();
    dcmap[key] ++;
    std::stringstream out;
    out << val << " " << dcmap[key];
    pwset( key.c_str(), out.str());
    */

    static quint64 dcCounter = 0;
    quint64 dc;

    // thread-safe 'dc = dcCounter ++'
    // the extra scope is to minimize critical section
    {
        static QMutex mutex;
        QMutexLocker mutexLocker( & mutex);
        dc = dcCounter;
        dcCounter ++;
    }

    std::stringstream out;
    out << val << " " << dc;
    pwset( key, out.str());
}

void pwdeltree( const QString & path);




#endif // PUREWEB_H
