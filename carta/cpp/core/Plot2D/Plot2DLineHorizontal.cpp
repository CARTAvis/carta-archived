#include "Plot2D/Plot2DLineHorizontal.h"
#include <QPainter>
#include <QDebug>
#include <qwt_plot.h>
#include <qwt_symbol.h>

namespace Carta {
namespace Plot2D {


Plot2DLineHorizontal::Plot2DLineHorizontal():
    m_color( 0, 0, 0 ){
    m_position = 0;
}


void Plot2DLineHorizontal::draw ( QPainter* painter, const QwtScaleMap& /*xMap*/,
        const QwtScaleMap& yMap, const QRectF& canvasRect ) const{

    int positionPixel = yMap.transform( m_position );

    //Mark the vertical boundary lines of the rectangle
    QPen oldPen = painter->pen();
    QPen boundaryPen( m_color );
    painter->setPen( boundaryPen );
    painter->drawLine( canvasRect.left(), positionPixel, canvasRect.left() + canvasRect.width(), positionPixel );
    painter->setPen( oldPen );
}


void Plot2DLineHorizontal::setColor( QColor color ){
    m_color = color;
}


void Plot2DLineHorizontal::setPosition( double val ){
    m_position = val;
}


Plot2DLineHorizontal::~Plot2DLineHorizontal(){
}
}
}
