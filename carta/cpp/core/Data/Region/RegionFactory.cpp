#include "RegionFactory.h"
#include "RegionPolygon.h"
#include "RegionEllipse.h"
#include "Data/Util.h"
#include "CartaLib/Regions/Ellipse.h"
#include <QPolygonF>
#include <QDebug>

namespace Carta {

namespace Data {


RegionFactory::RegionFactory(){
}


std::shared_ptr<Region>
RegionFactory::makeRegion( std::shared_ptr<Carta::Lib::Regions::RegionBase> regionInfo ){
    std::shared_ptr<Region> region( nullptr );
    if ( regionInfo ){
        QString regionType = regionInfo->typeName();
        region = _makeRegionType( regionType );
        if ( regionType == Carta::Lib::Regions::Polygon::TypeName ){
            Carta::Lib::Regions::Polygon* polyRegion = dynamic_cast<Carta::Lib::Regions::Polygon*>( regionInfo.get() );
            QPolygonF poly = polyRegion->qpolyf();
            int cornerCount = poly.size();
            std::vector<std::pair<double,double> > pairs( cornerCount );
            for ( int i = 0; i < cornerCount; i++ ){
                QPointF pt = poly.value(i);
                pairs[i] = std::pair<double,double>( pt.x(), pt.y());
            }
            RegionPolygon* regionPoly = dynamic_cast<RegionPolygon*>(region.get());
            regionPoly->addCorners( pairs );
        }
    }
    return region;
}

std::shared_ptr<Region> RegionFactory::_makeRegionType( const QString& regionType ){
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    std::shared_ptr<Region> region( nullptr );
    if ( regionType == Carta::Lib::Regions::Polygon::TypeName ){
        region.reset( objMan->createObject<RegionPolygon>() );

    }
    else if ( regionType == Carta::Lib::Regions::Ellipse::TypeName ){
        region.reset( objMan->createObject<RegionEllipse>() );
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
    std::shared_ptr<Region> region = _makeRegionType( regionType );
    if ( region ){
        region->_restoreState( regionState );
    }
    return region;
}

RegionFactory::~RegionFactory(){

}
}
}
