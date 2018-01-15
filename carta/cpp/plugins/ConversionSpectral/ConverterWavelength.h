#pragma once

#include <Converter.h>

class ConverterWavelength : public Converter {
public:
    ConverterWavelength(const QString& oldUnits,const QString& newUnits);
    virtual double toPixel( double value, casacore::SpectralCoordinate spectralCoordinate ) Q_DECL_OVERRIDE;
    static void convertWavelength( casacore::Vector<double> &resultValues,
            QString& sourceUnits, QString& destUnits, casacore::SpectralCoordinate& coord);
    virtual casacore::Vector<double> convert( const casacore::Vector<double>& oldValues,
            casacore::SpectralCoordinate spectralCoordinate)  Q_DECL_OVERRIDE;
    virtual ~ConverterWavelength();
};
