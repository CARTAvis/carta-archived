#include "../Histogram/HistogramGenerator.h"
#include "../Histogram/HistogramPlot.h"
#include "../Histogram/HistogramSelection.h"
#include <qwt_scale_engine.h>
#include <qwt_scale_map.h>
#include <QPaintDevice>
#include <QRectF>
#include <QPainter>
#include <qwt_plot_renderer.h>
#include <QImage>
#include <QWidget>
#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_samples.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_histogram.h>
#include "CartaLib/PixelPipeline/IPixelPipeline.h"

namespace Carta {
namespace Histogram {


const double HistogramGenerator::EXTRA_RANGE_PERCENT = 0.05;


HistogramGenerator::HistogramGenerator():
    m_font( "Helvetica", 10){
    m_plot = new QwtPlot();
    m_plot->setCanvasBackground( Qt::white );
    QwtText yTitle("Count(pixels)");
    yTitle.setFont( m_font );
    m_plot->setAxisTitle(QwtPlot::yLeft, yTitle);
    QwtText xTitle( "Intensity()");
    xTitle.setFont( m_font );
    m_plot->setAxisTitle(QwtPlot::xBottom, xTitle );
    QwtScaleWidget* leftWidget = m_plot->axisWidget( QwtPlot::yLeft );
    leftWidget->setFont( m_font );
    leftWidget->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding);
    QwtScaleWidget* bottomWidget = m_plot->axisWidget( QwtPlot::xBottom );
    bottomWidget->setFont( m_font );

    QWidget* canvas = m_plot->canvas();
    canvas->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding);


    m_histogram = new HistogramPlot();
    m_histogram->attach(m_plot);
    
    m_height = 335;
    m_width = 335;

    m_range = new HistogramSelection();
    m_range->attach(m_plot);

    m_clipMin = 0;
    m_clipMax = 0;

    setLogScale( true );
}

std::vector<double> HistogramGenerator::_getAxisRange(double minIntensity, double maxIntensity){
    std::vector<double> result;
    double difference = maxIntensity - minIntensity;
    double percent = difference*EXTRA_RANGE_PERCENT;
    result.push_back((minIntensity - percent));
    result.push_back((maxIntensity + percent));
    return result;
}

std::vector<double> HistogramGenerator::getHistogramClips(){
    std::vector<double> result;
    result.push_back(m_clipMin);
    result.push_back(m_clipMax);
    return result;
}


void HistogramGenerator::setColored( bool colored ){
    m_histogram->setColored( colored );
}

void HistogramGenerator::setColorMap(std::shared_ptr<Carta::Lib::PixelPipeline::IColormapNamed> cMap){
    m_histogram->setColorMap( cMap );
}

void HistogramGenerator::setData(Carta::Lib::Hooks::HistogramResult data, double minIntensity, double maxIntensity){
    QwtText name = data.getName();
    name.setFont( m_font );
    m_plot->setTitle(name);
    QwtText xAxisTitle = "Intensity("+ data.getUnits()+")";
    xAxisTitle.setFont( m_font );
    m_plot->setAxisTitle(QwtPlot::xBottom, xAxisTitle );

    m_clipMin = minIntensity;
    m_clipMax = maxIntensity; 

    std::vector<std::pair<double,double>> dataVector = data.getData();

    int dataCount = dataVector.size();
    QVector<QwtIntervalSample> samples(dataCount);
    for ( int i = 0; i < dataCount-1; i++ ){
        QwtIntervalSample sample(dataVector[i].second, dataVector[i].first, dataVector[i+1].first);
        samples[i]=sample;
    }

    m_histogram->setData(samples);

    std::vector<double> range =_getAxisRange(minIntensity, maxIntensity);
    m_plot->setAxisScale(QwtPlot::xBottom, range[0], range[1]);
    m_plot->replot();

}

void HistogramGenerator::setHistogramSelection(double min, double max){
    m_range->setHeight(m_height);
    m_range->setBoundaryValues(min, max);
    m_plot->replot();
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

void HistogramGenerator::setSelectionMode(bool selection){
    m_range->setSelectionMode( selection );
}

void HistogramGenerator::setSize( int width, int height ){
    int minLength = qMin( width, height );
    if ( minLength > 0 ){
        m_width = width;
        m_height = height;
        m_range->setHeight( m_height );
    }
    else {
        qWarning() << "Invalid histogram dimensions: "<<width<<" x "<< height;
    }
}

void HistogramGenerator::setStyle( QString style ){
    m_histogram->setDrawStyle( style );
}

QImage * HistogramGenerator::toImage(){
    QwtPlotRenderer * renderer = new QwtPlotRenderer();
    QImage * histogramImage =new QImage(m_width, m_height, QImage::Format_RGB32);
    renderer->renderTo(m_plot,*histogramImage);
    return histogramImage;

}

void HistogramGenerator::updateHistogramClips(){
    m_clipMin = m_range->getClipMin();
    m_clipMax = m_range->getClipMax();
    if(m_clipMin != m_clipMax){
        std::vector<double> range =_getAxisRange(m_clipMin, m_clipMax);
        m_plot->setAxisScale(QwtPlot::xBottom, range[0], range[1]);
        m_plot->replot();
    }
}

}
}

