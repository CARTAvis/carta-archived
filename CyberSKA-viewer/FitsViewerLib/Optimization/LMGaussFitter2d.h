#ifndef LMGAUSSFITTER2D_H
#define LMGAUSSFITTER2D_H

#include <QString>
#include <QRectF>
#include <vector>
#include <stdexcept>
#include <cmath>
#include "LevMar.h"
#include "common.h"
#include "Gauss2d.h"

/*
 * 2d gaussian fitter for an image, using levenberg-marquardt
 */

namespace Optimization {

template < class Image >
class LMGaussFitter2d
{

public:
    enum IterateStatus { Done, NotDone };
    enum BackgroundType { None = 0, Constant, Linear };

    LMGaussFitter2d();

    /// set the image
    void setImage(Image * img);

    /// set the bounding rectangle where the fit will take place
    void setRect( int x1, int x2, int y1, int y2);

    /// set the requested number of gaussians to try to fit
    void setNumGaussians( int nGaussians);

    /// set initial estimate
    void setInitialParams( const std::vector<double> & v);

    /// use heuristics for the initial estimate
    void setGaussianParametersUsingHeuristics();

//    /// get the gaussian parameters back
//    const double * getGaussianParameters( int which);

//    /// get the background parameters back
//    const double * getBackgroundParameters();

    std::vector<double> getResults();
    double getChiSq() { return levmar.chiSq(); }

    /// initialize stuff for iterate()
    void initOnce();

    /// invoke an iteration
    IterateStatus iterate();

    /// get number of parameters, based on nGaussians and bgType
    int numParams() {
        if( bgType == None)
            return nGaussians * 6;
        if( bgType == Constant)
            return nGaussians * 6 + 1;
        return nGaussians * 6 + 3;
    }

    /// callback for LevMar to calculate F
    static
    void levmarFFunc( const double * params, double * results, void * userData) {
        LMGaussFitter2d<Image> & gf = * (static_cast< LMGaussFitter2d<Image> * > ( userData));
        gf.calculateF( params, results);
    }

    /// function to compute F
    void calculateF( const double * paramsOrig, double * results) {
        // TODO: is this killing performace to put constraints here? or is
        // this the best we can do?
        double params[numParams()];
        for( int i = 0 ; i < numParams() ; i ++) params[i] = paramsOrig[i];
        constraintParameters( params);

        double * p = results;
        double f = 0.0, yy = 0.0;
        for( int y = y1 ; y <= y2 ; y ++ ) {
            for( int x = x1 ; x <= x2 ; x ++) {
                yy = img-> imageValue(x,y);
                if( isnan(yy)) {
                    * p = 0.0;
                } else {
//                    f = 0;
//                    for( int i = 0 ; i < nGaussians ; i ++ )
//                        f += evalGauss2di( x, y, & params[i*6]);
//                    f += params[ nGaussians*6];
//                    * p = yy - f;
                    f = evalNGauss2dBkg( x, y, nGaussians, bgType, params);
                    *p = yy - f;
                }
                p ++;
            }
        }
    }

    /// callback for LevMar to constraint parameters
    static
    void levmarConstraintsFunc( double * params, void * userData) {
        LMGaussFitter2d<Image> & gf = * (static_cast< LMGaussFitter2d<Image> * > ( userData));
        gf.constraintParameters( params);
    }

    /// function that constraints parameters
    void constraintParameters( double * params) {
        double maxSize = 2 * std::max(x2-x1+1,y2-y1+1);
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
            params[i*6+4] = clamp( params[i*6+4], 0.25, 4.0);
            // angle
//            params[i*6+5] = clamp( params[i*6+5],
//                    regionMin - (regionMax-regionMin),
//                    regionMax + (regionMax-regionMin));
        }
    }


    Image * img;
    int x1, x2, y1, y2;
    BackgroundType bgType;
    int nGaussians;

    /// guard to execute initOnce()
    bool firstTime;

    /// region min/max, used in constraints
    double regionMin, regionMax;

    LevMar levmar;

    std::vector<double> params;
//    double dbgGauss[10*6];


};

template <class Image>
LMGaussFitter2d<Image>::LMGaussFitter2d()
{
    // some basic tests for Image::Value capabilities (making sure its a number),
    // surrounded by if(0)
//    if( 0) {
//        typename Image::ImageValueType x = 1;
//        x ++;
//        x /= 2.0;
//        if( x < x + 1) { x = 0; }
//    }

    img = 0;
    x1 = x2 = y1 = y2 = 0;
    firstTime = true;
    nGaussians = 0;
    bgType = Constant;

}

template <class Image>
void LMGaussFitter2d<Image>::setImage( Image * pimg)
{
    img = pimg;
}

