/**
 *
 **/

#include "Colormaps.h"

namespace Carta
{
namespace Core
{
PWLinear &
PWLinear::add( double x, double y )
{
    points_.push_back( QPointF( x, y ) );
    return * this; // allow chaining
}

double
PWLinear::operator() ( double x )
{
    if ( ! std::isfinite( x ) || points_.empty() ) {
        return std::numeric_limits < double >::quiet_NaN();
    }

    // test boundary conditions
    if ( x <= points_.first().x() ) {
        return points_.first().y();
    }
    if ( x >= points_.last().x() ) {
        return points_.last().y();
    }

    // find the segment and interpolate within it
    for ( int i = 1 ; i < points_.size() ; i++ ) {
        if ( x <= points_[i].x() ) {
            double a =
                ( points_[i - 1].y() - points_[i].y() ) / ( points_[i - 1].x() - points_[i].x() );
            double b = points_[i].y() - a * points_[i].x();
            return a * x + b;
        }
    }
    return std::numeric_limits < double >::quiet_NaN();
} // ()

ColormapFunction::ColormapFunction()
{ }

ColormapFunction::ColormapFunction( PWLinear & red, PWLinear & green, PWLinear & blue )
{
    red_   = red;
    green_ = green;
    blue_  = blue;
}

Rgb
ColormapFunction::operator() ( double x )
{
    if ( ! std::isfinite( x ) ) {
        return 0;
    }
    return qRgb( 255 * red_( x ), 255 * green_( x ), 255 * blue_( x ) );
}

HistogramColormapFunctor::HistogramColormapFunctor()
{
    histogramFunction_ = PWLinear().add( 0, 0.0 ).add( 1, 1.0 );
    colormapFunction_  = ColormapFunction::gray();
}

// combines histogram & colormap functions into one
HistogramColormapFunctor::HistogramColormapFunctor( double histMin,
                                                    double histMax,
                                                    ColormapFunction cmap )
{
    histogramFunction_ = PWLinear().add( histMin, 0.0 ).add( histMax, 1.0 );
    colormapFunction_  = cmap;
}

Rgb
HistogramColormapFunctor::operator() ( double x )
{
    return colormapFunction_( histogramFunction_( x ) );
}

// PREDEFINED COLORMAPS
// --------------------

ColormapFunction
ColormapFunction::gray()
{
    static ColormapFunction map;
    static bool initialized = false;
    if ( ! initialized ) {
        initialized = true;
        map         = ColormapFunction(
            PWLinear().add( 0, 0 ).add( 1, 1 ),
            PWLinear().add( 0, 0 ).add( 1, 1 ),
            PWLinear().add( 0, 0 ).add( 1, 1 )
            );
    }
    return map;
}

ColormapFunction
ColormapFunction::heat()
{
    static ColormapFunction map;
    static bool initialized = false;
    if ( ! initialized ) {
        initialized = true;
        map         = ColormapFunction(
            PWLinear().add( 0, 0 ).add( 1.0 / 3, 1 ),
            PWLinear().add( 0, 0 ).add( 1, 1 ),
            PWLinear().add( 2.0 / 3, 0 ).add( 1, 1 )
            );
    }
    return map;
}

ColormapFunction
ColormapFunction::fire()
{
    static ColormapFunction map;
    static bool initialized = false;
    if ( ! initialized ) {
        initialized = true;
        map         = ColormapFunction(
            PWLinear().add( 0, 0 ).add( 1.0 / 2, 1 ),
            PWLinear().add( 1.0 / 4, 0 ).add( 3.0 / 4, 1 ),
            PWLinear().add( 1.0 / 2, 0 ).add( 1, 1 )
            );
    }
    return map;
}

ColormapFunction
ColormapFunction::spring()
{
    static ColormapFunction map;
    static bool initialized = false;
    if ( ! initialized ) {
        initialized = true;
        map         = ColormapFunction(
            PWLinear().add( 0.25, 0 ).add( 0.75, 1 ),
            PWLinear().add( 0, 0 ).add( 0.5, 1 ),
            PWLinear().add( 0.5, 0 ).add( 1, 1 )
            );
    }
    return map;
}

ColormapFunction
ColormapFunction::sea()
{
    static ColormapFunction map;
    static bool initialized = false;
    if ( ! initialized ) {
        initialized = true;
        map         = ColormapFunction(
            PWLinear().add( 0.5, 0 ).add( 1, 1 ),
            PWLinear().add( 0, 0 ).add( 0.5, 1 ),
            PWLinear().add( 0.25, 0 ).add( 0.75, 1 )
            );
    }
    return map;
}

ColormapFunction
ColormapFunction::sunbow()
{
    static ColormapFunction map;
    static bool initialized = false;
    if ( ! initialized ) {
        initialized = true;
        map         = ColormapFunction(
            PWLinear().add( 0.5, 1 ).add( 0.75, 0 ),
            PWLinear().add( 0, 0 ).add( 0.25, 1 ).add( 0.75, 1 ).add( 1, 0 ),
            PWLinear().add( 0.25, 0 ).add( 0.5, 1 )
            );
    }
    return map;
}

ColormapFunction
ColormapFunction::mutant()
{
    static ColormapFunction map;
    static bool initialized = false;
    if ( ! initialized ) {
        initialized = true;
        map         = ColormapFunction(
            PWLinear().add( 0, 0 ).add( 0.25, 1 ).add( 0.75, 1 ).add( 1, 0 ),
            PWLinear().add( 0.5, 1 ).add( 0.75, 0 ),
            PWLinear().add( 0.25, 0 ).add( 0.5, 1 )
            );
    }
    return map;
}

ColormapFunction
ColormapFunction::aberration()
{
    static ColormapFunction map;
    static bool initialized = false;
    if ( ! initialized ) {
        initialized = true;
        map         = ColormapFunction(
            PWLinear().add( 0.5, 0 ).add( 0.75, 1 ),
            PWLinear().add( 0.25, 0 ).add( 0.5, 1 ),
            PWLinear().add( 0, 0 ).add( 0.25, 1 ).add( 0.75, 1 ).add( 1, 0 )
            );
    }
    return map;
}

ColormapFunction
ColormapFunction::rgb()
{
    static ColormapFunction map;
    static bool initialized = false;
    if ( ! initialized ) {
        initialized = true;
        map         = ColormapFunction(
            PWLinear().add( 0, 0 ).add( 0.25, 1 ).add( 0.5, 0 ),
            PWLinear().add( 0.25, 0 ).add( 0.5, 1 ).add( 0.75, 0 ),
            PWLinear().add( 0.5, 0 ).add( 0.75, 1 ).add( 1, 0 )
            );
    }
    return map;
}

ColormapFunction
ColormapFunction::velocity()
{
    static ColormapFunction map;
    static bool initialized = false;
    if ( ! initialized ) {
        initialized = true;
        map         = ColormapFunction(
            PWLinear().add( 0.5, 0 ).add( 1, 1 ),
            PWLinear().add( 0, 0 ).add( 0.5, 1 ).add( 1, 0 ),
            PWLinear().add( 0, 1 ).add( 0.5, 0 )
            );
    }
    return map;
}

CachedRgbFunction::CachedRgbFunction()
{
    init( HistogramColormapFunctor( 0, 1, ColormapFunction::heat() ), 0, 1, 10000, 0 );
}

void
CachedRgbFunction::init(
    HistogramColormapFunctor fn,
    double min,
    double max,
    int n,
    Rgb nanColor )
{
    min_      = min;
    max_      = max;
    nanColor_ = nanColor;

    cache.resize( n );
    diff   = max - min;
    n1     = n - 1;
    dInvN1 = n1 / diff;
    double x;
    int i;
    double delta = diff / n;
    for ( i = 0, x = min ; i < n ; i++, x += delta ) {
        cache[i] = fn( x );
    }
    if ( diff <= 0 ) {
        minRes = fn( min - 1e-9 );
        maxRes = fn( max + 1e-9 );
    }
    else {
        minRes = cache.first();
        maxRes = cache.last();
    }
} // init

CachedRgbFunction::CachedRgbFunction( HistogramColormapFunctor fn,
                                      double min,
                                      double max,
                                      int n,
                                      Rgb nanColor )
{
    init( fn, min, max, n, nanColor );
}

// cube helix function
// as described here: http://www.mrao.cam.ac.uk/~dag/CUBEHELIX/
// and in
//  Green, D. A., 2011, Bulletin of the Astronomical Society of India, Vol.39, p.289
ColormapFunction
ColormapFunction::cubeHelix( double start, double rots, double hue, double gamma )
{
    PWLinear red, green, blue;

    int nlev = 1000;
    for ( int i = 0 ; i < nlev ; i++ ) {
        double fract = double (i) / nlev;
        double angle = 2 * M_PI * ( start / 3 + 1 + rots + fract );
        fract = pow( fract, gamma );

        double amp = hue * fract * ( 1 - fract ) / 2.0;

        double r = fract + amp * ( - 0.14861 * cos( angle ) + 1.78277 * sin( angle ) );
        double g = fract + amp * ( - 0.29227 * cos( angle ) - 0.90649 * sin( angle ) );
        double b = fract + amp * ( + 1.97294 * cos( angle ) );

        if ( r < 0 ) {
            r = 0;
        }
        if ( r > 1 ) {
            r = 1;
        }
        if ( g < 0 ) {
            g = 0;
        }
        if ( g > 1 ) {
            g = 1;
        }
        if ( b < 0 ) {
            b = 0;
        }
        if ( b > 1 ) {
            b = 1;
        }

        red.add( fract, r );
        green.add( fract, g );
        blue.add( fract, b );
    }
    return ColormapFunction( red, green, blue );
} // cubeHelix
}
}
