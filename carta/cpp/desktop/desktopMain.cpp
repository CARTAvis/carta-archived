#include "DesktopPlatform.h"
#include "core/coreMain.h"

int
main( int argc, char * * argv )
{
    return Carta::Core::coreMain<DesktopPlatform>( "desktop", argc, argv);
}

