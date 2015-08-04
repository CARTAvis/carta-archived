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

void
GrayColormap::convert( Lib::PixelPipeline::IColormapNamed::norm_double val,
                       Lib::PixelPipeline::IColormapNamed::NormRgb & result )
{
    result.fill( val );
}
}
}
