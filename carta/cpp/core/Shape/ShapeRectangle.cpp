#include "ShapeRectangle.h"
#include "ControlPointEditable.h"
#include "CartaLib/VectorGraphics/VGList.h"
#include "CartaLib/Regions/Rectangle.h"

namespace Carta {
namespace Shape {


ShapeRectangle::ShapeRectangle( ):
	m_rectRegion( new Carta::Lib::Regions::Rectangle() ){
}


void ShapeRectangle::_controlPointCB( int index, bool final ){
	QRectF & rect = m_shadowRect;
	if ( index >= 0 && index < CORNER_COUNT ) {
		QPointF corner = m_controlPoints[index]-> getCenter();
		//Update the corner points of the shadow rectangle to match
		if ( index == 0 ){
			rect.setTopLeft( corner );
		}
		else if ( index == 1 ){
			rect.setTopRight( corner );
		}
		else if ( index == 2 ){
			rect.setBottomRight( corner );
		}
		else {
			rect.setBottomLeft( corner );
		}
		_syncShadowToCPs();
	}

	if ( final ) {
		m_rectRegion->setRectangle( m_shadowRect );
	}
}

QPointF ShapeRectangle::getCenter() const {
	return m_rectRegion->outlineBox().center();
}

QSizeF ShapeRectangle::getSize() const {
	return m_rectRegion->outlineBox().size();
}

Carta::Lib::VectorGraphics::VGList ShapeRectangle::getVGList() const {
	Carta::Lib::VectorGraphics::VGComposer comp;
	QPen pen = shadowPen;
	QBrush brush = Qt::NoBrush;

	//Draw the basic polygon
	comp.append < vge::SetPen > ( pen );
	comp.append < vge::SetBrush > ( brush );
	comp.append < vge::DrawRect > ( m_shadowRect );

	//Only draw the rest if we are not creating the region.
	if ( !isEditMode() ){
		//Draw the control points
		if ( isHovered() || isSelected()){
			int cornerCount = m_controlPoints.size();
			for ( int i = 0; i < cornerCount; i++ ){
				comp.appendList( m_controlPoints[i]->getVGList() );
			}
		}
	}
	return comp.vgList();
}


void ShapeRectangle::_editShadow( const QPointF& pt ){
	m_shadowRect.setBottomRight( pt );
}


void ShapeRectangle::handleDragDone( const QPointF & pt ){
	int controlPointCount = m_controlPoints.size();
	if ( m_dragControlIndex >= 0 && m_dragControlIndex < controlPointCount ){
		m_controlPoints[m_dragControlIndex]->handleDragDone( pt );
		m_dragControlIndex = -1;
	}
	else {
		// calculate offset to move the shadow polygon to match the un-edited shape
		if ( !isEditMode() ){
			_moveShadow( pt );
			_syncShadowToCPs();
		}

		// exit drag mode
		m_dragMode = false;

		// update the region
		m_shadowRect = m_shadowRect.normalized();

		m_rectRegion-> setRectangle( m_shadowRect );
		setEditMode( false );
	}
	emit shapeChanged( m_rectRegion->toJson() );
}


bool ShapeRectangle::isCorner( const QPointF& pt ) const {
	bool corner = false;
	if ( pt == m_shadowRect.topLeft() || pt == m_shadowRect.topRight() ||
			pt == m_shadowRect.bottomRight() || pt == m_shadowRect.bottomLeft() ){
		corner = true;
	}
	return corner;
}

bool ShapeRectangle::isPointInside( const QPointF & pt ) const {
	bool pointInside = m_rectRegion-> isPointInside( {pt} );
	return pointInside;
}

void ShapeRectangle::_moveShadow( const QPointF& pt ){
	QRectF modelRect = m_rectRegion->outlineBox();
	QPointF offset = modelRect.topLeft() - m_shadowRect.topLeft();

	// now move it by the amount of drag
	offset += pt - m_dragStart;

	// apply the offset to the shadow
	m_shadowRect.translate( offset );
}


void ShapeRectangle::setModel( const QJsonObject& json ){
	m_rectRegion->initFromJson( json );
	m_shadowRect = m_rectRegion->outlineBox();
	_syncShadowToCPs();
}


void ShapeRectangle::_syncShadowToCPs(){
	// make missing control points if necessary
	while ( int ( m_controlPoints.size() ) < CORNER_COUNT ) {
		int index = m_controlPoints.size();
		auto cb = [this, index] ( bool final ) {
			_controlPointCB( index, final );
		};
		auto cp = std::make_shared < ControlPointEditable > ( cb );
		cp-> setActive( false );
		m_controlPoints.push_back( cp );
	}

	CARTA_ASSERT( CORNER_COUNT == int ( m_controlPoints.size() ) );
	m_controlPoints[0]->setPosition( m_shadowRect.topLeft() );
	m_controlPoints[1]->setPosition( m_shadowRect.topRight() );
	m_controlPoints[2]->setPosition( m_shadowRect.bottomRight() );
	m_controlPoints[3]->setPosition( m_shadowRect.bottomLeft() );
}
}
}


