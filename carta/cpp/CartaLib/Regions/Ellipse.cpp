#include "Ellipse.h"

#include <QJsonDocument>
#include "math.h"

namespace Carta
{
namespace Lib
{
namespace Regions
{

const QString Ellipse::RADIUS_MAJOR = "radiusMajor";
const QString Ellipse::RADIUS_MINOR = "radiusMinor";
const QString Ellipse::ANGLE = "angle";


Ellipse::Ellipse( RegionBase * parent  ) :
								RegionBase( parent ) {
}


Ellipse::Ellipse( const RegionPoint & center, double radiusMajor, double radiusMinor, double angle ){
	m_center = center;
	m_radiusMajor = radiusMajor;
	m_radiusMinor = radiusMinor;
	m_angle = angle;
}

double Ellipse::getAngle() const {
	return m_angle;
}

const RegionPoint & Ellipse::getCenter() const {
	return m_center;
}

double Ellipse::getRadiusMajor() const {
	return m_radiusMajor;
}


double Ellipse::getRadiusMinor() const {
	return m_radiusMinor;
}

bool Ellipse::initFromJson( QJsonObject obj ){
	if ( ! RegionBase::initFromJson( obj ) ) {
		return false;
	}
	if ( ! obj[CENTER_X].isDouble() ) {
		return false;
	}
	if ( ! obj[CENTER_Y].isDouble() ) {
		return false;
	}
	if ( ! obj[RADIUS_MAJOR].isDouble() ) {
		return false;
	}
	if ( ! obj[RADIUS_MINOR].isDouble() ) {
		return false;
	}
	if ( !obj[ANGLE].isDouble() ){
		return false;
	}
	m_center = QPointF( obj[CENTER_X].toDouble(), obj[CENTER_Y].toDouble() );
	m_radiusMajor = obj[RADIUS_MAJOR].toDouble();
	m_radiusMinor = obj[RADIUS_MINOR].toDouble();
	m_angle = obj[ANGLE].toDouble();
	return true;
}

bool Ellipse::isPointInside( const RegionPointV & pts ) const {
	const QPointF & p = pts[csId()];
	//((x-h)cosA+(y-k)sinA)^2/rMaj^2 + ((x-h)sinA-(y-k)cosA)^2/rMin^2 = 1
	double xDiff = p.x() - m_center.x();
	double yDiff = p.y() - m_center.y();
	double termMajor = xDiff*cos(m_angle) + yDiff*sin(m_angle);
	double termMinor = xDiff*sin(m_angle) + yDiff*cos(m_angle);
	double d = (termMajor * termMajor)/(m_radiusMajor * m_radiusMajor) +
			(termMinor*termMinor)/(m_radiusMinor * m_radiusMinor);
	return ( d < 1 );
}

bool Ellipse::isPointInsideUnion( const RegionPointV & pts ) const {
	return isPointInside( pts );
}

QRectF Ellipse::outlineBox() const {
	double cosAngle = cos( m_angle * ( M_PI / 180));
	double sinAngle = sin( m_angle * ( M_PI / 180));

	//Now rotate by the given angle.
	double xTransform = m_radiusMinor * cosAngle - m_radiusMajor * sinAngle;
	double yTransform = m_radiusMinor * sinAngle + m_radiusMajor * cosAngle;
	double left = m_center.x() - xTransform;
	double top = m_center.y() - yTransform;
	double right = m_center.x() + xTransform;
	double bottom = m_center.y() + yTransform;

	//Height and width
	double height = bottom - top;
	double width = right - left;
	return QRectF( left, top, width, height );
}

void Ellipse::setAngle( double angle ){
	m_angle = angle;
}

void Ellipse::setCenter( const RegionPoint & pt ){
	m_center = pt;
}

void Ellipse::setRadiusMajor( double radius ) {
	m_radiusMajor = radius;
}

void Ellipse::setRadiusMinor( double radius ) {
	m_radiusMinor = radius;
}

QJsonObject Ellipse::toJson() const {
	// get the common properties
	QJsonObject doc = RegionBase::toJson();

	// and add our own
	doc[CENTER_X] = m_center.x();
	doc[CENTER_Y] = m_center.y();
	doc[RADIUS_MAJOR] = m_radiusMajor;
	doc[RADIUS_MINOR] = m_radiusMinor;
	doc[ANGLE] = m_angle;
	doc["type"] = TypeName;
	return doc;
}

QString Ellipse::typeName() const {
	return TypeName;
}

VectorGraphics::VGList Ellipse::vgList() const {
	VectorGraphics::VGComposer composer;
	composer.append < VectorGraphics::Entries::SetPenColor > ( getLineColor() );
	composer.append < VectorGraphics::Entries::SetBrush > ( getFillColor() );
	QRectF rect = outlineBox();
	composer.append < VectorGraphics::Entries::DrawEllipse > ( rect );
	return composer.vgList();
}

}
}
}
