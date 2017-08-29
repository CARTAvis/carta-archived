#pragma once

#include <QString>
#include <QList>
#include <QMap>
#include <QStringList>
#include <QRegExp>
#include <vector>
#include <functional>
#include "CartaLib/IntensityUnitConverter.h"

class DivideByFrequencySquared : public Carta::Lib::IntensityUnitConverter {
public:
    DivideByFrequencySquared(const QString fromUnits, const QString toUnits, const double multiplier);
    double _frameDependentConvert(const double y_val, const double x_val) override;
    double _frameDependentConvertInverse(const double y_val, const double x_val) override;
};

class MultiplyByFrequencySquared : public Carta::Lib::IntensityUnitConverter {
public:
    MultiplyByFrequencySquared(const QString fromUnits, const QString toUnits, const double multiplier);
    double _frameDependentConvert(const double y_val, const double x_val) override;
    double _frameDependentConvertInverse(const double y_val, const double x_val) override;
};

class ConstantMultiplier : public Carta::Lib::IntensityUnitConverter {
public:
    ConstantMultiplier(const QString fromUnits, const QString toUnits, const double multiplier);
};

/**
 * Converts intensity units:  Jy/Beam, Kelvin, Fraction of Peak, etc.
 */

class ConverterIntensity {
public:
    static const double BOLTZMANN;
    static const double LIGHT_SPEED_FACTOR;
    static const double ARCSECONDS_SQUARED_PER_STERADIAN;
    static const double JY_IN_MJY;
    
    /** Returns a tuple for converting a sequence of values from the old to the new units.
     * 
     * maxValue and maxUnits are required for conversions from and to fraction of peak.
     * beamArea is required for conversions to and from Jy/beam.
     * 
     * The tuple elements are:
     * - the frame-dependent portion of the conversion; a function of the value and its corresponding hertz value
     * - the frame-independent portion; a constant multiplier
     * - a boolean flag indicating whether the conversion is frame dependent
     * If the flag is false, a no-op function will be returned. 
     */
    static Carta::Lib::IntensityUnitConverter::SharedPtr converters(
            const QString& oldUnits, const QString& newUnits,
            double maxValue, const QString& maxUnits,
            double beamArea);

    /** Splits units into a tuple containing an exponent based on the prefix, and the base units.
     */
    static std::tuple<int, QString> splitUnits(const QString& units);

    virtual ~ConverterIntensity();

private:
    ConverterIntensity();
};
