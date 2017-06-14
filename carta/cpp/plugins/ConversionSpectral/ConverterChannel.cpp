#include "ConverterChannel.h"
#include <QDebug>

ConverterChannel::ConverterChannel(const QString& oldUnits, const QString& newUnits) :
Converter( oldUnits, newUnits) {

    //Old units will be channels.
    //New units could be anything.

}


double ConverterChannel::toPixel( double value, casacore::SpectralCoordinate spectralCoordinate ) {
    casacore::Double pixelValue;
    spectralCoordinate.toPixel( pixelValue, value );
    return pixelValue;
}

casacore::Vector<double> ConverterChannel::convert( const casacore::Vector<double>& oldValues,
        casacore::SpectralCoordinate spectralCoordinate ) {
    std::vector<double> resultValues( oldValues.size());
    for ( int i = 0; i < static_cast<int>(resultValues.size()); i++ ) {
        double result;
        bool correct = spectralCoordinate.toWorld( result, oldValues[i]);
        if ( correct ) {
            casacore::Vector<casacore::String> worldUnitsVector = spectralCoordinate.worldAxisUnits();
            QString worldUnit(worldUnitsVector[0].c_str());
            if ( worldUnit == newUnits ) {
                resultValues[i] = result;
            }
            else {
                Converter* helper = Converter::getConverter( worldUnit, newUnits);
                if ( helper != nullptr ){
                    resultValues[i] = helper->convert( result, spectralCoordinate );
                    delete helper;
                }
                else {
                    qDebug() << "Could not convert from "<<worldUnit<<" to "<<newUnits;
                }
            }
        }
        else {
            qDebug() << "Could not convert channel="<<oldValues[i];
        }
    }
    return resultValues;
}

double ConverterChannel::convert ( double oldValue, casacore::SpectralCoordinate spectralCoordinate ) {
    std::vector<double> oldValues(1);
    oldValues[0] = oldValue;
    casacore::Vector<double> newValues = convert( oldValues, spectralCoordinate );
    return newValues[0];
}

ConverterChannel::~ConverterChannel() {
    // TODO Auto-generated destructor stub
}
