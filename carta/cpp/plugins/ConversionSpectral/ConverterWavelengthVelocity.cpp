#include "ConverterWavelengthVelocity.h"
#include <QDebug>

ConverterWavelengthVelocity::ConverterWavelengthVelocity(const QString& oldUnits,
        const QString& newUnits) :
        ConverterWavelength( oldUnits, newUnits) {
}

casacore::Vector<double> ConverterWavelengthVelocity::convert( const casacore::Vector<double>& oldValues,
        casacore::SpectralCoordinate spectralCoordinate ) {
    casacore::Vector<double> resultValues( oldValues.size());

    bool validWavelength = spectralCoordinate.setWavelengthUnit( oldUnits.toStdString() );
    bool validVelocity = spectralCoordinate.setVelocity( newUnits.toStdString() );

    bool successfulConversion = false;
    if ( validWavelength && validVelocity ) {
        casacore::Vector<double> frequencyValues( oldValues.size());
        successfulConversion = spectralCoordinate.wavelengthToFrequency( frequencyValues,oldValues);
        if ( successfulConversion ) {
            successfulConversion = spectralCoordinate.frequencyToVelocity( resultValues, frequencyValues);
        }
    }
    if ( !successfulConversion ) {
        resultValues = oldValues;
        qDebug() << "Could not convert wavelength to velocity";
    }
    return resultValues;
}
ConverterWavelengthVelocity::~ConverterWavelengthVelocity() {
    // TODO Auto-generated destructor stub
}
