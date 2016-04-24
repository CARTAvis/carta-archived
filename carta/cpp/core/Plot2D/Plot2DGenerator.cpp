#include "Plot2DGenerator.h"
#include "Plot2DHistogram.h"
#include "Plot2DProfile.h"
#include "Plot2DSelection.h"
#include "Plot2DLine.h"
#include "Plot.h"
#include <qwt_scale_engine.h>
#include <qwt_plot_renderer.h>
#include "Data/Plotter/LegendLocations.h"
#include "CartaLib/PixelPipeline/CustomizablePixelPipeline.h"

namespace Carta {
namespace Plot2D {


const double Plot2DGenerator::EXTRA_RANGE_PERCENT = 0.05;


Plot2DGenerator::Plot2DGenerator( PlotType plotType ):
    m_rangeColor( nullptr ),
    m_vLine( nullptr ),
    m_gridLines( nullptr),
    m_font( "Helvetica", 10){
    m_legendVisible = false;
    m_logScale = false;
    m_legendPosition = Carta::Data::LegendLocations::BOTTOM;
    m_legendExternal = true;
    m_legendLineShow = true;
    m_plot = new Plot();
    m_plotType = plotType;

    m_plot->setFont( m_font );
    m_height = 335;
    m_width = 335;

    m_range = new Plot2DSelection();
    m_range->attach(m_plot);


    if ( plotType == PlotType::PROFILE ){
        m_vLine = new Plot2DLine();
        m_vLine->attach( m_plot );
        setLegendVisible( true );
    }
    else {
        m_logScale = true;
        m_rangeColor = new Plot2DSelection();
        QColor shadeColor( "#CCCC99");
        shadeColor.setAlpha( 100 );
        m_rangeColor->setColoredShade( shadeColor );
        m_rangeColor->attach( m_plot );
    }
}


void Plot2DGenerator::addData(std::vector<std::pair<double,double> > dataVector,
        const QString& id ){

    if ( dataVector.size() == 0 ){
        return;
    }

    std::shared_ptr<Plot2D> pData = _findData( id );
    if ( !pData ){
       if ( m_plotType == PlotType::PROFILE ){
           pData.reset( new Plot2DProfile() );
       }
       else if ( m_plotType == PlotType::HISTOGRAM ){
           //For right now, just one histogram plot
           if ( m_datas.size() > 0 ){
               clearData();
           }
           pData.reset( new Plot2DHistogram() );
       }
       else {
           qWarning() << "Unrecognized plot type: "<<(int)( m_plotType );
       }
       pData->setLegendLine( m_legendLineShow );
       m_datas.append( pData );
       pData->attachToPlot(m_plot);
       pData->setId( id );
    }

    if ( pData ){
        pData->setData( dataVector );
        _updateScales();
    }
}


void Plot2DGenerator::clearData(){
    int dataCount = m_datas.size();
    for ( int i = 0; i < dataCount; i++ ){
        m_datas[i]->detachFromPlot();
    }
    m_datas.clear();
}


void Plot2DGenerator::clearSelection(){
    m_range->reset();
    m_plot->replot();
}


void Plot2DGenerator::clearSelectionColor(){
    if ( m_rangeColor != nullptr ){
        m_rangeColor->reset();
        m_plot->replot();
    }
}


QString Plot2DGenerator::getAxisUnitsY() const {
    QString unitStr = m_plot->axisTitle( QwtPlot::yLeft ).text();
    return unitStr;
}


std::pair<double,double>  Plot2DGenerator::getPlotBoundsY( const QString& id, bool* valid ) const {
    std::pair<double,double> result;
    *valid = false;
    std::shared_ptr<Plot2D> plotData = _findData(id);
    if ( plotData ){
        result = plotData->getBoundsY();
        *valid = true;
    }
    else {
        qDebug() << "Generator could not get bounds for id="<<id;
    }
    return result;
}


QString Plot2DGenerator::getPlotTitle() const {
    return m_plot->title().text();
}


std::pair<double,double> Plot2DGenerator::getRange( bool* valid ) const {
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


std::shared_ptr<Plot2D> Plot2DGenerator::_findData( const QString& id ) const {
    std::shared_ptr<Plot2D> data( nullptr );
    int dataCount = m_datas.size();
    for ( int i = 0; i < dataCount; i++ ){
        if ( m_datas[i]->getId()  == id ){
            data = m_datas[i];
            break;
        }
    }
    return data;
}

double Plot2DGenerator::getVLinePosition( bool* valid ) const {
    *valid = false;
    double pos = 0;
    if ( m_vLine ){
        *valid = true;
        pos = m_vLine->getPosition( );
    }
    return pos;
}

void Plot2DGenerator::removeData( const QString& dataName ){
    std::shared_ptr<Plot2D> pData = _findData( dataName );
    if ( pData ){
        pData->detachFromPlot();
        m_datas.removeOne( pData );
    }
}

void Plot2DGenerator::setAxisXRange( double min, double max ){
    m_plot->setAxisScale( QwtPlot::xBottom, min, max );
    m_plot->replot();
}


void Plot2DGenerator::setColor( QColor color, const QString& id ){
    if ( id.isEmpty() || id.trimmed().length() == 0 ){
        int dataCount = m_datas.size();
        for ( int i = 0; i < dataCount; i++ ){
            m_datas[i]->detachFromPlot();
            m_datas[i]->setColor( color );
            m_datas[i]->attachToPlot( m_plot );
        }
    }
    else {
        std::shared_ptr<Plot2D> plotData = _findData( id );
        if ( plotData ){
            plotData->detachFromPlot();
            plotData->setColor( color );
            plotData->attachToPlot( m_plot );
        }
    }
}


void Plot2DGenerator::setColored( bool colored, const QString& id ){
    if ( id.isEmpty() || id.trimmed().length() == 0 ){
        int dataCount = m_datas.size();
        for ( int i = 0; i < dataCount; i++ ){
            m_datas[i]->setColored( colored );
        }
    }
    else {
        std::shared_ptr<Plot2D> plotData = _findData( id );
        if ( plotData ){
            plotData->setColored( colored );
        }
    }
}

void Plot2DGenerator::setCurveName( const QString& oldName, const QString& newName ){
    std::shared_ptr<Plot2D> plotData = _findData( oldName );
    if ( plotData ){
        plotData->setId( newName );
    }
}

void Plot2DGenerator::setGridLines( bool showGrid ){
    if ( !m_gridLines ){
        m_gridLines = new QwtPlotGrid();
        m_gridLines->enableX( false );
        m_gridLines->enableY( true );
    }
    if ( showGrid ){
        m_gridLines->attach( m_plot );
    }
    else {
        m_gridLines->detach();
    }
    m_plot->replot();
}


void Plot2DGenerator::setLegendExternal( bool externalLegend ){
    if ( m_legendExternal != externalLegend ){
        m_legendExternal = externalLegend;
        _updateLegend();
    }
}


void Plot2DGenerator::setLegendLocation( const QString& position ){
    if ( position != m_legendPosition ){
        m_legendPosition = position;
        _updateLegend();
    }
}

void Plot2DGenerator::setLegendLine( bool showLegendLine ){
    int dataCount = m_datas.size();
    m_legendLineShow = showLegendLine;
    for ( int i = 0; i < dataCount; i++ ){
        m_datas[i]->setLegendLine( m_legendLineShow );
    }
}


void Plot2DGenerator::setLegendVisible( bool visible ){
    if ( m_legendVisible != visible ){
        m_legendVisible = visible;
        _updateLegend();
    }
}


void Plot2DGenerator::setLogScale(bool logScale){
    m_logScale = logScale;
    _updateScales();
}


void Plot2DGenerator::setMarkerLine( double xPos ){
    if ( m_vLine ){
        m_vLine->setPosition( xPos );
        m_plot->replot();
    }
}


void Plot2DGenerator::setPipeline( std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> pipeline){
    int dataCount = m_datas.size();
    for ( int i = 0; i < dataCount; i++ ){
        m_datas[i]->setPipeline( pipeline );
    }
    m_plot->replot();
}


void Plot2DGenerator::setRange(double min, double max){
    m_range->setClipValues(min, max);
    m_plot->replot();
}


void Plot2DGenerator::setRangeColor(double min, double max){
    if ( m_rangeColor ){
        m_rangeColor->setClipValues(min, max);
    }
    m_plot->replot();
}


void Plot2DGenerator::setRangePixels(double min, double max){
    m_range->setHeight(m_height);
    m_range->setBoundaryValues(min, max);
    m_plot->replot();
}


void Plot2DGenerator::setRangePixelsColor(double min, double max){
    if ( m_rangeColor ){
        m_rangeColor->setHeight(m_height);
        m_rangeColor->setBoundaryValues(min, max);
    }
    if ( m_vLine ){
        m_vLine->setPositionPixel( min, max );
    }
    m_plot->replot();
}


void Plot2DGenerator::setSelectionMode(bool selection){
    m_range->setSelectionMode( selection );
}


void Plot2DGenerator::setSelectionModeColor( bool selection ){
    if ( m_rangeColor ){
        m_rangeColor->setSelectionMode( selection );
    }
    if ( m_vLine ){
        m_vLine->setSelectionMode( selection );
    }
}


bool Plot2DGenerator::setSize( int width, int height ){
    bool newSize = false;
    if ( width != m_width || height != m_height ){
        int minLength = qMin( width, height );
        if ( minLength > 0 ){
            m_width = width;
            m_height = height;
            m_range->setHeight( m_height );
            if ( m_rangeColor ){
                m_rangeColor->setHeight( m_height );
            }
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


void Plot2DGenerator::setLineStyle( const QString& style, const QString& id ){
    if ( id.isEmpty() || id.trimmed().length() == 0 ){
        int dataCount = m_datas.size();
        for ( int i = 0; i < dataCount; i++ ){
            //Attach & reattach so the legend icon updates.
            m_datas[i]->detachFromPlot();
            m_datas[i]->setLineStyle( style );
            m_datas[i]->attachToPlot( m_plot );
        }
    }
    else {
        std::shared_ptr<Plot2D> data = _findData( id );
        if ( data ){
            //Detaching & reattaching so that the icon will be redrawn
            //when the style changes.
            data->detachFromPlot();
            data->setLineStyle( style );
            data->attachToPlot( m_plot );
        }
    }
}


void Plot2DGenerator::setStyle( const QString& style, const QString& id ){
    if ( id.isEmpty() || id.trimmed().length() == 0 ){
        int dataCount = m_datas.size();
        for ( int i = 0; i < dataCount; i++ ){
            m_datas[i]->setDrawStyle( style );
        }
    }
    else {
        std::shared_ptr<Plot2D> data = _findData( id );
        if ( data ){
            data->setDrawStyle( style );
        }
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
    QString axisTitle = m_axisNameY;
    if ( !m_axisUnitY.isEmpty()){
        axisTitle = axisTitle + "(" + m_axisUnitY + ")";
    }
    if ( m_logScale ){
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


void Plot2DGenerator::_updateLegend(){
    m_plot->setLegendPosition( m_legendVisible, m_legendPosition, m_legendExternal );
}


void Plot2DGenerator::_updateScales(){
    int dataCount = m_datas.size();
    if ( dataCount > 0 ){
        std::pair<double,double> firstBounds = m_datas[0]->getBoundsY();
        double yMin = firstBounds.first;
        double yMax = firstBounds.second;
        for ( int i = 0; i < dataCount; i++ ){
            std::pair<double,double> plotBounds = m_datas[i]->getBoundsY();
            if ( plotBounds.first < yMin ){
                yMin = plotBounds.first;
            }
            if ( plotBounds.second > yMax ){
                yMax = plotBounds.second;
            }
            if( m_logScale ){
                m_datas[i]->setBaseLine(1.0);
            }
            else{
                m_datas[i]->setBaseLine(0.0);
            }
        }
        if ( m_logScale ){
            m_plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine());
            m_plot->setAxisScale( QwtPlot::yLeft, 1, yMax );
        }
        else {
            m_plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine());
            m_plot->setAxisScale( QwtPlot::yLeft, yMin, yMax );
        }
        m_plot->replot();
    }
}


Plot2DGenerator::~Plot2DGenerator(){
    clearData();
    m_range->detach();
    if ( m_rangeColor ){
        m_rangeColor->detach();
        delete m_rangeColor;
    }
    if ( m_vLine ){
        m_vLine->detach();
        delete m_vLine;
    }
    delete m_range;
}
}
}

