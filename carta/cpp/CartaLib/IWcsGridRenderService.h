/**
 * API for drawing WCS grids.
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/IImage.h"
#include "CartaLib/VectorGraphics/VGList.h"
#include <memory>

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

    /// job IDs
    typedef int64_t JobId;

    /// shortcut for VGList
    typedef VectorGraphics::VGList VGList;

    /// types of elements supported
    /// their appearance can be customized
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
        MarginDim, ///< rectangles around the grid border
        __count ///< internal, don't use
    };

    /// set the input data, shold have at least 2 dimensions
    /// \note the data is not imporant, only meta data attached to this is important
    virtual void
    setInputImage( Image::ImageInterface::SharedPtr image ) = 0;

    /// which part of the input image data to render (in casa coordinates)
    /// this always assumes we are doing axis1/axis2 grid
    virtual void
    setImageRect( const QRectF & rect ) = 0;

    /// how big the resulting rendering should be
    virtual void
    setOutputSize( const QSize & size ) = 0;

    /// where in the output to render the imageRect
    /// so the coordinate mapping from casa pixel coordinates to screen coordinates
    /// is imageRect -> outputRect
    virtual void
    setOutputRect( const QRectF & rect ) = 0;

    //Set the label for an axis.
    virtual void
    setAxisLabel( int axisIndex, const QString& axisLabel ) = 0;

    // set on which edge (top,bottom,left,right) the given axis should
    // be drawn; setting the edge to "" will remove labelling for the axis.
    virtual void
    setAxisLabelLocation( int axisIndex, const QString& edge ) = 0;

    // set the format to use when labelling an axis.
    virtual void
    setAxisLabelFormat( int axisIndex, const QString& formatStr ) = 0;

    //Set whether or not to draw axes/border.
    virtual void
    setAxesVisible( bool flag ) = 0;

    /// set a pen attribute for the given element
    virtual void
    setPen( Element e, const QPen & pen ) = 0;

//    /// get a pen attribute for the given element
//    virtual const QPen &
//    pen( Element e ) = 0;


    /// set the font attribute for the given text element
    /// does nothing for non-text elements, i.e. lines
    virtual void
    setFont( Element e, int fontIndex, double pointSize ) = 0;

    /// set the grid density modifier...
    virtual void
    setGridDensityModifier( double density ) = 0;

    //Set whether or not grid lines should be drawn.
    virtual void
    setGridLinesVisible( bool on ) = 0;

    /// set whether the grid labels should be external or internal
    virtual void
    setInternalLabels( bool on ) = 0;

    /// which celestial system to draw
    virtual void
    setSkyCS( KnownSkyCS cs ) = 0;

    //Set the length of the ticks
    virtual void
    setTickLength( double tickLength ) = 0;

    /// set whether or not tick marks should be drawn.
    virtual void
    setTicksVisible( bool on ) = 0;


    /// start rendering with current parameters
    /// if possible, previously scheduled rendering will be canceled
    /// or you can think of it as this: multiple calls to this will normally only
    /// result in a single result reported
    ///
    /// \param jobId id assigned to the rendering request, which will be reported back
    /// witht the done() signal, which can be used to make sure the arrived done() signal
    /// corresponds to the latest request. It should be a positive number. If unspecified
    /// (or negative number is supplied, a new id will be generated, which will
    /// the previous one + 1)
    /// \return the jobId to expect when the rendering is done (useful for unspecified
    /// jobId)
    virtual JobId
    startRendering( JobId jobId = - 1 ) = 0;

    /// \brief whether the service should report empty grid or not
    /// \param flag if set to true, the result will always be an empty grid
    ///
    /// This is a convenience function to make synchronization a little bit easier when
    /// one needs to combine a rendered image with a grid, but the grid drawing is disabled.
    /// Insteady of handling it as a special case, you can just set this flag to true, and
    /// grid will be reported as drawn, but the VGList will be empty.
    virtual void
    setEmptyGrid( bool flag ) = 0;

    /// virtual destructor
    ///\note looks like this needs to be defined out of line, otherwise dlopen() fails
    /// to load any classes inherited from this...
    virtual
    ~IWcsGridRenderService();

signals:

    /// gets emitted when rendering is done
    /// \param vg the rendered vector graphics
    /// \param id the jobId this result corresponds to
    void
    done( Carta::Lib::VectorGraphics::VGList vg, JobId id );
};
}
}
