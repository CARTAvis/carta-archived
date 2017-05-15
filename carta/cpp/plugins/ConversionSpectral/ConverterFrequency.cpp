#include "ConverterFrequency.h"
#include <QDebug>

ConverterFrequency::ConverterFrequency(const QString& oldUnits, const QString& newUnits) :
Converter( oldUnits, newUnits ) {
}

double ConverterFrequency::toPixel( double value, casacore::SpectralCoordinate spectralCoordinate) {
    casacore::Double pixelValue;
    casacore::Vector<casacore::String> spectralUnits = spectralCoordinate.worldAxisUnits();
    casacore::String spectralUnit = spectralUnits[0];
    QString spectralUnitStr( spectralUnit.c_str());
    if ( spectralUnitStr != oldUnits ) {
        casacore::Vector<double> freqValues(1);
        freqValues[0] = value;
        convertFrequency( freqValues, oldUnits, spectralUnitStr, spectralCoordinate );
        value = freqValues[0];
    }
    spectralCoordinate.toPixel( pixelValue, value );
    return pixelValue;
}

casacore::Vector<double> ConverterFrequency::convert( const casacore::Vector<double>& oldValues,
        casacore::SpectralCoordinate spectralCoordinate) {
    casacore::Vector<double> resultValues( oldValues.size() );
    resultValues = oldValues;
    convertFrequency( resultValues, oldUnits, newUnits, spectralCoordinate );
    return resultValues;
}

void ConverterFrequency::convertFrequency( casacore::Vector<double> &resultValues,
        QString& frequencySourceUnits, QString& frequencyDestUnits,
        casacore::SpectralCoordinate& coord ) {
    int sourceIndex = Converter::FREQUENCY_UNITS.indexOf( frequencySourceUnits );
    int destIndex = Converter::FREQUENCY_UNITS.indexOf( frequencyDestUnits );
    Converter::convert( resultValues, sourceIndex, destIndex, coord );
}

ConverterFrequency::~ConverterFrequency() {
    // TODO Auto-generated destructor stub
}
