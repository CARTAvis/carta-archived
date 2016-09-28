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


double RegionRectangle::getWidth() const {
	return m_state.getValue<double>( Util::WIDTH );
}


void RegionRectangle::handleDragStart( const QPointF & pt ){
	if ( isDraggable() ){
		if ( isEditMode() ){
			//Add the point
			setCenter( pt );
			m_shape->handleDragStart( pt );
		}
		else if ( m_shape->isPointInside( pt ) ){
			m_shape->handleDragStart( pt );
		}
	}
}

void RegionRectangle::handleDragDone( const QPointF & pt ) {
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

void RegionRectangle::_initializeState(){
    m_state.setValue<QString>( REGION_TYPE, Carta::Lib::Regions::Rectangle::TypeName );
    m_state.insertValue<double>( Util::WIDTH, 0);
    m_state.insertValue<double>( Util::HEIGHT, 0 );
    m_state.insertValue<double>( Util::XCOORD, 0 );
    m_state.insertValue<double>( Util::YCOORD, 0 );
}

bool RegionRectangle::setHeight( double height ){
	CARTA_ASSERT( height >= 0 );
	bool heightChanged = false;
	double oldHeight = m_state.getValue<double>( Util::HEIGHT );
	if ( qAbs( oldHeight - height ) > ERROR_MARGIN ){
		heightChanged = true;
		m_state.setValue<double>( Util::HEIGHT, height );
		m_shape->setModel( toJSON() );
		_updateName();
	}
	return heightChanged;
}


void RegionRectangle::setModel( Carta::Lib::Regions::RegionBase* model ){
	if ( model ){
		QString regionType = model->typeName();
		CARTA_ASSERT( regionType == Carta::Lib::Regions::Polygon::TypeName );
		QJsonObject modelJson = model->toJson();
		m_shape->setModel( modelJson );
		_updateStateFromJson( modelJson );
	}
}


bool RegionRectangle::setWidth( double width ){
	CARTA_ASSERT( width >= 0 );
	bool widthChanged = false;
	double oldWidth = m_state.getValue<double>( Util::WIDTH );
	if ( qAbs( width - oldWidth ) > 0 ){
		widthChanged = true;
		m_state.setValue<double>( Util::WIDTH, width );
		m_shape->setModel( toJSON() );
		_updateName();
	}
	return widthChanged;
}


bool RegionRectangle::setCenter( const QPointF& pt ){
	bool centerChanged = false;
	double oldX = m_state.getValue<double>( Util::XCOORD );
	double oldY = m_state.getValue<double>( Util::YCOORD );
	if ( qAbs( oldX - pt.x()) > ERROR_MARGIN || qAbs( oldY - pt.y() ) > ERROR_MARGIN ){
		centerChanged = true;
		m_state.setValue<double>( Util::XCOORD, pt.x());
		m_state.setValue<double>( Util::YCOORD, pt.y());
		m_shape->setModel( toJSON() );

		_updateName();
	}
	return centerChanged;
}


QJsonObject RegionRectangle::toJSON() const {
    QJsonObject descript = Region::toJSON();
    descript.insert( Util::WIDTH, getWidth());
    descript.insert( Util::HEIGHT, getHeight() );
    descript.insert( Util::XCOORD, m_state.getValue<double>( Util::XCOORD) );
    descript.insert( Util::YCOORD, m_state.getValue<double>( Util::YCOORD) );
    return descript;
}

void RegionRectangle::_updateStateFromJson( const QJsonObject& json ){
	if ( !json[Util::XCOORD].isDouble() || !json[Util::YCOORD].isDouble() ||
			!json[Util::WIDTH].isDouble() || !json[Util::HEIGHT].isDouble() ){
		return;
	}
	m_state.setValue<double>( Util::XCOORD, json[Util::XCOORD].toDouble() );
	m_state.setValue<double>( Util::YCOORD, json[Util::YCOORD].toDouble() );
	m_state.setValue<double>( Util::WIDTH, qAbs(json[Util::WIDTH].toDouble()));
	m_state.setValue<double>( Util::HEIGHT, qAbs(json[Util::HEIGHT].toDouble()));
	_updateName();
	emit regionShapeChanged();
}

RegionRectangle::~RegionRectangle(){
}
}
}
