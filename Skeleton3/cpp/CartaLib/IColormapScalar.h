/**
 *
 **/

#pragma once

#include "CartaLib.h"
#include <QColor>
#include <QString>
#include <memory>

namespace Carta
{
namespace Lib
{
/**
 * @brief The IColormapScalar class
 *
 * This is an interface that plugins have to implement to add a new colormap for
 * scalcar-type pixels.
 */
class IColormapScalar
{
    CLASS_BOILERPLATE( IColormapScalar );

public:

    virtual QString
    name() = 0;

    virtual QRgb
    convert( const double & val ) = 0;

    virtual
    ~IColormapScalar() { }
};

/**
 * @brief The ColormapScalarNamed class
 *
 * Abstract helper class for deriving from IColormapScalar. It just adds a static
 * name to the colormap.
 */
class ColormapScalarNamed : public IColormapScalar
{
    CLASS_BOILERPLATE( ColormapScalarNamed );

public:

    ColormapScalarNamed( QString name = "n/a" )
    {
        m_name = name;
    }

    virtual QString
    name() override { return m_name; }

protected:

    QString m_name;
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
