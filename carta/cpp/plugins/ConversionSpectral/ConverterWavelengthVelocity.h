#pragma once

#include <ConverterWavelength.h>

class ConverterWavelengthVelocity : public ConverterWavelength {
public:
    ConverterWavelengthVelocity(const QString& oldUnits,const QString& newUnits);
    virtual casa::Vector<double> convert( const casa::Vector<double>& oldValues,
            casa::SpectralCoordinate spectralCoordinate ) Q_DECL_OVERRIDE;
    virtual ~ConverterWavelengthVelocity();
};

