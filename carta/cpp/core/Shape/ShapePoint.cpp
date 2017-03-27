#include "ShapePoint.h"
#include "ControlPointEditable.h"
#include "CartaLib/VectorGraphics/VGList.h"
#include "CartaLib/Regions/Point.h"

namespace Carta {
namespace Shape {


ShapePoint::ShapePoint( ):
	m_pointRegion( new Carta::Lib::Regions::Point() ){
}


void ShapePoint::_editShadow( const QPointF& /*pt*/ ){
}

QPointF ShapePoint::getCenter() const {
	return m_pointRegion->outlineBox().center();
}

QSizeF ShapePoint::getSize() const {
	return m_pointRegion->outlineBox().size();
}

Carta::Lib::VectorGraphics::VGList ShapePoint::getVGList() const {
	Carta::Lib::VectorGraphics::VGComposer comp;
	QPen pen = shadowPen;
	pen.setCosmetic(true);
        QPen rectPen = outlinePen;
	rectPen.setCosmetic(true);
	QBrush brush = Qt::NoBrush;

	//Draw the basic polygon (exterior manipulation region of point)
	comp.append < vge::SetPen > ( pen );
	comp.append < vge::SetBrush > ( brush );
	comp.append < vge::DrawEllipse > ( m_shadowRect );

        //Draw inner region of point
        QRectF innerRect;
        double width = m_shadowRect.width()/4;
        double x = m_shadowRect.center().x() - (width/2);
        double y = m_shadowRect.center().y() - (width/2);
        innerRect.setRect(x,y,width,width);
        comp.append < vge::SetPen > ( rectPen );
	comp.append < vge::SetBrush > ( brush );
        comp.append < vge::DrawEllipse > ( innerRect );

	return comp.vgList();
}


void ShapePoint::handleDragDone( const QPointF & pt ){
	// calculate offset to move the shadow polygon to match the un-edited shape
	if ( !isEditMode() ){
		_moveShadow( pt );
	}

	// exit drag mode
	m_dragMode = false;

	// update the region
	m_shadowRect = m_shadowRect.normalized();

	m_pointRegion-> setPoint( m_shadowRect.center() );
	setEditMode( false );

	emit shapeChanged( m_pointRegion->toJson() );
}


bool ShapePoint::isPointInside( const QPointF & pt ) const {
	bool pointInside = m_pointRegion->isPointInside( {pt} );
	int cornerCount = m_controlPoints.size();
	bool onControlPoint = false;
	for ( int i = 0; i < cornerCount; i++ ){
	  onControlPoint = m_controlPoints[i]->isPointInside( {pt} );
	  if (onControlPoint){
	    break;
	  }
	}
	return (pointInside|onControlPoint);
}

void ShapePoint::_moveShadow( const QPointF& pt ){
	QRectF modelRect = m_pointRegion->outlineBox();
	QPointF offset = modelRect.topLeft() - m_shadowRect.topLeft();

	// now move it by the amount of drag
	offset += pt - m_dragStart;

	// apply the offset to the shadow
	m_shadowRect.translate( offset );
}


void ShapePoint::setModel( const QJsonObject& json ){
	m_pointRegion->initFromJson( json );
	m_shadowRect = m_pointRegion->outlineBox();
}

void ShapePoint::_syncShadowToCPs(){
}
}
}
