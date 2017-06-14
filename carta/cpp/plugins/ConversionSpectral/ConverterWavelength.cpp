#include "ConverterWavelength.h"
#include <QDebug>

ConverterWavelength::ConverterWavelength(const QString& oldUnits,const QString& newUnits ) :
Converter( oldUnits, newUnits ) {

}

double ConverterWavelength::toPixel( double value, casacore::SpectralCoordinate spectralCoordinate) {
    spectralCoordinate.setWavelengthUnit( oldUnits.toStdString() );
    casacore::Vector<casacore::Double> frequencyVector(1);
    casacore::Vector<casacore::Double> wavelengthVector(1);
    wavelengthVector[0] = value;
    spectralCoordinate.wavelengthToFrequency(frequencyVector, wavelengthVector );
    casacore::Double pixelValue;
    spectralCoordinate.toPixel( pixelValue, frequencyVector[0]);
    return pixelValue;
}

casacore::Vector<double> ConverterWavelength::convert( const casacore::Vector<double>& oldValues,
        casacore::SpectralCoordinate spectralCoordinate) {
    casacore::Vector<double> resultValues( oldValues.size() );
    resultValues = oldValues;
    convertWavelength( resultValues, oldUnits, newUnits, spectralCoordinate );
    return resultValues;
}

void ConverterWavelength::convertWavelength( casacore::Vector<double> &resultValues,
        QString& sourceUnits, QString& destUnits, casacore::SpectralCoordinate& coord) {
    int sourceIndex = Converter::WAVELENGTH_UNITS.indexOf( sourceUnits );
    int destIndex = Converter::WAVELENGTH_UNITS.indexOf( destUnits );
    Converter::convert( resultValues, sourceIndex, destIndex, coord );
}


ConverterWavelength::~ConverterWavelength() {
    // TODO Auto-generated destructor stub
}
