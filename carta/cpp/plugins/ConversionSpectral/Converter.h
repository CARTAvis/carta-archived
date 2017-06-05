#pragma once

#include <QList>
#include <QString>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>

class Converter {
public:
    Converter( const QString& oldUnits, const QString& newUnits);

    //Factory for producing the appropriate converter.
    //Note:  user is responsible for deleting the converter.
    static Converter* getConverter( const QString& oldUnits,const QString& newUnits );

    //Converts units withen a unit type (for example frequency MHz -> GHz
    static void convert( casacore::Vector<double> &resultValues, int sourceIndex,
            int destIndex, casacore::SpectralCoordinate coordinate);


    //Abstract methods to be implemented by subclasses.
    virtual double toPixel( double value, casacore::SpectralCoordinate coordinate ) = 0;
    virtual casacore::Vector<double> convert( const casacore::Vector<double>& oldValues,
            casacore::SpectralCoordinate coordinate) = 0;
    virtual double convert ( double oldValue, casacore::SpectralCoordinate coordinate);
    virtual ~Converter();

    typedef enum {FREQUENCY_UNIT, VELOCITY_UNIT, WAVELENGTH_UNIT, CHANNEL_UNIT, UNRECOGNIZED } UnitType;

    static UnitType getUnitType( const QString& unit );

protected:
    static const QList<QString> FREQUENCY_UNITS;
    static const QList<QString> WAVELENGTH_UNITS;
    static const QList<QString> VELOCITY_UNITS;

    QString oldUnits;
    QString newUnits;


};
