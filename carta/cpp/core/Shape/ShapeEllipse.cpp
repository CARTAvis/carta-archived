
#include "ShapeEllipse.h"
#include "ControlPoint.h"
#include "CartaLib/Regions/Ellipse.h"

namespace Carta {
namespace Shape {

ShapeEllipse::ShapeEllipse( std::shared_ptr<Carta::Lib::Regions::Ellipse> ellipseRegion ):
		m_dragOffset( QPointF(0,0) ),
		m_centerCP(nullptr),
		m_radiusYCP(nullptr),
		m_radiusXCP(nullptr){
	m_ellipseRegion = ellipseRegion;
	m_inDragMode = false;
}


Carta::Lib::VectorGraphics::VGList ShapeEllipse::getVGList() const {
	Carta::Lib::VectorGraphics::VGComposer comp;

	//        QRectF ( m_center.x() - m_radius, m_center.y() - m_radius,
	//                       m_radius * 2, m_radius * 2 );

	if ( isHovered() ) {
		double penWidth = 2;

		penWidth = 4;
		comp.append < vge::SetPen > ( QPen( QColor( 255, 255, 255, 128 ), penWidth ) );
		comp.append < vge::DrawEllipse > ( m_ellipseRegion->outlineBox() );
	}

	if ( isSelected() ) {
		comp.append < vge::SetPen > ( QPen( QColor( 0, 255, 0, 128 ), 1 ) );
		comp.append < vge::DrawRect > ( m_ellipseRegion->outlineBox() );
	}

	if ( m_inDragMode || isEditable() ) {
		comp.append < vge::SetPen > ( QPen( QColor( 0, 255, 255, 128 ), 1 ) );
		comp.append < vge::DrawEllipse > ( m_dragRect );
	}

	if ( isEditable() ) {
		comp.append < vge::SetPen > ( QPen( QColor( 255, 0, 0 ), 1 ) );
		comp.append < vge::DrawLine > ( m_centerCP-> getPosition(), m_radiusXCP-> getPosition() );
		comp.append < vge::DrawLine > ( m_centerCP-> getPosition(), m_radiusYCP->getPosition() );
	}
	return comp.vgList();
} // getVGList

bool ShapeEllipse::isPointInside( const QPointF & pt ) const {
	return m_ellipseRegion->isPointInside( {pt} );
}

void ShapeEllipse::handleDragStart( const QPointF & pt ){
	m_inDragMode = true;
	m_dragOffset = pt - m_ellipseRegion->getCenter();
	setDragRect( m_ellipseRegion->outlineBox() );
}

void ShapeEllipse::handleDrag( const QPointF & pt ){
	// calculate offset if we are starting the drag
	if ( ! m_inDragMode ) {
		qWarning() << "dragging but not in drag mode";
		return;
	}
	auto r = m_dragRect;
	r.translate( pt - m_dragRect.center() - m_dragOffset );
	setDragRect( r );
}

void ShapeEllipse::handleDragDone( const QPointF & pt ){
	Q_UNUSED( pt );
	m_inDragMode = false;
	auto newCenter = m_dragRect.center();
	m_ellipseRegion->setCenter( newCenter );
}

void ShapeEllipse::setDragRect( const QRectF & r ){
	m_dragRect = r;
	if ( m_centerCP ) {
		m_centerCP-> setPosition( m_dragRect.center() );
		m_radiusXCP-> setPosition( QPointF( m_dragRect.right(), m_dragRect.center().y() ) );
		m_radiusYCP-> setPosition( QPointF( m_dragRect.center().x(), m_dragRect.bottom() ) );
	}
}

void ShapeEllipse::editableChanged(){

}

void ShapeEllipse::controlPointCB( bool /*movingCenter*/, bool /*final*/ ){

}
}
}


