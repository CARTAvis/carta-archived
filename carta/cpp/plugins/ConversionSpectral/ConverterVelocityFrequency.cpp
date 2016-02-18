#include "ConverterVelocityFrequency.h"
#include <QDebug>
#include <assert.h>

ConverterVelocityFrequency::ConverterVelocityFrequency(const QString& oldUnits,const QString& newUnits) :
ConverterVelocity( oldUnits, newUnits) {
}


void ConverterVelocityFrequency::convertFrequency( casa::Vector<double> &resultValues,
        QString& frequencySourceUnits, casa::SpectralCoordinate& coord ) {
    //Decide on the multiplier
    int sourceUnitIndex = FREQUENCY_UNITS.indexOf( frequencySourceUnits );
    int destUnitIndex = FREQUENCY_UNITS.indexOf( newUnits );
    Converter::convert( resultValues, sourceUnitIndex, destUnitIndex, coord );
}

casa::Vector<double> ConverterVelocityFrequency::convert( const casa::Vector<double>& oldValues,
        casa::SpectralCoordinate spectralCoordinate ) {
    bool unitsUnderstood = spectralCoordinate.setVelocity( oldUnits.toStdString() );
    casa::Vector<double> resultValues(oldValues.size());
    bool successfulConversion = false;
    if ( unitsUnderstood ) {
        successfulConversion = spectralCoordinate.velocityToFrequency( resultValues, oldValues );
        if ( successfulConversion ) {
            //The frequency unit will be whatever the spectralCoordinate is currently using.
            casa::Vector<casa::String> frequencyUnits = spectralCoordinate.worldAxisUnits();
            assert (frequencyUnits.size() == 1);
            casa::String frequencyUnit = frequencyUnits[0];
            QString freqUnitStr( frequencyUnit.c_str());
            //Now we convert it to appropriate units;
            convertFrequency( resultValues, freqUnitStr, spectralCoordinate );
        }
    }
    if ( !successfulConversion ) {
        resultValues = oldValues;
        qDebug() << "Could not convert velocity to frequency";
    }
    return resultValues;
}

ConverterVelocityFrequency::~ConverterVelocityFrequency() {
    // TODO Auto-generated destructor stub
}
