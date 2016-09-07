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
	m_rectf = QRectF( obj[POINT_X].toDouble(), obj[POINT_Y].toDouble(),
			obj[WIDTH].toDouble(), obj[HEIGHT].toDouble() );
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
	doc[POINT_X] = m_rectf.x();
	doc[POINT_Y] = m_rectf.y();
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
