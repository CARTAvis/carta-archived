#pragma once

#include <Converter.h>

class ConverterWavelength : public Converter {
public:
    ConverterWavelength(const QString& oldUnits,const QString& newUnits);
    virtual double toPixel( double value, casa::SpectralCoordinate spectralCoordinate );
    static void convertWavelength( casa::Vector<double> &resultValues,
            QString& sourceUnits, QString& destUnits, casa::SpectralCoordinate& coord);
    virtual casa::Vector<double> convert( const casa::Vector<double>& oldValues,
            casa::SpectralCoordinate spectralCoordinate)  Q_DECL_OVERRIDE;
    virtual ~ConverterWavelength();
};

