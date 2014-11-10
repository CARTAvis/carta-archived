#ifdef DONT_COMPILE

/**
 * Some built in colormaps...
 **/

#pragma once

#include "CartaLib/IColormapScalar.h"

#include <limits>
#include <cmath>
#include <QColor>
#include <QList>
#include <QPointF>
#include <QVector>

namespace Carta
{
namespace Core
{

/// this class implements core colormaps
class ColormapFunction : public Lib::ColormapScalarNamed
{
public:

    typedef Carta::Lib::PWLinear PWLinear;

    QRgb
    operator() ( double x );

    virtual QRgb
    convert( const double & val )
    {
        return operator() ( val );
    }

    ColormapFunction( PWLinear & red, PWLinear & green, PWLinear & blue );
    ColormapFunction();




private:

    PWLinear red_, green_, blue_;
};

// combines histogram (clipping) & colormap functions into one
class HistogramColormapFunctor
{
public:

    typedef Carta::Lib::PWLinear PWLinear;

    HistogramColormapFunctor( double min, double max, ColormapFunction cmap );
    HistogramColormapFunctor();
    QRgb
    operator() ( double x );

private:

    PWLinear histogramFunction_;
    ColormapFunction colormapFunction_;
};

// caches a double -> Rgb function by precomputing values
/**
 * precalculates values for a function to speed up computation
 *
 * function is (x) --> y
 *
 * Parameters: {
 *   fn: function(x) --> y
 *   min: minimum value for x
 *   max: maximum value for y
 *   n: number of intervals to cache
 *   nanColor: what to return if x == NaN (or other non-finite value)
 * }
 */
class CachedRgbFunction
{
public:

    CachedRgbFunction();
    CachedRgbFunction( HistogramColormapFunctor fn, double min, double max, int n, QRgb nanColor );
    inline QRgb
    qrgb( const double & x ) const
    {
        if ( ! std::isfinite( x ) ) {
            return nanColor_;
        }
        if ( diff <= 0 ) {
            if ( x < min_ ) {
                return minRes;
            }
            else {
                return maxRes;
            }
        }
        int ind = round( ( x - min_ ) * dInvN1 );
        if ( ind < 0 ) { return minRes; } if ( ind > n1 ) { return maxRes; } return cache[ind];
    } // qrgb

private:

    inline const QRgb &
    operator() ( const double & x ) const;

    QVector < QRgb > cache;
    double min_, max_;
    QRgb nanColor_;
    double diff, dInvN1;
    QRgb minRes, maxRes;
    int n1;

    void
    init( HistogramColormapFunctor fn, double min, double max, int n, QRgb nanColor );
};

inline const QRgb &
CachedRgbFunction::operator() ( const double & x ) const
{
    if ( ! std::isfinite( x ) ) {
        return nanColor_;
    }
    if ( diff <= 0 ) {
        if ( x < min_ ) {
            return minRes;
        }
        else {
            return maxRes;
        }
    }
    int ind = round( ( x - min_ ) * dInvN1 );
    if ( ind < 0 ) {
        return minRes;
    }
    if ( ind > n1 ) {
        return maxRes;
    }
    return cache[ind];
} // ()
}
}

#endif
