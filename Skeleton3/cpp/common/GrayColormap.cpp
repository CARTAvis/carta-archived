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

void GrayColormap::convert(Lib::IColormapScalar::norm_double val, Lib::IColormapScalar::NormRgb & result)
{
    result.fill( val);
}

}
}
