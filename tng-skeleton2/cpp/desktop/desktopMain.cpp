/*
 * This is the desktop main
 */

#include <QApplication>
#include <iostream>

#include "DesktopPlatform.h"
#include "common/Viewer.h"

int main(int argc, char ** argv)
{
    // setup Qt
    // TODO: does this belong inside platform? probably yes
    QApplication app(argc, argv);

    std::cerr << "Command line args:\n";
    for( int i = 0 ; i < argc ; i ++ ) {
        std::cerr << "   " << i << ".) " << argv[i] << "\n";
    }

    // create a platform
    IPlatform * platform = new DesktopPlatform( argc, argv);

    // run the viewer with this platorm
    Viewer viewer( platform);
    viewer.start();

    return app.exec();
}
