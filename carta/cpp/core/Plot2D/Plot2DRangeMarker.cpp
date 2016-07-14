#include "Plot2DRangeMarker.h"
#include <QPainter>
#include <QDebug>
#include <qwt_plot.h>

namespace Carta {
namespace Plot2D {


Plot2DRangeMarker::Plot2DRangeMarker():
        m_shadeColor( "#CCCC99"),
        m_interval( 0, 10 ){
	m_shadeColor.setAlpha( 100 );
}


void Plot2DRangeMarker::draw ( QPainter* painter, const QwtScaleMap& /*xMap*/,
        const QwtScaleMap& yMap, const QRectF& canvasRect) const{
    double y1 = m_interval.minValue();
    double y2 = m_interval.maxValue();
    int pixelY1 = yMap.transform( y1 );
    int pixelY2 = yMap.transform( y2 );
    if ( pixelY1 > pixelY2 ){
        int tmp = pixelY1;
        pixelY1 = pixelY2;
        pixelY2 = tmp;
    }
    qDebug() << "drawing from "<<pixelY1<<" to "<<pixelY2<<" height="<<canvasRect.height();
    painter->fillRect( QRectF( canvasRect.left(), pixelY1, canvasRect.width(),
            pixelY2-pixelY1 ), m_shadeColor );
}


void Plot2DRangeMarker::setRange( const QwtInterval& interval ){
    m_interval = interval;
}


Plot2DRangeMarker::~Plot2DRangeMarker(){
}
}
}
