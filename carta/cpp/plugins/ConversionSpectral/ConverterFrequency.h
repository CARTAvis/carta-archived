#pragma once

#include <Converter.h>

class ConverterFrequency : public Converter {
public:
    ConverterFrequency(const QString& oldUnits, const QString& newUnits);
    static void convertFrequency( casa::Vector<double> &resultValues,
            QString& frequencySourceUnits, QString& frequencyDestUnits,
            casa::SpectralCoordinate& spectralCoordinate );
    virtual double toPixel( double value, casa::SpectralCoordinate spectralCoordinate);
    virtual casa::Vector<double> convert( const casa::Vector<double>& oldValues,
            casa::SpectralCoordinate spectralCoordinate)  Q_DECL_OVERRIDE;
    virtual ~ConverterFrequency();
};
