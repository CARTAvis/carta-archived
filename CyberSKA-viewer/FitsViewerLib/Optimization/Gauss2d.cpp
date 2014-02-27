#include <iostream>
#include "Gauss2d.h"

namespace Optimization {

enum { HEIGHT=0, XCENTER, YCENTER, YWIDTH, RATIO, ANGLE};

// convert 6 parameters in an array to nice (named) parameters
Gauss2dNiceParams Gauss2dNiceParams::convert(const double *p)
{

    Gauss2dNiceParams res;

    res.amp = p[HEIGHT];
    res.xc = p[XCENTER];
    res.yc = p[YCENTER];
    res.major = p[YWIDTH];
    res.minor = p[RATIO] * res.major;
    res.angle = p[ANGLE];

    if( res.major < res.minor) {
        std::swap( res.major, res.minor);
        res.angle += 90;
    }

    while( res.angle < -90) res.angle += 180;
    while( res.angle > 90) res.angle -= 180;

    return res;

}

} // napespace Optimization
