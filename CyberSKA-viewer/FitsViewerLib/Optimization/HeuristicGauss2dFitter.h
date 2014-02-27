#ifndef HEURISTICGAUSS2DFITTER_H
#define HEURISTICGAUSS2DFITTER_H

#include <QString>
#include <QRectF>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <queue>
#include "common.h"
#include "Gauss2d.h"

/*
 * 2d gaussian fitter for an image
 */

namespace Optimization {

template < class Image >
class HeuristicGauss2dFitter
{

public:
    enum BackgroundType { None = 0, Constant, Linear };

    typedef std::vector<double> VD;

    HeuristicGauss2dFitter();


    /// set the image
    void setImage(Image * img);

    /// set the bounding rectangle where the fit will take place
    void setRect( int x1, int x2, int y1, int y2);

    /// set the requested number of gaussians to try to fit
    void setNumGaussians( int nGaussians);

    const VD & getResults();
    double getChiSq();

    /// initialize stuff for iterate()
    void initOnce();

    /// invoke an iteration
    ///  result: true = finished, fales = not finished
    bool iterate();

    /// return the degrees of freedom, which is nSamples - params
    /// but we need to account for nans
    int calculateDof();

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

    /// cost calculation for the given parameters
    double calculateCost( const VD & x);

    /// neighbor calculation for the given parameters
    VD calculateNeighbor( const VD & x);

    /// bounding parameters
    struct RangeParam{ double min, max; };
    std::vector<RangeParam> ranges;
    void setRanges();

    std::priority_queue<double> tHeap;

//    /// function to compute F
//    void calculateF( const VD & x, VD & results);

};

template <class Image>
HeuristicGauss2dFitter<Image>::HeuristicGauss2dFitter()
{
    img = 0;
    x1 = x2 = y1 = y2 = 0;
    firstTime = true;
    nGaussians = 0;
    bgType = Constant;

}

template <class Image>
void
HeuristicGauss2dFitter<Image>::setImage( Image * pimg)
{
    img = pimg;
}

template <class Image>
void
HeuristicGauss2dFitter<Image>::setRect(int px1, int px2, int py1, int py2)
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
HeuristicGauss2dFitter<Image>::calculateCost(const HeuristicGauss2dFitter::VD & par)
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
void
HeuristicGauss2dFitter<Image>::setRanges()
{
    struct Annon {
        static RangeParam pr( double min, double max) {
            RangeParam res;
            res.min = min; res.max = max;
            return res;
        }
    };

    double maxFwhm = 2* std::max(x2-x1+1,y2-y1+1);
    if( maxFwhm < 2) maxFwhm = 2;

    ranges.resize( numParams());
    for( int i = 0 ; i < nGaussians ; i ++ ) {
        // amplitude
        ranges[i*6+0] = Annon::pr( regionMin, regionMax);
        // center x/y
        ranges[i*6+1] = Annon::pr( x1, x2);
        ranges[i*6+2] = Annon::pr( y1, y2);
        // fwmh
        ranges[i*6+3] = Annon::pr( 1.5, maxFwhm);
        // ratio
        ranges[i*6+4] = Annon::pr( 0.25, 4.0);
        // angle
        ranges[i*6+5] = Annon::pr( 0, 360);
    }
    if( bgType == Constant) {
        ranges[nGaussians*6 + 0] = Annon::pr( regionMin, regionMax);
    } else if( bgType == Linear) {
        ranges[nGaussians*6 + 0] = Annon::pr( regionMin, regionMax);
        // TODO: verify:
        ranges[nGaussians*6 + 1] = Annon::pr( regionMin, regionMax);
        ranges[nGaussians*6 + 2] = Annon::pr( regionMin, regionMax);
    }

    dbg(1) << "ranges: ------------------------------------------------\n";
    for( size_t i = 0 ; i < ranges.size() ; i ++ ) {
        dbg(1) << QString("  %1) %2 .. %3\n")
                  .arg(i, 3)
                  .arg(ranges[i].min, 15)
                  .arg(ranges[i].max, 15);
    }
    dbg(1) << "---------------------------------------------------------\n";
}



template <class Image>
void HeuristicGauss2dFitter<Image>::constraintParameters(VD &params)
{
    int np = numParams();
    for( int i = 0 ; i < np ; i ++ ) {
        params[i] = clamp( params[i], ranges[i].min, ranges[i].max);
    }
}


template <class Image>
void
HeuristicGauss2dFitter<Image>::setNumGaussians( int ng)
{
    nGaussians = ng;
    params.resize( numParams(), 0.0);
}

template < class Image >
const typename HeuristicGauss2dFitter<Image>::VD &
HeuristicGauss2dFitter < Image >::getResults()
{
    return params;
}

template < class Image >
double
HeuristicGauss2dFitter < Image >::getChiSq()
{
    return calculateCost( params);
}

template <class Image>
void
HeuristicGauss2dFitter<Image>::initOnce()
{
    if( ! firstTime) return;
    firstTime = false;

    if( ! img) throw std::runtime_error( "TAGauss2dFitter::uninitialized img");

    if( ranges.empty()) {
        setRanges();
    }

    params.resize( numParams(), 0.0);

    // generate random parameters, to make sure we have something
    for( size_t i = 0 ; int(i) < numParams() ; i ++ ) {
        params[i] = rnd( ranges[i].min, ranges[i].max);
    }

    // fix 1st gaussian on max in the region
    if( nGaussians > 0) {
        // find the max x/y & peak value
        double maxVal = 0.0/0.0;
        double maxX = x1;
        double maxY = y1;
        for( int y = y1 ; y <= y2 ; y ++ ) {
            for( int x = x1 ; x <= x2 ; x ++ ) {
                double val = img-> imageValue( x, y);
                if( isnan(val)) continue;
                if( isnan(maxVal) || val > maxVal) {
                    maxX = x;
                    maxY = y;
                    maxVal = val;
                }
            }
        }
        dbg(1) << QString("Max (%1) at [%2,%3]\n").arg(maxVal).arg(maxX).arg(maxY);
        params[0] = maxVal;
        params[1] = maxX;
        params[2] = maxY;
        params[3] = 3;
        params[4] = 1.0;
        params[5] = 0.0;
    }

    if( bgType != None) {
        params[nGaussians*6] = regionMin;
    }
}


template <class Image>
bool
HeuristicGauss2dFitter<Image>::iterate()
{

    initOnce();

    return true;
}

template <class Image>
int HeuristicGauss2dFitter<Image>::calculateDof()
{
    int res = - numParams();
    for( int y = y1 ; y <= y2 ; y ++ ) {
        for( int x = x1 ; x <= x2 ; x ++ ) {
            if( ! isnan( img-> imageValue( x, y)))
                res ++;
        }
    }
    return res;
}

} // namespace Optimization


#endif // HEURISTICGAUSS2DFITTER_H
