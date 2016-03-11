#include "ConverterFrequencyWavelength.h"
#include <QDebug>

ConverterFrequencyWavelength::ConverterFrequencyWavelength(const QString& oldUnits,
        const QString& newUnits):
        ConverterFrequency( oldUnits, newUnits) {

}

casa::Vector<double> ConverterFrequencyWavelength::convert( const casa::Vector<double>& oldValues,
        casa::SpectralCoordinate spectralCoordinate ) {

    casa::Vector<double> resultValues(oldValues.size());
    resultValues = oldValues;

    bool unitsUnderstood = spectralCoordinate.setWavelengthUnit( newUnits.toStdString() );
    bool successfulConversion = false;
    if ( unitsUnderstood ) {

        successfulConversion = spectralCoordinate.frequencyToWavelength( resultValues, oldValues );

        if ( successfulConversion ) {
            casa::Vector<casa::String> spectralUnits = spectralCoordinate.worldAxisUnits();
            casa::String spectralUnit = spectralUnits[0];
            QString spectralUnitStr( spectralUnit.c_str() );
            if ( spectralUnitStr != oldUnits ) {
                ConverterFrequency::convertFrequency( resultValues, spectralUnitStr,
                        oldUnits, spectralCoordinate );
            }
        }
    }
    if ( !successfulConversion ) {
        qDebug() << "Could not convert frequency to wavelength";
    }
    return resultValues;
}

ConverterFrequencyWavelength::~ConverterFrequencyWavelength() {
    // TODO Auto-generated destructor stub
}
