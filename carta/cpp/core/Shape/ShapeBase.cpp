#include "ShapeBase.h"
#include "ControlPointEditable.h"
namespace Carta {

namespace Shape{

const QPen ShapeBase::shadowPen = QPen( QBrush( QColor( 255, 106, 0 ) ), 3 );
const QBrush ShapeBase::shadowBrush = QBrush( QColor( 255, 106, 0 ) );
const QPen ShapeBase::outlinePen = QPen( QBrush( QColor( 102, 153, 204 ) ), 2 );

ShapeBase::ShapeBase( QObject* parent ):
		QObject( parent ),
		m_dragStart(QPointF(0,0)){
		m_dragMode = false;
		m_dragControlIndex = -1;
}

void ShapeBase::editModeChanged(){
	// update the positions of control points
	_syncShadowToCPs();

	// activate/deactivate control points based on edit status
	for ( auto & pt : m_controlPoints ) {
		pt-> setActive( !isEditMode() );
	}
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
	int controlPointCount = m_controlPoints.size();
	if ( m_dragControlIndex >= 0 && m_dragControlIndex < controlPointCount ){
		m_controlPoints[m_dragControlIndex]->handleDrag( pt );
	}
	else {

		// calculate offset if we are starting the drag
		if ( ! m_dragMode ) {
			qWarning() << "dragging but not in drag mode";
			return;
		}

		if ( !isEditMode() ){
			_moveShadow( pt );
			_syncShadowToCPs();
		}
		else {
			_editShadow( pt );
		}
	}
}

void ShapeBase::handleDragDone( const QPointF & pt ) {
	Q_UNUSED( pt );
}

void ShapeBase::handleDragStart( const QPointF & pt ) {
	//First see if any of our control points are being dragged
	m_dragControlIndex = -1;
	int controlCount = m_controlPoints.size();
	for ( int i = 0; i < controlCount; i++ ){
		if ( m_controlPoints[i]->isPointInside( pt )){
			m_dragControlIndex = i;
			m_controlPoints[i]->handleDragStart( pt );
			break;
		}
	}
	//We'll drag ourselves if none of the control points was targeted.
	if ( m_dragControlIndex < 0 ){
		m_dragMode = true;
		m_dragStart = pt;
	}
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

bool ShapeBase::isEditMode() const {
	return m_editMode;
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

void ShapeBase::setEditMode( bool editMode ){
	bool oldEditMode = m_editMode;
	m_editMode = editMode;
	if ( oldEditMode != editMode ) {
		editModeChanged();
	}
}

void ShapeBase::setHovered( bool value ){
	m_hovered = value;
	int controlCount = m_controlPoints.size();
	for ( int i = 0; i < controlCount; i++ ){
		m_controlPoints[i]->setActive( value );
	}
}

void ShapeBase::setSelected( bool value ){
	m_selected = value;
	int controlCount = m_controlPoints.size();
	for ( int i = 0; i < controlCount; i++ ){
		m_controlPoints[i]->setSelected( value );
	}
}

void ShapeBase::setUserData( void * value ){
	m_userData = value;
}
}
}
