#include "RegionFactory.h"
#include "RegionPolygon.h"
#include "RegionEllipse.h"
#include "RegionRectangle.h"
#include "RegionPoint.h"
#include "Data/Util.h"
#include "CartaLib/Regions/Ellipse.h"
#include "CartaLib/Regions/Rectangle.h"
#include "CartaLib/Regions/Point.h"
#include <QDebug>

namespace Carta {

namespace Data {


RegionFactory::RegionFactory(){
}


std::shared_ptr<Region> RegionFactory::makeRegion( Carta::Lib::Regions::RegionBase* regionInfo ){
    std::shared_ptr<Region> region( nullptr );
    if ( regionInfo ){
        QString regionType = regionInfo->typeName();
        region = makeRegionType( regionType );
        region->setModel( regionInfo );

    }
    return region;
}

std::shared_ptr<Region> RegionFactory::makeRegionType( const QString& regionType ){
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    std::shared_ptr<Region> region( nullptr );
    if ( regionType == Carta::Lib::Regions::Rectangle::TypeName ){
    	region.reset( objMan->createObject<RegionRectangle>() );
    }
    else if ( regionType == Carta::Lib::Regions::Polygon::TypeName ){
        region.reset( objMan->createObject<RegionPolygon>() );
    }
    else if ( regionType == Carta::Lib::Regions::Ellipse::TypeName ){
        region.reset( objMan->createObject<RegionEllipse>() );
    }
    else if ( regionType == Carta::Lib::Regions::Point::TypeName ){
    	region.reset( objMan->createObject<RegionPoint>() );
    }
    else {
        qWarning()<<"makeRegion:: Unrecognized region: "<<regionType;
    }
    return region;
}


std::shared_ptr<Region> RegionFactory::makeRegion( const QString& regionState ){
    Carta::State::StateInterface rState("");
    rState.setState( regionState );
    QString regionType = rState.getValue<QString>( Region::REGION_TYPE );
    std::shared_ptr<Region> region = makeRegionType( regionType );
    if ( region ){
        region->_restoreState( regionState );
    }
    return region;
}

RegionFactory::~RegionFactory(){

}
}
}
