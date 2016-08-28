#pragma once

//#include "common.h"
//#include "Gauss1d.h"
//#include "PolynomialFitter1D.h"
#include "PolynomialFitter1D.h"

#include <QString>
#include <QRectF>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <queue>

/*
 * 1d heuristic fitter for an array of doubles
 */

namespace Optimization
{
namespace Gaussian1DFitting
{
class HeuristicFitter
{
public:

    typedef Optimization::VD VD;
    HeuristicFitter( FitterInput & dataInterface )
        : di( dataInterface )
    {
        firstTime = true;
    }

//    /// initialize stuff for iterate()
//    void initOnce();

    /// invoke an iteration
    ///  result: true = finished, fales = not finished
    bool
    iterate();

    /// set initial guess for gaussians

    /// data interface
    FitterInput & di;

    const VD &
    getResults();

    double
    getDiffSq();

    /// guard to execute only once
    bool firstTime;

    /// current parameters
    std::vector < double > params;
};

const VD &
HeuristicFitter::getResults()
{
    return params;
}

double
HeuristicFitter::getDiffSq()
{
    return di.calculateDiffSq( params );
}

//void
//HeuristicFitter::initOnce()
//{
//}

bool
HeuristicFitter::iterate()
{
    if ( ! firstTime ) {
        return true;
    }
    firstTime = false;

    // set every parameter to 0.0
    params.clear();
    params.resize( di.numParams(), 0.0 );

    bool guoSuccessful = true;

    // run guo fitters for all requested gaussians, subtracting them from a copy of
    // the data 1 at a time
    if ( di.nGaussians > 0 ) {
        // prepare a copy of the data to be fit
        // we will be subtracting from this the gaussians
        std::vector < std::pair < double, double > > fitData;
        for ( int x = di.x1 ; x <= di.x2 ; x++ ) {
            double y = di.get( x );
            if ( std::isnan( y ) ) {
                continue;
            }
            fitData.push_back( std::pair < double, double > ( x, y ) );
        }

        // fit the gaussians 1 at a time
        for ( int ng = 0 ; ng < di.nGaussians ; ng++ ) {
            GuoFit guoFitter( fitData );
            guoFitter.setAlgorithm( GuoFit::IterativeGuo );
            bool guoOk = guoFitter.iterate();
            if ( ! guoOk ) {
                qDebug() << "Guo fitter failure.";
                guoSuccessful = false;
                break;
            }
            qDebug() << "Guo fitter #" << ng + 1 << " successful.";
            VD res = guoFitter.getResults();
            if ( res.size() != 3 ) {
                LTHROW( "guoFitter did not return 3 numbers" );
            }
            qDebug() << "guoFitter = " << res[0] << " " << res[1] << " " << res[2];
            params[ng * 3 + 0] = res[0];
            params[ng * 3 + 1] = res[1];
            params[ng * 3 + 2] = res[2];

            // subtract this gaussian from the data
            for ( size_t i = 0 ; i < fitData.size() ; i++ ) {
                double x = fitData[i].first;
                fitData[i].second -= evalGauss1d( x, & res[0] );
            }
        }
    }

    // if guo fitter failed, at least position the first gaussian on the max
    if ( di.nGaussians > 0 && ! guoSuccessful ) {
        qDebug() << "Guo failed, positioning on max in the region.";
        params.clear();
        params.resize( di.numParams(), 0.0 );

        // find the max x/y & peak value
        double maxVal = 0.0 / 0.0;
        double maxX = di.x1;
        for ( int x = di.x1 ; x <= di.x2 ; x++ ) {
            double val = di.get( x );
            if ( std::isnan( val ) ) {
                continue;
            }
            if ( std::isnan( maxVal ) || val > maxVal ) {
                maxX = x;
                maxVal = val;
            }
        }
        qDebug() << QString( "Max (%1) at [%2]\n" ).arg( maxVal ).arg( maxX );
        params[0] = maxX;
        params[1] = maxVal;
        params[2] = - 1 / ( ( di.x2 - di.x1 + 1.0 ) * ( di.x2 - di.x1 + 1.0 ) );
    }

    // figure out the polynomial terms by
    // doing a polynomial fit on the residual
    // (after subtracting the gaussians calculated
    // in the step above)
    {
        // prepare the fit data
        std::vector < std::pair < double, double > > fitData;
        for ( int x = di.x1 ; x <= di.x2 ; x++ ) {
            double y = di.get( x );
            if ( std::isnan( y ) ) {
                continue;
            }
            y -= evalNGauss1dBkg( x, di.nGaussians, di.nPolyTerms, & params[0] );
            if ( std::isnan( y ) ) {
                continue;
            }
            fitData.push_back( std::pair < double, double > ( x, y ) );
        }
        PolynomialFitter1D pf;
        VD polyParams = pf.fit( di.nPolyTerms, fitData );
        for ( int i = 0 ; i < di.nPolyTerms ; i++ ) {
            params[di.nGaussians * 3 + i] = polyParams[i];
        }
    }

    return true;
} // HeuristicFitter::iterate
} // namespace Gaussian1DFitting
} // namespace Optimization

