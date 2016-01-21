/**
 *
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/ICoordinateFormatter.h"

#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <memory>

class CCCoordinateFormatter : public CoordinateFormatterInterface
{
    /// shortcut to AxisInfo type
    typedef Carta::Lib::AxisInfo AxisInfo;

    CLASS_BOILERPLATE( CCCoordinateFormatter );

public:

    CCCoordinateFormatter( std::shared_ptr < casa::CoordinateSystem > casaCS );

    virtual CCCoordinateFormatter * clone() const override;

    virtual int
    nAxes() const override;

    virtual QStringList
    formatFromPixelCoordinate( const VD & pix ) override;

    virtual QString
    calculateFormatDistance( const VD & p1, const VD & p2 ) override;

    virtual int
    axisPrecision( int axis ) override;

    virtual Me &
    setAxisPrecision( int precision, int axis ) override;

    virtual bool
    toWorld( const VD & pixel, VD & world ) const override;

    virtual bool
    toPixel( const VD & world, VD & pixel ) const override;

    virtual void
    setTextOutputFormat( TextFormat fmt ) override;

    virtual const Carta::Lib::AxisInfo &
    axisInfo( int ind ) const override;

    virtual Me &
    disableAxis( int ind ) override;

    virtual Me &
    enableAxis( int ind ) override;

    virtual KnownSkyCS
    skyCS() override;

    virtual Me &
    setSkyCS( const KnownSkyCS & scs ) override;

    virtual SkyFormatting
    skyFormatting() override;

    virtual Me &
    setSkyFormatting( SkyFormatting format ) override;

protected:

    /// parse casacore's coordinate system (all axes)
    void
    parseCasaCS();
    /// parse casacores's coordinate system (one axis)
    void
    parseCasaCSi(int pixelAxis);

    std::shared_ptr < casa::CoordinateSystem > m_casaCS;

    /// cached info per axis
    std::vector < AxisInfo > m_axisInfos;

    /// display axes
    std::vector < AxisInfo::KnownType> m_displayAxes;

    /// precisions
    std::vector < int > m_precisions;

    /// plain on html output
    TextFormat m_textOutputFormat;

    /// selected format for sky formatting
    SkyFormatting m_skyFormatting = SkyFormatting::Radians;

    /// format a world value for the selected axis
    QString formatWorldValue( int whichAxis, double worldValue);

};
