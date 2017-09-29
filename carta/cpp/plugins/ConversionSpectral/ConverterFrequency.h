#pragma once

#include <Converter.h>

class ConverterFrequency : public Converter {
public:
    ConverterFrequency(const QString& oldUnits, const QString& newUnits);
    static void convertFrequency( casacore::Vector<double> &resultValues,
            QString& frequencySourceUnits, QString& frequencyDestUnits,
            casacore::SpectralCoordinate& spectralCoordinate );
    virtual double toPixel( double value, casacore::SpectralCoordinate spectralCoordinate) Q_DECL_OVERRIDE;
    virtual casacore::Vector<double> convert( const casacore::Vector<double>& oldValues,
            casacore::SpectralCoordinate spectralCoordinate)  Q_DECL_OVERRIDE;
    virtual ~ConverterFrequency();
};
