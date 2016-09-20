#include "Region.h"
#include "Data/Util.h"
#include "State/UtilState.h"
#include "Shape/ShapeBase.h"
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
	_initializeState();
}


QString Region::getCursor() const {
	return "";
}

std::shared_ptr<Carta::Lib::Regions::RegionBase> Region::getModel() const {
	std::shared_ptr<Carta::Lib::Regions::RegionBase> info (nullptr);
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

Carta::Lib::VectorGraphics::VGList Region::getVGList() const {
	Carta::Lib::VectorGraphics::VGList graphicsList = m_shape->getVGList();
	return graphicsList;
}


void Region::handleDrag( const Carta::Lib::InputEvents::Drag2Event& ev, const QPointF& location ){
	if ( isDraggable() ){
		Carta::Lib::InputEvents::Drag2Event::Phase phase = ev.phase();
		if ( phase == Carta::Lib::InputEvents::Drag2Event::Phase::Start ){
			handleDragStart( location );
		}
		else if ( phase == Carta::Lib::InputEvents::Drag2Event::Phase::Progress ){
			handleDrag( location );
		}
		else if ( phase == Carta::Lib::InputEvents::Drag2Event::Phase::End ){
			handleDragDone( location );
		}
		else {
			qWarning() << "Unrecognized drag event phase: "<<(int)(phase);
		}
	}
}

void Region::handleDrag( const QPointF & pt ) {
	if ( isDraggable() ){
		if ( m_shape ){
			m_shape->handleDrag( pt );
		}
	}
}


void Region::handleDragDone( const QPointF & pt ) {
	if ( isDraggable() ){
		if ( m_shape ){
			m_shape->handleDragDone( pt );
			if ( isEditMode() ){
				emit editDone();
			}
		}
	}
}

void Region::handleDragStart( const QPointF & pt ) {
	if ( isDraggable() ){
		if ( m_shape ){
			m_shape->handleDragStart( pt );
		}
	}
}

void Region::handleHover( const QPointF& pt ){
	//Only active shapes participate in user events
	if ( isActive() ){
		bool hovered = false;
		if ( m_shape->isPointInside( pt ) ){
			hovered = true;
		}
		setHovered( hovered );
	}
}

void Region::handleTouch( const QPointF& pt ){
	//Only active shapes participate in user events
	if ( isActive() ){
		bool tapped = false;
		if ( m_shape->isPointInside( pt ) ){
			tapped = true;
		}
		setSelected( tapped );
	}
}

void Region::handleTapDouble( const QPointF& /*pt*/ ){
	if ( isActive() ){
		qDebug() << "Unhandled region double tap "<<getRegionType();
	}
}


void Region::handleEvent( Carta::Lib::InputEvents::JsonEvent & ev ) {
	if ( isActive() ){
		qDebug() << "Unhandled region extra event" << ev.type()
				<<" region type="<<getRegionType();
	}
}


void Region::_initializeState(){
	m_state.insertValue<QString>( REGION_TYPE, "" );
	m_state.insertValue<bool>( Util::SELECTED, true );
	m_state.insertValue<bool>( ACTIVE, true );
	m_state.insertValue<bool>( HOVERED, true );
	m_state.insertValue<QString>( Util::NAME, "");
}


bool Region::isActive() const {
	return m_state.getValue<bool>( ACTIVE );
}


bool Region::isDraggable() const {
	bool draggable = isActive() && (isSelected() || isEditMode() );
	return draggable;
}

bool Region::isEditMode() const {
	return m_shape->isEditMode();
}


bool Region::isHovered() const {
	return m_state.getValue<bool>( HOVERED );
}


bool Region::isSelected() const {
	return m_state.getValue<bool>( Util::SELECTED );
}

void Region::_restoreState( const QString& stateStr ){
	Carta::State::StateInterface rectState( "");
	rectState.setState( stateStr );
	m_state = rectState;
	m_shape->setModel( toJSON() );
}


void Region::setActive( bool active ) {
	bool oldActive = isActive();
	if ( active != oldActive ){
		m_state.setValue<bool>( ACTIVE, active );
		m_state.flushState();
		m_shape->setActive( active );
	}
}


void Region::setCursor( const QString & /*value*/ ) {

}


void Region::setDeletable( bool /*value*/ ) {

}


void Region::setEditMode( bool editMode ) {
	m_shape->setEditMode( editMode );
}


bool Region::setHovered( bool hovered ) {
	bool oldHovered = isHovered();
	bool redrawNeeded = false;
	if ( hovered != oldHovered ){
		m_state.setValue<bool>( HOVERED, hovered );
		m_state.flushState();
		m_shape->setHovered( hovered );
		redrawNeeded = true;
	}
	return redrawNeeded;
}

void Region::setModel( Carta::Lib::Regions::RegionBase* /*model*/ ){

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
		m_shape->setSelected( selected );
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

void Region::_updateShapeFromState(){
	m_shape->setActive( isActive() );
	m_shape->setSelected( isSelected() );
	m_shape->setHovered( isHovered() );
}

Region::~Region(){

}
}
}
