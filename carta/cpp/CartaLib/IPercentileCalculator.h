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

template <typename Scalar>    
class IPercentilesToPixels {
    CLASS_BOILERPLATE( IPercentilesToPixels );
public:
    /**
     * Constructor.
     */
    IPercentilesToPixels();
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
