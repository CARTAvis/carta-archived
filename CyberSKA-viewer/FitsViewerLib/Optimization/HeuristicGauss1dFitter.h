#ifndef HEURISTICGAUSS1DFITTER_H
#define HEURISTICGAUSS1DFITTER_H

#include <QString>
#include <QRectF>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <queue>
#include "common.h"
#include "Gauss1d.h"
#include "PolynomialFitter1D.h"

/*
 * 1d heuristic fitter for an array of doubles
 */

namespace Optimization {
namespace Gaussian1DFitting {

class HeuristicFitter
{

public:

    typedef Optimization::VD VD;
    HeuristicFitter( DataInterface & dataInterface)
        : di( dataInterface)
    {
        firstTime = true;
    }



//    /// initialize stuff for iterate()
//    void initOnce();

    /// invoke an iteration
    ///  result: true = finished, fales = not finished
    bool iterate();

    /// set initial guess for gaussians


    /// data interface
    DataInterface & di;

    const VD & getResults();
    double getDiffSq();

    /// guard to execute only once
    bool firstTime;

    /// current parameters
    std::vector<double> params;
};


const VD &
HeuristicFitter::getResults()
{
    return params;
}

double
HeuristicFitter::getDiffSq()
{
    return di.calculateDiffSq( params);
}

//void
//HeuristicFitter::initOnce()
//{
//}


bool
HeuristicFitter::iterate()
{
    if( ! firstTime) return true;
    firstTime = false;

    // set every parameter to 0.0
    params.clear();
    params.resize( di.numParams(), 0.0);

    bool guoSuccessful = true;

    // run guo fitters for all requested gaussians, subtracting them from a copy of
    // the data 1 at a time
    if( di.nGaussians > 0) {
        // prepare a copy of the data to be fit
        // we will be subtracting from this the gaussians
        std::vector< std::pair<double,double> > fitData;
        for( int x = di.x1 ; x <= di.x2 ; x ++) {
            double y = di.get(x);
            if( isnan(y)) continue;
            fitData.push_back( std::pair<double,double>( x, y));
        }

        // fit the gaussians 1 at a time
        for( int ng = 0 ; ng < di.nGaussians ; ng ++ ) {
            GuoFit guoFitter( fitData);
            guoFitter.setAlgorithm( GuoFit::IterativeGuo);
            bool guoOk = guoFitter.iterate();
            if( ! guoOk) {
                dbg(1) << "Guo fitter failure.\n";
                guoSuccessful = false;
                break;
            }
            dbg(1) << "Guo fitter #" << ng+1 << " successful.\n";
            VD res = guoFitter.getResults();
            if( res.size() != 3) LTHROW( "guoFitter did not return 3 numbers");
            dbg(1) << "guoFitter = " << res[0] << " " << res[1] << " " << res[2] << "\n";
            params[ng * 3 + 0] = res[0];
            params[ng * 3 + 1] = res[1];
            params[ng * 3 + 2] = res[2];

            // subtract this gaussian from the data
            for( size_t i = 0 ; i < fitData.size() ; i ++ ) {
                double x = fitData[i].first;
                fitData[i].second -= evalGauss1d( x, & res[0]);
            }
        }
    }

    // if guo fitter failed, at least position the first gaussian on the max
    if( di.nGaussians > 0 && ! guoSuccessful) {
        dbg(1) << "Guo failed, positioning on max in the region.\n";
        params.clear();
        params.resize( di.numParams(), 0.0);
        // find the max x/y & peak value
        double maxVal = 0.0/0.0;
        double maxX = di.x1;
            for( int x = di.x1 ; x <= di.x2 ; x ++ ) {
                double val = di.get(x);
                if( isnan(val)) continue;
                if( isnan(maxVal) || val > maxVal) {
                    maxX = x;
                    maxVal = val;
                }
            }
        dbg(1) << QString("Max (%1) at [%2]\n").arg(maxVal).arg(maxX);
        params[0] = maxX;
        params[1] = maxVal;
        params[2] = -1 / ((di.x2-di.x1+1.0)*(di.x2-di.x1+1.0));
    }

    // figure out the polynomial terms by
    // doing a polynomial fit on the residual
    // (after subtracting the gaussians calculated
    // in the step above)
    {
        // prepare the fit data
        std::vector< std::pair<double,double> > fitData;
        for( int x = di.x1 ; x <= di.x2 ; x ++) {
            double y = di.get(x);
            if( isnan(y)) continue;
            y -= evalNGauss1dBkg( x, di.nGaussians, di.nPolyTerms, & params[0]);
            if( isnan(y)) continue;
            fitData.push_back( std::pair<double,double>( x, y));
        }
        PolynomialFitter1D pf;
        VD polyParams = pf.fit( di.nPolyTerms, fitData);
        for( int i = 0 ; i < di.nPolyTerms ; i ++ )
            params[di.nGaussians*3+i] = polyParams[i];
    }

    return true;
}

} // namespace Gaussian1DFitting
} // namespace Optimization

#endif // HEURISTICGAUSS1DFITTER_H
