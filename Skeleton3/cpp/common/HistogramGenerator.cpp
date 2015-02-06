#include "HistogramGenerator.h"
#include <qwt_plot.h>
#include <qwt_plot_renderer.h>
#include <qwt_samples.h>
#include <qwt_plot_histogram.h>
#include <QImage>
#include <QPaintDevice>
#include <QString>
#include <qwt_scale_engine.h>

HistogramGenerator::HistogramGenerator(){
    m_plot = new QwtPlot();
    m_plot->setCanvasBackground( Qt::white );
    m_plot->setAxisTitle(QwtPlot::yLeft, QString("count(pixels)"));
    m_plot->setAxisTitle(QwtPlot::xBottom, QString("intensity()"));

    m_histogram = new QwtPlotHistogram();
    m_histogram->attach(m_plot);

}

void HistogramGenerator::setData(Carta::Lib::HistogramResult data){

    QString name = data.getName();
    m_plot->setTitle(name);


    std::vector<std::pair<double,double>> dataVector = data.getData();
    QVector<QwtIntervalSample> samples(0);
    int dataCount = dataVector.size();

    for ( int i = 0; i < dataCount-1; i++ ){
        QwtIntervalSample sample(dataVector[i].second, dataVector[i].first, dataVector[i+1].first);
        samples.append(sample);
    }

    m_histogram->setPen(Qt::blue);
    m_histogram->setSamples(samples);
    m_plot->replot();

}

QImage * HistogramGenerator::toImage(){
	QwtPlotRenderer * renderer = new QwtPlotRenderer();
	QSize size(335,335);
    QImage * histogramImage =new QImage(size, QImage::Format_RGB32);
    renderer->renderTo(m_plot,*histogramImage);
    return histogramImage;

}

void HistogramGenerator::setStyle( QString style ){
    if(style == "Outline")
        m_histogram->setStyle(QwtPlotHistogram::Columns);
    else if(style == "Line")
        m_histogram->setStyle(QwtPlotHistogram::Outline);
    else if(style == "Fill"){
        m_histogram->setStyle(QwtPlotHistogram::Outline);
        m_histogram->setBrush(QBrush(Qt::blue));
    }
}

void HistogramGenerator::setLogScale(bool display){

    if(display){
        m_plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine());
        m_histogram->setBaseline(1.0);

    }

    else{
        m_plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine());
        m_histogram->setBaseline(0.0);
    }
    

}

// void HistogramGenerator::setColored( bool colored ){
//    if(colored) m_histogram->setPen(Qt::blue);
//    else m_histogram->setPen(Qt::black);
// }

