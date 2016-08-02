#include <iostream>
#include "Gauss1d.h"

namespace Optimization
{
const double SIGMA2FWHM = 2.354820045030949382023138652919399275494771378771641077045051;

// convert 3 parameters in an array to nice (named) parameters
Gauss1dNiceParams
Gauss1dNiceParams::convert( const double * p )
{
    Gauss1dNiceParams res;

    res.center = p[0];
    res.amplitude = p[1];
    res.fwhm = SIGMA2FWHM * sqrt( - 1 / ( 2 * p[2] ) );

    return res;
}

VD
Gauss1dNiceParams::convertToUgly() const
{
    VD res( 3 );
    res[0] = center;
    res[1] = amplitude;
    res[2] = - 0.5 / ( ( fwhm / SIGMA2FWHM ) * ( fwhm / SIGMA2FWHM ) );
    return res;
}
} // napespace Optimization
