#ifndef COLORMAPFUNCTION_H
#define COLORMAPFUNCTION_H

#include <limits>
#include <cmath>

#include <QList>
#include <QPointF>

#include "common.h"

class PWLinear {
public:
    PWLinear & add( double x, double y);
    double operator()(double x);
private:
    QList<QPointF> points_;
};

class ColormapFunction {
public:
    virtual Rgb operator()( double x);
    ColormapFunction( PWLinear & red, PWLinear & green, PWLinear & blue);
    void setInvert( bool);
    void setReverse( bool);
    void setRgbScales( double r, double g, double b);
    void setScales( double s1, double s2);
    double getGamma() const { return m_gamma; }
    ColormapFunction();

    // predefined colormaps
    static ColormapFunction gray();
    static ColormapFunction heat();
    static ColormapFunction fire();
    static ColormapFunction spring();
    static ColormapFunction sea();
    static ColormapFunction sunbow();
    static ColormapFunction mutant();
    static ColormapFunction aberration();
    static ColormapFunction rgb();
    static ColormapFunction velocity();
    static ColormapFunction cubeHelix( double start, double rots, double hue, double gamma);


private:
    PWLinear red_, green_, blue_;
    bool invert_, reverse_;
    double redScale_, greenScale_, blueScale_;
    double scale1_, scale2_, m_gamma;
};

class HistFunctor
{
public:
    HistFunctor( double min, double max);
    Rgb operator() ( double v) const ;
private:
    double min_, max_;
    double la_, lb_;
};

// combines histogram & colormap functions into one
class HistogramColormapFunctor
{
public:
    HistogramColormapFunctor( double min, double max, ColormapFunction cmap);
    HistogramColormapFunctor();
    Rgb operator() (double x);

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
 *   nanColor: what to return if x == NaN
 * }
 */
class CachedRgbFunction {
public:
    CachedRgbFunction();
    CachedRgbFunction( HistogramColormapFunctor fn, double min, double max, int n, Rgb nanColor);
    inline QRgb qrgb( const double & x) const
    {
        if( ! std::isfinite(x))
            return qRgb( nanColor_.r, nanColor_.g, nanColor_.b);

        if( diff <= 0) {
            if( x < min_) {
                return qRgb( minRes.r, minRes.g, minRes.b);
            }
            else {
                return qRgb( maxRes.r, maxRes.g, maxRes.b);
            }
        }
        int ind = round( (x-min_)*dInvN1);
        if( ind < 0) return qRgb( minRes.r, minRes.g, minRes.b);
        if( ind > n1) return qRgb( maxRes.r, maxRes.g, maxRes.b);
        const Rgb & rgb = cache[ind];
        return qRgb( rgb.r, rgb.g, rgb.b);
    }

private:
    inline const Rgb & operator()( const double & x) const;

    QVector<Rgb> cache;
    double min_, max_;
    Rgb nanColor_;
    double diff, dInvN1;
    Rgb minRes, maxRes;
    int n1;

    void init( HistogramColormapFunctor fn, double min, double max, int n, Rgb nanColor);

};

inline const Rgb &
CachedRgbFunction::operator () (const double & x) const
{
    if( ! std::isfinite(x))
        return nanColor_;

    if( diff <= 0) {
        if( x < min_) return minRes; else return maxRes;
    }
    int ind = round( (x-min_)*dInvN1);
    if( ind < 0) return minRes;
    if( ind > n1) return maxRes;
    return cache[ind];

}



#endif // COLORMAPFUNCTION_H
