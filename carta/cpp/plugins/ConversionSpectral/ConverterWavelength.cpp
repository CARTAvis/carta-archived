#include "ConverterWavelength.h"
#include <QDebug>

ConverterWavelength::ConverterWavelength(const QString& oldUnits,const QString& newUnits ) :
Converter( oldUnits, newUnits ) {

}

double ConverterWavelength::toPixel( double value, casa::SpectralCoordinate spectralCoordinate) {
    spectralCoordinate.setWavelengthUnit( oldUnits.toStdString() );
    casa::Vector<casa::Double> frequencyVector(1);
    casa::Vector<casa::Double> wavelengthVector(1);
    wavelengthVector[0] = value;
    spectralCoordinate.wavelengthToFrequency(frequencyVector, wavelengthVector );
    casa::Double pixelValue;
    spectralCoordinate.toPixel( pixelValue, frequencyVector[0]);
    return pixelValue;
}

casa::Vector<double> ConverterWavelength::convert( const casa::Vector<double>& oldValues,
        casa::SpectralCoordinate spectralCoordinate) {
    casa::Vector<double> resultValues( oldValues.size() );
    resultValues = oldValues;
    convertWavelength( resultValues, oldUnits, newUnits, spectralCoordinate );
    return resultValues;
}

void ConverterWavelength::convertWavelength( casa::Vector<double> &resultValues,
        QString& sourceUnits, QString& destUnits, casa::SpectralCoordinate& coord) {
    int sourceIndex = Converter::WAVELENGTH_UNITS.indexOf( sourceUnits );
    int destIndex = Converter::WAVELENGTH_UNITS.indexOf( destUnits );
    Converter::convert( resultValues, sourceIndex, destIndex, coord );
}


ConverterWavelength::~ConverterWavelength() {
    // TODO Auto-generated destructor stub
}
