/**
 *
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/IImage.h"
#include "CartaLib/VectorGraphics/VGList.h"
#include <memory>

#ifdef DONT_COMPILE

class QPainter;

/// API for drawing grids of coordinate systems.
///
/// \warning this is most likely not going to be used at all.
class CoordinateGridPlotterInterface
{
    CLASS_BOILERPLATE( CoordinateGridPlotterInterface );

public:
    /// set line width
    virtual CoordinateGridPlotterInterface &
    setLineWidth( double width ) = 0;

    /// change to a different sky CS (if applicable)
    virtual void
    setSkyCS( const KnownSkyCS & cs ) = 0;

    /// change which axes to plot (first one is horizontal, second is vertical)
    virtual void
    setAxes( int axis1, int axis2 ) = 0;

    /// plot the grid with the current settings
    virtual void
    plot( QPainter & img ) = 0;

    /// virtual destructor
    virtual
    ~CoordinateGridPlotterInterface()
    { }
};

#endif

namespace Carta
{
namespace Lib
{

/// API for world coordinate rendering service
class IWcsGridRenderService : public QObject
{
    Q_OBJECT
    CLASS_BOILERPLATE( IWcsGridRenderService );

    typedef VectorGraphics::VGList VGList;

public:

    virtual void
    setInputImage( Image::ImageInterface::SharedPtr image ) = 0;

    virtual void
    setOutputSize( const QSize & size ) = 0;

    virtual void
    setImageRect( const QRectF & rect ) = 0;

    virtual void
    setOutputRect( const QRectF & rect ) = 0;

    virtual void
    setLineThickness( double thickness) = 0;

    virtual double
    lineThickness() = 0;

    virtual void
    setLineColor( QColor color) = 0;

    virtual QColor
    lineColor() = 0;

    virtual void
    setGridDensity( double density) = 0;

    virtual void
    setInternalLabels( bool on) = 0;

    /// start rendering with current parameters
    /// if possible, previously scheduled rendering will be canceled
    /// or you can think of it as this: multiple calls to this will normally only
    /// result in a single result reported
    virtual void
    startRendering() = 0;

    // looks like this needs to be defined out of line, otherwise dlopen() fails
    // to load any classes inherited from this...
    virtual
    ~IWcsGridRenderService();

signals:

    void
    done( Carta::Lib::VectorGraphics::VGList vg );

};

}
}
