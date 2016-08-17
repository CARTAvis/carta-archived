#include "Region.h"
#include "Data/Util.h"
#include "State/UtilState.h"
#include "CartaLib/CartaLib.h"

#include <QDebug>
#include <QString>

namespace Carta {

namespace Data {

const QString Region::REGION_TYPE = "regionType";

Region::Region(const QString& className, const QString& path, const QString& id )
    :CartaObject( className, path, id ){
    m_regionNameSet = false;
    _initializeCallbacks();
    _initializeState();
}




std::shared_ptr<Carta::Lib::Regions::RegionBase> Region::getInfo() const {
    std::shared_ptr<Carta::Lib::Regions::RegionBase> info( nullptr);
    return info;
}

QString Region::getRegionName() const {
    QString name = m_state.getValue<QString>( Util::NAME );
    return name;
}


QString Region::getRegionType( const QString& regionTypeStr ){
    QString regionStr;
    int result = QString::compare( regionTypeStr, Carta::Lib::Regions::Polygon::TypeName, Qt::CaseInsensitive );
    if ( result == 0 ){
        regionStr = Carta::Lib::Regions::Polygon::TypeName;
    }
    else {
        result = QString::compare( regionTypeStr, Carta::Lib::Regions::Circle::TypeName, Qt::CaseInsensitive );
        if ( result == 0 ){
            regionStr = Carta::Lib::Regions::Circle::TypeName;
        }
    }
    return regionStr;
}


QString Region::getRegionType() const {
    QString regionTypeStr = m_state.getValue<QString>( REGION_TYPE );
    return regionTypeStr;
}

QString Region::_getStateString() const {
    return m_state.toString();
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

void Region::_initializeState(){
    m_state.insertValue<QString>( REGION_TYPE, "" );
    m_state.insertValue<bool>( Util::SELECTED, true );
    m_state.insertValue<QString>( Util::NAME, "");
    m_state.flushState();
}

void Region::_restoreState( const QString& stateStr ){
    Carta::State::StateInterface dataState( "" );
    dataState.setState( stateStr );
    QString regionType = dataState.getValue<QString>(REGION_TYPE);
    m_state.setValue<QString>( REGION_TYPE, regionType);
    QString name = dataState.getValue<QString>( Util::NAME );
    m_state.setValue<QString>(Util::NAME, name );
    m_state.flushState();
}


QString Region::setRegionName( const QString& name ){
    QString result;
    if ( name.trimmed().length() > 0 ){
        QString oldName = m_state.getValue<QString>( Util::NAME );
        if ( oldName != name ){
            m_regionNameSet = true;
            m_state.setValue<QString>( Util::NAME, name );
            m_state.flushState();
        }
    }
    return result;
}


QJsonObject Region::toJSON() const {
    QString regionType = getRegionType();
    QJsonObject regionObject;
    regionObject.insert( REGION_TYPE, regionType );
    return regionObject;
}


Region::~Region(){

}
}
}
