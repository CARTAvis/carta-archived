#include "Region.h"
#include "RegionFactory.h"
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
        region ->addCorners( regionInfo->getCorners() );
    }
    return region;
}

std::shared_ptr<Region>
RegionFactory::makeRegion( Carta::Lib::RegionInfo::RegionType regionType ){
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    std::shared_ptr<Region> region( objMan->createObject<Region>() );
    region->setRegionType( regionType );
    return region;
}

std::shared_ptr<Region>
RegionFactory::makeRegion( const QString& regionState ){
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    std::shared_ptr<Region> region( objMan->createObject<Region>() );
    region->_restoreState( regionState );
    return region;
}
RegionFactory::~RegionFactory(){

}
}
}
