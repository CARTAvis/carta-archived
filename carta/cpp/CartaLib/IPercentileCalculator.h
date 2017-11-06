/**
 * Interfaces and helper classes for percentile-to-pixel and pixel-to-percentile calculations
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/IImage.h"
#include "CartaLib/IntensityUnitConverter.h"
#include <QString>
#include <vector>

// TODO TODO TODO hertz values, converter and spectral index (?) should be calculated once and cached on the object?

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
    IPercentilesToPixels(const double error, const QString label);
    virtual ~IPercentilesToPixels();
    
    /** The error margin for the returned values. In future this may be deprecated in favour of independent per-value error margins, if necessary. */
    const double error;
    
    /** The name of this algorithm, for logging purposes. */
    const QString label;
    
    virtual std::map<double, Scalar> percentile2pixels(
        Carta::Lib::NdArray::TypedView < Scalar > & view,
        std::vector <double> percentiles,
        int spectralIndex=-1,
        Carta::Lib::IntensityUnitConverter::SharedPtr converter=nullptr,
        std::vector<double> hertzValues={}
    );
};


template <typename Scalar>    
class IPixelsToPercentiles {
    CLASS_BOILERPLATE( IPixelsToPercentiles );
public:
    /**
     * Constructor.
     */
    IPixelsToPercentiles();
    virtual ~IPixelsToPercentiles();
    
    /** The error margin for the returned values. In future this may be deprecated in favour of independent per-value error margins, if necessary. */
    const double error;
    
    /** The name of this algorithm, for logging purposes. */
    const QString label;
    
    virtual std::vector<double> pixels2percentiles(
        Carta::Lib::NdArray::TypedView < Scalar > & view,
        std::vector <Scalar> pixels,
        int spectralIndex=-1,
        Carta::Lib::IntensityUnitConverter::SharedPtr converter=nullptr,
        std::vector<double> hertzValues={}
    );
};

}
}
