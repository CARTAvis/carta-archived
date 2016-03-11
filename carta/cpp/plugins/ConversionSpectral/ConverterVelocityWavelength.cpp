
#include "ConverterVelocityWavelength.h"
#include <QDebug>

ConverterVelocityWavelength::ConverterVelocityWavelength(const QString& oldUnits,const QString& newUnits) :
ConverterVelocity( oldUnits, newUnits ) {
}

casa::Vector<double> ConverterVelocityWavelength::convert( const casa::Vector<double>& oldValues,
        casa::SpectralCoordinate spectralCoordinate ) {
    casa::Vector<double> resultValues( oldValues.size());
    bool velocitySet = spectralCoordinate.setVelocity( oldUnits.toStdString() );
    bool wavelengthSet = spectralCoordinate.setWavelengthUnit( newUnits.toStdString() );
    bool successfulConversion = false;
    if ( velocitySet && wavelengthSet ) {
        casa::Vector<double> frequencyValues( oldValues.size());
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
