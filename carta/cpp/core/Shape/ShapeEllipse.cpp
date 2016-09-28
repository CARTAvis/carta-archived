#include "ShapeEllipse.h"
#include "ControlPointEditable.h"
#include "CartaLib/VectorGraphics/VGList.h"
#include "CartaLib/Regions/Ellipse.h"

namespace Carta {
namespace Shape {


ShapeEllipse::ShapeEllipse( ):
	m_ellipseRegion( new Carta::Lib::Regions::Ellipse() ){
}


void ShapeEllipse::_controlPointCB( int index, bool final ){
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
		_updateEllipseFromShadow();
	}
}

void ShapeEllipse::_editShadow( const QPointF & pt ){
	m_shadowRect.setBottomRight( pt );
}

QPointF ShapeEllipse::getCenter() const {
	return m_ellipseRegion->getCenter();
}

QSizeF ShapeEllipse::getSize() const {
	return m_ellipseRegion->outlineBox().size();
}


Carta::Lib::VectorGraphics::VGList ShapeEllipse::getVGList() const {
	Carta::Lib::VectorGraphics::VGComposer comp;
	QPen pen = shadowPen;
	QBrush brush = Qt::NoBrush;

	//Draw the basic polygon
	comp.append < vge::SetPen > ( pen );
	comp.append < vge::SetBrush > ( brush );
	comp.append < vge::DrawRect > ( m_shadowRect );
	comp.append < vge::DrawEllipse > ( m_shadowRect );

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


void ShapeEllipse::handleDragDone( const QPointF & pt ){
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
		_updateEllipseFromShadow();

		setEditMode( false );
	}
	emit shapeChanged( m_ellipseRegion->toJson() );
}


bool ShapeEllipse::isCorner( const QPointF& pt ) const {
	bool corner = false;
	if ( pt == m_shadowRect.topLeft() || pt == m_shadowRect.topRight() ||
			pt == m_shadowRect.bottomRight() || pt == m_shadowRect.bottomLeft() ){
		corner = true;
	}
	return corner;
}

bool ShapeEllipse::isPointInside( const QPointF & pt ) const {
	bool pointInside = m_ellipseRegion-> isPointInside( {pt} );
	return pointInside;
}

void ShapeEllipse::_moveShadow( const QPointF& pt ){
	// calculate offset to move the shadow polygon to match the un-edited shape
	QRectF fixedRect = m_ellipseRegion->outlineBox();
	QPointF alreadyMoved = m_shadowRect.topLeft() - fixedRect.topLeft();

	// now move it by the amount of drag
	QPointF totalDrag = pt - m_dragStart;
	QPointF amountToMove = totalDrag - alreadyMoved;
	m_shadowRect.translate( amountToMove );
}


void ShapeEllipse::setModel( const QJsonObject& json ){
	m_ellipseRegion->initFromJson( json );
	m_shadowRect = m_ellipseRegion->outlineBox();
	_syncShadowToCPs();
}


void ShapeEllipse::_syncShadowToCPs(){
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

void ShapeEllipse::_updateEllipseFromShadow(){
	m_shadowRect = m_shadowRect.normalized();
	double width = m_shadowRect.width();
	double height = m_shadowRect.height();
	if ( width < height ){
		m_ellipseRegion->setRadiusMajor( height / 2 );
		m_ellipseRegion->setRadiusMinor( width / 2 );
		m_ellipseRegion->setAngle( 0 );
	}
	else {
		m_ellipseRegion->setRadiusMajor( width / 2 );
		m_ellipseRegion->setRadiusMinor( height / 2 );
		m_ellipseRegion->setAngle( 90 );
	}
	m_ellipseRegion->setCenter( m_shadowRect.center() );
}

}
}


