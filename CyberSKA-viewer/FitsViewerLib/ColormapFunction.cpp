#include "ColormapFunction.h"

PWLinear & PWLinear::add( double x, double y) {
    points_.push_back ( QPointF(x,y));
    return * this; // allow chaining
}

double PWLinear::operator()(double x) {
    if( ! std::isfinite(x) || points_.empty ())
        return std::numeric_limits<double>::quiet_NaN();
    // test boundary conditions
    if( x <= points_.first ().x ())
        return points_.first ().y ();
    if( x >= points_.last ().x ())
        return points_.last ().y ();
    // find the segment and interpolate within it
    for( int i = 1 ; i < points_.size() ; i ++ ) {
        if( x <= points_[i].x ()) {
            double a = (points_[i-1].y() - points_[i].y()) / (points_[i-1].x() - points_[i].x());
            double b = points_[i].y() - a * points_[i].x();
            return a * x + b;
        }
    }
    dbg(0) << ConsoleColors::error ()
           << "Weird. PWLinear could not find segment for x = " << x
           << ConsoleColors::resetln ();
    return std::numeric_limits<double>::quiet_NaN();
}

ColormapFunction::ColormapFunction()
{
    invert_ = false;
    reverse_ = false;
    redScale_ = greenScale_ = blueScale_ = 1.0;
    setScales( 0.0, 0.0);
}
ColormapFunction::ColormapFunction(
        PWLinear & red, PWLinear & green, PWLinear & blue)
{
    red_ = red; green_ = green; blue_ = blue;
    invert_ = false;
    reverse_ = false;
    redScale_ = greenScale_ = blueScale_ = 1.0;
    setScales( 0.0, 0.0);
}
Rgb ColormapFunction::operator()( double x) {
    if( ! std::isfinite( x))
        return Rgb(0,0,0);

////    x = x - 0.5 * scale1_;
////    x = (x-0.5) * exp( - 3 * scale2_) + 0.5;

//// version 1
////    double eps = 0.001;
////    double xx = (scale1_ + 1)/2;
////    double yy = (scale2_ + 1)/2;
////    xx = xx * ( 1 - 2 * eps) + eps;
////    yy = yy * ( 1 - 2 * eps) + eps;
////    double a = log(yy) / log(xx);
////    x = pow( x, a);

//// version 2
////    double max = 10;
////    double xx = (scale1_ + 1)/2;
////    double yy = scale2_;
////    double a = fabs(yy) * (max-1) + 1;
////    a = xx * (a-1) + 1;
////    if( yy > 0) a = 1/a;
////    x = pow( x, a);

//// version 3
//    double maxndig = 10;
//    double ndig = (scale2_ + 1) / 2 * maxndig;
//    double expo = std::pow( 2.0, ndig);
//    double xx = pow(scale1_ * 2, 3) / 8.0;
//    m_gamma = fabs(xx) * expo + 1;
//    if( scale1_ < 0) m_gamma = 1 / m_gamma;
    x = std::pow( x, m_gamma);

    if( reverse_) x = 1 - x;
    Rgb rgb( 255 * red_(x), 255 * green_(x), 255 * blue_(x));
    if( invert_) {
        rgb.r = 255 - rgb.r;
        rgb.g = 255 - rgb.g;
        rgb.b = 255 - rgb.b;
    }
    rgb.r = round ( rgb.r * redScale_);
    rgb.g = round ( rgb.g * greenScale_);
    rgb.b = round ( rgb.b * blueScale_);
    return rgb;
}
void ColormapFunction::setInvert (bool on) {
    invert_ = on;
}
void ColormapFunction::setReverse (bool on) {
    reverse_ = on;
}
void ColormapFunction::setRgbScales (double r, double g, double b) {
    redScale_ = r; greenScale_ = g; blueScale_ = b;
}

void ColormapFunction::setScales(double s1, double s2)
{
    scale1_ = clamp<double>( s1, -1.0, 1.0);
    scale2_ = clamp<double>( s2, -1.0, 1.0);

    //    x = x - 0.5 * scale1_;
    //    x = (x-0.5) * exp( - 3 * scale2_) + 0.5;

    // version 1
    //    double eps = 0.001;
    //    double xx = (scale1_ + 1)/2;
    //    double yy = (scale2_ + 1)/2;
    //    xx = xx * ( 1 - 2 * eps) + eps;
    //    yy = yy * ( 1 - 2 * eps) + eps;
    //    double a = log(yy) / log(xx);
    //    x = pow( x, a);

    // version 2
    //    double max = 10;
    //    double xx = (scale1_ + 1)/2;
    //    double yy = scale2_;
    //    double a = fabs(yy) * (max-1) + 1;
    //    a = xx * (a-1) + 1;
    //    if( yy > 0) a = 1/a;
    //    x = pow( x, a);

    // version 3
    double maxndig = 10;
    double ndig = (scale2_ + 1) / 2 * maxndig;
    double expo = std::pow( 2.0, ndig);
    double xx = pow(scale1_ * 2, 3) / 8.0;
    m_gamma = fabs(xx) * expo + 1;
    if( scale1_ < 0) m_gamma = 1 / m_gamma;
}

