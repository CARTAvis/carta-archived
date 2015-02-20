#include "HistogramGenerator.h"
#include <qwt_scale_engine.h>
#include <qwt_scale_map.h>
#include <QPaintDevice>
#include <QRectF>
#include <QPainter>
#include <qwt_plot_renderer.h>
#include <QImage>
#include <QWidget>
#include <qwt_plot.h>
#include <qwt_samples.h>
#include <qwt_plot_histogram.h>

const double HistogramGenerator::EXTRA_RANGE_PERCENT = 0.05;


HistogramGenerator::HistogramGenerator(){
    m_plot = new QwtPlot();
    m_plot->setCanvasBackground( Qt::white );
    m_plot->setAxisTitle(QwtPlot::yLeft, QString("count(pixels)"));
    m_plot->setAxisTitle(QwtPlot::xBottom, QString("intensity()"));

    m_histogram = new HistogramPlot();
    m_histogram->attach(m_plot);
    

    m_height = 335;
    m_width = 335;

    m_range = new HistogramSelection();

    m_height = 335;
    m_width = 335;

}

void HistogramGenerator::setData(Carta::Lib::Hooks::HistogramResult data, double minIntensity, double maxIntensity){

    QString name = data.getName();
    m_plot->setTitle(name);


    std::vector<std::pair<double,double>> dataVector = data.getData();
    QVector<QwtIntervalSample> samples(0);
    int dataCount = dataVector.size();

    for ( int i = 0; i < dataCount-1; i++ ){
        QwtIntervalSample sample(dataVector[i].second, dataVector[i].first, dataVector[i+1].first);
        samples.append(sample);
    }

    m_histogram->setSampleCount(dataCount);
    // m_histogram->setPen(Qt::blue);
    m_histogram->setSamples(samples);

    // QwtPlotHistogram* m_histogram2 = new QwtPlotHistogram();
    // m_histogram2->attach(m_plot);
    // m_histogram2->setPen(Qt::red);
    // m_histogram2->setSamples(samples);

    std::vector<double> range =_getAxisRange(minIntensity, maxIntensity);
    m_plot->setAxisScale(QwtPlot::xBottom, range[0], range[1]);

    m_plot->replot();

}

std::vector<double> HistogramGenerator::_getAxisRange(double minIntensity, double maxIntensity){
    std::vector<double> result;
    double difference = maxIntensity - minIntensity;
    double percent = difference*EXTRA_RANGE_PERCENT;
    result.push_back((minIntensity - percent));
    result.push_back((maxIntensity + percent));
    qDebug()<<"Axis min: "<<result[0]<<" , max: "<<result[1];
    return result;
}

QImage * HistogramGenerator::toImage(){
	QwtPlotRenderer * renderer = new QwtPlotRenderer();
	QSize size(m_height,m_width);
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

// void HistogramGenerator::setHistogramRange(double min, double max){
   // HistogramSelection * range = new HistogramSelection();
    //QRect rect = m_plot->frameRect();
    //int w = rect.width();

   // double minClip = m_plot -> transform(QwtPlot::xBottom, min);
   // double maxClip = m_plot -> transform(QwtPlot::xBottom, max);

   // QwtScaleMap canvasMap = m_plot-> canvasMap(QwtPlot::xBottom);
    //double minClip = canvasMap.transform(min);
   //double maxClip = canvasMap.transform(max);
//    range->setHeight(m_height);
//    range->setBoundaryValues(min, max);
//    range->attach(m_plot);
//    m_plot->replot();
// }

void HistogramGenerator::lineSelected(){
    // int lowerBound = m_range->getLowerBound();
    // int upperBound = m_range->getUpperBound();
    // double lowerBoundWorld = binPlot.invTransform( QwtPlot::xBottom, lowerBound );
    // double upperBoundWorld = binPlot.invTransform( QwtPlot::xBottom, upperBound );
    // setMinMaxValues( lowerBoundWorld, upperBoundWorld, false );
    qDebug()<<"line selected";
}

void HistogramGenerator::lineMoved( const QPointF& pt ){
    // m_range->boundaryLineMoved( pt );
    // m_range->show();
    // m_plot->replot();
    qDebug()<<"line moved";
}

void HistogramGenerator::setHistogramRange(double min, double max){
    m_range->setHeight(m_height);
    m_range->setBoundaryValues(min, max);
    m_range->attach(m_plot);
}

// void HistogramGenerator::setColored( bool colored ){
//    if(colored) m_histogram->setPen(Qt::blue);
//    else m_histogram->setPen(Qt::black);
// }

