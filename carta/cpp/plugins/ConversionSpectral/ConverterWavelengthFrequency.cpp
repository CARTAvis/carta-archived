#include "ConverterWavelengthFrequency.h"
#include <ConverterFrequency.h>
#include <QDebug>
#include <assert.h>

ConverterWavelengthFrequency::ConverterWavelengthFrequency(const QString& oldUnits,const QString& newUnits) :
ConverterWavelength( oldUnits, newUnits ) {
}

casa::Vector<double> ConverterWavelengthFrequency::convert( const casa::Vector<double>& oldValues,
        casa::SpectralCoordinate spectralCoordinate ) {
    casa::Vector<double> resultValues( oldValues.size());
    bool wavelengthRecognized = spectralCoordinate.setWavelengthUnit( oldUnits.toStdString() );
    bool successfulConversion = false;
    if ( wavelengthRecognized ) {
        successfulConversion = spectralCoordinate.wavelengthToFrequency( resultValues, oldValues );
        if ( successfulConversion ) {
            casa::Vector<casa::String> coordUnits = spectralCoordinate.worldAxisUnits();
            assert ( coordUnits.size() == 1 );
            casa::String coordUnit = coordUnits[0];
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
