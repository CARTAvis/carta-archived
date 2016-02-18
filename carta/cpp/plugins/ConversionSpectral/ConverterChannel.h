#pragma once

#include <Converter.h>

//Purpose of this class is to convert from channels= pixels to the world units/pixels
//used by the spectral axis.
class ConverterChannel : public Converter {
public:
    ConverterChannel( const QString& oldUnits, const QString& newUnits );

    virtual double toPixel( double value, casa::SpectralCoordinate spectralCoordinate );
    virtual casa::Vector<double> convert( const casa::Vector<double>& oldValues,
            casa::SpectralCoordinate spectralCoordinate )  Q_DECL_OVERRIDE;
    virtual double convert ( double oldValue, casa::SpectralCoordinate spectralCoordinate );
    virtual ~ConverterChannel();
};
