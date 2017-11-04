#include "IntensityUnitConverter.h"

namespace Carta {
namespace Lib {
    
IntensityUnitConverter::IntensityUnitConverter(const QString fromUnits, const QString toUnits, 
        const double multiplier, const bool frameDependent, const QString label) : 
    multiplier(multiplier), frameDependent(frameDependent), label(label), fromUnits(fromUnits), toUnits(toUnits) {
}


IntensityUnitConverter::~IntensityUnitConverter() {
}


double IntensityUnitConverter::_frameDependentConvert(const double y_val, const double x_val) {
    Q_UNUSED(x_val);
    return y_val;
}


double IntensityUnitConverter::_frameDependentConvertInverse(const double y_val, const double x_val) {
    Q_UNUSED(x_val);
    return y_val;
}


double IntensityUnitConverter::convert(const double y_val, const double x_val) {
    double result;
    
    if (frameDependent) {
        result = _frameDependentConvert(y_val, x_val) * multiplier;
    } else {
        result = y_val * multiplier;
    }
    
    return result;
}


double IntensityUnitConverter::convert(const double y_val) {
    double result;
    
    if (frameDependent) {
        throw QString("Could not convert intensity value from %1 to %2 because a corresponding Hertz value was not provided.").arg(fromUnits).arg(toUnits);
    } else {
        result = y_val * multiplier;
    }
    
    return result;
}


std::vector<double> IntensityUnitConverter::convert(const std::vector<double> y_vals, const std::vector<double> x_vals) {
    std::vector<double> results;
    
    if (frameDependent) {
        if(x_vals.size() < y_vals.size()) {
            throw QString("Could not convert intensity values from %1 to %2 because not enough corresponding Hertz values were provided.").arg(fromUnits).arg(toUnits);
        }

        for (size_t i = 0; i < y_vals.size(); i++) {
            // TODO: what if a hertz value is zero?
            results.push_back(_frameDependentConvert(y_vals[i], x_vals[i]) * multiplier);
        }
    } else {
        for (size_t i = 0; i < y_vals.size(); i++) {
            results.push_back(y_vals[i] * multiplier);
        }
    }
    
    return results;
}
    
}
}
