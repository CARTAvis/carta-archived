#pragma once

#include <Converter.h>

//Purpose of this class is to convert from channels= pixels to the world units/pixels
//used by the spectral axis.
class ConverterChannel : public Converter {
public:
    ConverterChannel( const QString& oldUnits, const QString& newUnits );

    virtual double toPixel( double value, casacore::SpectralCoordinate spectralCoordinate ) Q_DECL_OVERRIDE;
    virtual casacore::Vector<double> convert( const casacore::Vector<double>& oldValues,
            casacore::SpectralCoordinate spectralCoordinate )  Q_DECL_OVERRIDE;
    virtual double convert ( double oldValue, casacore::SpectralCoordinate spectralCoordinate ) Q_DECL_OVERRIDE;
    virtual ~ConverterChannel();
};
