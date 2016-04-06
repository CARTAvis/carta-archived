#pragma once

#include <QString>
#include <QList>
#include <coordinates/Coordinates/SpectralCoordinate.h>

/**
 * Converts intensity units:  Jy/Beam, Kelvin, Fraction of Peak, etc.
 */

class ConverterIntensity {
public:
    static const QString FRACTION_OF_PEAK;
    static const QString KELVIN;
    static const QString JY_SR;
    static const QString JY_ARCSEC;
    static const QString JY_BEAM;
    static const QString JY;
    static const QString ADU;
    static const QString TIMES_PIXELS;
    static bool isSupportedUnits( const QString& yUnit );
    //Hertz values are needed corresponding to the values for Jy/Beam Kelvin conversions
    //only.  Both oldUnits and newUnits refer to the old and new units of the values
    //array.  In order to do FRACTION_OF_PEAK conversions, a maximum value with
    //corresponding maximum units must be passed in.
    static void convert( std::vector<double>& values, const std::vector<double>& hertzValues,
            const QString& oldUnits, const QString& newUnits,
            double maxValue, const QString& maxUnits,
            double beamAngle, double beamArea);

    static void convert( std::vector<double> &resultValues, int sourceIndex, int destIndex);

    //Converts between Jy/Beam units.  For example, MJy/Beam <-> Jy/Beam
    static double convertJyBeams( const QString& sourceUnits, const QString& destUnits,
            double value );
    static double convertJY( const QString& oldUnits, const QString& newUnits,
            double value );
    static double convertJYSR( const QString& oldUnits,const QString& newUnits,
            double value );
    static double convertKelvin( const QString& oldUnits,const QString& newUnits,
            double value );
    virtual ~ConverterIntensity();

private:
    ConverterIntensity();
    static double percentToValue( double yValue, double maxValue );
    static double valueToPercent( double yValue, double maxValue );
    static double convertQuantity( double yValue, double frequencyValue,
            const QString& oldUnits, const QString& newUnits,
            double beamSolidAngle, double beamArea );
    static void convertJansky( std::vector<double>& values, const QString& oldUnits,
            const QString& newUnits );
    static void convertKelvin( std::vector<double>& values, const QString& oldUnits,
            const QString& newUnits );
    static bool isJansky( const QString& units );
    static bool isKelvin( const QString& units );
    static double convertNonKelvinUnits( double yValue, const QString& oldUnits,
            const QString& newUnits, double beamArea );
    static QString getJanskyBaseUnits( const QString& units );
    static QString getKelvinBaseUnits( const QString& units );
    static QString stripPixels( const QString& units );
    static double beamToArcseconds( double yValue, double beamArea );
    static double arcsecondsToBeam( double yValue, double beamArea );
    static double srToArcseconds( double yValue );
    static double arcsecondsToSr( double yValue );
    static const QList<QString> BEAM_UNITS;
    static const QList<QString> JY_UNITS;
    static const QList<QString> JY_SR_UNITS;
    static const QList<QString> KELVIN_UNITS;
    static const double SPEED_LIGHT_FACTOR;
    static const double FREQUENCY_FACTOR;
    static const double ARCSECONDS_PER_STERADIAN;
};
