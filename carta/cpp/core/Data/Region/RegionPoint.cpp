#include "RegionPoint.h"
#include "Data/Util.h"
#include "State/UtilState.h"
#include "CartaLib/CartaLib.h"
#include "CartaLib/Regions/Point.h"
#include "Shape/ShapePoint.h"

#include <QDebug>

namespace Carta {

namespace Data {

using Carta::Shape::ShapeBase;
using Carta::Shape::ShapePoint;

const QString RegionPoint::CLASS_NAME = "RegionPoint";


class RegionPoint::Factory : public Carta::State::CartaObjectFactory {
public:

    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new RegionPoint (path, id);
    }
};

bool RegionPoint::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new RegionPoint::Factory());


RegionPoint::RegionPoint(const QString& path, const QString& id )
    :Region( CLASS_NAME, path, id ){
	m_shape.reset( new ShapePoint() );
	 connect( m_shape.get(), SIGNAL(shapeChanged( const QJsonObject&)),
			 this, SLOT(_updateStateFromJson(const QJsonObject&)));
    _initializeState();
    _updateShapeFromState();
}


std::shared_ptr<Carta::Lib::Regions::RegionBase> RegionPoint::getModel() const {
    std::shared_ptr<Carta::Lib::Regions::RegionBase> info( new Carta::Lib::Regions::Point() );
    QJsonObject jsonObject = toJSON();
       bool jsonValid = info->initFromJson( jsonObject );
       if ( !jsonValid ){
           qWarning()<<"Invalid json string: "<<jsonObject;
       }
    return info;
}



void RegionPoint::handleDragStart( const QPointF & pt ){
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

void RegionPoint::handleDragDone( const QPointF & pt ) {
	bool editable = isEditMode();
	if ( isDraggable() || editable ){
		if ( m_shape ){
			m_shape->handleDragDone( pt );
		}
	}
}

void RegionPoint::handleTouch( const QPointF & pt ){
	Region::handleTouch( pt );
	if ( isEditMode() ){
		//Set the location of the point
		setCenter( pt );
		emit editDone();
	}
}

void RegionPoint::_initializeState(){
    m_state.setValue<QString>( REGION_TYPE, Carta::Lib::Regions::Point::TypeName );
    m_state.insertValue<double>( Util::XCOORD, 0 );
    m_state.insertValue<double>( Util::YCOORD, 0 );
}


void RegionPoint::setModel( Carta::Lib::Regions::RegionBase* model ){
	if ( model ){
		QString regionType = model->typeName();
		CARTA_ASSERT( regionType == Carta::Lib::Regions::Point::TypeName );
		QJsonObject modelJson = model->toJson();
		m_shape->setModel( modelJson );
		_updateStateFromJson( modelJson );
	}
}


bool RegionPoint::setCenter( const QPointF& pt ){
	bool centerChanged = false;
	double oldX = m_state.getValue<double>( Util::XCOORD );
	double oldY = m_state.getValue<double>( Util::YCOORD );
	double errorMargin = _getErrorMargin();
	double centerX = Util::roundToDigits( pt.x(), SIGNIFICANT_DIGITS );
	double centerY = Util::roundToDigits( pt.y(), SIGNIFICANT_DIGITS );
	if ( qAbs( oldX - centerX ) > errorMargin || qAbs( oldY - centerY ) > errorMargin ){
		centerChanged = true;
		m_state.setValue<double>( Util::XCOORD, centerX );
		m_state.setValue<double>( Util::YCOORD, centerY );
		m_shape->setModel( toJSON() );
		_updateName();
	}
	return centerChanged;
}


QJsonObject RegionPoint::toJSON() const {
    QJsonObject descript = Region::toJSON();
    descript.insert( Util::XCOORD, m_state.getValue<double>( Util::XCOORD) );
    descript.insert( Util::YCOORD, m_state.getValue<double>( Util::YCOORD) );
    return descript;
}


void RegionPoint::_updateStateFromJson( const QJsonObject& json ){
	if ( !json[Util::XCOORD].isDouble() || !json[Util::YCOORD].isDouble() ){
		return;
	}
	double xCoord = Util::roundToDigits( json[Util::XCOORD].toDouble(), SIGNIFICANT_DIGITS );
	m_state.setValue<double>( Util::XCOORD, xCoord );
	double yCoord = Util::roundToDigits( json[Util::YCOORD].toDouble(), SIGNIFICANT_DIGITS );
	m_state.setValue<double>( Util::YCOORD, yCoord );

	_updateName();
	emit regionShapeChanged();
}

RegionPoint::~RegionPoint(){
}
}
}
