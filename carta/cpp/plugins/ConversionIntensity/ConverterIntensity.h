#pragma once

#include <QString>
#include <QList>
#include <QMap>
#include <QStringList>
#include <QRegExp>
#include <vector>
#include <functional>

/**
 * Converts intensity units:  Jy/Beam, Kelvin, Fraction of Peak, etc.
 */

class ConverterIntensity {
public:
    static const double BOLTZMANN;
    static const double LIGHT_SPEED_FACTOR;
    static const double ARCSECONDS_SQUARED_PER_STERADIAN;
    static const double JY_IN_MJY;
    
    //Hertz values are needed corresponding to the values for Jy/Beam Kelvin conversions
    //only.  Both oldUnits and newUnits refer to the old and new units of the values
    //array.  In order to do FRACTION_OF_PEAK conversions, a maximum value with
    //corresponding maximum units must be passed in.
    static void convert( std::vector<double>& values, const std::vector<double>& hertzValues,
            const QString& oldUnits, const QString& newUnits,
            double maxValue, const QString& maxUnits,
            double beamAngle, double beamArea);

    // Returns a (lambda, multiplier) tuple for converting a sequence of values from the old to the new units.
    // The lambda is the frame-dependent portion; the multiplier is the frame-independent portion.
    static std::tuple<std::function<double(double, double)>, double> converters( std::vector<double>& values, const std::vector<double>& hertzValues,
            const QString& oldUnits, const QString& newUnits,
            double maxValue, const QString& maxUnits,
            double beamAngle, double beamArea);

    // Split units into a tuple containing an exponent based on the prefix, and the base units.
    static std::tuple<int, QString> splitUnits(const QString& units);

    virtual ~ConverterIntensity();

private:
    ConverterIntensity();
};
