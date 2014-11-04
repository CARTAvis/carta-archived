/**
 *
 **/

#pragma once

#include "CartaLib.h"
#include <QColor>
#include <memory>

namespace Carta
{
namespace Lib
{
/**
 * @brief The IColormapScalar class
 */
class IColormapScalar
{
    CLASS_BOILERPLATE( IColormapScalar );

public:
    virtual QRgb
    convert( const double & val ) = 0;
};

/**
 * @brief The IColormapScalarOptimized class
 */
class IColormapScalarOptimized
{
    CLASS_BOILERPLATE( IColormapScalarOptimized );

public:
    virtual void
    convertArray( const double * val, QRgb * output, u_int64_t count ) = 0;
};
}
}
