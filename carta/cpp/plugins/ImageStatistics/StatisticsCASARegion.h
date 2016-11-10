/**
 * Generates statistics for a region of an image.
 */

#pragma once

#include <QMap>
#include <QString>

#include "CartaLib/Regions/IRegion.h"
#include "CartaLib/StatInfo.h"
#include "casacore/images/Images/ImageInterface.h"
#include "casacore/casa/Containers/Record.h"

class StatisticsCASARegion {

public:

    /**
     * Returns a map of (key,value) pairs representing the statistics of the specified region
     * in the specified image.
     * @param image - a specified image.
     * @param regionInfo - a specified region.
     * @param slice - information about the frames that are selected on the image.
     * @return - a map of (key,value) pairs which are the statistics for the region in the
     *      image.
     */
    static QList<Carta::Lib::StatInfo>
    getStats( casa::ImageInterface<casa::Float>* image, std::shared_ptr<Carta::Lib::Regions::RegionBase> regionInfo,
            const std::vector<int>& slice );
private:
    StatisticsCASARegion();
    static void _getStatsFromCalculator( casa::ImageInterface<casa::Float>* image,
           const casa::Record& region, const std::vector<int>& slice,
           QList<Carta::Lib::StatInfo>& stats, const QString& typeStr );

    static void _insertScalar( const casa::Record& result, const casa::String& key,
            Carta::Lib::StatInfo::StatType statType, QList<Carta::Lib::StatInfo>& stats );
    static void _insertList( const casa::Record& result, const casa::String& key,
            Carta::Lib::StatInfo::StatType statType, QList<Carta::Lib::StatInfo>& stats );
    static void _insertString( const casa::Record& result, const casa::String& key,
            Carta::Lib::StatInfo::StatType statType, QList<Carta::Lib::StatInfo>& stats );
    static QString _vectorToString( const casa::Vector<int>& valArray );

    virtual ~StatisticsCASARegion();

};
