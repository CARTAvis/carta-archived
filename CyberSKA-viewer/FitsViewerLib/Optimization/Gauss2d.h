#ifndef GAUSS2D_H
#define GAUSS2D_H

#include <cmath>

namespace Optimization {

const double fwhm2int = 0.600561204393224897;
const double deg2rad = M_PI / 180.0;

// nice (named) gaussian 2d parameters
struct Gauss2dNiceParams {
    double amp, xc, yc, major, minor, angle;
    static Gauss2dNiceParams convert( const double * p);
};

/*
/// evaluate gauss2d function using parameters p, at position x,y
inline double evalGauss2di( int x, int y, const double * p)
{
    // symbolic values for g2d offsets
    enum { HEIGHT=0, XCENTER, YCENTER, YWIDTH, RATIO, PANGLE};

    double dx = x - p[XCENTER];
    double dy = y - p[YCENTER];

    double alpha = p[PANGLE] * deg2rad;
    double cosa = cos(alpha);
    double sina = sin(alpha);

    const double temp(dx);
    dx =   cosa*temp  + sina*dy;
    dy = - sina*temp  + cosa*dy;
    dx /= p[YWIDTH]*p[RATIO]*fwhm2int;
    dy /= p[YWIDTH]*fwhm2int;
    return p[HEIGHT]*exp(-(dx*dx + dy*dy));
}
*/

/// evaluate gauss2d function using parameters p, at position x,y
inline double evalGauss2dd( double x, double y, const double * p)
{
    // symbolic values for g2d offsets
    enum { HEIGHT=0, XCENTER, YCENTER, YWIDTH, RATIO, PANGLE};

    double dx = x - p[XCENTER];
    double dy = y - p[YCENTER];

    double alpha = p[PANGLE] * deg2rad;
    double cosa = cos(alpha);
    double sina = sin(alpha);

    const double temp(dx);
    dx =   cosa*temp  + sina*dy;
    dy = - sina*temp  + cosa*dy;
    dx /= p[YWIDTH]*p[RATIO]*fwhm2int;
    dy /= p[YWIDTH]*fwhm2int;
    return p[HEIGHT]*exp(-(dx*dx + dy*dy));
}

/// return sum of N gaussians
inline double evalNGauss2d( double x, double y, int nGaussians, const double * p)
{
    double sum = 0;
    for( int i = 0 ; i < nGaussians ; i ++ )
        sum += evalGauss2dd( x, y, & p[i*6]);

    return sum;
}

/// return sum of N gaussians + background term
/// if bkg = 0 -> no background, no extra parameters
/// if bkg = 1 -> constant background, 1 extra parameter
/// if bkg = 2 -> linear background, 3 extra parameters

inline double evalNGauss2dBkg( double x, double y, int nGaussians, int bkg, const double * p)
{
    double sum = evalNGauss2d( x, y, nGaussians, p);
    if( bkg == 0) {
    } else if( bkg == 1) {
        sum += p[nGaussians*6 + 0];
    } else if( bkg == 2) {
        sum += p[nGaussians*6 + 0]
                + x * p[nGaussians*6+1]
                + y * p[nGaussians*6+2];
    }

    return sum;
}



} // namespace Optimization

#endif // GAUSS2D_H
