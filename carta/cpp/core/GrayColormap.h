/**
 * Implementation of gray colormap. This is the only built-in colormap in carta core.
 **/

#include "CartaLib/PixelPipeline/IPixelPipeline.h"

#pragma once

namespace Carta
{
namespace Core
{
/// Gray colormap (default if no other colormaps are present)
class GrayColormap : public Lib::PixelPipeline::IColormapNamed
{
    CLASS_BOILERPLATE( GrayColormap );

public:

    GrayColormap();

    virtual QString
    name() override;

    virtual void
    convert( norm_double val, NormRgb & result ) override;
};
}
}
