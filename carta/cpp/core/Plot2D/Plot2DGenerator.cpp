#include "Plot2DGenerator.h"
#include "Plot2DHistogram.h"
#include "Plot2DProfile.h"
#include "Plot2DSelection.h"
#include "Plot2DLine.h"
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
#include <qwt_scale_widget.h>
#include "CartaLib/PixelPipeline/CustomizablePixelPipeline.h"

namespace Carta {
namespace Plot2D {


const double Plot2DGenerator::EXTRA_RANGE_PERCENT = 0.05;


Plot2DGenerator::Plot2DGenerator( PlotType plotType ):
    m_plot2D( nullptr ),
    m_vLine( nullptr ),
    m_font( "Helvetica", 10){
    m_plot = new QwtPlot();
    m_plot->setCanvasBackground( Qt::white );
    m_plot->setAxisAutoScale( QwtPlot::yLeft, false );

    QwtScaleWidget* leftWidget = m_plot->axisWidget( QwtPlot::yLeft );
    leftWidget->setFont( m_font );
    leftWidget->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding);
    QwtScaleWidget* bottomWidget = m_plot->axisWidget( QwtPlot::xBottom );
    bottomWidget->setFont( m_font );

    QWidget* canvas = m_plot->canvas();
    canvas->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding);

    if ( plotType == PlotType::PROFILE ){
        m_plot2D = new Plot2DProfile();
    }
    else if ( plotType == PlotType::HISTOGRAM ){
        m_plot2D = new Plot2DHistogram();
    }
    else {
        qWarning() << "Unrecognized plot type: "<<(int)(plotType );
    }

    if ( m_plot2D ){
        m_plot2D->attachToPlot(m_plot);
    }
    
    m_height = 335;
    m_width = 335;

    m_range = new Plot2DSelection();
    m_range->attach(m_plot);

    m_rangeColor = new Plot2DSelection();
    QColor shadeColor( "#CCCC99");
    shadeColor.setAlpha( 100 );
    m_rangeColor->setColoredShade( shadeColor );
    m_rangeColor->attach( m_plot );

    if ( plotType == PlotType::PROFILE ){
        m_vLine = new Plot2DLine();
        m_vLine->attach( m_plot );
    }
}


void Plot2DGenerator::clearSelection(){
    m_range->reset();
    m_plot->replot();
}


void Plot2DGenerator::clearSelectionColor(){
    m_rangeColor->reset();
    m_plot->replot();
}


QString Plot2DGenerator::getAxisUnitsY() const {
    QString unitStr = m_plot->axisTitle( QwtPlot::yLeft ).text();
    return unitStr;
}


std::pair<double,double>  Plot2DGenerator::getPlotBoundsY( bool* valid ) const {
    std::pair<double,double> result;
    *valid = false;
    if ( m_plot2D ){
        result = m_plot2D->getBoundsY();
        *valid = true;
    }
    return result;
}


QString Plot2DGenerator::getPlotTitle() const {
    return m_plot->title().text();
}


std::pair<double,double> Plot2DGenerator::getRange(bool* valid ) const {
    std::pair<double,double> result;
    *valid = false;
    if ( m_range ){
        result.first = m_range->getClipMin();
        result.second = m_range->getClipMax();
        *valid = true;
    }
    return result;
}


std::pair<double,double> Plot2DGenerator::getRangeColor(bool* valid ) const {
    std::pair<double,double> result;
    *valid = false;
    if ( m_rangeColor ){
        result.first = m_rangeColor->getClipMin();
        result.second = m_rangeColor->getClipMax();
        *valid = true;
    }
    return result;
}


bool Plot2DGenerator::isSelectionOnCanvas( int xPos ) const {
    bool selectionOnCanvas = false;
    if ( xPos >= 0 ){
        //Get the ratio of the canvas margin to the plot width;
        float plotWidth = m_plot->size().width();
        float canvasWidth = m_plot->canvas()->size().width();
        float plotMargin = plotWidth - canvasWidth;
        if ( xPos > plotMargin ){
            selectionOnCanvas = true;
        }
    }
    return selectionOnCanvas;
}


void Plot2DGenerator::setAxisXRange( double min, double max ){
    m_plot->setAxisScale( QwtPlot::xBottom, min, max );
    m_plot->replot();
}


void Plot2DGenerator::setColored( bool colored ){
    if ( m_plot2D ){
        m_plot2D->setColored( colored );
    }
}


void Plot2DGenerator::setData(Carta::Lib::Hooks::Plot2DResult data){
    QwtText name = data.getName();
    name.setFont( m_font );
    m_plot->setTitle(name);

    m_axisUnitX = data.getUnitsX();
    m_axisUnitY = data.getUnitsY();
    setTitleAxisX( m_axisNameX );
    setTitleAxisY( m_axisNameY );

    std::vector<std::pair<double,double>> dataVector = data.getData();
    if ( m_plot2D ){
        m_plot2D->setData( dataVector );
        _updateScales();
    }
}


