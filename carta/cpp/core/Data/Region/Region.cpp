#include "Region.h"
#include "Data/Util.h"
#include "State/UtilState.h"
#include "CartaLib/CartaLib.h"
#include "CartaLib/Regions/IRegion.h"
#include "CartaLib/Regions/Ellipse.h"

#include <QDebug>

namespace Carta {

namespace Data {

const QString Region::ACTIVE = "active";
const QString Region::HOVERED = "hovered";
const QString Region::REGION_TYPE = "regionType";

Region::Region(const QString& className, const QString& path, const QString& id )
:CartaObject( className, path, id ),
 m_shape(nullptr){
	m_regionNameSet = false;
	_initializeCallbacks();
	_initializeState();
}


void Region::editableChanged (){
}


QString Region::getCursor() const {
	return "";
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
        result = QString::compare( regionTypeStr, Carta::Lib::Regions::Ellipse::TypeName, Qt::CaseInsensitive );
        if ( result == 0 ){
            regionStr = Carta::Lib::Regions::Ellipse::TypeName;
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

void * Region::getUserData() const {
	return nullptr;
}

void Region::handleDrag( const QPointF & /*pt*/ ) {
}


void Region::handleDragDone( const QPointF & /*pt*/ ) {
}

void Region::handleDragStart( const QPointF & /*pt*/ ) {
}


void Region::handleEvent( Carta::Lib::InputEvents::JsonEvent & ev ) {
	 qDebug() << "Region got extra event" << ev.type();
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
    m_state.insertValue<bool>( ACTIVE, true );
    m_state.insertValue<bool>( HOVERED, true );
    m_state.insertValue<QString>( Util::NAME, "");
    m_state.flushState();
}


bool Region::isActive() const {
	return m_state.getValue<bool>( ACTIVE );
}


bool Region::isDeletable() const {
	return true;
}


bool Region::isEditable() const {
	return true;
}


bool Region::isHovered() const {
	return m_state.getValue<bool>( HOVERED );
}


bool Region::isSelected() const {
	return m_state.getValue<bool>( Util::SELECTED );
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


void Region::setActive( bool active ) {
	bool oldActive = isActive();
	if ( active != oldActive ){
		m_state.setValue<bool>( ACTIVE, active );
		m_state.flushState();
	}
}


void Region::setCursor( const QString & /*value*/ ) {

}


void Region::setDeletable( bool /*value*/ ) {

}


void Region::setEditable( bool /*editable*/ ) {

}


bool Region::setHovered( bool hovered ) {
	bool oldHovered = isHovered();
	bool redrawNeeded = false;
	if ( hovered != oldHovered ){
		m_state.setValue<bool>( HOVERED, hovered );
		m_state.flushState();
		redrawNeeded = true;
	}
	return redrawNeeded;
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

void Region::setSelected( bool selected ) {
	bool oldSelected = isSelected();
	if ( oldSelected != selected ){
		m_state.setValue<bool>( Util::SELECTED, selected );
		m_state.flushState();
	}
}


void Region::setUserData( void * /*value*/ ){

}

QJsonObject Region::toJSON() const {
    QString regionType = getRegionType();
    QJsonObject regionObject;
    regionObject.insert( REGION_TYPE, regionType );
    return regionObject;
}

Carta::Lib::VectorGraphics::VGList Region::getVGList() const {
	std::shared_ptr<Carta::Lib::Regions::RegionBase> regionBase = getModel();
	Carta::Lib::VectorGraphics::VGList graphicsList = regionBase->vgList();
	return graphicsList;
}


Region::~Region(){

}
}
}
