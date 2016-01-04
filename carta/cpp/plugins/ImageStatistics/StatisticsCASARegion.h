/**
 * Generates statistics for a region of an image.
 */

#pragma once

#include <QMap>
#include <QString>
#include "CartaLib/RegionInfo.h"
#include "casacore/images/Images/ImageInterface.h"
#include "casacore/casa/Containers/Record.h"

class StatisticsCASARegion {

public:

    /**
     * Returns a map of (key,value) pairs representing the statistics of the specified region
     * in the specified image.
     * @param image - a specified image.
     * @param regionInfo - a specified region.
     * @return - a map of (key,value) pairs which are the statistics for the region in the
     *      image.
     */
    static QMap<QString,QString>
    getStats( const casa::ImageInterface<casa::Float>* image, Carta::Lib::RegionInfo& regionInfo );
private:
    StatisticsCASARegion();
    static void _insertScalar( const casa::Record& result, const casa::String& key,
            const QString& label, QMap<QString,QString>& stats );
    static void _insertList( const casa::Record& result, const casa::String& key,
            const QString& label, QMap<QString,QString>& stats );
    static void _insertString( const casa::Record& result, const casa::String& key,
            const QString& label, QMap<QString,QString>& stats );

    virtual ~StatisticsCASARegion();

};
