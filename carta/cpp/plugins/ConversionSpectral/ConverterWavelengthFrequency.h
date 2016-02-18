#pragma once

#include <ConverterWavelength.h>

class ConverterWavelengthFrequency : public ConverterWavelength {
public:
    ConverterWavelengthFrequency(const QString& oldUnits,
            const QString& newUnits );
    virtual casa::Vector<double> convert( const casa::Vector<double>& oldValues,
            casa::SpectralCoordinate spectralCoordinate)  Q_DECL_OVERRIDE;
    virtual ~ConverterWavelengthFrequency();
};

