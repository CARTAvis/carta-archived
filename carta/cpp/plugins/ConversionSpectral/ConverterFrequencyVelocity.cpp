
#include "ConverterFrequencyVelocity.h"
#include <QDebug>

ConverterFrequencyVelocity::ConverterFrequencyVelocity(const QString& oldUnits,
        const QString& newUnits):
        ConverterFrequency( oldUnits, newUnits) {
}


casacore::Vector<double> ConverterFrequencyVelocity::convert( const casacore::Vector<double>& oldValues,
        casacore::SpectralCoordinate spectralCoordinate ) {
    casacore::Vector<double> frequencyValues(oldValues.size());
    frequencyValues = oldValues;

    //Find out the frequency units the spectral coordinate is using and
    //compare them to the frequency units we are using.  Transform the
    //data if necessary to the units used by the spectral coordinate.
    casacore::Vector<casacore::String> spectralUnits = spectralCoordinate.worldAxisUnits();
    casacore::String spectralUnit = spectralUnits[0];
    QString spectralUnitStr( spectralUnit.c_str() );
    if ( spectralUnitStr != oldUnits ) {
        ConverterFrequency::convertFrequency( frequencyValues, oldUnits, spectralUnitStr, spectralCoordinate );
    }
    bool unitsUnderstood = spectralCoordinate.setVelocity( newUnits.toStdString() );
    bool successfulConversion = false;
    int dataCount = oldValues.size();
    casacore::Vector<double> resultValues( dataCount );
    if ( unitsUnderstood ) {
        successfulConversion = spectralCoordinate.frequencyToVelocity( resultValues, frequencyValues );
    }
    if ( !successfulConversion ) {
        qDebug() << "Could not convert frequency to velocity";
    }
    return resultValues;
}

ConverterFrequencyVelocity::~ConverterFrequencyVelocity() {
    // TODO Auto-generated destructor stub
}

