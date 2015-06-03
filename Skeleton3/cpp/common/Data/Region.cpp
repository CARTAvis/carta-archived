#include "Region.h"
#include "RegionRectangle.h"
#include "Util.h"
#include "State/UtilState.h"

#include <QDebug>

namespace Carta {

namespace Data {

Region::Region(const QString& className, const QString& path, const QString& id )
    :CartaObject( className, path, id ){
    _initializeCallbacks();
}

QString Region::makeRegion( const QString& type ){
    QString regionPath;
    if ( type == RegionRectangle::CLASS_NAME ){
        Carta::State::ObjectManager* objManager = Carta::State::ObjectManager::objectManager();
        regionPath = objManager->createObject( type );
    }
    else {
        qDebug() << "Region::makeRegion unsupported region type:"<<type;
    }
    return regionPath;
}



void Region::_initializeCallbacks(){
    addCommandCallback( "shapeChanged", [=] (const QString & /*cmd*/,
                                    const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = { "info"};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        resetStateData( dataValues[ *keys.begin()]);
        return "";
    });
}

Region::~Region(){

}
}
}
