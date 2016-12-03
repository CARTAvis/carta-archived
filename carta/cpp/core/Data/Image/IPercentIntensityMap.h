/*
 * Interface implemented by classes wanting to receive notification of
 * color map changes.
 */

#pragma once

#include <QString>



namespace Carta {

namespace Data {

class DataContours;

class IPercentIntensityMap {

public:

    /**
     * Returns the intensity corresponding to a given percentile in the current frame.
     * @param percentiles - a list of numbers in [0,1] for which an intensity is desired.
     * @return - a list of corresponding (location,intensity) pairs.
     */
    virtual std::vector<std::pair<int,double> > getIntensity( const std::vector<double>& percentile) const = 0;

    /**
     * Return the percentile corresponding to the given intensity in the current frame.
     * @param intensity a value for which a percentile is needed.
     * @return the percentile corresponding to the intensity.
     */
    virtual double getPercentile( int frameLow, int frameHigh, double intensity ) const = 0;

    /**
     * Add a contour set.
     * @param contourSet - the contour set to add.
     */
    virtual void addContourSet( std::shared_ptr<DataContours> contourSet) = 0;

    /**
     * Remove a contour set.
     * @param contourSet - the contour set to remove.
     */
    virtual void removeContourSet( std::shared_ptr<DataContours> contourSet) = 0;
};
}
}