HistFunctor::HistFunctor( double min, double max)
{
    min_ = min; max_ = max;
    la_ = 255 / (max_ - min_);
    lb_ = - 255 * min_ / (max_ - min_) + 0.5;
}
Rgb HistFunctor::operator() ( double v) const {
    if( ! std::isfinite(v))
        return Rgb(0,0,0);
    if( v < min_)
        return Rgb(255,0,0);
    if( v > max_)
        return Rgb(0,255,0);
    int gray = round(v * la_ + lb_);
    if( gray < 0) gray = 0; if( gray > 255) gray = 255;

    return Rgb(gray, gray, gray);
}

HistogramColormapFunctor::HistogramColormapFunctor()
{
    histogramFunction_ = PWLinear().add (0,0.0).add (1,1.0);
    colormapFunction_ = ColormapFunction::gray ();
}

// combines histogram & colormap functions into one
HistogramColormapFunctor::HistogramColormapFunctor(
        double histMin, double histMax, ColormapFunction cmap)
{
    histogramFunction_ = PWLinear().add (histMin,0.0).add (histMax,1.0);
    colormapFunction_ = cmap;
}
Rgb HistogramColormapFunctor::operator() (double x)
{
    return colormapFunction_( histogramFunction_( x));
}

// PREDEFINED COLORMAPS
// --------------------

ColormapFunction
ColormapFunction::gray ()
{
    static ColormapFunction map;
    static bool initialized = false;
    if( ! initialized) {
        initialized = true;
        map = ColormapFunction(
                    PWLinear().add(0,0).add(1,1),
                    PWLinear().add(0,0).add(1,1),
                    PWLinear().add(0,0).add(1,1)
                    );
    }
    return map;
}

ColormapFunction
ColormapFunction::heat ()
{
    static ColormapFunction map;
    static bool initialized = false;
    if( ! initialized) {
        initialized = true;
        map = ColormapFunction(
                    PWLinear().add(0,0).add(1.0/3,1),
                    PWLinear().add(0,0).add(1,1),
                    PWLinear().add(2.0/3,0).add(1,1)
                    );
    }
    return map;
}

ColormapFunction
ColormapFunction::fire ()
{
    static ColormapFunction map;
    static bool initialized = false;
    if( ! initialized) {
        initialized = true;
        map = ColormapFunction(
                    PWLinear().add(0,0).add(1.0/2,1),
                    PWLinear().add(1.0/4,0).add(3.0/4,1),
                    PWLinear().add(1.0/2,0).add(1,1)
                    );
    }
    return map;
}

ColormapFunction
ColormapFunction::spring ()
{
    static ColormapFunction map;
    static bool initialized = false;
    if( ! initialized) {
        initialized = true;
        map = ColormapFunction(
                    PWLinear().add(0.25,0).add(0.75,1),
                    PWLinear().add(0,0).add(0.5,1),
                    PWLinear().add(0.5,0).add(1,1)
                    );
    }
    return map;
}

ColormapFunction
ColormapFunction::sea ()
{
    static ColormapFunction map;
    static bool initialized = false;
    if( ! initialized) {
        initialized = true;
        map = ColormapFunction(
                    PWLinear().add(0.5,0).add(1,1),
                    PWLinear().add(0,0).add(0.5,1),
                    PWLinear().add(0.25,0).add(0.75,1)
                    );
    }
    return map;
}

ColormapFunction
ColormapFunction::sunbow ()
{
    static ColormapFunction map;
    static bool initialized = false;
    if( ! initialized) {
        initialized = true;
        map = ColormapFunction(
                    PWLinear().add(0.5,1).add(0.75,0),
                    PWLinear().add(0,0).add(0.25,1).add (0.75,1).add (1,0),
                    PWLinear().add(0.25,0).add(0.5,1)
                    );
    }
    return map;
}

ColormapFunction
ColormapFunction::mutant ()
{
    static ColormapFunction map;
    static bool initialized = false;
    if( ! initialized) {
        initialized = true;
        map = ColormapFunction(
                    PWLinear().add(0,0).add (0.25,1).add (0.75,1).add (1,0),
                    PWLinear().add(0.5,1).add (0.75,0),
                    PWLinear().add(0.25,0).add(0.5,1)
                    );
    }
    return map;
}

