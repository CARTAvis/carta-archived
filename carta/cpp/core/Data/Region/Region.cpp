#include "Region.h"
#include "Data/Region/RegionTypes.h"
#include "Data/Util.h"
#include "State/UtilState.h"
#include "Shape/ShapeBase.h"
#include "CartaLib/CartaLib.h"
#include "CartaLib/Regions/IRegion.h"
#include "CartaLib/Regions/Ellipse.h"

#include <QDebug>
#include <QtMath>

namespace Carta {

namespace Data {

const QString Region::ACTIVE = "active";
const QString Region::HOVERED = "hovered";
const QString Region::REGION_TYPE = "regionType";
const QString Region::CUSTOM_NAME = "customName";
const int Region::SIGNIFICANT_DIGITS = 6;

Region::Region(const QString& className, const QString& path, const QString& id )
:CartaObject( className, path, id ),
 m_shape(nullptr){
	_initializeState();
}

QPointF Region::getCenter() const {
	return m_shape->getCenter();
}


QString Region::getCursor() const {
	return "";
}

double Region::_getErrorMargin() const {
	double errorMargin = 1.0 / qPow( 10, SIGNIFICANT_DIGITS );
	return errorMargin;
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

QSizeF Region::getSize() const {
	return m_shape->getSize();
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
	m_state.insertValue<bool>( CUSTOM_NAME, false );
	m_state.insertValue<bool>( ACTIVE, true );
	m_state.insertValue<bool>( HOVERED, true );
	m_state.insertValue<QString>( Util::NAME, "");
	m_state.insertValue<QString>( Util::ID, getId() );
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
	Carta::State::StateInterface regState( "");
	regState.setState( stateStr );
	m_state = regState;
	m_state.setValue<QString>(Util::ID, getId() );
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

bool Region::setHeight( double /*value*/ ){
	return false;
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
	//_updateName();
}

QString Region::setRadiusMajor( double /*length*/, bool* changed ){
	QString result = "Major radius is not supported for shapes of type: "+getType();
	*changed = false;
	return result;
}

QString Region::setRadiusMinor( double /*length*/, bool* changed ){
	QString result = "Minor radius is not supported for shapes of type: "+getType();
	*changed = false;
	return result;
}

QString Region::setRegionName( const QString& name ){
	QString result;
	if ( name.trimmed().length() > 0 ){
		QString oldName = m_state.getValue<QString>( Util::NAME );
		if ( oldName != name ){
			m_state.setValue<bool>( CUSTOM_NAME, true );
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
		m_shape->setSelected( selected );
		emit regionSelectionChanged( getId());
	}
}


void Region::setUserData( void * /*value*/ ){
}

bool Region::setWidth( double /*value*/ ){
	return false;
}

QJsonObject Region::toJSON() const {
	QJsonObject regionObject;
	regionObject.insert( Util::ID, getId());
	regionObject.insert( REGION_TYPE, getRegionType() );
	return regionObject;
}

void Region::_updateName(){
	if ( !m_state.getValue<bool>(CUSTOM_NAME) ){
		QPointF center = getCenter();
		QSizeF size = getSize();
		QString type = getRegionType();
		QString dName = type + "[cX="+QString::number( center.x()) + " cY=" +QString::number( center.y() );
		if ( type != RegionTypes::POINT ){
				dName = dName + " width=" +QString::number(size.width())+" height="+QString::number(size.height());
		}
		dName = dName+"]";
		QString oldName = m_state.getValue<QString>( Util::NAME );
		if ( oldName != dName ){
			m_state.setValue<QString>( Util::NAME, dName );
			m_state.flushState();
		}
	}
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
