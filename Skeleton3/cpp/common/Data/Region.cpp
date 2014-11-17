#include "Region.h"
#include "RegionRectangle.h"
#include "Util.h"

#include <QDebug>

Region::Region(const QString& className, const QString& path, const QString& id )
    :CartaObject( className, path, id ){
    _initializeCallbacks();
}

QString Region::makeRegion( const QString& type ){
    QString regionPath;
    if ( type == RegionRectangle::CLASS_NAME ){
        ObjectManager* objManager = ObjectManager::objectManager();
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
        QList<QString> keys = {"info"};
        QVector<QString> dataValues = Util::parseParamMap( params, keys );
        if ( dataValues.size() == keys.size()){
            resetState( dataValues[0]);
        };
        return "";
    });
}

Region::~Region(){

}
