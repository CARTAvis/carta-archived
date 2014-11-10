/**
 *
 **/

#include "GrayColormap.h"

namespace Carta
{
namespace Core
{
GrayColormap::GrayColormap() { }

QString
GrayColormap::name()
{
    return "Gray";
}

QRgb
GrayColormap::convert( const double & val )
{
    int gray = 255 * val + 0.5; // round
    return qRgb( gray, gray, gray );
}
}
}
