#include "Histogram/HistogramPlot.h"

HistogramPlot::HistogramPlot(){

}

void HistogramPlot::drawSeries(QPainter* painter, const QwtScaleMap& xMap, const QwtScaleMap& yMap,
		const QRectF& canvasRect, int from, int to) const{

	QBrush brush(Qt::yellow);
	QPen pen(Qt::red);
	//setPen ( const QPen & pen )
	painter->setBrush(brush);
	painter->setPen(pen);
	// QwtPlotHistogram::drawSeries(painter, xMap, yMap, canvasRect, from, to);
	qDebug()<<"draw series ";
	// QwtPlotHistogram::setPen(Qt::blue); 		
	//QwtPlotHistogram::drawOutline(painter,xMap,yMap,0,m_sampleCount-1);
	// QwtPlotHistogram::setPen(Qt::red);
	// QwtPlotHistogram::drawOutline(painter,xMap,yMap,m_sampleCount/2,m_sampleCount-1);
	QwtPlotHistogram::drawSeries( painter, xMap, yMap, canvasRect, from, to );
}

void HistogramPlot::setSampleCount(int sampleNumber){
	m_sampleCount = sampleNumber;
	//m_style = style;

}

HistogramPlot::~HistogramPlot(){

}
	
