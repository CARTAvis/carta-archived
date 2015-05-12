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

#endif // ifdef DONT_COMPILE

namespace Carta
{
namespace Lib
{
/// API for world coordinate rendering service
class IWcsGridRenderService : public QObject
{
    Q_OBJECT
    CLASS_BOILERPLATE( IWcsGridRenderService );

public:

    /// shortcut for VGList
    typedef VectorGraphics::VGList VGList;

    /// types of elements supported
    enum class Element
    {
        BorderLines = 0, ///< border around the image (for external labels)
        AxisLines1, ///< thick axis1 lines (for internal render)
        AxisLines2, ///< thick axis2 lines (for internal render)
        GridLines1, ///< regular grid lines for axis1
        GridLines2, ///< regular grid lines for axis2
        TickLines1, ///< tick lines for axis1
        TickLines2, ///< tick lines for axis2
        NumText1, ///< numerical labels for axis1
        NumText2, ///< numerical labels for axis2
        LabelText1, ///< annotation for axis1
        LabelText2, ///< annotation for axis2
        Shadow, ///< shadow under text/lines
        __count ///< internal, don't use
    };

    /// set the input data, shold have at least 2 dimensions
    virtual void
    setInputImage( Image::ImageInterface::SharedPtr image ) = 0;

    /// how big the resulting image is
    virtual void
    setOutputSize( const QSize & size ) = 0;

    /// which part of the image data to render (in casa coordinates)
    virtual void
    setImageRect( const QRectF & rect ) = 0;

    /// where in the output to render the imageRect
    virtual void
    setOutputRect( const QRectF & rect ) = 0;

    /// set the pen attribute for the given element
    virtual void
    setPen( Element e, const QPen & pen) = 0;

    /// get the pen attribute for the given element
    virtual const QPen &
    pen( Element e) = 0;

    /// set the shadow color
    /// anything with alpha=0 will mean no shadow
    virtual void setShadowColor( const QColor & color) = 0;

    /// return the current shadow color
    virtual const QColor & shadowColor() = 0;

    /// set the font attribute for the given text element
    /// does nothing for non-text elements, i.e. lines
    virtual void
    setFont( Element e, const QFont & font) = 0;

    /// get the font attribute for the given element
    virtual const QFont &
    font( Element e) = 0;

    virtual void
    setLineThickness( double thickness ) = 0;

    virtual double
    lineThickness() = 0;

    virtual void
    setLineColor( QColor color ) = 0;

    virtual QColor
    lineColor() = 0;

    /// set the grid density modifier...
    virtual void
    setGridDensityModifier( double density ) = 0;

    /// set whether the grid labels should be external or internal
    /// \warning: sometimes external will result in internal if external
    /// request is hard to satisfy
    virtual void
    setInternalLabels( bool on ) = 0;

    /// which celestial system to draw
    virtual void
    setSkyCS( KnownSkyCS cs ) = 0;

    /// start rendering with current parameters
    /// if possible, previously scheduled rendering will be canceled
    /// or you can think of it as this: multiple calls to this will normally only
    /// result in a single result reported
    virtual void
    startRendering() = 0;

    ///\note looks like this needs to be defined out of line, otherwise dlopen() fails
    /// to load any classes inherited from this...
    virtual
    ~IWcsGridRenderService();

signals:

    /// gets emitted when rendering is done
    void
    done( Carta::Lib::VectorGraphics::VGList vg );
};
}
}
