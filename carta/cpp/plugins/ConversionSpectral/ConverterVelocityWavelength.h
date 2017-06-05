#pragma once

#include <ConverterVelocity.h>

class ConverterVelocityWavelength : public ConverterVelocity {
public:
    ConverterVelocityWavelength(const QString& oldUnits,
            const QString& newUnits );
    virtual casacore::Vector<double> convert( const casacore::Vector<double>& oldValues,
            casacore::SpectralCoordinate spectralCoordinate )  Q_DECL_OVERRIDE;
    virtual ~ConverterVelocityWavelength();
};

