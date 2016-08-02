/***
 * Maps a percentile to an intensity.
 */

#pragma once

#include <QString>

namespace Carta {

namespace Data {

class LeastRecentlyUsedCacheEntry {
public:
    /**
     * Constructor.
     * @param frameLow - lower bound for the spectral index.
     * @param frameHigh - upper bound for the spectral index.
     * @param location - index of the source of the information in the image.
     * @param percentile - a number in [0,1].
     * @param intensity - the intensity corresponding to the percentile.
     */
    LeastRecentlyUsedCacheEntry( int frameLow, int frameHigh, int location,
            double percentile, double intensity );

    /**
     * Returns the lower boundary of the spectral index.
     * @return - the lower boundary of the spectral index.
     */
    int getFrameLow() const;

    /**
     * Returns the upper boundary of the spectral index.
     * @return - the upper boundary of the spectral index.
     */
    int getFrameHigh() const;

    /**
     * Returns the index in the image data where the information is located.
     * @return - the image data index.
     */
    int getLocation() const;

    /**
     * Return the percentile.
     * @return - the percentile.
     */
    double getPercentile() const;

    /**
     * Return the intensity.
     * @return - the intensity.
     */
    double getIntensity() const;

    /**
     * Equality operator.
     * @param other - the entry to compare this one to.
     * @return - true if the entries are equal; false otherwise.
     */
    bool operator==( const LeastRecentlyUsedCacheEntry& other ) const;

    /**
     * Returns a string representation of the cache entry.
     * @return - a string representation of the cache entry.
     */
    QString toString() const;

    /**
     * Destructor.
     */
    ~LeastRecentlyUsedCacheEntry();

private:
    int m_frameLow;
    int m_frameHigh;
    int m_location;
    double m_percentile;
    double m_intensity;
    LeastRecentlyUsedCacheEntry& operator=( const LeastRecentlyUsedCacheEntry& other );
};
}
}
