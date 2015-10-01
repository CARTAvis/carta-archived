#include "Histogram/HistogramSelection.h"
#include <QPainter>
#include <QDebug>
#include <qwt_plot.h>

namespace Carta {
namespace Histogram {


HistogramSelection::HistogramSelection():
        m_shadeColor( 200,200,200 )
    {
	reset();
	m_clipMin = 0;
	m_clipMax = 0;
	m_selection = false;
	m_shadeColor.setAlpha( 100 );
}

void HistogramSelection::boundaryLineMoved( const QPoint& pos ){
    int xValue = pos.x();
    if ( !rangeSet ){
        m_lowerBound = xValue;
        m_upperBound = xValue;
        rangeSet = true;
    }
    else {
        int lowDistance = qAbs( xValue - m_lowerBound );
        int highDistance = qAbs( xValue - m_upperBound );
        if ( lowDistance <= highDistance ){
            m_lowerBound = xValue;
        }
        else {
            m_upperBound = xValue;
        }
    }
}

void HistogramSelection::draw ( QPainter* painter, const QwtScaleMap& xMap,
        const QwtScaleMap& /*yMap*/, const QRectF& /*canvasRect*/) const{

    double lowerBound = m_lowerBound;
    double upperBound = m_upperBound;
    //User is not selecting anything, use stored values for the bounds.
    if(!m_selection){
        lowerBound = xMap.transform( m_clipMin );
        upperBound = xMap.transform( m_clipMax );
    }
    else {
        //Store the clip min and max from pixel values.
        m_clipMin= xMap.invTransform( lowerBound );
        m_clipMax = xMap.invTransform( upperBound );
    }

    if ( lowerBound != upperBound ){

        //Draw the rectangle
        int startX = lowerBound;
        if ( upperBound < lowerBound ){
            startX = upperBound;
        }
        int rectHeight = m_height;
        int rectWidth = qAbs( lowerBound - upperBound );
        QRect rect( startX, 0, rectWidth, rectHeight );
        painter->fillRect( rect , m_shadeColor );

        //Mark the vertical boundary lines of the rectangle
        QPen oldPen = painter->pen();
        QPen boundaryPen( Qt::black );
        painter->setPen( boundaryPen );
        painter->drawLine( lowerBound, 0, lowerBound, rectHeight );
        painter->drawLine( upperBound, 0, upperBound, rectHeight );
        painter->setPen( oldPen );
    }

}

double HistogramSelection::getClipMax() const {
    return m_clipMax;
}

double HistogramSelection::getClipMin() const {
    return m_clipMin;
}

int HistogramSelection::getLowerBound() const {
    int value = m_lowerBound;
    if ( m_upperBound < m_lowerBound ){
        value = m_upperBound;
    }
    return value;
}

int HistogramSelection::getUpperBound() const {
    int value = m_upperBound;
    if ( m_lowerBound > m_upperBound ){
        value = m_lowerBound;
    }
    return value;
}

void HistogramSelection::reset(){
    rangeSet = false;
    m_lowerBound = 0;
    m_upperBound = 0;

    m_clipMin = 0;
    m_clipMax = 0;

}

void HistogramSelection::setClipValues( double minX, double maxX ){
    m_clipMin = minX;
    m_clipMax = maxX;
}


void HistogramSelection::setColoredShade( QColor color ){
    m_shadeColor = color;
}

void HistogramSelection::setHeight( int h ){
	m_height = h;
}

void HistogramSelection::setBoundaryValues( double minX, double maxX ){
	m_lowerBound = minX;
	m_upperBound = maxX;
}


void HistogramSelection::setSelectionMode(bool selection){
	m_selection = selection;
}



HistogramSelection::~HistogramSelection(){
}
}
}
