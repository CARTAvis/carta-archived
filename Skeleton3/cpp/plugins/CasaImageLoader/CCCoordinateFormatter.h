/**
 *
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "common/CoordinateFormatter.h"

#include <coordinates/Coordinates/CoordinateSystem.h>
#include <memory>

class CCCoordinateFormatter : public CoordinateFormatterInterface
{
    CLASS_BOILERPLATE( CCCoordinateFormatter );

public:
    CCCoordinateFormatter( std::shared_ptr<casa::CoordinateSystem> casaCS) {
        m_casaCS = casaCS;
    }

    virtual CoordinateFormatterInterface *
    clone() const override;

    virtual int
    nAxes() override;

    virtual QStringList
    formatFromPixelCoordinate( const VD & pix ) override;

    virtual QString
    calculateFormatDistance( const VD & p1, const VD & p2 ) override;

    virtual int
    axisPrecision( int axis ) override;

    virtual Chain
    setAxisPrecision( int precision, int axis ) override;

    virtual bool
    toWorld( const VD & pixel, VD & world ) const override;

    virtual bool
    toPixel( const VD & world, VD & pixel ) const override;

protected:

    std::shared_ptr<casa::CoordinateSystem> m_casaCS;
};
