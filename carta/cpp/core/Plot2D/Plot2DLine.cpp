#include "Plot2D/Plot2DLine.h"
#include <QPainter>
#include <QDebug>
#include <qwt_plot.h>
#include <qwt_symbol.h>

namespace Carta {
namespace Plot2D {


Plot2DLine::Plot2DLine():
    m_color( "#6699CC" ){
}


void Plot2DLine::draw ( QPainter* painter, const QwtScaleMap& xMap,
        const QwtScaleMap& /*yMap*/, const QRectF& /*canvasRect*/) const{
    double xPos = xMap.transform( m_position );
    //Mark the vertical boundary lines of the rectangle
    QPen oldPen = painter->pen();
    QPen boundaryPen( m_color );
    painter->setPen( boundaryPen );
    painter->drawLine( xPos, 0, xPos, m_height );
    painter->setPen( oldPen );
}


void Plot2DLine::setColor( QColor color ){
    m_color = color;
}

void Plot2DLine::setHeight( int h ){
	m_height = h;
}

void Plot2DLine::setPosition( double val ){
    m_position = val;
}

Plot2DLine::~Plot2DLine(){
}
}
}
