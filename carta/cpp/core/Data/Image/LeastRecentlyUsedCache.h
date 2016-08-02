/***
 * A least recently used cache of percentile<->intensity lookup information.
 */

#pragma once

#include "LeastRecentlyUsedCacheEntry.h"
#include <QLinkedList>
namespace Carta {

namespace Data {


class LeastRecentlyUsedCache {

public:

    /**
     * Constructor.
     * @param size - the size of the cache.
     */
    LeastRecentlyUsedCache(int size);

    /**
     * Insert an entry into the cache.
     * @param frameLow - lower channel bound of the cache.
     * @param frameHigh - upper channel bound of the cache.
     * @param location - index in the image of the entry.
     * @param percentile - the percentile as a number in [0,1].
     * @param intensity - the intensity.
     */
    void put(int frameLow, int frameHigh, int location, double percentile, double intensity );

    /**
     * Returns the (location,intensity) corresponding to the given frame range and
     * percentile.
     * @param frameLow - lower boundary of the channel range.
     * @param frameHigh - upper boundary of the channel range.
     * @param percentile - the percentile in [0,1] that is needed.
     * @return - the (location,intensity) corresponding to the passed in information.
     */
    std::pair<int,double> getIntensity( int frameLow, int frameHigh, double percentile);

    /**
     * Return a string representation of the contents of the cache.
     * @return - a string representation of the contents of the cache.
     */
    QString toString() const;

    /**
     * Destructor.
     */
    ~LeastRecentlyUsedCache();

private:
    //Removes the item at the specified index from the cache and inserts it at the
    //end in order to update its time stamp.
    void _refresh( int i );
    QLinkedList<LeastRecentlyUsedCacheEntry> m_cache;
    int m_maxSize;
    static const double ERROR_MARGIN;
	LeastRecentlyUsedCache& operator=( const LeastRecentlyUsedCache& other );
};
}
}
