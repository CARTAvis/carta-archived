#include "ConverterVelocity.h"
#include <QDebug>

ConverterVelocity::ConverterVelocity(const QString& oldUnits, const QString& newUnits) :
Converter( oldUnits, newUnits) {

}

double ConverterVelocity::toPixel( double value, casacore::SpectralCoordinate spectralCoordinate ) {
    double pixelVal;
    spectralCoordinate.setVelocity( oldUnits.toStdString() );
    spectralCoordinate.velocityToPixel( pixelVal, value );
    return pixelVal;
}

casacore::Vector<double> ConverterVelocity::convert( const casacore::Vector<double>& oldValues,
        casacore::SpectralCoordinate spectralCoordinate ) {
    casacore::Vector<double> resultValues( oldValues.size() );
    resultValues = oldValues;
    convertVelocity( resultValues, oldUnits, newUnits, spectralCoordinate );
    return resultValues;
}

void ConverterVelocity::convertVelocity( casacore::Vector<double> &resultValues,
        QString& sourceUnits, QString& destUnits, casacore::SpectralCoordinate& coord ) {
    int sourceIndex = Converter::VELOCITY_UNITS.indexOf( sourceUnits );
    int destIndex = Converter::VELOCITY_UNITS.indexOf( destUnits );
    Converter::convert( resultValues, sourceIndex, destIndex, coord );
}


ConverterVelocity::~ConverterVelocity() {
    // TODO Auto-generated destructor stub
}
