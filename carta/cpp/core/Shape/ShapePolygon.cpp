#include "ShapePolygon.h"
#include "ControlPointEditable.h"
#include "CartaLib/VectorGraphics/VGList.h"
#include "CartaLib/Regions/IRegion.h"

namespace Carta {
namespace Shape {

ShapePolygon::ShapePolygon( std::shared_ptr<Carta::Lib::Regions::Polygon> polygonRegion ):
	m_dragStart(QPointF(0,0)){
	m_polygonRegion = polygonRegion;
	m_shadowPolygon = m_polygonRegion-> qpolyf();
	CARTA_ASSERT( m_shadowPolygon.size() > 0 );
}

Carta::Lib::VectorGraphics::VGList ShapePolygon::getVGList() const {
	Carta::Lib::VectorGraphics::VGComposer comp;

	bool drawShadow = false;
	QPen pen = shadowPen;
	QBrush brush = Qt::NoBrush;

	if ( isHovered() ) {
		drawShadow = true;
	}

	if ( isSelected() ) {
		drawShadow = true;
		comp.append < vge::SetPen > ( outlinePen );
		comp.append < vge::SetBrush > ( QBrush( Qt::NoBrush ) );
		comp.append < vge::DrawRect > ( m_shadowPolygon.boundingRect() );
	}

	if ( m_inDragMode || isEditable() ) {
		drawShadow = true;
		brush = shadowBrush;

		//            comp.append < vge::SetPen > ( QPen( QColor( 0, 255, 255, 128 ), 1 ) );
		//            auto poly = m_polygonRegion-> qpolyf();
		//            poly.translate( m_dragCenter - poly.boundingRect().center() );
		//            comp.append < vge::DrawPolygon > ( m_shadowPolygon );
	}

	if ( drawShadow ) {
		comp.append < vge::SetPen > ( pen );
		comp.append < vge::SetBrush > ( brush );
		comp.append < vge::DrawPolygon > ( m_shadowPolygon );
	}

	if ( isEditable() ) {
		//            comp.append < vge::SetPen > ( QPen( QColor( 255, 0, 0 ), 1 ) );

		//            comp.append < vge::DrawLine > ( m_centerCP-> pos(), m_radiusCP-> pos() );
	}

	return comp.vgList();
} // getVGList

bool ShapePolygon::isPointInside( const QPointF & pt ) const {
	return m_polygonRegion-> isPointInside( {pt} );
}

void ShapePolygon::handleDragStart( const QPointF & pt ){
	m_inDragMode = true;
	m_dragStart = pt;
}

void ShapePolygon::handleDrag( const QPointF & pt ){
	// calculate offset if we are starting the drag
	if ( ! m_inDragMode ) {
		qWarning() << "dragging but not in drag mode";
		return;
	}

	// calculate offset to move the shadow polygon to match the un-edited shape
	QPointF offset = m_polygonRegion->qpolyf().at( 0 ) - m_shadowPolygon.at( 0 );

	// now move it by the amount of drag
	offset += pt - m_dragStart;

	// apply the offset to the shadow
	m_shadowPolygon.translate( offset );
	_syncShadowToCPs();
} // handleDrag

void ShapePolygon::handleDragDone( const QPointF & pt ){
	// calculate offset to move the shadow polygon to match the un-edited shape
	QPointF offset = m_polygonRegion->qpolyf().at( 0 ) - m_shadowPolygon.at( 0 );

	// now move it by the amount of drag
	offset += pt - m_dragStart;

	// apply the offset to the shadow
	m_shadowPolygon.translate( offset );
	_syncShadowToCPs();

	// exit drag mode
	m_inDragMode = false;

	// update the region
	m_polygonRegion-> setqpolyf( m_shadowPolygon );
} // handleDragDone



void ShapePolygon::_syncShadowToCPs(){
	// make missing control points if necessary
	while ( int ( m_cps.size() ) < m_shadowPolygon.size() ) {
		int index = m_cps.size();
		auto cb = [this, index] ( bool final ) {
			controlPointCB( index, final );
		};
		auto cp = std::make_shared < ControlPointEditable > ( cb );
		cp-> setActive( false );
		m_cps.push_back( cp );
		//m_erc.editableShapesController().addShape( cp );
	}

	//        // make the control points if we have not made them yet
	//        if ( m_cps.size() == 0 ) {
	//            for ( int index = 0 ; index < m_shadowPolygon.size() ; index++ ) {
	//                auto cb = [this, index] ( bool final ) {
	//                    controlPointCB( index, final );
	//                };
	//                auto cp = std::make_shared < ControlPointEditable > ( cb );
	//                m_cps.push_back( cp );
	//                m_erc.editableShapesController().addShape( cp );
	//            }
	//        }

	CARTA_ASSERT( m_shadowPolygon.size() == int ( m_cps.size() ) );
	for ( int index = 0 ; index < m_shadowPolygon.size() ; index++ ) {
		m_cps[index]->setPosition( m_shadowPolygon[index] );
	}
} // syncShadowToCPs

void ShapePolygon::editableChanged(){
	// update the positions of control points
	_syncShadowToCPs();

	// activate/deactivate control points based on edit status
	for ( auto & pt : m_cps ) {
		pt-> setActive( isEditable() );
	}
} // do_editModeChanged

void ShapePolygon::controlPointCB( int index, bool final ){
	auto & poly = m_shadowPolygon;
	if ( index >= 0 && index < poly.size() ) {
		poly[index] = m_cps[index]-> getPosition();
	}

	if ( final ) {
		m_polygonRegion->setqpolyf( m_shadowPolygon );
	}

	//    auto poly = m_polygonRegion-> qpolyf();
	//    if ( index >= 0 && index < poly.size() ) {
	//        poly[index] = m_cps[index]-> pos();
	//        m_polygonRegion-> setqpolyf( poly );
	//    }
}
}
}


