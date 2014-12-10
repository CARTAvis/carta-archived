/**
 *
 **/


#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/HtmlString.h"
#include <QString>
#include <vector>
#include <memory>

/// \brief Interface for generating labels for 2d plots along an axis from image metadata.
/// Dual labels are supported, in case we want to display both pixel and wcs
/// labels...
class PlotLabelGeneratorInterface
{
    CLASS_BOILERPLATE(PlotLabelGeneratorInterface);
public:

    /// \todo we can remove this once we put this class into carta namespace
    typedef Carta::Lib::TextFormat TextFormat;

    /// shortcut for my own type
    typedef PlotLabelGeneratorInterface This;

    /// Text measuring function signature. Should return the size of this text
    /// in pixels.
    typedef std::function< double( const QString & str) > TextMeasureFunc;

    /// entry for a label (dual labels)
    struct Label {
        /// computed label: the pixel label
        QString pixText;
        /// computed label: the world coordinate label
        QString wcsText;
        /// center of the label in pixel coordinates
        /// this is also where the grid line should be drawn
        double centerPix;
    };

    /// which labels to generate
    enum class Mode { PixelOnly, WCSOnly, Both };

    /// set text measure function (used for both labels)
    virtual This & setTextMeasureFunc( TextMeasureFunc fn) = 0;

    /// compute the labels using current settings
    virtual std::vector<Label> computeLabels() = 0;

    /// set the number of pixels available for drawing
    virtual This & setPixels( double npix) = 0;

    /// which axis to compute labels for
    virtual This & setAxis( int axis) = 0;

    /// which labels to generate
    virtual This & setMode( const Mode & mode) = 0;

    /// set text output mode
    virtual This & setTextMode( const TextFormat & format) = 0;

    virtual ~ PlotLabelGeneratorInterface();

};
