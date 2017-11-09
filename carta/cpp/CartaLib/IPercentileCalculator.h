/**
 * Interfaces and helper classes for percentile-to-pixel and pixel-to-percentile calculations
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/IImage.h"
#include "CartaLib/IntensityUnitConverter.h"
#include <QString>
#include <vector>

namespace Carta {
namespace Lib {

/// helper function for calculating a slice for a single 2D frame of the image
static Carta::Lib::NdArray::Double viewSliceForFrame(Carta::Lib::NdArray::Double view, int spectralIndex, int frameIndex) {
    // Create a slice for retrieving a single frame
    SliceND frame;
    for (size_t d = 0; d < view.dims().size(); d++) {
        if ((int)d == spectralIndex) {
            frame.index(frameIndex);
        } else {
            frame.next();
        }
    }

    // Create a new view for a single frame using this slice
    Carta::Lib::NdArray::Double viewSlice( view.rawView()->getView(frame), false ); // create a new view for this slice
    return viewSlice;
}


template <typename Scalar>
class IPercentilesToPixels {
    CLASS_BOILERPLATE( IPercentilesToPixels );
public:
    /**
     * Constructor.
     */
    IPercentilesToPixels(const double error, const QString label, const bool isApproximate=false, const bool needsMinMax=false);
    
    virtual ~IPercentilesToPixels();
    
    /** The error margin for the returned values. In future this may be deprecated in favour of independent per-value error margins, if necessary. */
    const double error;
    
    /** The name of this algorithm, for logging purposes. */
    const QString label;
    
    /** This may need to be rethought, but for now it's the simplest way to achieve this without having to calculate the minimum and maximum unnecessarily. */
    void setMinMax(std::vector<double> minMaxIntensities);
    
    /** The actual calculator function */
    virtual std::map<double, Scalar> percentile2pixels(
        Carta::Lib::NdArray::TypedView < Scalar > & view,
        std::vector <double> percentiles,
        int spectralIndex,
        Carta::Lib::IntensityUnitConverter::SharedPtr converter,
        std::vector<double> hertzValues
    );
    
    const bool isApproximate=false;
    const bool needsMinMax=false;
private:
    std::vector<double> minMaxIntensities;
};

IPercentilesToPixels::IPercentilesToPixels(const double error, const QString label, const bool isApproximate=false, const bool needsMinMax=false) : error(error), label(label), isApproximate(isApproximate), needsMinMax(needsMinMax) {
}

void IPercentilesToPixels::setMinMax(std::vector<double> minMaxIntensities) {
    this->minMaxIntensities = minMaxIntensities;
}

template <typename Scalar>    
class IPixelsToPercentiles {
    CLASS_BOILERPLATE( IPixelsToPercentiles );
public:
    /**
     * Constructor.
     */
    IPixelsToPercentiles(const double error, const QString label const bool isApproximate);
    virtual ~IPixelsToPercentiles();
    
    /** The error margin for the returned values. In future this may be deprecated in favour of independent per-value error margins, if necessary. */
    const double error;
    
    /** The name of this algorithm, for logging purposes. */
    const QString label;
    
    /** The actual calculator function */
    virtual std::vector<double> pixels2percentiles(
        Carta::Lib::NdArray::TypedView < Scalar > & view,
        std::vector <Scalar> pixels,
        int spectralIndex,
        Carta::Lib::IntensityUnitConverter::SharedPtr converter,
        std::vector<double> hertzValues
    );

    const bool isApproximate=false;
};

IPixelsToPercentiles::IPixelsToPercentiles(const double error, const QString label, const bool isApproximate=false) : error(error), label(label), isApproximate(isApproximate) {
}

}
}
