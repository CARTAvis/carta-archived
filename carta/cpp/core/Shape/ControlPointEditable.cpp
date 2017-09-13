#include "ControlPointEditable.h"

namespace Carta {
namespace Shape {

ControlPointEditable::ControlPointEditable( std::function < void (bool) > cb ){
	m_cb = cb;
	m_pos = QPointF(0,0);
	m_fillColor = shadowBrush.color();
}

void ControlPointEditable::editModeChanged(){

}

void ControlPointEditable::_editShadow( const QPointF& /*pt*/ ){

}


QPointF ControlPointEditable::getCenter() const {
	return m_pos;
}

QSizeF ControlPointEditable::getSize() const {
	return QSize(0,0);
}

Carta::Lib::VectorGraphics::VGList ControlPointEditable::getVGList() const {
	Carta::Lib::VectorGraphics::VGComposer comp;
	QPen pen = outlinePen;
	pen.setCosmetic(true);
	QRectF rect( m_pos.x() - m_size / 2, m_pos.y() - m_size / 2, m_size, m_size );
	QColor color = m_fillColor;
	if ( isSelected() ) {
		//comp.append < vge::FillRect > ( rect, color );
		comp.append < vge::SetPen > ( pen );
		comp.append < vge::DrawRect > ( rect );
	}
	else {
		color.setAlpha( 128 );
		//comp.append < vge::FillRect > ( rect, color );
		comp.append < vge::SetPen > ( pen );
		comp.append < vge::DrawRect > ( rect );
	}
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

void ControlPointEditable::_moveShadow( const QPointF& /*pt*/ ){

}

void ControlPointEditable::setFillColor( QColor color ) {
	m_fillColor = color;
}

void ControlPointEditable::setModel( const QJsonObject& /*json*/ ){

}

void ControlPointEditable::setPosition( const QPointF & pt ) {
	m_pos = pt;
}

void ControlPointEditable::_syncShadowToCPs(){

}
ControlPointEditable::~ControlPointEditable(){
}
}
}
