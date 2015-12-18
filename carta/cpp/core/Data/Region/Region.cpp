#include "Region.h"
#include "RegionPolygon.h"
#include "RegionEllipse.h"
#include "Data/Util.h"
#include "State/UtilState.h"

#include <QDebug>

namespace Carta {

namespace Data {

const QString Region::POLYGON_REGION = "Polygon";
const QString Region::ELLIPSE_REGION = "Ellipse";

Region::Region(const QString& className, const QString& path, const QString& id )
    :CartaObject( className, path, id ){
    _initializeCallbacks();
}

Carta::Lib::RegionInfo::RegionType Region::getRegionType( const QString& regionTypeStr ){
    Carta::Lib::RegionInfo::RegionType regionType = Carta::Lib::RegionInfo::RegionType::Unknown;
    int result = QString::compare( regionTypeStr, POLYGON_REGION, Qt::CaseInsensitive );
    if ( result == 0 ){
        regionType = Carta::Lib::RegionInfo::RegionType::Polygon;
    }
    else {
        result = QString::compare( regionTypeStr, ELLIPSE_REGION, Qt::CaseInsensitive );
        if ( result == 0 ){
            regionType = Carta::Lib::RegionInfo::RegionType::Ellipse;
        }
    }
    return regionType;
}

void Region::_initializeCallbacks(){
    addCommandCallback( "shapeChanged", [=] (const QString & /*cmd*/,
                                    const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = { "info"};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        //resetStateData( dataValues[ *keys.begin()]);
        return "";
    });
}

QString Region::makeRegion( const QString& type ){
    QString regionPath;
    Carta::Lib::RegionInfo::RegionType regionType = getRegionType( type );
    if ( regionType == Carta::Lib::RegionInfo::RegionType::Polygon ){
        Carta::State::ObjectManager* objManager = Carta::State::ObjectManager::objectManager();
        Region* region = objManager->createObject<RegionPolygon>( );
        regionPath = region->getId();
    }
    else if ( regionType == Carta::Lib::RegionInfo::RegionType::Ellipse ){
        Carta::State::ObjectManager* objManager = Carta::State::ObjectManager::objectManager();
        Region* region = objManager->createObject<RegionEllipse>( );
        regionPath = region->getId();
    }
    else {
        qDebug() << "Region::makeRegion unsupported region type";
    }
    return regionPath;
}


Region::~Region(){

}
}
}
