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
	QJsonObject jsonObject = toJSON();
	bool jsonValid = info->initFromJson( jsonObject );
	if ( !jsonValid ){
		qWarning()<<"Invalid json string: "<<jsonObject;
	}
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


bool RegionEllipse::setCenter( const QPointF& center ){
	double oldX = m_state.getValue<double>( Carta::Lib::Regions::RegionBase::CENTER_X );
	double oldY = m_state.getValue<double>( Carta::Lib::Regions::RegionBase::CENTER_Y );
	bool centerChanged = false;
	double centerX = Util::roundToDigits( center.x(), SIGNIFICANT_DIGITS );
	double centerY = Util::roundToDigits( center.y(), SIGNIFICANT_DIGITS );
	double errorMargin = _getErrorMargin();
	if ( qAbs( centerX - oldX ) > errorMargin || qAbs( centerY - oldY ) > errorMargin ){
		centerChanged = true;
		m_state.setValue<double>( Carta::Lib::Regions::RegionBase::CENTER_X, centerX );
		m_state.setValue<double>( Carta::Lib::Regions::RegionBase::CENTER_Y, centerY );
		m_shape->setModel( toJSON() );
		_updateName();
	}
	return centerChanged;
}

void RegionEllipse::setModel( Carta::Lib::Regions::RegionBase* model ){
	if ( model ){
		QString regionType = model->typeName();
		CARTA_ASSERT( regionType == Carta::Lib::Regions::Ellipse::TypeName );
		QJsonObject modelJson = model->toJson();
		m_shape->setModel( modelJson );
		_updateStateFromJson( modelJson );
	}
}

QString RegionEllipse::setRadiusMajor( double length, bool* changed ){
	QString result;
	*changed = false;
	CARTA_ASSERT( length >= 0 );
	double oldRadiusMajor = getRadiusMajor();
	double oldRadiusMinor = getRadiusMinor();
	double roundedLength = Util::roundToDigits( length, SIGNIFICANT_DIGITS );
	double errorMargin = _getErrorMargin();
	if ( roundedLength < oldRadiusMinor ){
		result = "The major radius must be larger than the minor radius.";
	}
	else if ( qAbs( oldRadiusMajor - roundedLength ) > errorMargin ){
		*changed = true;
		m_state.setValue<double>( Carta::Lib::Regions::Ellipse::RADIUS_MAJOR, roundedLength );
		m_shape->setModel( toJSON() );
		_updateName();
	}
	return result;;
}

QString RegionEllipse::setRadiusMinor( double length, bool* changed ){
	QString result;
	*changed = false;
	CARTA_ASSERT( length >= 0 );
	double oldRadiusMajor = getRadiusMajor();
	double oldRadiusMinor = getRadiusMinor();
	double errorMargin = _getErrorMargin();
	double roundedLength = Util::roundToDigits( length, SIGNIFICANT_DIGITS );
	if ( roundedLength > oldRadiusMajor ){
		result = "The minor radius must be smaller than the major radius.";
	}
	else if ( qAbs( oldRadiusMinor - roundedLength) > errorMargin ){
		*changed = true;
		m_state.setValue<double>( Carta::Lib::Regions::Ellipse::RADIUS_MINOR, roundedLength );
		m_shape->setModel( toJSON() );
		_updateName();
	}
	else {
		qDebug()<< "Distance exceeded error margin";
	}
	return result;
}

QJsonObject RegionEllipse::toJSON() const {
    QJsonObject descript = Region::toJSON();
    QString centerKeyX = Carta::Lib::Regions::RegionBase::CENTER_X;
    QString centerKeyY = Carta::Lib::Regions::RegionBase::CENTER_Y;
    descript.insert( centerKeyX, m_state.getValue<double>(centerKeyX) );
    descript.insert( centerKeyY, m_state.getValue<double>(centerKeyY) );
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
	double centerX = Util::roundToDigits( json[Carta::Lib::Regions::RegionBase::CENTER_X].toDouble(), SIGNIFICANT_DIGITS );
	m_state.setValue<double>( Carta::Lib::Regions::RegionBase::CENTER_X, centerX );
	double centerY = Util::roundToDigits( json[Carta::Lib::Regions::RegionBase::CENTER_Y].toDouble(), SIGNIFICANT_DIGITS );
	m_state.setValue<double>( Carta::Lib::Regions::RegionBase::CENTER_Y, centerY );
	double radiusMajor = qAbs(Util::roundToDigits( json[Carta::Lib::Regions::Ellipse::RADIUS_MAJOR].toDouble(), SIGNIFICANT_DIGITS ));
	m_state.setValue<double>( Carta::Lib::Regions::Ellipse::RADIUS_MAJOR, radiusMajor );
	double radiusMinor = qAbs(Util::roundToDigits( json[Carta::Lib::Regions::Ellipse::RADIUS_MINOR].toDouble(), SIGNIFICANT_DIGITS ));
	m_state.setValue<double>( Carta::Lib::Regions::Ellipse::RADIUS_MINOR, radiusMinor );
	double angle = Util::roundToDigits( json[Carta::Lib::Regions::Ellipse::ANGLE].toDouble(), SIGNIFICANT_DIGITS );
	m_state.setValue<double>( Carta::Lib::Regions::Ellipse::ANGLE, angle );
	_updateName();
	emit regionShapeChanged();
}

RegionEllipse::~RegionEllipse(){
}
}
}