void Plot2DGenerator::setLogScale(bool logScale){
    if ( m_plot2D ){
        m_plot2D->setLogScale( logScale );
        _updateScales();
    }
}


void Plot2DGenerator::setMarkerLine( double xPos ){
    if ( m_vLine ){
        m_vLine->setPosition( xPos );
        m_plot->replot();
    }
}


void Plot2DGenerator::setPipeline( std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> pipeline){
    if ( m_plot2D ){
        m_plot2D->setPipeline( pipeline );
    }
}


void Plot2DGenerator::setRange(double min, double max){
    m_range->setClipValues(min, max);
    m_plot->replot();
}


void Plot2DGenerator::setRangeColor(double min, double max){
    m_rangeColor->setClipValues(min, max);
    m_plot->replot();
}


void Plot2DGenerator::setRangePixels(double min, double max){
    m_range->setHeight(m_height);
    m_range->setBoundaryValues(min, max);
    m_plot->replot();
}


void Plot2DGenerator::setRangePixelsColor(double min, double max){
    m_rangeColor->setHeight(m_height);
    m_rangeColor->setBoundaryValues(min, max);
    m_plot->replot();
}


void Plot2DGenerator::setSelectionMode(bool selection){
    m_range->setSelectionMode( selection );
}


void Plot2DGenerator::setSelectionModeColor( bool selection ){
    m_rangeColor->setSelectionMode( selection );
}


bool Plot2DGenerator::setSize( int width, int height ){
    bool newSize = false;
    if ( width != m_width || height != m_height ){
        int minLength = qMin( width, height );
        if ( minLength > 0 ){
            m_width = width;
            m_height = height;
            m_range->setHeight( m_height );
            m_rangeColor->setHeight( m_height );
            if ( m_vLine ){
                m_vLine->setHeight( m_height );
            }
            newSize = true;
        }
        else {
            qWarning() << "Invalid plot dimensions: "<<width<<" x "<< height;
        }
    }
    return newSize;
}


void Plot2DGenerator::setStyle( QString style ){
    if ( m_plot2D ){
        m_plot2D->setDrawStyle( style );
    }
}


void Plot2DGenerator::setTitleAxisX( const QString& title){
    m_axisNameX = title;
    QString axisTitle = m_axisNameX;
    if ( !m_axisUnitX.isEmpty() ){
        axisTitle = axisTitle + "(" + m_axisUnitX + ")";
    }
    QwtText xTitle( axisTitle );
    xTitle.setFont( m_font );
    m_plot->setAxisTitle( QwtPlot::xBottom, xTitle );
}


void Plot2DGenerator::setTitleAxisY( const QString& title){
    m_axisNameY = title;
    bool logScale = false;
    if ( m_plot2D ){
        logScale = m_plot2D->isLogScale();
    }
    QString axisTitle = m_axisNameY;
    if ( !m_axisUnitY.isEmpty()){
        axisTitle = axisTitle + "(" + m_axisUnitY + ")";
    }
    if ( logScale ){
        axisTitle = "Log " + axisTitle;
    }
    QwtText yTitle( axisTitle );
    yTitle.setFont( m_font );
    m_plot->setAxisTitle(QwtPlot::yLeft, yTitle);
}


QImage * Plot2DGenerator::toImage( int width, int height ) const {
    QwtPlotRenderer renderer;
    if ( width <= 0 ){
        width = m_width;
    }
    if ( height <= 0 ){
        height = m_height;
    }
    QImage * plotImage =new QImage(width, height, QImage::Format_RGB32);
    renderer.renderTo(m_plot, *plotImage );
    return plotImage;
}


void Plot2DGenerator::_updateScales(){
    if ( m_plot2D ){
        bool logScale = m_plot2D->isLogScale();
        std::pair<double,double> plotBounds = m_plot2D->getBoundsY();
        if( logScale ){
            m_plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine());
            m_plot2D->setBaseLine(1.0);
            m_plot->setAxisScale( QwtPlot::yLeft, 1, plotBounds.second );
        }
        else{
            m_plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine());
            m_plot2D->setBaseLine(0.0);
            m_plot->setAxisScale( QwtPlot::yLeft, plotBounds.first, plotBounds.second );
        }
        m_plot->replot();
    }
}


Plot2DGenerator::~Plot2DGenerator(){
    if ( m_plot2D ){
        m_plot2D->detachFromPlot( );
    }
    m_range->detach();
    m_rangeColor->detach();
    if ( m_vLine ){
        m_vLine->detach();
        delete m_vLine;
    }
    delete m_plot2D;
    delete m_range;
    delete m_rangeColor;
}
}
}

