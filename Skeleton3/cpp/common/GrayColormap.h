/**
 * Implementation of gray colormap. This is the only built-in colormap in carta core.
 **/

#include "CartaLib/IColormapScalar.h"

#pragma once

namespace Carta
{
namespace Core
{
/// Gray colormap (default if no other colormaps are present)
class GrayColormap : public Lib::IColormapScalar
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
