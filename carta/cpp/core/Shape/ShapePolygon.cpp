#include "ShapePolygon.h"
#include "ControlPointEditable.h"
#include "CartaLib/VectorGraphics/VGList.h"
#include "CartaLib/Regions/IRegion.h"

namespace Carta {
namespace Shape {

ShapePolygon::ShapePolygon( ):
	m_polygonRegion( new Carta::Lib::Regions::Polygon() ){
}

void ShapePolygon::_controlPointCB( int index, bool final ){
	auto & poly = m_shadowPolygon;
	if ( index >= 0 && index < poly.size() ) {
		poly[index] = m_controlPoints[index]-> getCenter();
	}

	if ( final ) {
		m_polygonRegion->setqpolyf( m_shadowPolygon );
	}
}

void ShapePolygon::_editShadow( const QPointF& pt ){
	_moveShadow( pt );
	_syncShadowToCPs();
}

QPointF ShapePolygon::getCenter() const {
	QRectF box = m_polygonRegion->outlineBox();
	return box.center();
}

QSizeF ShapePolygon::getSize() const {
	return m_polygonRegion->outlineBox().size();
}



Carta::Lib::VectorGraphics::VGList ShapePolygon::getVGList() const {
	Carta::Lib::VectorGraphics::VGComposer comp;
	QPen pen = shadowPen;
	QBrush brush = Qt::NoBrush;

	//Draw the basic polygon
	comp.append < vge::SetPen > ( pen );
	comp.append < vge::SetBrush > ( brush );
	comp.append < vge::DrawPolygon > ( m_shadowPolygon );

	//If there is only one point, in the polygon, it will not draw so
	//we draw a square for the point.
	if ( m_shadowPolygon.size() == 1 ){
		ControlPointEditable cpe( nullptr );
		cpe.setPosition( m_shadowPolygon.value( 0 ) );
		Carta::Lib::VectorGraphics::VGList pointVGList = cpe.getVGList();
		comp.appendList( pointVGList );
	}

	//Only draw the rest if we are not creating the region.
	if ( !isEditMode() ){
		//Draw the outline box; use a different style if it is selected.
		if ( isSelected() ){
			comp.append < vge::SetPen > ( outlinePen );
			QRectF shadowRect = m_shadowPolygon.boundingRect();
			comp.append < vge::DrawRect > ( shadowRect );
			comp.append < vge::SetPen > ( pen );
		}
		else if ( isHovered() || isActive() ){
			comp.append <vge::DrawRect >( m_shadowPolygon.boundingRect() );
		}

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




void ShapePolygon::handleDragDone( const QPointF & pt ){
	int controlPointCount = m_controlPoints.size();
	if ( m_dragControlIndex >= 0 && m_dragControlIndex < controlPointCount ){
			m_controlPoints[m_dragControlIndex]->handleDragDone( pt );
			m_dragControlIndex = -1;
		}
	else {
		_moveShadow( pt );
		_syncShadowToCPs();

		// exit drag mode
		m_dragMode = false;

		// update the region
		m_polygonRegion-> setqpolyf( m_shadowPolygon );
	}
	emit shapeChanged( m_polygonRegion->toJson() );

}

bool ShapePolygon::isClosed() const {
	QPolygonF poly = m_polygonRegion->qpolyf();
	return poly.isClosed();
}

bool ShapePolygon::isCorner( const QPointF& pt ) const {
	return m_shadowPolygon.contains( pt );
}

bool ShapePolygon::isPointInside( const QPointF & pt ) const {
	bool pointInside = m_polygonRegion-> isPointInside( {pt} );
	return pointInside;
}

void ShapePolygon::_moveShadow( const QPointF& pt ){
	// calculate offset to move the shadow polygon to match the un-edited shape
	QPointF offset = m_polygonRegion->qpolyf().at( 0 ) - m_shadowPolygon.at( 0 );

	// now move it by the amount of drag
	offset += pt - m_dragStart;

	// apply the offset to the shadow
	m_shadowPolygon.translate( offset );
}


void ShapePolygon::setModel( const QJsonObject& json ){
	m_polygonRegion->initFromJson( json );
	m_shadowPolygon = m_polygonRegion->qpolyf();
	_syncShadowToCPs();
}


void ShapePolygon::_syncShadowToCPs(){
	// make missing control points if necessary
	while ( int ( m_controlPoints.size() ) < m_shadowPolygon.size() ) {
		int index = m_controlPoints.size();
		auto cb = [this, index] ( bool final ) {
			_controlPointCB( index, final );
		};
		auto cp = std::make_shared < ControlPointEditable > ( cb );
		cp-> setActive( false );
		m_controlPoints.push_back( cp );
	}

	CARTA_ASSERT( m_shadowPolygon.size() == int ( m_controlPoints.size() ) );
	for ( int index = 0 ; index < m_shadowPolygon.size() ; index++ ) {
		m_controlPoints[index]->setPosition( m_shadowPolygon[index] );
	}
}
}
}


