#pragma once

#include <ConverterVelocity.h>

class ConverterVelocityFrequency : public ConverterVelocity {
public:
    ConverterVelocityFrequency(const QString& oldUnits,const QString& newUnits);
    virtual casacore::Vector<double> convert( const casacore::Vector<double>& oldValues,
            casacore::SpectralCoordinate spectralCoordinate )  Q_DECL_OVERRIDE;
    virtual ~ConverterVelocityFrequency();

private:
    void convertFrequency( casacore::Vector<double> &resultValues, QString& frequencyUnits,
            casacore::SpectralCoordinate& coord);
};
