/*
 * This is the client-server main
 */

#include "ServerPlatform.h"
#include "core/Viewer.h"
#include "ServerPlatform.h"
#include "core/coreMain.h"

int
main( int argc, char * * argv )
{
    return Carta::Core::coreMain<ServerPlatform>( "server", argc, argv);
}