template <class Image>
void LMGaussFitter2d<Image>::setRect(int px1, int px2, int py1, int py2)
{
    if( ! img) throw std::runtime_error( "GaussFitter2d::uninitialized img");

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

template <class Image>
void LMGaussFitter2d<Image>::setNumGaussians( int ng)
{
    nGaussians = ng;
}

template <class Image>
void LMGaussFitter2d<Image>::setInitialParams(const std::vector<double> &v)
{
    if( int(v.size()) != numParams())
        throw std::runtime_error("LMGaussFitter2d::setInitialParams: bad param size");
    params = v;
}

template <class Image>
void LMGaussFitter2d<Image>::setGaussianParametersUsingHeuristics()
{
    dbg(1) << "Heuristics\n";
    dbg(1) << "  nG = " << nGaussians << "\n"
           << "  np = " << numParams() << "\n";
    QTime t; t.restart();
    double best = 0.0/0.0;
    params.resize( numParams(), 0.0);

    std::vector<double> test( params.size());
    std::vector<double> f((x2-x1+1) * (y2-y1+1));
    dbg(1) << "test size = " << test.size() << "\n";
    dbg(1) << "f size = " << f.size() << "\n";

    if( nGaussians == 1) {
        double dbgparams[] = { regionMax, (x2+x1)/2.0, (y2+y1)/2.0, x2-x1, 1, 0};
        for( int i = 0 ; i < 6 ; i ++ ) {
            params[i] = dbgparams[i];
        }

        calculateF( & params[0], & f[0]);
        double sumSq = 0.0;
        for( size_t i = 0 ; i < f.size() ; i ++ ) sumSq += f[i] * f[i];
        best = sumSq;
    }

    while( t.elapsed() < 100) {
        // generate random parameters 'test'
        for( int i = 0 ; i < nGaussians ; i ++ ) {
            test[i*6 + 0] = rnd(0, regionMax);
            test[i*6 + 1] = rnd(x1,x2);
            test[i*6 + 2] = rnd(y1,y2);
            test[i*6 + 3] = rnd(1,(x2 - x1));
            test[i*6 + 4] = rnd(0.5,2.0);
            test[i*6 + 5] = drand48() * 360;
        }
        if( bgType == Constant) {
            test[nGaussians * 6] = (regionMin + regionMax) / 2;
        } else if( bgType == Linear) {
            test[nGaussians * 6] = (regionMin + regionMax) / 2;
            test[nGaussians * 6+1] = 0.0;
            test[nGaussians * 6+2] = 0.0;
        }
        // get the error of this test
        calculateF( & test[0], & f[0]);
        double sumSq = 0.0;
        for( size_t i = 0 ; i < f.size() ; i ++ ) sumSq += f[i] * f[i];
        // if the error is better than what we had, remember this choice
        if( isnan(best) || sumSq < best) {
            best = sumSq;
            params = test;
        }
    }

    /*
    /// TODO: make this better
    for( int i = 0 ; i < nGaussians ; i ++ ) {
        params[i*6 + 0] = (regionMax + regionMin) / 2.0;
        double r = 0.5 * sqrt((x2-x1+1)*(y2-y1+1.0));
        double a = double(i) / nGaussians * M_PI * 2;
        params[i*6 + 1] = (x1+x2)/2.0 + r * sin(a);
        params[i*6 + 2] = (y1+y2)/2.0 + r * cos(a);
        params[i*6 + 3] = 0.5 * sqrt((x2-x1+1)*(y2-y1+1.0));
        params[i*6 + 4] = 1;
        params[i*6 + 5] = i * 30;
//        constraintParameters( & params[i*6]);
    }

    if( nGaussians == 1) {
        double dbgparams[] = { 100, (x2+x1)/2.0, (y2+y1)/2.0, x2-x1, 1, 0};
        for( int i = 0 ; i < 6 ; i ++ ) {
            params[i] = dbgparams[i] + 0.1;
        }
        params[6] = 123;
    }
    */

}

template < class Image >
std::vector<double> LMGaussFitter2d < Image >::getResults()
{
    const double * x = levmar.getSolutionRef();
    if( ! x) throw std::runtime_error("GaussFitter2d::getResults: x = null");

    std::vector<double> res( numParams());
    for( int i = 0 ; i < numParams() ; i ++ )
        res[i] = x[i];

    return res;
}

//template < class T >
//static void clamp( T & val, const T & v1, const T & v2)
//{
//    if( val < v1) val = v1;
//    if( val > v2) val = v2;
//}

template <class Image>
void LMGaussFitter2d<Image>::initOnce()
{
    if( ! firstTime) return;
    firstTime = false;

    if( ! img) throw std::runtime_error( "GaussFitter2d::uninitialized img");

    levmar.setStartParameters( params);
    levmar.setNumSamples( (x2-x1+1) * (y2-y1+1));
    levmar.setFFunc( levmarFFunc, this);
    levmar.setConstraintsFunc( levmarConstraintsFunc, this);
    levmar.init();

}


template <class Image>
typename LMGaussFitter2d<Image>::IterateStatus
LMGaussFitter2d<Image>::iterate()
{

    initOnce();

    LevMar::Status status = levmar.iterate();

    if( status == LevMar::Done) {
        dbg(1) << "Levmar finished\n";
        return Done;
    }

    if( status == LevMar::Error) {
        dbg(1) << "Levmar error\n";
        return Done;
    }

    return NotDone;
}

//template <class Image>
//const double * GaussFitter2d<Image>::getBackgroundParameters()
//{
//    return & params[nGaussians * 6];
//}

//template <class Image>
//const double * GaussFitter2d<Image>::getGaussianParameters(int which)
//{
//    return & params[which*6];
//}


} // namespace Optimization


#endif // LMGAUSSFITTER2D_H
