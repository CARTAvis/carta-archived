/*
 * Interface implemented by classes wanting to receive notification of
 * color map changes.
 */

#pragma once

#include <QString>
#include "CartaLib/IntensityUnitConverter.h"



namespace Carta {

namespace Data {

class DataContours;

class IPercentIntensityMap {

public:

    /**
     * Returns the intensity corresponding to a given percentile in the current frame.
     * @param percentiles - a list of numbers in [0,1] for which an intensity is desired.
     * @return - a list of corresponding intensity values.
     */
    virtual std::vector<double> getIntensity( const std::vector<double>& percentile, Carta::Lib::IntensityUnitConverter::SharedPtr converter=nullptr) const = 0;

    /**
     * Return the percentiles corresponding to the given intensities.
     * @param frameLow a lower bound for the channel range or -1 if there is no lower bound.
     * @param frameHigh an upper bound for the channel range or -1 if there is no upper bound.
     * @param intensities values for which percentiles are needed.
     * @return the percentiles corresponding to the intensities.
     */
    virtual std::vector<double> getPercentiles( int frameLow, int frameHigh, std::vector<double> intensities, Carta::Lib::IntensityUnitConverter::SharedPtr converter=nullptr ) const = 0;

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

