#include "ConverterWavelengthFrequency.h"
#include <ConverterFrequency.h>
#include <QDebug>
#include <assert.h>

ConverterWavelengthFrequency::ConverterWavelengthFrequency(const QString& oldUnits,const QString& newUnits) :
ConverterWavelength( oldUnits, newUnits ) {
}

casacore::Vector<double> ConverterWavelengthFrequency::convert( const casacore::Vector<double>& oldValues,
        casacore::SpectralCoordinate spectralCoordinate ) {
    casacore::Vector<double> resultValues( oldValues.size());
    bool wavelengthRecognized = spectralCoordinate.setWavelengthUnit( oldUnits.toStdString() );
    bool successfulConversion = false;
    if ( wavelengthRecognized ) {
        successfulConversion = spectralCoordinate.wavelengthToFrequency( resultValues, oldValues );
        if ( successfulConversion ) {
            casacore::Vector<casacore::String> coordUnits = spectralCoordinate.worldAxisUnits();
            assert ( coordUnits.size() == 1 );
            casacore::String coordUnit = coordUnits[0];
            QString coordUnitStr( coordUnit.c_str());
            ConverterFrequency::convertFrequency( resultValues, coordUnitStr, newUnits, spectralCoordinate );
        }
    }
    if ( !successfulConversion ) {
        resultValues = oldValues;
        qDebug() << "Could not convert wavelength to frequency";
    }
    return resultValues;
}

ConverterWavelengthFrequency::~ConverterWavelengthFrequency() {
    // TODO Auto-generated destructor stub
}
