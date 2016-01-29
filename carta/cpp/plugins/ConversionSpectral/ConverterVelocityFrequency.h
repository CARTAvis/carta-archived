#pragma once

#include <ConverterVelocity.h>

class ConverterVelocityFrequency : public ConverterVelocity {
public:
    ConverterVelocityFrequency(const QString& oldUnits,const QString& newUnits);
    virtual casa::Vector<double> convert( const casa::Vector<double>& oldValues,
            casa::SpectralCoordinate spectralCoordinate )  Q_DECL_OVERRIDE;
    virtual ~ConverterVelocityFrequency();

private:
    void convertFrequency( casa::Vector<double> &resultValues, QString& frequencyUnits,
            casa::SpectralCoordinate& coord);
};
