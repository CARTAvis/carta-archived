/**
 * A helper class for intensity unit conversion which includes:
 * - a flag which determines whether the unit conversion is dependent on the frame values of the data (frequencies in Hz)
 * - a function which performs the frame-dependent portion of the conversion on a value and corresponding Hz value
 * - a label describing the frame-dependent function
 * - a string recording the old units
 * - a string recording the new units
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
    IntensityUnitConverter(const QString fromUnits, const QString toUnits, 
        const double multiplier=1, const bool frameDependent=false, 
        const QString label="NONE");
    virtual ~IntensityUnitConverter();
    
    double multiplier;
    bool frameDependent;
    
    /** This value is intended for use in the percentile cache key. 
     * Percentiles have to be recalculated for each frame-dependent 
     * conversion, but do not have to be recalculated for each possible 
     * constant multiplier. */
    QString label;
    
    /** These values are for informational purposes only and may be 
     * modified from the original unit strings e.g. to indicate a 
     * conversion to or from Fraction of Peak */
    QString fromUnits;
    QString toUnits;
    
    virtual double convert(const double y_val, const double x_val);
    virtual double convert(const double y_val);
    virtual std::vector<double> convert(const std::vector<double> y_vals, const std::vector<double> x_vals={});
    virtual double _frameDependentConvert(const double y_val, const double x_val);
    virtual double _frameDependentConvertInverse(const double y_val, const double x_val);
};

}
}
