#ifdef DONT_COMPILE

/**
 *
 **/

#include "Colormaps.h"

namespace Carta
{
namespace Core
{
ColormapFunction::ColormapFunction()
    : ColormapScalarNamed( "some colormap" )
{ }

ColormapFunction::ColormapFunction( PWLinear & red, PWLinear & green, PWLinear & blue )
    : ColormapFunction()
{
    red_   = red;
    green_ = green;
    blue_  = blue;
}

QRgb
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
    colormapFunction_  = ColormapFunction(
                             PWLinear().add( 0, 0 ).add( 1, 1 ),
                             PWLinear().add( 0, 0 ).add( 1, 1 ),
                             PWLinear().add( 0, 0 ).add( 1, 1 )
                             );
    colormapFunction_.setName( "Gray");
}

// combines histogram & colormap functions into one
HistogramColormapFunctor::HistogramColormapFunctor( double histMin,
                                                    double histMax,
                                                    ColormapFunction cmap )
{
    histogramFunction_ = PWLinear().add( histMin, 0.0 ).add( histMax, 1.0 );
    colormapFunction_  = cmap;
}

QRgb
HistogramColormapFunctor::operator() ( double x )
{
    return colormapFunction_( histogramFunction_( x ) );
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
    QRgb nanColor )
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
                                      QRgb nanColor )
{
    init( fn, min, max, n, nanColor );
}

}
}

#endif

