/*
 *  Functionality expected of a histogram implementation.
 */

#pragma once

#include <vector>
#include <QString>


/**
 * Generates and Manages the data corresponding to a histogram.
 */

class IImageHistogram{
public:

    /* Recomputes the histogram data.
     * @return true if the data was successfully generated; false otherwise.
     */
    virtual bool compute() = 0;

    /**
     * Returns the histogram data as (intensity,count) pairs.
     * @return a list of (intensity,count) data pairs.
     */
    virtual std::vector< std::pair<double,double> > getData() const = 0;

    /**
     * Returns a display name for the histogram.
     * @return a display name.
     */
    virtual QString getName() const = 0;

    /**
     * Returns the intensity units.
     * @return the image intensity units.
     */
    virtual QString getUnits() const = 0;

    /**
     * Sets the number of bins in the histogram.
     * @param count the number of histogram bins.
     */
    virtual void setBinCount( int count ) = 0;

    /**
     * Set the range of channels to include in the histogram.
     * @param minChannel a lower bound for the range of channels to include or -1 for no minimum.
     * @param maxChannel an upper bound for the range of channels to include or -1 for no maximum.
     */
    virtual void setChannelRange( int minChannel, int maxChannel )=0;

    /**
     * Sets the minimum and maximum intensity for the histogram.
     * @param minimumIntensity the minimum display intensity.
     * @param maximumIntensity the maximum display intensity.
     */
    virtual void setIntensityRange( double minimumIntensity, double maximumIntensity )=0;


protected:
    virtual ~IImageHistogram();
    IImageHistogram();
};
