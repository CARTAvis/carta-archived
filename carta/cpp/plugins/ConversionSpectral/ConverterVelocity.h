#pragma once

#include <Converter.h>

class ConverterVelocity : public Converter {
public:
    ConverterVelocity(const QString& oldUnits,const QString& newUnits);
    virtual double toPixel( double value, casa::SpectralCoordinate spectralCoordinate);
    static void convertVelocity( casa::Vector<double> &resultValues,
            QString& sourceUnits, QString& destUnits, casa::SpectralCoordinate& coord);
    virtual casa::Vector<double> convert( const casa::Vector<double>& oldValues,
            casa::SpectralCoordinate spectralCoordinate )  Q_DECL_OVERRIDE;
    virtual ~ConverterVelocity();
};
