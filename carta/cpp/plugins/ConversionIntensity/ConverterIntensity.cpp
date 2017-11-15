#include "ConverterIntensity.h"
#include <math.h>
#include <QDebug>
#include <cmath>

DivideByFrequencySquared::DivideByFrequencySquared(const QString fromUnits, const QString toUnits, const double multiplier)
    : Carta::Lib::IntensityUnitConverter(fromUnits, toUnits, multiplier, true, "DIV_BY_HZ_SQ") {
}

double DivideByFrequencySquared::_frameDependentConvert(const double y_val, const double x_val) {
    return y_val / pow(x_val, 2);
}

double DivideByFrequencySquared::_frameDependentConvertInverse(const double y_val, const double x_val) {
    return y_val * pow(x_val, 2);
}

MultiplyByFrequencySquared::MultiplyByFrequencySquared(const QString fromUnits, const QString toUnits, const double multiplier) 
    : Carta::Lib::IntensityUnitConverter(fromUnits, toUnits, multiplier, true, "MULT_BY_HZ_SQ") {
}

double MultiplyByFrequencySquared::_frameDependentConvert(const double y_val, const double x_val) {
    return y_val * pow(x_val, 2);
}

double MultiplyByFrequencySquared::_frameDependentConvertInverse(const double y_val, const double x_val) {
    return y_val / pow(x_val, 2);
}

ConstantMultiplier::ConstantMultiplier(const QString fromUnits, const QString toUnits, const double multiplier) 
    : Carta::Lib::IntensityUnitConverter(fromUnits, toUnits, multiplier, false, "NONE") {
}

const double ConverterIntensity::BOLTZMANN = 1.38e-23;
const double ConverterIntensity::LIGHT_SPEED_FACTOR = 9e-10; // includes a unit conversion; find out exactly what this is
const double ConverterIntensity::ARCSECONDS_SQUARED_PER_STERADIAN = pow(180 * 3600 / M_PI, 2);
const double ConverterIntensity::JY_IN_MJY = 1e6;

ConverterIntensity::ConverterIntensity() {
}

