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

/// API for world coordinate renderer
class IWcsGridRenderer : public QObject
{
    Q_OBJECT
    CLASS_BOILERPLATE( IWcsGridRenderer );

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
    setLineColor( QColor color) = 0;

    /// start rendering with current parameters
    /// previously scheduled rendering may be aborted as a result of calling this
    virtual void
    startRendering() = 0;

    // looks like this needs to be defined out of line, otherwise dlopen() fails
    // to load any classes inherited from this...
    virtual
    ~IWcsGridRenderer();

signals:

    void
    done( Carta::Lib::VectorGraphics::VGList vg );

};

}
}
