#pragma once

#include <Converter.h>

class ConverterVelocity : public Converter {
public:
    ConverterVelocity(const QString& oldUnits,const QString& newUnits);
    virtual double toPixel( double value, casacore::SpectralCoordinate spectralCoordinate) Q_DECL_OVERRIDE;
    static void convertVelocity( casacore::Vector<double> &resultValues,
            QString& sourceUnits, QString& destUnits, casacore::SpectralCoordinate& coord);
    virtual casacore::Vector<double> convert( const casacore::Vector<double>& oldValues,
            casacore::SpectralCoordinate spectralCoordinate )  Q_DECL_OVERRIDE;
    virtual ~ConverterVelocity();
};
