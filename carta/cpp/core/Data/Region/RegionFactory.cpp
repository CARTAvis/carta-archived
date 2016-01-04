#include "Region.h"
#include "RegionFactory.h"
#include "RegionPolygon.h"
#include "RegionEllipse.h"
#include "Data/Util.h"

#include <QDebug>

namespace Carta {

namespace Data {


RegionFactory::RegionFactory(){
}


std::shared_ptr<Region>
RegionFactory::makeRegion( std::shared_ptr<Carta::Lib::RegionInfo> regionInfo ){
    Carta::Lib::RegionInfo::RegionType regionType = regionInfo->getRegionType();
    std::shared_ptr<Region> region = makeRegion( regionType );
    if ( region ){
        region ->setInfo( regionInfo );
    }
    return region;
}

std::shared_ptr<Region>
RegionFactory::makeRegion( Carta::Lib::RegionInfo::RegionType regionType ){
    std::shared_ptr<Region> region( nullptr);
    Carta::State::ObjectManager* objManager = Carta::State::ObjectManager::objectManager();
    if ( regionType == Carta::Lib::RegionInfo::RegionType::Polygon ){
        region.reset( objManager->createObject<RegionPolygon>( ) );
    }
    else if ( regionType == Carta::Lib::RegionInfo::RegionType::Ellipse ){
        region.reset( objManager->createObject<RegionEllipse>( ) );
    }
    else {
        qDebug() << "RegionFactory::makeRegion unsupported region type";
    }
    return region;
}


RegionFactory::~RegionFactory(){

}
}
}
