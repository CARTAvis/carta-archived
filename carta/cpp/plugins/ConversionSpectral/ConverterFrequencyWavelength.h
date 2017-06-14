#pragma once

#include <ConverterFrequency.h>

class ConverterFrequencyWavelength : public ConverterFrequency {
public:
    ConverterFrequencyWavelength(const QString& oldUnits,
            const QString& newUnits);
    virtual casacore::Vector<double> convert( const casacore::Vector<double>& oldValues,
            casacore::SpectralCoordinate spectralCoordinate )  Q_DECL_OVERRIDE;
    virtual ~ConverterFrequencyWavelength();
};