ColormapFunction
ColormapFunction::aberration ()
{
    static ColormapFunction map;
    static bool initialized = false;
    if( ! initialized) {
        initialized = true;
        map = ColormapFunction(
                    PWLinear().add(0.5,0).add (0.75,1),
                    PWLinear().add(0.25,0).add (0.5,1),
                    PWLinear().add(0,0).add(0.25,1).add (0.75,1).add (1,0)
                    );
    }
    return map;
}

ColormapFunction
ColormapFunction::rgb ()
{
    static ColormapFunction map;
    static bool initialized = false;
    if( ! initialized) {
        initialized = true;
        map = ColormapFunction(
                    PWLinear().add(0,0).add (0.25,1).add (0.5,0),
                    PWLinear().add(0.25,0).add (0.5,1).add (0.75,0),
                    PWLinear().add(0.5,0).add (0.75,1).add (1,0)
                    );
    }
    return map;
}

ColormapFunction
ColormapFunction::velocity ()
{
    static ColormapFunction map;
    static bool initialized = false;
    if( ! initialized) {
        initialized = true;
        map = ColormapFunction(
                    PWLinear().add(0.5,0).add (1,1),
                    PWLinear().add(0,0).add (0.5,1).add (1,0),
                    PWLinear().add(0,1).add (0.5,0)
                    );
    }
    return map;
}

CachedRgbFunction::CachedRgbFunction()
{
    init( HistogramColormapFunctor(0,1,ColormapFunction::heat()), 0, 1, 10000, Rgb(0,0,0));
}

void
CachedRgbFunction::init (
        HistogramColormapFunctor fn,
        double min,
        double max,
        int n,
        Rgb nanColor)
{
    min_ = min;
    max_ = max;
    nanColor_ = nanColor;

    cache.resize ( n);
    diff = max - min;
    n1 = n - 1;
    dInvN1 = n1 / diff ;
    double x; int i;
    double delta = diff / n;
    for( i = 0 , x = min ; i < n ; i ++ , x+= delta ) {
        cache[i] = fn( x);
    }
    if( diff <= 0) {
        minRes = fn( min - 1e-9);
        maxRes = fn( max + 1e-9);
    } else {
        minRes = cache.first ();
        maxRes = cache.last ();
    }
}

CachedRgbFunction::CachedRgbFunction(
        HistogramColormapFunctor fn,
        double min,
        double max,
        int n,
        Rgb nanColor)
{
    init( fn, min, max, n, nanColor);
}

// cube helix function
// as described here: http://www.mrao.cam.ac.uk/~dag/CUBEHELIX/
// and in
//  Green, D. A., 2011, Bulletin of the Astronomical Society of India, Vol.39, p.289
ColormapFunction
ColormapFunction::cubeHelix (double start, double rots, double hue, double gamma)
{
    PWLinear red, green, blue;

    int nlev = 1000;
    for( int i = 0 ; i < nlev ; i ++ ) {

        double fract = double(i) / nlev;
        double angle = 2 * M_PI * ( start/3 + 1 + rots + fract);
        fract = pow( fract, gamma);

        double amp = hue * fract * (1-fract)/2.0;

        double r = fract + amp*(-0.14861*cos(angle)+1.78277*sin(angle));
        double g = fract + amp*(-0.29227*cos(angle)-0.90649*sin(angle));
        double b = fract + amp*(+1.97294*cos(angle));

        if( r < 0) r = 0;
        if( r > 1) r = 1;
        if( g < 0) g = 0;
        if( g > 1) g = 1;
        if( b < 0) b = 0;
        if( b > 1) b = 1;

        red.add( fract, r);
        green.add( fract, g);
        blue.add( fract, b);
    }
    return ColormapFunction( red, green, blue);
}

/*
pflib.cachedFunction = function(p) {
    p = Ext.apply({}, p, {
        min: 0, max: 1, n: 1000
    });
    if(p.nanRes === undefined) p.nanRes = p.fn(p.min);

    var cache = new Array(p.n);
    var min = p.min, max = p.max, n = p.n, n1 = n-1;
    var d = max - min, dInv = 1/d, dInvN1 = dInv * n1;
    for( var x = p.min, i = 0 ; x <= p.max ; x += d / n , i ++)
        cache[i] = p.fn(x);
    var minRes = cache[0];
    var maxRes = cache[n1];
    var nanRes = p.nanRes;

    if( d <= 0) {
        minRes = p.fn(min-1/1000000000);
        maxRes = p.fn(max+1/1000000000);
        return function(x) {
            if( isNaN(x)) return nanRes;
            if( x < min) return minRes
            return maxRes;
        }
    } else {
        return function(x) {
            if( isNaN(x)) return nanRes;
            var ind = Math.round( (x-min)*dInvN1);
            if( ind < 0) return minRes;
            if( ind > n1) return maxRes;
            return cache[ind];
        };
    }
};
*/
