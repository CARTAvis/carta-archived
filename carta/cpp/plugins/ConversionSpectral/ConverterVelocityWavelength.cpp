
#include "ConverterVelocityWavelength.h"
#include <QDebug>

ConverterVelocityWavelength::ConverterVelocityWavelength(const QString& oldUnits,const QString& newUnits) :
ConverterVelocity( oldUnits, newUnits ) {
}

casacore::Vector<double> ConverterVelocityWavelength::convert( const casacore::Vector<double>& oldValues,
        casacore::SpectralCoordinate spectralCoordinate ) {
    casacore::Vector<double> resultValues( oldValues.size());
    bool velocitySet = spectralCoordinate.setVelocity( oldUnits.toStdString() );
    bool wavelengthSet = spectralCoordinate.setWavelengthUnit( newUnits.toStdString() );
    bool successfulConversion = false;
    if ( velocitySet && wavelengthSet ) {
        casacore::Vector<double> frequencyValues( oldValues.size());
        successfulConversion = spectralCoordinate.velocityToFrequency( frequencyValues, oldValues );
        if ( successfulConversion ) {
            successfulConversion = spectralCoordinate.frequencyToWavelength( resultValues , frequencyValues );
        }
    }
    if ( !successfulConversion ) {
        resultValues = oldValues;
        qDebug() << "Could not convert velocity to wavelength";
    }
    return resultValues;
}
ConverterVelocityWavelength::~ConverterVelocityWavelength() {
    // TODO Auto-generated destructor stub
}
