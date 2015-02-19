/**
 *
 **/

#include "CartaLib.h"

QString
Carta::Lib::double2base64( double x )
{
    return QByteArray( (char *) ( & x ), sizeof( x ) ).toBase64();
}
