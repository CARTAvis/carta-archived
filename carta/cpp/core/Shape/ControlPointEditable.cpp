#include "ControlPointEditable.h"

namespace Carta {
namespace Shape {

ControlPointEditable::ControlPointEditable( std::function < void (bool) > cb ){
	m_cb = cb;
}

const QPointF & ControlPointEditable::getPosition() const {
	return m_pos;
}

Carta::Lib::VectorGraphics::VGList ControlPointEditable::getVGList() const {
	if ( ! isActive() ) {
		return Carta::Lib::VectorGraphics::VGList();
	}

	Carta::Lib::VectorGraphics::VGComposer comp;
	QColor color = m_fillColor;
	color.setAlpha( 128 );
	if ( isHovered() ) { color.setAlpha( 227 ); }
	if ( isSelected() ) { color.setAlpha( 255 ); }
	QRectF rect( m_pos.x() - m_size / 2, m_pos.y() - m_size / 2,
			m_size, m_size );
	comp.append < vge::FillRect > ( rect, color );
	return comp.vgList();
}


void ControlPointEditable::handleDragStart( const QPointF & pt ) {
	m_pos = pt;
	m_cb( false );
}

void ControlPointEditable::handleDrag( const QPointF & pt ) {
	m_pos = pt;
	m_cb( false );
}

void ControlPointEditable::handleDragDone( const QPointF & pt ) {
	m_pos = pt;
	m_cb( true );
}

bool ControlPointEditable::isPointInside( const QPointF & pt ) const {
	auto dv = pt - m_pos;
	auto dsq = QPointF::dotProduct( dv, dv );
	return dsq < m_size * m_size;
}

void ControlPointEditable::setFillColor( QColor color ) {
	m_fillColor = color;
}

void ControlPointEditable::setPosition( const QPointF & pt ) {
	m_pos = pt;
}

ControlPointEditable::~ControlPointEditable(){
}
}
}
