/**
 * A helper class for intensity unit conversion which includes:
 * - a flag which determines whether the unit conversion is dependent on the frame values of the data (frequencies in Hz)
 * - a function which performs the frame-dependent portion of the conversion on a value and corresponding Hz value
 * - a label describing the frame-dependent function, which can be incorporanted into a cache key
 * - a frame-independent multiplier
 * - a helper function which performs the full conversion on a single value and Hz value
 * - a helper function which performs the full conversion on vectors of values and Hz values
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include <QString>
#include <vector>

namespace Carta {
namespace Lib {
    
class IntensityUnitConverter {
    CLASS_BOILERPLATE( IntensityUnitConverter );
public:
    /**
     * Constructor.
     */
    IntensityUnitConverter(const double multiplier=1, const bool frameDependent=false, const QString label="NONE");
    virtual ~IntensityUnitConverter();
    
    double multiplier;
    bool frameDependent;
    QString label;
    
    virtual double convert(const double y_val, const double x_val);
    virtual std::vector<double> convert(const std::vector<double> y_vals, const std::vector<double> x_vals);
private:
    virtual double _frameDependentConvert(const double y_val, const double x_val);
};

}
}
