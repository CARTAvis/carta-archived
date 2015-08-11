/*
 * Interface implemented by classes wanting to receive notification of
 * color map changes.
 */

#pragma once

#include <QString>

namespace Image {
class ImageInterface;
}

namespace Carta {

namespace Data {

class IPercentIntensityMap {

public:

    /**
     * Returns the intensity corresponding to a given percentile in the current frame.
     * @param percentile a number [0,1] for which an intensity is desired.
     * @param intensity the computed intensity corresponding to the percentile.
     * @return true if the computed intensity is valid; otherwise false.
     */
    virtual bool getIntensity( double percentile, double* intensity ) const = 0;

    /**
     * Return the percentile corresponding to the given intensity in the current frame.
     * @param intensity a value for which a percentile is needed.
     * @return the percentile corresponding to the intensity.
     */
    virtual double getPercentile( int frameLow, int frameHigh, double intensity ) const = 0;
};
}
}

