/**
 * The main purpose of this class is to provide coordinate formatting, using fairly
 * high level APIs. These APIs were designed from the perspective of what the viewer
 * needs with respect to the coordinate related metadata contained in images.
 **/


#pragma once

#include "CartaLib/CartaLib.h"
#include "AxisInfo.h"
#include <vector>
#include <QStringList>
#include <memory>

/// \brief coordinate system formatter
/// \todo make all methods pure virtual
///
class CoordinateFormatterInterface {

    CLASS_BOILERPLATE(CoordinateFormatterInterface);

public:

    // \todo temporary typedef until we move this entire class to Carta::Lib
    typedef Carta::Lib::KnownSkyCS KnownSkyCS;

    /// \todo we can remove this once we put this class into carta namespace
    typedef Carta::Lib::TextFormat TextFormat;

    /// shortcut to a vector of doubles
    typedef std::vector<double> VD;

    /// clone makes a copy of itself
    virtual CoordinateFormatterInterface * clone() const = 0;

    /// how many axes are there?
    virtual int nAxes() const = 0;

    /// for the given pixel coordinate, calculate the world coordinates and
    /// format them using current settings, with units appended where appropriate
    virtual QStringList formatFromPixelCoordinate(const VD& pix) = 0;

    /// calculate and format distance between two pixels
    virtual QString calculateFormatDistance( const VD & p1, const VD & p2) = 0;

    /// set text output format
    virtual void setTextOutputFormat(TextFormat fmt) = 0;

    /// get information about an axis (such as labels, suffix)
    virtual const Carta::Lib::AxisInfo & axisInfo(int ind) const = 0;

    /// disable printing of an axis
    virtual Me & disableAxis(int ind) = 0;

    /// enable printing of an axis
    virtual Me & enableAxis(int ind) = 0;

    /// get the sky coordinate system used right now
    virtual KnownSkyCS skyCS() = 0;

    /// set the sky coordinate system
    /// \warning this can fail if conversion not possible/supported. Check skyCS() to see
    /// if it was successful.
    /// \warning this resets skyFormatting and precisions for the sky axes
    virtual Me & setSkyCS( const KnownSkyCS & scs) = 0;

    /// get the current sky formatting (degree, sexagesimal, etc)
    /// \note never returns Default
    virtual SkyFormatting skyFormatting() = 0;

    /// set the current sky formatting
    /// if 'Default' is specified, it is automatically converted to the most appropriate
    /// formatting
    virtual Me & setSkyFormatting(SkyFormatting format) = 0;

    /// get the current precision for an axis
    virtual int axisPrecision(int axis) = 0;

    /// adjust precision for a given axis (or all if axis = -1)
    /// precision = -1 denotes default precision
    virtual Me & setAxisPrecision(int precision, int axis = -1) = 0;

    /// convert pixel coordinates to world coordinates
    virtual bool toWorld(const VD& pixel, VD& world) const = 0;

    /// convert world coordinates to pixel coordinates
    virtual bool toPixel(const VD& world, VD& pixel) const = 0;

    /// virtual destructor
    virtual ~CoordinateFormatterInterface() {}

};

