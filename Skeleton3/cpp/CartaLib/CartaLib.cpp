/**
 *
 **/

#include "CartaLib.h"

QString
Carta::Lib::double2base64( double x )
{
    return QByteArray( (char *) ( & x ), sizeof( x ) ).toBase64();
}

int
Carta::Lib::knownSkyCS2int( Carta::Lib::KnownSkyCS cs )
{
    return static_cast < int > ( cs );
}

Carta::Lib::KnownSkyCS
Carta::Lib::int2knownSkyCS( int cs )
{
    if ( cs < 0 || cs >= static_cast < int > ( KnownSkyCS::Error ) ) {
        return KnownSkyCS::Error;
    }
    else {
        return static_cast < KnownSkyCS > ( cs );
    }
}
