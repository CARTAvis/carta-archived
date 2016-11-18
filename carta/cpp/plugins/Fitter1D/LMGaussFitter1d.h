#pragma once

#include "Gauss1d.h"
#include "LevMar.h"
#include <QString>
#include <QRectF>
#include <vector>
#include <stdexcept>
#include <cmath>

/*
 * 1d gaussian fitter for 1d data of doubles, using levenberg-marquardt
 */

namespace Optimization
{
namespace Gaussian1DFitting
{
class LMFitter
{
public:

    LMFitter( FitterInput & dataInterface )
        : di( dataInterface )
    {
        firstTime = true;
    }

    FitterInput & di;

    /// set initial estimate
    void
    setInitialParams( const std::vector < double > & v )
    {
        if ( int ( v.size() ) != numParams() ) {
            LTHROW( "LMGaussFitter1d::setInitialParams: bad param size" );
        }
        params = v;
    }

    std::vector < double >
    getResults();

    /// initialize stuff for iterate()
    void
    initOnce();

    /// invoke an iteration
    bool
    iterate();

    /// get number of parameters, based on nGaussians and poly
    int
    numParams()
    {
        return di.nGaussians * 3 + di.nPolyTerms;
    }

    /// callback for LevMar to calculate F
    static
    void
    levmarFFunc( const double * params, double * results, void * userData )
    {
        LMFitter & gf = * ( static_cast < LMFitter * > ( userData ) );
        gf.calculateF( params, results );
    }

    /// function to compute F
    void
    calculateF( const double * paramsOrig, double * results )
    {
        // TODO: is this killing performace to put constraints here? or is
        // this the best we can do?
        double params[numParams()];
        for ( int i = 0 ; i < numParams() ; i++ ) { params[i] = paramsOrig[i]; }
        constraintParameters( params );

        double * p = results;
        double f = 0.0, yy = 0.0;
        for ( int x = di.x1 ; x <= di.x2 ; x++ ) {
            yy = di.get( x );
            if ( std::isnan( yy ) ) {
                * p = 0.0;
            }
            else {
                f = evalNGauss1dBkg( x, di.nGaussians, di.nPolyTerms, params );
                * p = yy - f;
            }
            p++;
        }
    } // calculateF

    /// callback for LevMar to constraint parameters
    static
    void
    levmarConstraintsFunc( double * params, void * userData )
    {
        LMFitter & gf = * ( static_cast < LMFitter * > ( userData ) );
        gf.constraintParameters( params );
    }

    /// function that constraints parameters
    void
    constraintParameters( double * params )
    {
        di.clampParams( params );
    }

    /// guard to execute initOnce()
    bool firstTime;

    LevMar levmar;

    std::vector < double > params;
};

std::vector < double > LMFitter::getResults()
{
    const double * x = levmar.getSolutionRef();
    if ( ! x ) {
        throw std::runtime_error( "GaussFitter1d::getResults: x = null" );
    }

    std::vector < double > res( numParams() );
    for ( int i = 0 ; i < numParams() ; i++ ) {
        res[i] = x[i];
    }

    return res;
}

void
LMFitter::initOnce()
{
    if ( ! firstTime ) {
        return;
    }
    firstTime = false;

    if ( int ( params.size() ) != di.numParams() ) {
        LTHROW( "Bad parm size" );
    }

//    dbg(1) << "ranges: ------------------------------------------------\n";
//    for( size_t i = 0 ; i < di.ranges.size() ; i ++ ) {
//        dbg(1) << QString("  %1) %2 .. %3\n")
//                  .arg(i, 3)
//                  .arg(di.ranges[i].min, 15)
//                  .arg(di.ranges[i].max, 15);
//    }
//    dbg(1) << "---------------------------------------------------------\n";

    levmar.setStartParameters( params );
    levmar.setNumSamples( di.x2 - di.x1 + 1 );
    levmar.setFFunc( levmarFFunc, this );
    levmar.setClampFunction( levmarConstraintsFunc, this );
    levmar.init();
} // LMFitter::initOnce

bool
LMFitter::iterate()
{
    initOnce();

    LevMar::Status status = levmar.iterate();

    if ( status == LevMar::Done ) {
//        dbg(1) << "Levmar finished\n";
        return true;
    }

    if ( status == LevMar::Error ) {
        qDebug() << "Levmar error";
        return true;
    }

    return false;
} // LMFitter::iterate
} // namespace Gaussian1DFitting
} // namespace Optimization

