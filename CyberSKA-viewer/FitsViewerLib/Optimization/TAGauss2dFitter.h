#ifndef TAGAUSS2DFITTER_H
#define TAGAUSS2DFITTER_H

#include <QString>
#include <QRectF>
#include <vector>
#include <stdexcept>
#include <cmath>
#include "common.h"
#include "Gauss2d.h"

/*
 * 2d gaussian fitter for an image
 */

namespace Optimization {

template < class Image >
class TAGauss2dFitter
{

public:
    enum BackgroundType { None = 0, Constant, Linear };

    typedef std::vector<double> VD;

    TAGauss2dFitter();


    /// set the image
    void setImage(Image * img);

    /// set the bounding rectangle where the fit will take place
    void setRect( int x1, int x2, int y1, int y2);

    /// set the requested number of gaussians to try to fit
    void setNumGaussians( int nGaussians);

    /// set initial estimate
    void setInitialParams( const std::vector<double> & v);

    /// use heuristics for the initial estimate
    void setIniatalParamsUsingHeuristics();

    const VD & getResults();
    double getChiSq();

    /// initialize stuff for iterate()
    void initOnce();

    /// invoke an iteration
    ///  result: true = finishe, fales = not finished
    bool iterate();

    /// get number of parameters, based on nGaussians and bgType
    int numParams() {
        if( bgType == None)
            return nGaussians * 6;
        if( bgType == Constant)
            return nGaussians * 6 + 1;
        return nGaussians * 6 + 3;
    }

    /// function that constraints parameters
    void constraintParameters( VD & params);


    /// source of observations
    Image * img;
    /// rectangle of observations
    int x1, x2, y1, y2;
    /// what type of background to simulate
    BackgroundType bgType;
    /// number of gaussians
    int nGaussians;

    /// guard to execute initOnce()
    bool firstTime;

    /// region min/max, used in constraints
    double regionMin, regionMax;

    /// current parameters
    std::vector<double> params;

    /// TA specific parameters
    int currIter;
    int maxIter;
    double initialT;
    double currT;
    VD currX; // current solution
    double currCost; // cost of current solution
    VD bestX; // best solution so far
    double bestCost; // cost of best solution
    double neighborStep; // TODO: make user configurable


    /// cost calculation for the given parameters
    double calculateCost( const VD & x);

    /// neighbor calculation for the given parameters
    VD calculateNeighbor( const VD & x);

