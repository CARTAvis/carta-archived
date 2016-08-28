#include "ShapeBase.h"
namespace Carta {

namespace Shape{

QPen ShapeBase::shadowPen = QPen( QBrush( QColor( 255, 0, 0, 128 ) ), 3 );
QBrush ShapeBase::shadowBrush = QBrush( QColor( 255, 255, 255, 64 ) );
QPen ShapeBase::outlinePen = QPen( QBrush( QColor( 0, 255, 0, 255 ) ), 1 );
QBrush ShapeBase::controlPointBrush = QBrush( QColor( 255, 255, 0, 255 ) );


void ShapeBase::editableChanged(){
}

QString ShapeBase::getCursor() const {
	return m_cursor;
}

Carta::Lib::VectorGraphics::VGList ShapeBase::getVGList() const {
	return Carta::Lib::VectorGraphics::VGList();
}


void * ShapeBase::getUserData() const {
	return m_userData;
}

void ShapeBase::handleDrag( const QPointF & pt ){
	Q_UNUSED( pt );
}

void ShapeBase::handleDragDone( const QPointF & pt ) {
	Q_UNUSED( pt );
}

void ShapeBase::handleDragStart( const QPointF & pt ) {
	Q_UNUSED( pt );
}


void ShapeBase::handleEvent(Carta::Lib::InputEvents::JsonEvent & ev){
	qDebug() << "Shape handling extra event" << ev.type();
}

bool ShapeBase::isActive() const {
	return m_active;
}

bool ShapeBase::isDeletable() const{
	return m_deletable;
}

bool ShapeBase::isEditable() const {
	return m_editable;
}

bool ShapeBase::isHovered() const{
	return m_hovered;
}

bool ShapeBase::isPointInside( const QPointF & pt ) const {
	Q_UNUSED( pt );
	return false;
}

bool ShapeBase::isSelected() const {
	return m_selected;
}

void ShapeBase::setActive( bool flag ) {
	m_active = flag;
}

void ShapeBase::setCursor( const QString & value ){
	m_cursor = value;
}

void ShapeBase::setDeletable( bool value ){
	m_deletable = value;
}

void ShapeBase::setEditable( bool editable ){
	bool oldEditable = m_editable;
	m_editable = editable;
	if ( oldEditable != editable ) {
		editableChanged();
	}
}

bool ShapeBase::setHovered( bool value ){
	m_hovered = value;
	return false;
}

void ShapeBase::setSelected( bool value ){
	m_selected = value;
}

void ShapeBase::setUserData( void * value ){
	m_userData = value;
}
}
}
