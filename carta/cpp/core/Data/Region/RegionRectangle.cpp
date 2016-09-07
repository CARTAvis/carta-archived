#include "RegionRectangle.h"
#include "Data/Util.h"
#include "State/UtilState.h"
#include "CartaLib/CartaLib.h"
#include "CartaLib/Regions/Rectangle.h"
#include "Shape/ShapeRectangle.h"

#include <QDebug>

namespace Carta {

namespace Data {

using Carta::Shape::ShapeBase;
using Carta::Shape::ShapeRectangle;

const QString RegionRectangle::CLASS_NAME = "RegionRectangle";


class RegionRectangle::Factory : public Carta::State::CartaObjectFactory {
public:

    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new RegionRectangle (path, id);
    }
};

bool RegionRectangle::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new RegionRectangle::Factory());


RegionRectangle::RegionRectangle(const QString& path, const QString& id )
    :Region( CLASS_NAME, path, id ){
	m_shape.reset( new ShapeRectangle() );
	 connect( m_shape.get(), SIGNAL(shapeChanged( const QJsonObject&)),
			 this, SLOT(_updateStateFromJson(const QJsonObject&)));

	m_constructed = false;
    _initializeState();

    _updateShapeFromState();
}


std::shared_ptr<Carta::Lib::Regions::RegionBase> RegionRectangle::getModel() const {
    std::shared_ptr<Carta::Lib::Regions::RegionBase> info( new Carta::Lib::Regions::Rectangle() );
    QJsonObject jsonObject = toJSON();
       bool jsonValid = info->initFromJson( jsonObject );
       if ( !jsonValid ){
           qWarning()<<"Invalid json string: "<<jsonObject;
       }
    return info;
}

double RegionRectangle::getHeight() const {
	return m_state.getValue<double>( Util::HEIGHT );
}

QPointF RegionRectangle::getTopLeft() const {
	QPointF pt;
	pt.setX( m_state.getValue<double>( Util::XCOORD) );
	pt.setY( m_state.getValue<double>( Util::YCOORD) );
	return pt;
}

double RegionRectangle::getWidth() const {
	return m_state.getValue<double>( Util::WIDTH );
}


void RegionRectangle::handleDragStart( const QPointF & pt ){

	if ( isDraggable() ){
		if ( isEditMode() ){
			//Add the lop left corner
			setTopLeft( pt );
			m_shape->handleDragStart( pt );
		}
		else if ( m_shape->isPointInside( pt ) ){
			m_shape->handleDragStart( pt );
		}
	}
}

void RegionRectangle::handleTouch( const QPointF& pt ){
	if ( m_shape->isActive() ){
		bool tapped = false;
		if ( m_shape->isPointInside( pt ) ){
			tapped = true;
		}
		setSelected( tapped );
	}
}

void RegionRectangle::_initializeState(){
    m_state.setValue<QString>( REGION_TYPE, Carta::Lib::Regions::Rectangle::TypeName );
    m_state.insertValue<double>( Util::WIDTH, 0);
    m_state.insertValue<double>( Util::HEIGHT, 0 );
    m_state.insertValue<double>( Util::XCOORD, 0 );
    m_state.insertValue<double>( Util::YCOORD, 0 );
    m_state.flushState();
}


void RegionRectangle::_restoreState( const QString& stateStr ){
    Region::_restoreState( stateStr );
    m_state.flushState();
}

void RegionRectangle::setModel( Carta::Lib::Regions::RegionBase* model ){
	if ( model ){
		QString regionType = model->typeName();
		CARTA_ASSERT( regionType == Carta::Lib::Regions::Polygon::TypeName );
		m_shape->setModel( model->toJson() );
		m_constructed = true;
	}
}

void RegionRectangle::_setRectPt( const QPointF& pt ){
	double width = qAbs( pt.x() - getTopLeft().x() );
	double height = qAbs( pt.y() - getTopLeft().y() );
	setRectangleSize( width, height );
}

void RegionRectangle::setRectangleSize( double width, double height ){
	CARTA_ASSERT( width >= 0 );
	CARTA_ASSERT( height >= 0 );
	m_state.setValue<double>( Util::WIDTH, width );
	m_state.setValue<double>( Util::HEIGHT, height );
	m_shape->setModel( toJSON() );
}

void RegionRectangle::setTopLeft( const QPointF& pt ){
	m_state.setValue<double>( Util::XCOORD, pt.x());
	m_state.setValue<double>( Util::YCOORD, pt.y());
	m_shape->setModel( toJSON() );
}

QJsonObject RegionRectangle::toJSON() const {
    QJsonObject descript = Region::toJSON();
    descript.insert( Util::WIDTH, getWidth() );
    descript.insert( Util::HEIGHT, getHeight() );
    QPointF topLeft = getTopLeft();
    descript.insert( Util::XCOORD, topLeft.x() );
    descript.insert( Util::YCOORD, topLeft.y() );
    return descript;
}

void RegionRectangle::_updateStateFromJson( const QJsonObject& json ){
	if ( !json[Util::XCOORD].isDouble() || !json[Util::YCOORD].isDouble() ||
			!json[Util::WIDTH].isDouble() || !json[Util::HEIGHT].isDouble() ){
		return;
	}
	m_state.setValue<double>( Util::XCOORD, json[Util::XCOORD].toDouble() );
	m_state.setValue<double>( Util::YCOORD, json[Util::YCOORD].toDouble() );
	m_state.setValue<double>( Util::WIDTH, json[Util::WIDTH].toDouble());
	m_state.setValue<double>( Util::HEIGHT, json[Util::HEIGHT].toDouble());
}

RegionRectangle::~RegionRectangle(){
}
}
}
