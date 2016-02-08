#include "Plot2D/Plot2DLine.h"
#include <QPainter>
#include <QDebug>
#include <qwt_plot.h>
#include <qwt_symbol.h>

namespace Carta {
namespace Plot2D {


Plot2DLine::Plot2DLine():
    m_color( "#6699CC" ){
    m_selection = false;
    m_position = 0;
    m_positionPixel = 0;
}


void Plot2DLine::draw ( QPainter* painter, const QwtScaleMap& xMap,
        const QwtScaleMap& /*yMap*/, const QRectF& /*canvasRect*/ ) const{
    double position = m_position;
    //User is not selecting anything, use stored values for the bounds.
    if(!m_selection){
        position = xMap.transform( m_position );
    }
    else {
        //Get the position from pixel values.
        position = m_positionPixel;
    }
    m_drawPosition = xMap.invTransform( position );

    //Mark the vertical boundary lines of the rectangle
    QPen oldPen = painter->pen();
    QPen boundaryPen( m_color );
    painter->setPen( boundaryPen );
    painter->drawLine( position, 0, position, m_height );
    painter->setPen( oldPen );
}

double Plot2DLine::getPosition() const {
    return m_drawPosition;
}


void Plot2DLine::setColor( QColor color ){
    m_color = color;
}

void Plot2DLine::setHeight( int h ){
	m_height = h;
}

void Plot2DLine::setPositionPixel( double valMin, double valMax ){
    //One of the two will be the current position, we use the other one
    //as the new position.
    double diffMin = qAbs( valMin - m_position );
    double diffMax = qAbs( valMax - m_position );
    if ( diffMin < diffMax ){
        m_positionPixel = valMax;
    }
    else {
        m_positionPixel = valMin;
    }
}

void Plot2DLine::setPosition( double val ){
    m_position = val;
}

void Plot2DLine::setSelectionMode( bool drawing ){
    m_selection = drawing;
}

Plot2DLine::~Plot2DLine(){
}
}
}
