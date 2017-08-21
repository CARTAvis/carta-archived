#include "Point.h"

namespace Carta {
namespace Lib {
namespace Regions {

Point::Point( RegionBase * parent ) :
		RegionBase( parent ) {
}


bool Point::initFromJson( QJsonObject obj ){
	if ( ! RegionBase::initFromJson( obj ) ) {
		return false;
	}
	if ( !obj.contains(POINT_X) || !obj.contains(POINT_Y) ){
		return false;
	}
	double centerX = obj[POINT_X].toDouble();
	double centerY = obj[POINT_Y].toDouble();
	m_point = QPointF( centerX, centerY );
	return true;
}


bool Point::isPointInside( const RegionPointV & pts ) const {
	const auto & p = pts[csId()];
	QRectF box = outlineBox();
	return box.contains( p );
}


bool Point::isPointInsideUnion( const RegionPointV & pts ) const {
	return isPointInside( pts );
}


QRectF Point::outlineBox() const {
	QRectF box;
	const int SMALL_SIZE = 4;
	double topLeftX = m_point.x() - SMALL_SIZE / 2;
	double topLeftY = m_point.y() - SMALL_SIZE / 2;
	box.setX( topLeftX );
	box.setY( topLeftY );
	box.setWidth( SMALL_SIZE  );
	box.setHeight( SMALL_SIZE  );
	return box;
}


void Point::setPoint( const QPointF & point ) {
	m_point = point;
}


QJsonObject Point::toJson() const {
	// get the common properties
	QJsonObject doc = RegionBase::toJson();
	doc[POINT_X] = m_point.x();
	doc[POINT_Y] = m_point.y();
	return doc;
}

QString Point::typeName() const {
	return TypeName;
}

VectorGraphics::VGList Point::vgList() const {
	VectorGraphics::VGComposer composer;
	composer.append < VectorGraphics::Entries::SetPenColor > ( getLineColor() );
	composer.append < VectorGraphics::Entries::SetBrush > ( getFillColor() );
	QRectF drawRect( m_point.x(), m_point.y(), 1, 1 );
	composer.append < VectorGraphics::Entries::DrawRect > ( drawRect );
	return composer.vgList();
}
}
}
}
