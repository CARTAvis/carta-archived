#pragma once

#include <ConverterWavelength.h>

class ConverterWavelengthVelocity : public ConverterWavelength {
public:
    ConverterWavelengthVelocity(const QString& oldUnits,const QString& newUnits);
    virtual casacore::Vector<double> convert( const casacore::Vector<double>& oldValues,
            casacore::SpectralCoordinate spectralCoordinate ) Q_DECL_OVERRIDE;
    virtual ~ConverterWavelengthVelocity();
};

