#pragma once

#include <QString>
#include <QDebug>
#include <cmath>
#include <vector>
#include <cstdlib>

/// convenience macro to throw exception with file/line number
#define LTHROW( x ) \
    do { \
        QString s = QString( "%1:%2:%3" ).arg( __FILE__ ).arg( __LINE__ ).arg( x ); \
        qCritical() << s; \
        throw std::runtime_error( s.toStdString() ); \
    } \
    while ( 0 )

inline double
rnd( double min, double max )
{
    return drand48() * ( max - min ) + min;
}

template < typename T >
inline
T
clamp( const T & v, const T & v1, const T & v2 )
{
    if ( v < v1 ) {
        return v1;
    }
    if ( v > v2 ) {
        return v2;
    }
    return v;
}

namespace Optimization
{
typedef std::vector < double > VD;

/// bounding parameters
struct RangeParam {
    double min, max, heuristicMaxStep;
    bool isSet;

    void
    set( double pmin, double pmax, double relStep = 1.0 / 10.0 )
    {
        min = std::min( pmin, pmax );
        max = std::max( pmin, pmax );
        heuristicMaxStep = ( max - min ) * relStep;
        isSet = true;
    }

    RangeParam( double pmin, double pmax, double relStep = 1.0 )
    {
        set( pmin, pmax, relStep );
    }

    double
    clamped( const double & v ) const
    {
        double res = v;
        clamp( res );
        return res;
    }

    void
    clamp( double & v ) const
    {
        if ( ! isSet ) { return; }
        if ( v < min ) { v = min; }
        if ( v > max ) { v = max; }
    }

    double
    rnd()
    {
        if ( isSet ) { return ::rnd( min, max ); }
        else { return 0; }
    }

    RangeParam()
    {
        min = 0;
        max = 1;
        heuristicMaxStep = 1;
        isSet = false;
    }
};

// nice (named) gaussian 2d parameters
struct Gauss1dNiceParams {
    double center, amplitude, fwhm;
    static Gauss1dNiceParams
    convert( const double * p );

    VD
    convertToUgly() const;
};

/// evaluate gauss1d function using parameters p, at position x,y
/// let a=p[0], b=p[1] and c=p[2]
/// then this function returns
///
/// b * exp( c * sqr(x - a))
///
/// so b is mean, a is center, and c is 'sigma-controlling' term
///
inline double
evalGauss1d( double x, const double * p )
{
    // symbolic values for g2d offsets
    enum { CENTER = 0, AMPLITUDE, VARIANCE };

    double dx = x - p[CENTER];
    return p[AMPLITUDE] * exp( p[VARIANCE] * dx * dx );
}

/// return sum of N gaussians
inline double
evalNGauss1d( double x, int nGaussians, const double * p )
{
    double sum = 0;
    for ( int i = 0 ; i < nGaussians ; i++ ) {
        sum += evalGauss1d( x, & p[i * 3] );
    }

    return sum;
}

/// return sum of N gaussians + polynomial with 'poly' terms
/// in other words, the polynomial is of degree 'poly-1'
inline double
evalNGauss1dBkg( double x, int nGaussians, int poly, const double * p )
{
    double sum = evalNGauss1d( x, nGaussians, p );
    double xx = 1; // this is x ^ i
    for ( int i = 0 ; i < poly ; i++ ) {
        sum += p[nGaussians * 3 + i] * xx;
        xx *= x;
    }
    return sum;
}

inline double
evalNGauss1dBkg( double x, int nGaussians, int poly, const VD & v )
{
    return evalNGauss1dBkg( x, nGaussians, poly, & ( v[0] ) );
}

namespace Gaussian1DFitting
{
struct NoRangeCheckPolicy {
    void
    checkRange( int, int, int, size_t ) const { }
};

struct YesRangeCheckPolicy {
    void
    checkRange( int x, int x1, int x2, size_t n ) const
    {
        if ( x < x1 || x < 0 ) { LTHROW( "Out of range" ); }
        if ( x > x2 || x >= int (n) ) { LTHROW( "Out of range" ); }
    }
};

// all fitters will use this structure for input
struct FitterInput {
    static const bool DoRangeCheck = true;

    /// number of gaussians
    int nGaussians;

    /// number of terms in the polynomial (polynomial degree = nPolyTerms-1)
    int nPolyTerms;

    /// the data to be fitted in an array, first data element is assumed to have x = 0
    const VD & data;
    const double &
    get( int x ) const
    {
        if ( DoRangeCheck ) {
            if ( x < x1 || x < 0 ) { LTHROW( "Out of range" ); }
            if ( x > x2 || x >= int ( data.size() ) ) { LTHROW( "Out of range" ); }
        }
        return data[x];
    }

    /// which range of the data to fit (must be valid indices into the array)
    int x1, x2;

    /// precomputed ranges of the data between x1 and x2
    double rangeMin, rangeMax;

    /// interface to calculate diffSquare (different between fitted function defined
    /// by params and the data between x1 and x2)
    double
    calculateDiffSq( const VD & params )
    {
        if ( DoRangeCheck ) {
            if ( int ( params.size() ) != numParams() ) { LTHROW( "params.size != numParams" ); }
        }
        double sum = 0;
        double fn = 0.0, yy = 0.0, d = 0.0;
        for ( int x = x1 ; x <= x2 ; x++ ) {
            yy = data[x];
            if ( std::isnan( yy ) ) {
                continue;
            }
            else {
                fn = evalNGauss1dBkg( x, nGaussians, nPolyTerms, params );
                d = yy - fn;
                sum += d * d;
            }
        }
        return sum;
    } // calculateDiffSq

    /// ranges for parameters (the size of this array should be nGaussians * 3 + nPolyTerms)
    std::vector < RangeParam > ranges;

    int
    numParams() const { return nGaussians * 3 + nPolyTerms; }

    /// function that constraints the parameters
    void
    clampParams( VD & params )
    {
        clampParams( & params[0] );
    }

    /// function that clamps the parameters to the range
    void
    clampParams( double * params )
    {
        // TODO: constraints are currently disabled
//        return;

        int np = numParams();
        for ( int i = 0 ; i < np ; i++ ) {
//            params[i] = clamp( params[i], ranges[i].min, ranges[i].max );
            ranges[i].clamp( params[i] );
        }
    }

    /// precompute min/max of the selected range
    void
    precomputeRangeMinMax()
    {
        rangeMin = rangeMax = data[x1];
        for ( int x = x1 ; x <= x2 ; x++ ) {
            double v = data[x];
            rangeMin = ( std::isnan( rangeMin ) || v < rangeMin ) ? v : rangeMin;
            rangeMax = ( std::isnan( rangeMax ) || v > rangeMax ) ? v : rangeMax;
        }
    }

    /// constructor
    FitterInput( const VD & arr )
        : data( arr )
    { }

protected:

    std::vector < RangeParam > m_ranges;
};
}
}
