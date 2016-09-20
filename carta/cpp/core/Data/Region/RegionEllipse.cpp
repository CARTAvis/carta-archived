#include "RegionEllipse.h"
#include "Data/Util.h"
#include "State/UtilState.h"
#include "CartaLib/CartaLib.h"
#include "CartaLib/Regions/Ellipse.h"
#include "CartaLib/Regions/IRegion.h"
#include "Shape/ShapeEllipse.h"

#include <QDebug>

namespace Carta {

namespace Data {

using Carta::Shape::ShapeEllipse;

const QString RegionEllipse::CLASS_NAME = "RegionEllipse";



class RegionEllipse::Factory : public Carta::State::CartaObjectFactory {
public:

    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new RegionEllipse (path, id);
    }
};

bool RegionEllipse::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new RegionEllipse::Factory());


RegionEllipse::RegionEllipse(const QString& path, const QString& id )
    :Region( CLASS_NAME, path, id ){
	m_shape.reset( new Shape::ShapeEllipse() );
	connect( m_shape.get(), SIGNAL(shapeChanged( const QJsonObject&)),
				this, SLOT(_updateStateFromJson( const QJsonObject&)));

    _initializeState();
    _updateShapeFromState();
}

double RegionEllipse::getAngle() const {
	double angle = m_state.getValue<double>( Carta::Lib::Regions::Ellipse::ANGLE );
	return angle;
}

QPointF RegionEllipse::getCenter() const {
	double centerX = m_state.getValue<double>( Carta::Lib::Regions::RegionBase::CENTER_X );
	double centerY = m_state.getValue<double>( Carta::Lib::Regions::RegionBase::CENTER_Y );
	return QPointF( centerX, centerY );
}

std::shared_ptr<Carta::Lib::Regions::RegionBase> RegionEllipse::getModel() const {
    std::shared_ptr<Carta::Lib::Regions::RegionBase> info( new Carta::Lib::Regions::Ellipse() );
    return info;
}

double RegionEllipse::getRadiusMajor() const {
	double radiusMajor = m_state.getValue<double>( Carta::Lib::Regions::Ellipse::RADIUS_MAJOR );
	return radiusMajor;
}

double RegionEllipse::getRadiusMinor() const {
	double radiusMinor = m_state.getValue<double>( Carta::Lib::Regions::Ellipse::RADIUS_MINOR );
	return radiusMinor;
}

void RegionEllipse::handleDragStart( const QPointF & pt ){
	if ( isDraggable() ){
		if ( isEditMode() ){
			//Add the top left corner
			setCenter( pt );
			m_shape->handleDragStart( pt );
		}
		else if ( m_shape->isPointInside( pt ) ){
			m_shape->handleDragStart( pt );
		}
	}
}

void RegionEllipse::handleDragDone( const QPointF & pt ) {
	bool editable = isEditMode();
	if ( isDraggable() || editable ){
		if ( m_shape ){
			m_shape->handleDragDone( pt );
		}
	}

	if ( editable ){
		emit editDone();
	}
}

void RegionEllipse::_initializeState(){
    m_state.setValue<QString>( REGION_TYPE, Carta::Lib::Regions::Ellipse::TypeName );
    m_state.insertValue<double>( Carta::Lib::Regions::RegionBase::CENTER_X, 0 );
    m_state.insertValue<double>( Carta::Lib::Regions::RegionBase::CENTER_Y, 0 );
    m_state.insertValue<double>( Carta::Lib::Regions::Ellipse::RADIUS_MAJOR, 1 );
    m_state.insertValue<double>( Carta::Lib::Regions::Ellipse::RADIUS_MINOR, 1 );
    m_state.insertValue<double>( Carta::Lib::Regions::Ellipse::ANGLE, 0 );
    m_state.flushState();
}

void RegionEllipse::setCenter( const QPointF& center ){
	double oldX = m_state.getValue<double>( Carta::Lib::Regions::RegionBase::CENTER_X );
	double oldY = m_state.getValue<double>( Carta::Lib::Regions::RegionBase::CENTER_Y );
	if ( center.x() != oldX || center.y() != oldY ){
		m_state.setValue<double>( Carta::Lib::Regions::RegionBase::CENTER_X, center.x() );
		m_state.setValue<double>( Carta::Lib::Regions::RegionBase::CENTER_Y, center.y() );
		m_shape->setModel( toJSON() );
	}
}

void RegionEllipse::setModel( Carta::Lib::Regions::RegionBase* model ){
	if ( model ){
		QString regionType = model->typeName();
		CARTA_ASSERT( regionType == Carta::Lib::Regions::Ellipse::TypeName );
		m_shape->setModel( model->toJson() );
	}
}


QJsonObject RegionEllipse::toJSON() const {
    QJsonObject descript = Region::toJSON();
    QPointF center = getCenter();
    descript.insert( Carta::Lib::Regions::RegionBase::CENTER_X, center.x() );
    descript.insert( Carta::Lib::Regions::RegionBase::CENTER_Y, center.y() );
    descript.insert( Carta::Lib::Regions::Ellipse::RADIUS_MAJOR, getRadiusMajor());
    descript.insert( Carta::Lib::Regions::Ellipse::RADIUS_MINOR, getRadiusMinor());
    descript.insert( Carta::Lib::Regions::Ellipse::ANGLE, getAngle());
    return descript;
}

void RegionEllipse::_updateStateFromJson( const QJsonObject& json ){
	if ( !json[Carta::Lib::Regions::RegionBase::CENTER_X].isDouble() ||
			!json[Carta::Lib::Regions::RegionBase::CENTER_Y].isDouble() ||
			!json[Carta::Lib::Regions::Ellipse::RADIUS_MAJOR].isDouble() ||
			!json[Carta::Lib::Regions::Ellipse::RADIUS_MINOR].isDouble() ||
			!json[Carta::Lib::Regions::Ellipse::ANGLE].isDouble()){
		return;
	}
	m_state.setValue<double>( Carta::Lib::Regions::RegionBase::CENTER_X, json[Carta::Lib::Regions::RegionBase::CENTER_X].toDouble() );
	m_state.setValue<double>( Carta::Lib::Regions::RegionBase::CENTER_Y, json[Carta::Lib::Regions::RegionBase::CENTER_Y].toDouble() );
	m_state.setValue<double>( Carta::Lib::Regions::Ellipse::RADIUS_MAJOR, json[Carta::Lib::Regions::Ellipse::RADIUS_MAJOR].toDouble());
	m_state.setValue<double>( Carta::Lib::Regions::Ellipse::RADIUS_MINOR, json[Carta::Lib::Regions::Ellipse::RADIUS_MINOR].toDouble());
	m_state.setValue<double>( Carta::Lib::Regions::Ellipse::ANGLE, json[Carta::Lib::Regions::Ellipse::ANGLE].toDouble());
}

RegionEllipse::~RegionEllipse(){
}
}
}
