#include "HistogramSelection.h"
#include <QPainter>
#include <QDebug>
#include <qwt_plot.h>


HistogramSelection::HistogramSelection(){
    reset();
}

void HistogramSelection::setHeight( int h ){
    m_height = h;
}

int HistogramSelection::getLowerBound() const {
    int value = lowerBound;
    if ( upperBound < lowerBound ){
        value = upperBound;
    }
    return value;
}

int HistogramSelection::getUpperBound() const {
    int value = upperBound;
    if ( lowerBound > upperBound ){
        value = lowerBound;
    }
    return value;
}

void HistogramSelection::reset(){
    rangeSet = false;
    lowerBound = 0;
    upperBound = 0;
}

void HistogramSelection::setBoundaryValues( double minX, double maxX ){
    lowerBound = minX;
    upperBound = maxX;
}

void HistogramSelection::boundaryLineMoved( const QPoint& pos ){
    int xValue = pos.x();
    if ( !rangeSet ){
        lowerBound = xValue;
        upperBound = xValue;
        rangeSet = true;
    }
    else {
        int lowDistance = qAbs( xValue - lowerBound );
        int highDistance = qAbs( xValue - upperBound );
        if ( lowDistance <= highDistance ){
            lowerBound = xValue;
        }
        else {
            upperBound = xValue;
        }
    }
}

void HistogramSelection::draw ( QPainter* painter, const QwtScaleMap& xMap,
        const QwtScaleMap& /*yMap*/, const QRectF& /*canvasRect*/) const{

    double lowerBoundT = xMap.transform( lowerBound );
    double upperBoundT = xMap.transform( upperBound );
    if ( lowerBound != upperBound ){

        //Draw the rectangle
        QColor shadeColor( 200,200,200 );
        shadeColor.setAlpha( 100 );

        int startX = lowerBoundT;
        if ( upperBoundT < lowerBoundT ){
            startX = upperBoundT;
        }
        int rectHeight = m_height;
        int rectWidth = qAbs( lowerBoundT - upperBoundT );
        QRect rect( startX, 0, rectWidth, rectHeight );
        painter->fillRect( rect , shadeColor );

        //Mark the vertical boundary lines of the rectangle
        QPen oldPen = painter->pen();
        QPen boundaryPen( Qt::black );
        painter->setPen( boundaryPen );
        painter->drawLine( lowerBoundT, 0, lowerBoundT, rectHeight );
        painter->drawLine( upperBoundT, 0, upperBoundT, rectHeight );
        painter->setPen( oldPen );
    }
}

HistogramSelection::~HistogramSelection(){
}