    /// bounding parameters
    struct RangeParam{ double min, max, step; };
    std::vector<RangeParam> ranges;
    void setRanges();

//    /// function to compute F
//    void calculateF( const VD & x, VD & results);

};

template <class Image>
TAGauss2dFitter<Image>::TAGauss2dFitter()
{
    img = 0;
    x1 = x2 = y1 = y2 = 0;
    firstTime = true;
    nGaussians = 0;
    bgType = Constant;

}

template <class Image>
void
TAGauss2dFitter<Image>::setImage( Image * pimg)
{
    img = pimg;
}

template <class Image>
void
TAGauss2dFitter<Image>::setRect(int px1, int px2, int py1, int py2)
{
    if( ! img) throw std::runtime_error( "TAGauss2dFitter::uninitialized img");

    x1 = px1; x2 = px2; y1 = py1; y2 = py2;

    // fix up rectangle so that we don't need to do more bound checking
    if( x1 > x2) std::swap( x1, x2);
    if( y1 > y2) std::swap( y1, y2);
    x1 = clamp( x1, 0, img-> imageWidth-1);
    x2 = clamp( x2, 0, img-> imageWidth-1);
    y1 = clamp( y1, 0, img-> imageHeight-1);
    y2 = clamp( y2, 0, img-> imageHeight-1);
    dbg(1) << "x12y12 = " << x1 << ".." << x2 << " " << y1 << ".." << y2 << "\n";

    // compute min/max of the region, used in constraints
    regionMin = regionMax = img-> imageValue( x1, y1);
    for( int y = y1 ; y <= y2 ; y ++ ) {
        for( int x = x1 ; x <= x2 ; x ++ ) {
            double v = img-> imageValue( x, y);
            regionMin = (isnan(regionMin) || v < regionMin) ? v : regionMin;
            regionMax = (isnan(regionMax) || v > regionMax) ? v : regionMax;
        }
    }
    dbg(1) << "Region min/max = " << regionMin << " " << regionMax << "\n";


}

///// function to compute F vector
//template <class Image>
//void TAGauss2dFitter<Image>:: calculateF(
//        const VD & params, VD & results)
//{
//    double * p = results;
//    double fn = 0.0, yy = 0.0;
//    for( int y = y1 ; y <= y2 ; y ++ ) {
//        for( int x = x1 ; x <= x2 ; x ++) {
//            yy = img-> imageValue(x,y);
//            if( isnan(yy)) {
//                * p = 0.0;
//            } else {
//                fn = evalNGauss2dBkg( x, y, nGaussians, bgType, & params[0]);
//                *p = yy - fn;
//            }
//            p ++;
//        }
//    }
//}

template <class Image>
double
TAGauss2dFitter<Image>::calculateCost(const TAGauss2dFitter::VD & par)
{
    double sum = 0;
    double fn = 0.0, yy = 0.0, d = 0.0;
    for( int y = y1 ; y <= y2 ; y ++ ) {
        for( int x = x1 ; x <= x2 ; x ++) {
            yy = img-> imageValue(x,y);
            if( isnan(yy)) {
                continue;
            } else {
                fn = evalNGauss2dBkg( x, y, nGaussians, bgType, & par[0]);
                d = yy - fn;
                sum += d * d;
            }
        }
    }
    return sum;
}


template <class Image>
typename TAGauss2dFitter<Image>::VD
TAGauss2dFitter<Image>::calculateNeighbor(const TAGauss2dFitter::VD &x)
{
    VD res = x;
    int np = numParams();
    for( int i = 0 ; i < np ; i ++ ) {
        res[i] += (drand48() *2 - 1) * ranges[i].step;
    }
    constraintParameters( res);
    return res;
    // TODO: optimize this

}

template <class Image>
void
TAGauss2dFitter<Image>::setRanges()
{
    struct Annon {
        static RangeParam pr( double min, double max) {
            RangeParam res;
            res.min = min; res.max = max; res.step = (max - min) / 2.0;
            return res;
        }
    };

    double maxFwhm = std::max(x2-x1+1,y2-y1+1);
    if( maxFwhm < 2) maxFwhm = 2;

    ranges.resize( numParams());
    for( int i = 0 ; i < nGaussians ; i ++ ) {
        // amplitude
        ranges[i*6+0] = Annon::pr( regionMin, regionMax);
        // center x/y
        ranges[i*6+1] = Annon::pr( x1, x2);
        ranges[i*6+2] = Annon::pr( y1, y2);
        // fwmh
        ranges[i*6+3] = Annon::pr( 1.5, maxFwhm );
        // ratio
        ranges[i*6+4] = Annon::pr( 0.25, 4.0);
        // angle
        ranges[i*6+5] = Annon::pr( 0, 360);
        ranges[i*6+5].min = -360000;
        ranges[i*6+5].max = 360000;
    }
    if( bgType == Constant) {
        ranges[nGaussians*6 + 0] = Annon::pr( regionMin, regionMax);
    } else if( bgType == Linear) {
        ranges[nGaussians*6 + 0] = Annon::pr( regionMin, regionMax);
        // TODO: verify:
        ranges[nGaussians*6 + 1] = Annon::pr( regionMin, regionMax);
        ranges[nGaussians*6 + 2] = Annon::pr( regionMin, regionMax);
    }
}



template <class Image>
void TAGauss2dFitter<Image>::constraintParameters(VD &params)
{
    int np = numParams();
    for( int i = 0 ; i < np ; i ++ ) {
        params[i] = clamp( params[i], ranges[i].min, ranges[i].max);
    }

    /*

    double maxSize = std::max(x2-x1+1,y2-y1+1);
    if( maxSize < 2) maxSize = 2;
    for( int i = 0 ; i < nGaussians ; i ++ ) {
        // amplitude
        params[i*6+0] = clamp( params[i*6+0], 0.0, regionMax - regionMin);
        // center x/y
        params[i*6+1] = clamp<double>( params[i*6+1], x1, x2);
        params[i*6+2] = clamp<double>( params[i*6+2], y1, y2);
        // fwmh
        params[i*6+3] = clamp<double>( params[i*6+3],
                1.5,
                maxSize);
        // ratio
        params[i*6+4] = clamp( params[i*6+4], 0.25, 3.0);
        // angle
//            params[i*6+5] = clamp( params[i*6+5],
//                    regionMin - (regionMax-regionMin),
//                    regionMax + (regionMax-regionMin));
    }
    */
}


template <class Image>
void
TAGauss2dFitter<Image>::setNumGaussians( int ng)
{
    nGaussians = ng;
    params.resize( numParams(), 0.0);
}

template <class Image>
void
TAGauss2dFitter<Image>::setInitialParams(const std::vector<double> &v)
{
    params = v;
}

template <class Image>
void
TAGauss2dFitter<Image>::setIniatalParamsUsingHeuristics()
{
    if( this->ranges.empty())
        setRanges();

    QTime t; t.restart();
    params.resize( numParams(), 0.0);

    for( int i = 0 ; i < nGaussians ; i ++ ) {
        params[i*6 + 0] = rnd(0, regionMax);
        params[i*6 + 1] = rnd(x1,x2);
        params[i*6 + 2] = rnd(y1,y2);
        params[i*6 + 3] = rnd(1,(x2 - x1));
        params[i*6 + 4] = rnd(0.5,2.0);
        params[i*6 + 5] = drand48() * 360;
    }
    if( bgType == Constant) {
        params[nGaussians * 6] = (regionMin + regionMax) / 2;
    } else if( bgType == Linear) {
        params[nGaussians * 6] = (regionMin + regionMax) / 2;
        params[nGaussians * 6+1] = 0.0;
        params[nGaussians * 6+2] = 0.0;
    }
}

template < class Image >
const typename TAGauss2dFitter<Image>::VD &
TAGauss2dFitter < Image >::getResults()
{
    return bestX;
}

template < class Image >
double
TAGauss2dFitter < Image >::getChiSq()
{
    return bestCost;
}

template <class Image>
void
TAGauss2dFitter<Image>::initOnce()
{
    if( ! firstTime) return;
    firstTime = false;

    if( ! img) throw std::runtime_error( "TAGauss2dFitter::uninitialized img");

    if( ranges.empty()) setRanges();

    currIter = 0;
    maxIter = 100000; // TODO: should be user controllable
    initialT = 1000.0; // TODO: should be user controllable or automatic...
    neighborStep = 100.0; // TODO: make user configurable
    currT = initialT;

    currX = params;
    currCost = calculateCost( currX);
    bestX = currX;
    bestCost = currCost;
}


template <class Image>
bool
TAGauss2dFitter<Image>::iterate()
{

    initOnce();

    // calculate a neighbor
    VD xNew = calculateNeighbor( currX);
    double costNew = calculateCost( xNew);

//    double costDiff = costNew - currCost;
//    dbg(1) << "costDiff [" << currIter << "] = " << costDiff << " T = " << currT
//           << " best = " << currCost
//           << "\n";
    if( costNew - currCost < currT) {
        currX = xNew;
        currCost = costNew;
        if( currCost < bestCost) { // remember best found solution
            bestCost = currCost;
            bestX = currX;
        }
    }

    currIter += 1;
    currT = (1.0 - double(currIter)/maxIter) * initialT;

    if( currIter < maxIter)
        return false;
    else
        return true;
}

} // namespace Optimization


#endif // TAGAUSS2DFITTER_H
