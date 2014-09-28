/**
 *
 **/

#pragma once

#include "common/CoordinateFormatter.h"

class CCCoordinateFormatter : public CoordinateFormatterInterface
{
public:
    CCCoordinateFormatter();

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
};
