/** This is a helper class which provides intensity-specific accessor functions for a generic disk cache object */

#pragma once

#include "CartaLib/IPCache.h"

#include <QString>

namespace Carta {
namespace Lib {
    
struct IntensityValue {
    IntensityValue(double value, double error);
    ~IntensityValue();
    double value;
    double error;
}
    
class IntensityCacheHelper {
    CLASS_BOILERPLATE( IntensityCacheHelper );
public:
    IntensityCacheHelper(std::shared_ptr<Carta::Lib::IPCache> diskCache);
    ~IntensityCacheHelper();
    
    /** Returns a pointer to a (value, error) pair if the value exists in the cache, or a null pointer */
    IntensityValue * get(int frameLow, int frameHigh, double percentile, int stokeFrame, QString transformationLabel);
    
    /** Sets the provided value and error for this intensity */
    void set(double intensity, double error, int frameLow, int frameHigh, double percentile, int stokeFrame, QString transformationLabel);
private:
    std::shared_ptr<Carta::Lib::IPCache> m_diskCache;
}

}
}