Carta::Lib::IntensityUnitConverter::SharedPtr ConverterIntensity::converters(
        const QString& oldUnits, const QString& newUnits,
        double maxValue, const QString& maxUnits,
        double beamArea ) {

    double multiplier(1);
    Carta::Lib::IntensityUnitConverter::SharedPtr converter = nullptr;
    
    int fromExponent, toExponent;
    QString fromUnits, toUnits, fromBase, toBase;

    if (newUnits == "Fraction of Peak") {
        // Currently the only allowed option for converting *to* FOP is *from* the same units as the maximum value
        // No other conversions are possible
        if (oldUnits != "Fraction of Peak") { // which would be a no-op
            if (oldUnits != maxUnits) {
                // TODO: we need better error handling in the plugin
                // TODO: raise exceptions in here and handle them in the main plugin
                qWarning() << "In conversion to Fraction of Peak, maximum value units don't match intensity units. Result may be incorrect.";
            }
            
            toUnits = oldUnits + " (Fraction of Peak)";
            
            // do the conversion anyway for now; that's what the plugin did before
            multiplier /= maxValue;
        }
    } else {
        if (oldUnits == "Fraction of Peak") {
            multiplier *= maxValue;
            std::tie(fromExponent, fromBase) = splitUnits(maxUnits);
            fromUnits = maxUnits + " (Fraction of Peak)";
        } else {
            std::tie(fromExponent, fromBase) = splitUnits(oldUnits);
            fromUnits = oldUnits;
        }
        
        toUnits = newUnits;
    
        std::tie(toExponent, toBase) = splitUnits(newUnits);

        if (((fromBase == "Jy/beam" && toBase != "Jy/beam") || (fromBase != "Jy/beam" && toBase == "Jy/beam")) && !beamArea) {
            throw QString("Could not convert from %1 to %2 because beam information is missing.").arg(oldUnits).arg(newUnits);
        } else {
            multiplier *= pow(10, fromExponent - toExponent);
            
            double beamAngle = beamArea / ARCSECONDS_SQUARED_PER_STERADIAN;

            // TODO: precision -- optimise the order of terms in the multipliers?

            if (fromBase == "Kelvin") {
                if (toBase == "Jy/beam") {
                    multiplier *= (2 * BOLTZMANN * beamAngle) / LIGHT_SPEED_FACTOR;
                } else if (toBase == "Jy/arcsec^2") {
                    multiplier *= (2 * BOLTZMANN ) / (ARCSECONDS_SQUARED_PER_STERADIAN * LIGHT_SPEED_FACTOR);
                } else if (toBase == "MJy/sr") {
                    multiplier *= (2 * BOLTZMANN) / (JY_IN_MJY * LIGHT_SPEED_FACTOR);
                }
            } else if (fromBase == "Jy/beam") {
                if (toBase == "Kelvin") {
                    multiplier *= LIGHT_SPEED_FACTOR / (2 * BOLTZMANN * beamAngle);
                } else if (toBase == "Jy/arcsec^2") {
                    multiplier *= 1 / beamArea;
                } else if (toBase == "MJy/sr") {
                    multiplier *= 1 / (beamAngle * JY_IN_MJY);
                }
            } else if (fromBase == "Jy/arcsec^2") {
                if (toBase == "Kelvin") {
                    multiplier *= (ARCSECONDS_SQUARED_PER_STERADIAN * LIGHT_SPEED_FACTOR) / (2 * BOLTZMANN);
                } else if (toBase == "Jy/beam") {
                    multiplier *= beamArea;
                } else if (toBase == "MJy/sr") {
                    multiplier *= ARCSECONDS_SQUARED_PER_STERADIAN / JY_IN_MJY;
                }
            } else if (fromBase == "MJy/sr") {
                if (toBase == "Kelvin") {
                    multiplier *= (JY_IN_MJY * LIGHT_SPEED_FACTOR) / (2 * BOLTZMANN);
                } else if (toBase == "Jy/beam") {
                    multiplier *= beamAngle * JY_IN_MJY;
                } else if (toBase == "Jy/arcsec^2") {
                    multiplier *= JY_IN_MJY / ARCSECONDS_SQUARED_PER_STERADIAN;
                }
            }

            if (fromBase == "Kelvin" && toBase != "Kelvin") {
                converter = std::make_shared<MultiplyByFrequencySquared>(fromUnits, toUnits, multiplier);
            } else if (toBase == "Kelvin" && fromBase != "Kelvin") {
                converter = std::make_shared<DivideByFrequencySquared>(fromUnits, toUnits, multiplier);
            }

        }
        
    }

    if (!converter) {
        converter = std::make_shared<ConstantMultiplier>(fromUnits, toUnits, multiplier);
    }
    
    return converter;
}

std::tuple<int, QString> ConverterIntensity::splitUnits(const QString& units) {
    int exponent(0);
    QString baseUnits("");

    // simple special case -- should we have more general handling for K with prefixes?
    if (units == "K") {
        return std::make_tuple(0, QString("Kelvin"));
    }

    const QStringList BASE_UNITS = {"Jy/beam", "Jy/arcsec^2", "MJy/sr", "Kelvin"};
    const QMap<QString, int> SI_PREFIX = {{"p", -12}, {"n", -9}, {"u", -6}, {"m", -3}, {"", 0}, {"k", 3}, {"M", 6}, {"G", 9}};

    QString baseUnitList = BASE_UNITS.join("|");
    baseUnitList.replace("^", "\\^");
    QString siPrefixList = QStringList(SI_PREFIX.keys()).join("|");

    QRegExp rx("^(10+)?(" + siPrefixList + ")?(" + baseUnitList + ")(\\*pixels)?$");
    int pos = rx.indexIn(units);
    if (pos > -1) {
        if (rx.cap(1) != "") {
            exponent += std::round(log10(rx.cap(1).toInt()));
        }

        if (rx.cap(2) != "") {
            exponent += SI_PREFIX[rx.cap(2)];
        }

        baseUnits = rx.cap(3);
    } else {
        throw QString("Unable to parse unit string %1.").arg(units);
    }

    return std::make_tuple(exponent, baseUnits);
}

ConverterIntensity::~ConverterIntensity() {
}
