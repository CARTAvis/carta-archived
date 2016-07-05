#include "Plot2DGenerator.h"

#include "Data/Plotter/LegendLocations.h"
#include "CartaLib/PixelPipeline/CustomizablePixelPipeline.h"

namespace Carta {
namespace Plot2D {

using Carta::Data::LegendLocations;

Plot2DGenerator::Plot2DGenerator( /*Plot2DHolder::PlotType plotType*/ ){
   Plot2DHolder* plotHolder = new Plot2DHolder();
   m_plots.push_back( plotHolder );
   m_height = 0;
   m_width = 0;
}


void Plot2DGenerator::addData(std::vector<std::pair<double,double> > dataVector,
        const QString& id, int index  ){
    if ( _checkIndex( index )){
        m_plots[index]->addData( dataVector, id );
    }
    _resetExtents();
}

void Plot2DGenerator::addLabels( const std::vector<std::tuple<double,double,QString> >& labels, int index ){
    if ( _checkIndex( index )){
        m_plots[index]->addLabels( labels );
    }
}


int Plot2DGenerator::addPlot(){
    int plotIndex = m_plots.size();
    Plot2DHolder* holder = new Plot2DHolder();
    m_plots.push_back( holder );
    //Right now we are hard-coding the second plot to be a residual
    //and customizing the settings for a residual plot.  At some point,
    //we may be adding other plots and methods should be made more
    //general.
    //There are at least two plots
    if ( plotIndex >= 1 ){
        //First plot should use a top position for the axis and last
        //plot should use a bottom axis index.
        m_plots[0]->setAxisLocationX( QwtPlot::xTop );
        m_plots[plotIndex]->setAxisLocationX( QwtPlot::xBottom );

        //Get legend properties from existing plots.
        QString pos = m_plots[0]->getLegendLocation();
        m_plots[plotIndex]->setLegendLocation( pos );
        bool externalLegend = m_plots[0]->isExternalLegend();
        m_plots[plotIndex]->setLegendExternal( externalLegend );
        if ( externalLegend && pos == LegendLocations::BOTTOM ){
            bool visibleLegend = m_plots[plotIndex - 1]->isLegendVisible();
            setLegendVisible( visibleLegend );
        }
    }
    return plotIndex;
}


bool Plot2DGenerator::_checkIndex( int index ) const {
    bool validPlot = false;
    int plotCount = m_plots.size();
    if ( index >= 0 && index < plotCount ){
        validPlot = true;
    }
    else {
        qWarning()<<"Invalid plot index: "<<index;
    }
    return validPlot;
}


void Plot2DGenerator::clearData( int index ){
    if ( _checkIndex( index )){
        m_plots[index]->clearData();
    }
}

void Plot2DGenerator::clearLabels( int index ){
    if ( _checkIndex( index ) ){
        m_plots[index]->clearLabels();
    }
}


void Plot2DGenerator::clearSelection( int index ){
    if ( _checkIndex( index )){
        m_plots[index]->clearSelection();
    }
}


void Plot2DGenerator::clearSelectionColor( int index ){
    if ( _checkIndex( index )){
        m_plots[index]->clearSelectionColor();
    }
}


QString Plot2DGenerator::getAxisUnitsY( int index ) const {
    QString unitStr;
    if ( _checkIndex( index )){
        unitStr = m_plots[index]->getAxisUnitsY();
    }
    return unitStr;
}

int Plot2DGenerator::_getExternalLegendIndex() const {
    //Find the plot that is showing the external legend, if there is one.
    int plotCount = m_plots.size();
    int plotLegendIndex = -1;
    for ( int i = 0; i < plotCount; i++ ){
        if ( m_plots[i]->isLegendVisible() && m_plots[i]->isExternalLegend()){
            plotLegendIndex = i;
            break;
        }
    }
    return plotLegendIndex;
}

QPointF Plot2DGenerator::getImagePoint(const QPointF& screenPt, int index ) const {
    QPointF pt;
    if ( _checkIndex( index ) ){
        pt = m_plots[index]->getImagePoint( screenPt );
    }
    return pt;
}

std::pair<double,double>  Plot2DGenerator::getPlotBoundsY( const QString& id, bool* valid, int index ) const {
    std::pair<double,double> result;
    *valid = false;
    if ( _checkIndex( index )){
        result = m_plots[index]->getPlotBoundsY( id, valid );
    }
    return result;
}


QString Plot2DGenerator::getPlotTitle( int index ) const {
    QString title;
    if ( _checkIndex( index )){
        title = m_plots[index]->getPlotTitle();
    }
    return title;
}


std::pair<double,double> Plot2DGenerator::getRange( bool* valid, int index ) const {
    std::pair<double,double> result;
    *valid = false;
    if ( _checkIndex( index )){
        result = m_plots[index]->getRange( valid );
    }
    return result;
}


std::pair<double,double> Plot2DGenerator::getRangeColor(bool* valid, int index ) const {
    std::pair<double,double> result;
    *valid = false;
    if ( _checkIndex( index )){
        result = m_plots[index]->getRangeColor( valid );
    }
    return result;
}

QPointF Plot2DGenerator::getScreenPoint( const QPointF& dataPoint, int index ) const {
    QPointF pt;
    if ( _checkIndex( index )){
        pt = m_plots[index]->getScreenPoint( dataPoint );
    }
    return pt;
}


QSize Plot2DGenerator::getPlotSize( int index ) const {
    QSize size;
    if ( _checkIndex( index )){
        size = m_plots[index]->getPlotSize();
    }
    return size;
}

QPointF Plot2DGenerator::getPlotUpperLeft( int index ) const {
    QPointF pt;
    if ( _checkIndex( index )){
        pt = m_plots[index]->getPlotUpperLeft();
    }
    return pt;
}

double Plot2DGenerator::getVLinePosition( bool* valid, int index ) const {
    double position = 0;
    if ( _checkIndex( index )){
        position = m_plots[index]->getVLinePosition( valid );
    }
    else {
        *valid = false;
    }
    return position;
}

bool Plot2DGenerator::isSelectionOnCanvas( int xPos, int index ) const {
    bool selectionOnCanvas = false;
    if ( _checkIndex( index )){
        selectionOnCanvas = m_plots[index]->isSelectionOnCanvas( xPos );
    }
    return selectionOnCanvas;
}


void Plot2DGenerator::_paintLegend( int x, int y, int width, int height, QPainter* painter ) const{
    QRect printGeom( x, y, width, height );
    m_plots[0]->legendToImage( painter, printGeom );
}

void Plot2DGenerator::removeData( const QString& dataName, int index ){
    if ( _checkIndex( index )){
        m_plots[index]->removeData( dataName );
    }
}

void Plot2DGenerator::removePlot( int index ){
    if ( _checkIndex( index ) ){
        bool visible = m_plots[index]->isLegendVisible();
        delete m_plots[index];
        m_plots[index] = nullptr;
        m_plots.erase( m_plots.begin() + index );
        int plotCount = m_plots.size();
        if ( plotCount == 1 ){
            if ( visible ){
                m_plots[0]->setLegendVisible( true );
            }
            m_plots[0]->setAxisLocationX( QwtPlot::xBottom );
        }
    }
}

void Plot2DGenerator::_resetExtents(){
    int maxExtent = 0;
    int plotCount = m_plots.size();
    if ( plotCount > 0 ){
        for ( int i = 0; i < plotCount; i++ ){
            m_plots[i]->clearAxisExtentY();
            int extent = m_plots[i]-> getAxisExtentY();
            if ( extent > maxExtent ){
                maxExtent = extent;
            }
        }
        for ( int i = 0; i < plotCount; i++ ){
            m_plots[i]->setAxisExtentY( maxExtent );
        }
    }
}

void Plot2DGenerator::setAxisXRange( double min, double max, int index ){
    if ( _checkIndex( index )){
        m_plots[index]->setAxisXRange( min, max );
    }
}


void Plot2DGenerator::setColor( QColor color, const QString& id, int index ){
    if ( _checkIndex( index )){
        m_plots[index]->setColor( color, id );
    }
}


void Plot2DGenerator::setColored( bool colored, const QString& id, int index ){
    if ( _checkIndex( index )){
        m_plots[index]->setColored( colored, id );
    }
}

void Plot2DGenerator::setCurveName( const QString& oldName, const QString& newName, int index ){
    if ( _checkIndex( index )){
        m_plots[index]->setCurveName( oldName, newName );
    }
}

void Plot2DGenerator::setGridLines( bool showGrid ){
    int plotCount = m_plots.size();
    for ( int i = 0; i < plotCount; i++ ){
        m_plots[i]->setGridLines( showGrid );
    }
}

void Plot2DGenerator::setHistogram( bool histogram, int index  ){
    if ( _checkIndex( index )){
        Plot2DHolder::PlotType type = Plot2DHolder::PlotType::PROFILE;
        if ( histogram ){
            type = Plot2DHolder::PlotType::HISTOGRAM;
        }
        m_plots[index]->setPlotType( type );
    }
}

void Plot2DGenerator::setHLinePosition( double position, int index ){
    if ( _checkIndex( index )){
        m_plots[index]->setHLinePosition( position );
    }
}

void Plot2DGenerator::setHLineVisible( bool visible, int index ){
    if ( _checkIndex( index )){
        m_plots[index]->setHLineVisible( visible );
    }
}


void Plot2DGenerator::setLegendExternal( bool externalLegend ){
    int plotCount = m_plots.size();
    for ( int i = 0; i < plotCount; i++ ){
        m_plots[i]->setLegendExternal( externalLegend );
    }
}


void Plot2DGenerator::setLegendLocation( const QString& position ){
    int plotCount = m_plots.size();

    if ( plotCount > 1 ){
        int plotLegendIndex = _getExternalLegendIndex();
        if ( plotLegendIndex >= 0 ){
            QString oldLoc = m_plots[plotLegendIndex]->getLegendLocation();
            if ( oldLoc == LegendLocations::BOTTOM && position != LegendLocations::BOTTOM ){
                //If we are changing from bottom to something else, turn off
                //the legend on the last plot and turn on the legend on the first
                //plot
                m_plots[plotLegendIndex]->setLegendVisible( false );
                m_plots[0]->setLegendVisible( true );
            }
            else if ( oldLoc != LegendLocations::BOTTOM && position == LegendLocations::BOTTOM ){
                //Changing to bottom from something else;
                //Turn the legend on for the bottom plot and make it invisible for
                //the others.
                m_plots[plotLegendIndex]->setLegendVisible( false );
                m_plots[plotCount - 1]->setLegendVisible( true );
            }
        }
    }
    //Update the legend location on all the plots
    for ( int i = 0; i < plotCount; i++ ){
        m_plots[i]->setLegendLocation( position );
    }
    _resetExtents();
}

void Plot2DGenerator::setLegendLine( bool showLegendLine ){
    int plotCount = m_plots.size();
    for ( int i = 0; i < plotCount; i++ ){
        m_plots[i]->setLegendLine( showLegendLine );
    }
}


void Plot2DGenerator::setLegendVisible( bool visible ){
    int plotCount = m_plots.size();
    if ( plotCount > 0 ){
        for ( int i = 0; i < plotCount; i++ ){
            m_plots[i]->setLegendVisible( false );
        }
        if ( visible ){
            //Only one of the plots should show the legend.  Normally, it will be
            //the first plot, except when the legend is on the bottom and external.
            QString pos = m_plots[0]->getLegendLocation();
            bool external = m_plots[0]->isExternalLegend();
            if ( external && pos == LegendLocations::BOTTOM ){
                m_plots[plotCount - 1]->setLegendVisible( visible );
            }
            else {
                m_plots[0]->setLegendVisible( visible );
            }
        }
    }
}


void Plot2DGenerator::setLogScale(bool logScale, int index ){
    if ( _checkIndex( index )){
        m_plots[index]->setLogScale( logScale );
    }
}


void Plot2DGenerator::setMarkerLine( double xPos, int index ){
    if ( _checkIndex( index )){
        m_plots[index]->setMarkerLine( xPos );
    }
}



void Plot2DGenerator::setMarkedRange( double minY, double maxY, int index  ){
    if ( _checkIndex( index )){
        m_plots[index]->setMarkedRange( minY, maxY );
    }
}


void Plot2DGenerator::setPipeline( std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> pipeline,
        int index){
    if ( _checkIndex( index ) ){
        m_plots[index]->setPipeline( pipeline );
    }
}


void Plot2DGenerator::setRange(double min, double max, int index ){
    if ( _checkIndex( index ) ){
        m_plots[index]->setRange( min, max );
    }
}


void Plot2DGenerator::setRangeColor(double min, double max, int index ){
    if ( _checkIndex( index ) ){
        m_plots[index]->setRangeColor( min, max );
    }
}

void Plot2DGenerator::setRangeMarkerVisible( bool visible, int index ){
    if ( _checkIndex( index ) ){
        m_plots[index]->setRangeMarkerVisible( visible );
    }
}


void Plot2DGenerator::setRangePixels(double min, double max, int index ){
    if ( _checkIndex( index ) ){
        m_plots[index]->setRangePixels( min, max );
    }
}


void Plot2DGenerator::setRangePixelsColor(double min, double max, int index ){
    if ( _checkIndex( index ) ){
        m_plots[index]->setRangePixelsColor( min, max );
    }
}


void Plot2DGenerator::setSelectionMode(bool selection, int index ){
    if ( _checkIndex( index ) ){
        m_plots[index]->setSelectionMode( selection );
    }
}


void Plot2DGenerator::setSelectionModeColor( bool selection, int index ){
    if ( _checkIndex( index ) ){
        m_plots[index]->setSelectionModeColor( selection );
    }
}


bool Plot2DGenerator::setSize( int width, int height, int index ){
    bool newSize = false;
    if ( _checkIndex( index ) ){
        if ( width != m_width || height != m_height ){
            int minLength = qMin( width, height );
            if ( minLength > 0 ){
                m_width = width;
                m_height = height;
                m_plots[index]->setSize( width, height );
                newSize = true;
            }
            else {
                qWarning() << "Invalid plot dimensions: "<<width<<" x "<< height;
            }
        }
    }
    return newSize;
}


void Plot2DGenerator::setLineStyle( const QString& style, const QString& id, int index  ){
    if ( _checkIndex( index ) ){
        m_plots[index]->setLineStyle( style, id );
    }
}


void Plot2DGenerator::setStyle( const QString& style, const QString& id, int index ){
    if ( _checkIndex( index ) ){
        m_plots[index]->setStyle( style, id );
    }
}


void Plot2DGenerator::setTitleAxisX( const QString& title, int index ){
    if ( _checkIndex( index ) ){
        m_plots[index]->setTitleAxisX( title );
    }
}


void Plot2DGenerator::setTitleAxisY( const QString& title, int index ){
    if ( _checkIndex( index ) ){
        m_plots[index]->setTitleAxisY( title );
    }
}


QImage Plot2DGenerator::toImage( int width, int height ) const {
    if ( width <= 0 ){
        width = m_width;
    }
    if ( height <= 0 ){
        height = m_height;
    }

    QImage plotImage(width, height, QImage::Format_RGB32);
    int plotCount = m_plots.size();
    if ( height > 0 && plotCount > 0 ){
        //Decide if we need to add space for a legend.
        QSize legendSize;
        QString legendPos = LegendLocations::BOTTOM;
        int externalIndex = _getExternalLegendIndex();
        QSize plotSize = m_plots[0]->getSize();
        if ( externalIndex >= 0 ){
            legendPos = m_plots[externalIndex]->getLegendLocation();
            legendSize = m_plots[externalIndex]->getLegendSize();
        }
        double percentHeight = (legendSize.height() * 1.0) / plotSize.height();
        double percentWidth = (legendSize.width() * 1.0) / plotSize.width();
        int legendHeight = percentHeight * height;
        int legendWidth = percentWidth * width;
        QPainter painter( & plotImage );
        if ( legendPos == LegendLocations::BOTTOM || legendPos == LegendLocations::TOP ){
            int plotHeight = (height - legendHeight) / plotCount;
            int startY = 0;

            for ( int i = 0; i < plotCount; i++ ){
                int y = startY;
                int graphHeight = plotHeight;
                if ( i == externalIndex ){
                    //Add some extra height to accomodate the legend.
                    graphHeight = graphHeight + legendHeight;
                }
                startY = startY + graphHeight;
                QRect printGeom( 0, y, width, graphHeight );
                m_plots[i]->toImage( &painter, printGeom );
            }
        }
        //Left or right - we have to draw it ourselves.
        else {
            int plotWidth = width - legendWidth;
            int plotHeight = height / plotCount;
            int startX = 0;
            if ( legendWidth > 0 && legendPos == LegendLocations::LEFT ){
                _paintLegend( 0, 0, legendWidth, height, &painter );
                startX = legendWidth;
            }
            for ( int i = 0; i < plotCount; i++ ){
                QRect printGeom( startX,  plotHeight*i, plotWidth, plotHeight );
                m_plots[i]->toImage( &painter, printGeom );
            }
            if ( legendWidth > 0 && legendPos == LegendLocations::RIGHT ){
                _paintLegend( plotWidth, 0, legendWidth, height, &painter );
            }
        }
    }
    return plotImage;
}


Plot2DGenerator::~Plot2DGenerator(){
    int plotCount = m_plots.size();
    for ( int i = 0; i < plotCount; i++ ){
        delete m_plots[i];
    }
    m_plots.clear();
}
}
}

