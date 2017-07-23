#include "LeastRecentlyUsedCache.h"
#include <QDebug>

namespace Carta {

namespace Data {

const double LeastRecentlyUsedCache::ERROR_MARGIN = 0.000001;

LeastRecentlyUsedCache::LeastRecentlyUsedCache(int size){
    m_maxSize = size;
}


std::pair<int,double> LeastRecentlyUsedCache::getIntensity(int frameLow, int frameHigh, double percentile, int stokeFrame ){
    std::pair<int,double> intensities( -1, 0 );
    int position = 0;
    for ( QLinkedList<LeastRecentlyUsedCacheEntry>::iterator iter = m_cache.begin();
         iter != m_cache.end(); iter++ ){
        if ( (*iter).getFrameLow() == frameLow &&
             (*iter).getFrameHigh() == frameHigh &&
             (*iter).getStokeFrame() == stokeFrame &&
             qAbs( percentile - (*iter).getPercentile() )< ERROR_MARGIN ) {
            intensities.first = (*iter).getLocation();
            intensities.second = (*iter).getIntensity();
            _refresh(position);
            break;
        }
        position++;
    }
    return intensities;
}

void LeastRecentlyUsedCache::put(int frameLow, int frameHigh, int location, double percentile, double intensity, int stokeFrame){
    LeastRecentlyUsedCacheEntry entry( frameLow, frameHigh, location, percentile, intensity, stokeFrame);
    if ( ! m_cache.contains( entry )){
        //Store at the end of the list.
        m_cache.push_back( entry );
        //If we have exceeded the maximum size, remove the oldest entry at the front.
        if ( m_cache.size() > m_maxSize ){
            m_cache.pop_front();
        }
    }
}

void LeastRecentlyUsedCache::_refresh( int i ){
    QLinkedList<LeastRecentlyUsedCacheEntry>::iterator iter = m_cache.begin();
    iter = iter + i;
    LeastRecentlyUsedCacheEntry entry = (*iter);
    m_cache.erase( iter );
    m_cache.push_back( entry );
}

QString LeastRecentlyUsedCache::toString() const {
    int count = m_cache.size();
    QString result("Cache size=" + QString::number(count) + "\n");
    for ( QLinkedList<LeastRecentlyUsedCacheEntry>::const_iterator iter = m_cache.begin();
             iter != m_cache.end(); iter++ ){
        result = result + "Entry=" +iter->toString() + "\n";
    }
    return result;
}

LeastRecentlyUsedCache::~LeastRecentlyUsedCache(){

}
}
}