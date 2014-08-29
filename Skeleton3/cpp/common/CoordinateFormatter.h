/**
 * The main purpose of this class is to provide coordinate formatting, using fairly
 * high level APIs. These APIs were designed from the perspective of what the viewer
 * needs with respect to the coordinate related metadata contained in images.
 **/


#pragma once

#include "misc.h"
#include "AxisInfo.h"
#include <vector>
#include <QStringList>


/// \brief coordinate system formatter
/// \todo make all methods pure virtual
///
class CoordinateFormatterInterface {
public:
    /// shortcut to my own type
    typedef CoordinateFormatterInterface This;

    /// chaining shortcut
    typedef This & Chain;

    /// shortcut to a vector of doubles
    typedef std::vector<double> VD;

    /// clone makes a copy of itself
    virtual CoordinateFormatterInterface * clone() const = 0;

    /// how many axes are there?
    virtual int nAxes() = 0;

    /// calculate and format pixel coordinate cursor using current settings
    virtual QStringList formatFromPixelCoordinate(const VD& pix) = 0;

    /// calculate and format distance between two pixels
    virtual QString calculateFormatDistance( const VD & p1, const VD & p2) = 0;

    /// set text output format
    void setTextOutputFormat(TextFormat fmt);

    /// get information about an axis (such as labels, suffix)
    const AxisInfo & axisInfo(int ind) const;

    /// disable printing of an axis
    Chain disableAxis(int ind);

    /// enable printing of an axis
    Chain enableAxis(int ind);

    /// get the sky coordinate system used right now
    KnownSkyCS skyCS();

    /// set the sky coordinate system
    Chain setSkyCS( const KnownSkyCS & scs);

    /// get the current sky formatting (degree, sexagecimal, etc)
    SkyFormat skyFormat();

    /// set the current sky formatting
    Chain setSkyFormat(SkyFormat format);

    /// get the current precision for an axis
    virtual int axisPrecision(int axis) = 0;

    /// adjust precision for a given axis (or all if axis = -1)
    /// precision = -1 denotes default precision
    virtual Chain setAxisPrecision(int precision, int axis = -1) = 0;

    /// convert pixel coordinates to world coordinates
    virtual bool toWorld(const VD& pixel, VD& world) const = 0;

    /// convert world coordinates to pixel coordinates
    virtual bool toPixel(const VD& world, VD& pixel) const = 0;

    /// virtual destructor
    virtual ~CoordinateFormatterInterface() {}

};

