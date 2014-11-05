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
class PWLinear
{
public:
    PWLinear &
    add( double x, double y );

    double
    operator() ( double x );

private:
    QList < QPointF > points_;
};
typedef QRgb Rgb;

class ColormapFunction : public Lib::ColormapScalarNamed
{
public:
    Rgb
    operator() ( double x );

    virtual QRgb convert(const double &val) {
        return operator ()(val);
    }

    ColormapFunction( PWLinear & red, PWLinear & green, PWLinear & blue );
    ColormapFunction();

    // predefined colormaps
    static ColormapFunction
    gray();

    static ColormapFunction
    heat();

    static ColormapFunction
    fire();

    static ColormapFunction
    spring();

    static ColormapFunction
    sea();

    static ColormapFunction
    sunbow();

    static ColormapFunction
    mutant();

    static ColormapFunction
    aberration();

    static ColormapFunction
    rgb();

    static ColormapFunction
    velocity();

    static ColormapFunction
    cubeHelix( double start, double rots, double hue, double gamma );

private:
    PWLinear red_, green_, blue_;
};

// combines histogram (clipping) & colormap functions into one
class HistogramColormapFunctor
{
public:
    HistogramColormapFunctor( double min, double max, ColormapFunction cmap );
    HistogramColormapFunctor();
    Rgb
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
    CachedRgbFunction( HistogramColormapFunctor fn, double min, double max, int n, Rgb nanColor );
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
        if ( ind < 0 ) { return minRes; }
        if ( ind > n1 ) { return maxRes; }
        return cache[ind];
    } // qrgb

private:
    inline const Rgb &
    operator() ( const double & x ) const;

    QVector < Rgb > cache;
    double min_, max_;
    Rgb nanColor_;
    double diff, dInvN1;
    Rgb minRes, maxRes;
    int n1;

    void
    init( HistogramColormapFunctor fn, double min, double max, int n, Rgb nanColor );
};

inline const Rgb &
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
