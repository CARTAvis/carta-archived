/*
 * This is the desktop main
 */

#include "DesktopPlatform.h"
#include "common/Viewer.h"
#include "common/MyQApp.h"
#include "common/CmdLine.h"
#include "common/GlobalSettings.h"
#include <QDebug>
#include <QUrl>

int main(int argc, char ** argv)
{
    qDebug() << "Desktop app is starting up.";
#ifdef QT_DEBUG
    qDebug() << "Running a debug build";
#else
    qDebug() << "Running a release build";
#endif

    // initialize Qt
    MyQApp qapp( argc, argv);

    // parse command line arguments & environment variables
    CmdLine::ParsedInfo cmdLineInfo = CmdLine::parse();

    // load the config file
    GlobalSettings gs();

    // initialize platform
    auto platform = new DesktopPlatform( cmdLineInfo);

    // create viewer with this platform
    Viewer viewer( platform);

    // run the viewer
    viewer.start();
    return qapp.exec();
}

// TODO: MyQApp, command line parsing, config file parsing and Platform are all
// boiler plate code. All these should be probably into single code, maybe even
// into Platform. The problem with this right now is that DesktopPlatform is
// a QObject, and Qt for some reason chokes on instantiating QObject derived
// classes before QApplication is initialized... The reason DesktopPlatform is
// a QObject derived class is that we use it in webkit to give javascript direct
// access to platform commands, which at the moment is only the fullscreen api...
