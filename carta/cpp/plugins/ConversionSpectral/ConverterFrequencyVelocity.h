#pragma once

#include <ConverterFrequency.h>

class ConverterFrequencyVelocity : public ConverterFrequency {
public:
    ConverterFrequencyVelocity(const QString& oldUnits, const QString& newUnits);
    virtual casa::Vector<double> convert( const casa::Vector<double>& oldValues,
            casa::SpectralCoordinate spectralCoordinate )  Q_DECL_OVERRIDE;
    virtual ~ConverterFrequencyVelocity();
};
