#include "ConverterFrequency.h"
#include <QDebug>

ConverterFrequency::ConverterFrequency(const QString& oldUnits, const QString& newUnits) :
Converter( oldUnits, newUnits ) {
}

double ConverterFrequency::toPixel( double value, casa::SpectralCoordinate spectralCoordinate) {
    casa::Double pixelValue;
    casa::Vector<casa::String> spectralUnits = spectralCoordinate.worldAxisUnits();
    casa::String spectralUnit = spectralUnits[0];
    QString spectralUnitStr( spectralUnit.c_str());
    if ( spectralUnitStr != oldUnits ) {
        casa::Vector<double> freqValues(1);
        freqValues[0] = value;
        convertFrequency( freqValues, oldUnits, spectralUnitStr, spectralCoordinate );
        value = freqValues[0];
    }
    spectralCoordinate.toPixel( pixelValue, value );
    return pixelValue;
}

casa::Vector<double> ConverterFrequency::convert( const casa::Vector<double>& oldValues,
        casa::SpectralCoordinate spectralCoordinate) {
    casa::Vector<double> resultValues( oldValues.size() );
    resultValues = oldValues;
    convertFrequency( resultValues, oldUnits, newUnits, spectralCoordinate );
    return resultValues;
}

void ConverterFrequency::convertFrequency( casa::Vector<double> &resultValues,
        QString& frequencySourceUnits, QString& frequencyDestUnits,
        casa::SpectralCoordinate& coord ) {
    int sourceIndex = Converter::FREQUENCY_UNITS.indexOf( frequencySourceUnits );
    int destIndex = Converter::FREQUENCY_UNITS.indexOf( frequencyDestUnits );
    Converter::convert( resultValues, sourceIndex, destIndex, coord );
}

ConverterFrequency::~ConverterFrequency() {
    // TODO Auto-generated destructor stub
}
