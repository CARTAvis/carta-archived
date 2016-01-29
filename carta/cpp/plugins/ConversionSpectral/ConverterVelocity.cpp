#include "ConverterVelocity.h"
#include <QDebug>

ConverterVelocity::ConverterVelocity(const QString& oldUnits, const QString& newUnits) :
Converter( oldUnits, newUnits) {

}

double ConverterVelocity::toPixel( double value, casa::SpectralCoordinate spectralCoordinate ) {
    double pixelVal;
    spectralCoordinate.setVelocity( oldUnits.toStdString() );
    spectralCoordinate.velocityToPixel( pixelVal, value );
    return pixelVal;
}

casa::Vector<double> ConverterVelocity::convert( const casa::Vector<double>& oldValues,
        casa::SpectralCoordinate spectralCoordinate ) {
    casa::Vector<double> resultValues( oldValues.size() );
    resultValues = oldValues;
    convertVelocity( resultValues, oldUnits, newUnits, spectralCoordinate );
    return resultValues;
}

void ConverterVelocity::convertVelocity( casa::Vector<double> &resultValues,
        QString& sourceUnits, QString& destUnits, casa::SpectralCoordinate& coord ) {
    int sourceIndex = Converter::VELOCITY_UNITS.indexOf( sourceUnits );
    int destIndex = Converter::VELOCITY_UNITS.indexOf( destUnits );
    Converter::convert( resultValues, sourceIndex, destIndex, coord );
}


ConverterVelocity::~ConverterVelocity() {
    // TODO Auto-generated destructor stub
}
