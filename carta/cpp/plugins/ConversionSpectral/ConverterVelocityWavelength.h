#pragma once

#include <ConverterVelocity.h>

class ConverterVelocityWavelength : public ConverterVelocity {
public:
    ConverterVelocityWavelength(const QString& oldUnits,
            const QString& newUnits );
    virtual casa::Vector<double> convert( const casa::Vector<double>& oldValues,
            casa::SpectralCoordinate spectralCoordinate )  Q_DECL_OVERRIDE;
    virtual ~ConverterVelocityWavelength();
};

