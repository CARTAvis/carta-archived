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
    static void convert( casa::Vector<double> &resultValues, int sourceIndex,
            int destIndex, casa::SpectralCoordinate coordinate);


    //Abstract methods to be implemented by subclasses.
    virtual double toPixel( double value, casa::SpectralCoordinate coordinate ) = 0;
    virtual casa::Vector<double> convert( const casa::Vector<double>& oldValues,
            casa::SpectralCoordinate coordinate) = 0;
    virtual double convert ( double oldValue, casa::SpectralCoordinate coordinate);
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
