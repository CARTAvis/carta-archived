/*
 * This is the client-server main
 */

#include <QApplication>
#include <iostream>

#include "ServerPlatform.h"
#include "common/Viewer.h"

int main(int argc, char ** argv)
{
    // setup Qt
    QApplication app(argc, argv);

    std::cerr << "Command line args:\n";
    for( int i = 0 ; i < argc ; i ++ ) {
        std::cerr << "   " << i << ".) " << argv[i] << "\n";
    }

    // create a platform
    IPlatform * platform = new ServerPlatform( argc, argv);

    // run the viewer with this platorm
    Viewer viewer( platform);
    viewer.start();

    return app.exec();
}



