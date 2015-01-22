#include "HistogramGenerator.h"
#include <qwt_plot.h>
#include <qwt_plot_renderer.h>
#include <qwt_samples.h>
#include <qwt_plot_histogram.h>
#include <QImage>
#include <QPaintDevice>
#include <QString>

HistogramGenerator::HistogramGenerator(QString title){
    m_plot = new QwtPlot();
    m_plot->setTitle(title);
    m_plot->setCanvasBackground( Qt::white );
    m_plot->setAxisTitle(QwtPlot::yLeft, QString("count(pixels)"));
    m_plot->setAxisTitle(QwtPlot::xBottom, QString("intensity()"));

    m_histogram = new QwtPlotHistogram();
    m_histogram->attach(m_plot);

}

void HistogramGenerator::setData( QVector<QwtIntervalSample>& samples){
    m_histogram->setSamples(samples);
    m_plot->replot();

}

QImage * HistogramGenerator::toImage(){
	QwtPlotRenderer * renderer = new QwtPlotRenderer();
	QString imageTitle = "/scratch/Images/test.jpg";
	QSize size(100,100);
	QSizeF sizef(size);
	renderer->renderDocument(m_plot, imageTitle , sizef);
    QImage * histogramImage =new QImage();
    //renderer->renderTo(m_plot,histogramImage);
     
	
    histogramImage->load( "/scratch/Images/test.jpg");
    return histogramImage;

}

void HistogramGenerator::setStyle( QwtPlotHistogram::HistogramStyle style,
 QPen color, QBrush fill){
    m_histogram->setStyle(style);
    m_histogram->setPen(color);
    m_histogram->setBrush(fill);

}
