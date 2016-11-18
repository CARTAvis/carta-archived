#include "Rectangle.h"

namespace Carta {
namespace Lib {
namespace Regions {

Rectangle::Rectangle( RegionBase * parent ) :
		RegionBase( parent ) {
}


bool Rectangle::initFromJson( QJsonObject obj ){
	if ( ! RegionBase::initFromJson( obj ) ) {
		return false;
	}
	if ( ! obj.contains(WIDTH) || !obj.contains(HEIGHT) ||
			!obj.contains(POINT_X) || !obj.contains(POINT_Y) ){
		return false;
	}
	double centerX = obj[POINT_X].toDouble();
	double centerY = obj[POINT_Y].toDouble();
	double width = obj[WIDTH].toDouble();
	double height = obj[HEIGHT].toDouble();
	double left = centerX - width / 2;
	double top = centerY - height / 2;
	m_rectf = QRectF( left, top, width, height );
	return true;
}


bool Rectangle::isPointInside( const RegionPointV & pts ) const {
	const auto & p = pts[csId()];
	return m_rectf.contains( p );
}


bool Rectangle::isPointInsideUnion( const RegionPointV & pts ) const {
	return isPointInside( pts );
}


QRectF Rectangle::outlineBox() const {
	return m_rectf;
}


void Rectangle::setRectangle( const QRectF & rect ) {
	m_rectf = rect;
}


QJsonObject Rectangle::toJson() const {
	// get the common properties
	QJsonObject doc = RegionBase::toJson();
	doc[WIDTH] = m_rectf.width();
	doc[HEIGHT] = m_rectf.height();
	QPointF centerPt = m_rectf.center();
	doc[POINT_X] = centerPt.x();
	doc[POINT_Y] = centerPt.y();
	return doc;
}

QString Rectangle::typeName() const {
	return TypeName;
}

VectorGraphics::VGList Rectangle::vgList() const {
	VectorGraphics::VGComposer composer;
	composer.append < VectorGraphics::Entries::SetPenColor > ( getLineColor() );
	composer.append < VectorGraphics::Entries::SetBrush > ( getFillColor() );
	composer.append < VectorGraphics::Entries::DrawRect > ( m_rectf );
	return composer.vgList();
}
}
}
}
