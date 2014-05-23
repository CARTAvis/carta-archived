/*
 * This is the client-server main
 */

#include <iostream>

#include "common/MyQApp.h"
#include "ServerPlatform.h"
#include "common/Viewer.h"
#include "common/Globals.h"

int main(int argc, char ** argv)
{
    // setup Qt
    MyQApp app(argc, argv);

    std::cerr << "defer: deferring\n";
    defer( []() {
        std::cerr << "defer: running, yay\n";

        std::cerr << "+defer: deferring\n";
        defer( []() {
            std::cerr << "+defer: running, yay\n";

        });
        std::cerr << "+defer: deferred\n";


    });
    std::cerr << "defer: deferred\n";

    // run the viewer with the proper platorm
    Viewer viewer( new ServerPlatform( argc, argv));
    viewer.start();

    return app.exec();
}



