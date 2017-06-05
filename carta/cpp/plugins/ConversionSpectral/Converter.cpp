#include "Converter.h"

#include <ConverterFrequencyVelocity.h>
#include <ConverterFrequencyWavelength.h>
#include <ConverterVelocityFrequency.h>
#include <ConverterVelocityWavelength.h>
#include <ConverterWavelengthFrequency.h>
#include <ConverterWavelengthVelocity.h>
#include <ConverterFrequency.h>
#include <ConverterVelocity.h>
#include <ConverterWavelength.h>
#include <ConverterChannel.h>
#include <QDebug>

const QList<QString> Converter::FREQUENCY_UNITS =
        QList<QString>() << "Hz" << "10Hz"<<"100Hz"<< "KHz" <<
        "10KHz" << "100KHz" << "MHz" <<
        "10MHz" << "100MHz" << "GHz";
const QList<QString> Converter::WAVELENGTH_UNITS =
        QList<QString>() << "Angstrom" << "nm" << "10nm" << "100nm" << "um" <<
        "10um" << "100um" << "mm" << "cm" << "dm"<<"m";
const QList<QString> Converter::VELOCITY_UNITS =
        QList<QString>() << "m/s" << "10m/s" << "100m/s" << "km/s";


Converter* Converter::getConverter( const QString& oldUnits,
        const QString& newUnits) {
    Converter* converter = NULL;
    UnitType sourceUnitType = getUnitType( oldUnits );
    UnitType destUnitType = getUnitType( newUnits );
    if ( sourceUnitType == FREQUENCY_UNIT ) {
        if ( destUnitType == WAVELENGTH_UNIT || newUnits.isEmpty() ) {
            converter = new ConverterFrequencyWavelength( oldUnits, newUnits);
        } else if ( destUnitType == VELOCITY_UNIT ) {
            converter = new ConverterFrequencyVelocity( oldUnits, newUnits );
        } else if ( destUnitType == FREQUENCY_UNIT ) {
            converter = new ConverterFrequency( oldUnits, newUnits );
        }
    } else if ( sourceUnitType == VELOCITY_UNIT ) {
        if ( destUnitType == WAVELENGTH_UNIT || newUnits.isEmpty()) {
            converter = new ConverterVelocityWavelength( oldUnits, newUnits);
        } else if ( destUnitType == VELOCITY_UNIT ) {
            converter = new ConverterVelocity( oldUnits, newUnits );
        } else if ( destUnitType == FREQUENCY_UNIT ) {
            converter = new ConverterVelocityFrequency( oldUnits, newUnits );
        }
    } else if ( sourceUnitType == WAVELENGTH_UNIT ) {
        if ( destUnitType == WAVELENGTH_UNIT ) {
            converter = new ConverterWavelength( oldUnits, newUnits );
        } else if ( destUnitType == VELOCITY_UNIT || newUnits.isEmpty() ) {
            converter = new ConverterWavelengthVelocity( oldUnits, newUnits );
        } else if ( destUnitType == FREQUENCY_UNIT ) {
            converter = new ConverterWavelengthFrequency( oldUnits, newUnits );
        }
    } else if ( sourceUnitType == CHANNEL_UNIT ) {
        converter = new ConverterChannel( oldUnits, newUnits );
    }
    return converter;
}

Converter::UnitType Converter::getUnitType( const QString& unit ) {
    UnitType unitType = UNRECOGNIZED;
    if ( FREQUENCY_UNITS.contains( unit )) {
        unitType = FREQUENCY_UNIT;
    } else if ( WAVELENGTH_UNITS.contains( unit )) {
        unitType = WAVELENGTH_UNIT;
    } else if ( VELOCITY_UNITS.contains( unit )) {
        unitType = VELOCITY_UNIT;
    } else if ( unit.isEmpty() || unit == "pixel" || unit == "Channel" ) {
        unitType = CHANNEL_UNIT;
    }
    return unitType;
}

Converter::Converter( const QString& oldUnitsStr, const QString& newUnitsStr):
		        oldUnits( oldUnitsStr), newUnits( newUnitsStr ) {
}


double Converter::convert ( double oldValue, casacore::SpectralCoordinate spectralCoordinate ) {
    std::vector<double> sourceValues( 1 );
    sourceValues[0] = oldValue;
    casacore::Vector<double> destValues = convert( sourceValues, spectralCoordinate );
    double result = destValues[0];
    return result;
}

void Converter::convert( casacore::Vector<double> &resultValues, int sourceIndex,
        int destIndex, casacore::SpectralCoordinate /*spectralCoordinate*/) {
    if ( sourceIndex >= 0 && destIndex >= 0 ) {
        int diff = qAbs( destIndex - sourceIndex );
        float power = pow( 10, diff );
        if ( destIndex > sourceIndex ) {
            power = 1 / power;
        }
        for ( int i = 0; i < static_cast<int>(resultValues.size()); i++ ) {
            resultValues[i] = resultValues[i] * power;
        }
    } else {
        /*qDebug() <<  "Converter: could not convert sourceIndex=" <<
						sourceIndex << " destIndex=" << destIndex;*/
    }
}


Converter::~Converter() {
}
