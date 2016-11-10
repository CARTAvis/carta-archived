#include "Plot2DHolder.h"
#include "Plot2DHistogram.h"
#include "Plot2DProfile.h"
#include "Plot2DRangeMarker.h"
#include "Plot2DSelection.h"
#include "Plot2DLine.h"
#include "Plot2DLineHorizontal.h"
#include "Plot2DTextMarker.h"
#include "Plot.h"
#include <qwt_interval.h>
#include <qwt_scale_engine.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_renderer.h>
#include "Data/Plotter/LegendLocations.h"
#include "CartaLib/PixelPipeline/CustomizablePixelPipeline.h"

namespace Carta {
namespace Plot2D {


const double Plot2DHolder::EXTRA_RANGE_PERCENT = 0.05;


Plot2DHolder::Plot2DHolder():
    m_rangeColor( nullptr ),
    m_hLine( nullptr ),
    m_vLine( nullptr ),
    m_rangeMarker( nullptr ),
    m_gridLines( nullptr),
    m_font( "Helvetica", 10){

    m_legendVisible = false;
    m_logScale = false;
    m_legendPosition = Carta::Data::LegendLocations::BOTTOM;
    m_legendExternal = true;
    m_legendLineShow = true;
    m_plot = new Plot();

    m_plot->setFont( m_font );
    m_height = 335;
    m_width = 335;

    m_range = new Plot2DSelection();
    m_range->attach(m_plot);
    setPlotType( PlotType::PROFILE );
}



void Plot2DHolder::addData(std::vector<std::pair<double,double> > dataVector,
        const QString& id, bool primary ){
    if ( dataVector.size() == 0 ){
        return;
    }

    //First look for primary data & make it if that was requested.
    std::shared_ptr<Plot2D> pData = _findData( id, true );
    if ( !pData && primary  ){
        pData = _makeData();
        m_datas.append( pData );
    }
    else if ( pData && !primary ){
        //Store the primary color
        QColor pColor = pData->getColor();
        //See if there is a secondary curve with this id and make
        //one if there is not.
        pData = _findData(id, false );
        if ( !pData ){
            pData = _makeData();
            pData->setColor(pColor);
            pData->setLegendVisible( false );
            m_dataSecondary.append( pData );
        }
    }

    if ( pData ){
        pData->setId( id );
        pData->setData( dataVector );
        pData->attachToPlot(m_plot);
        _updateScales();
    }
}

void Plot2DHolder::addLabels( const std::vector<std::tuple<double,double,QString> >& labels ){
    int labelCount = labels.size();
    for ( int i = 0; i < labelCount; i++ ){
        Plot2DTextMarker* exp = new Plot2DTextMarker();
        exp->setXValue( std::get<0>( labels[i] ) );
        exp->setYValue( std::get<1>( labels[i] ) );
        exp->setContent( std::get<2>( labels[i] ) );
        exp->attach( m_plot );
        m_textMarkers.append( std::shared_ptr<Plot2DTextMarker>(exp) );
    }
}

void Plot2DHolder::clearAxisExtentY(){
    m_plot->clearAxisExtentY();
}

void Plot2DHolder::clearData(){
    //Note:  although this seems faster, we can't use it because
    //it also removes the legend
   // m_plot->detachItems( QwtPlotItem::Rtti_PlotItem, false );
    _detachData( m_datas );
    m_datas.clear();
    clearDataFit();

}

void Plot2DHolder::clearDataFit(){
    _detachData( m_dataSecondary );
    m_dataSecondary.clear();
}

void Plot2DHolder::clearLabels(){
    int labelCount = m_textMarkers.size();
    for ( int i = 0; i < labelCount; i++ ){
        m_textMarkers[i]->detach();
    }
    m_textMarkers.clear();
}


void Plot2DHolder::_clearItem( QwtPlotItem* item ){
    if ( item ){
        item->detach();
        delete item;
    }
}


void Plot2DHolder::_clearMarkers(){
    m_plot->detachItems( QwtPlotItem::Rtti_PlotMarker, false );

    delete m_range;
    m_range = nullptr;
    delete m_rangeColor;
    m_rangeColor = nullptr;

    _clearItem( m_gridLines );
    m_gridLines = nullptr;
    _clearItem( m_rangeMarker );
    m_rangeMarker = nullptr;

    delete m_hLine;
    m_hLine = nullptr;
    delete m_vLine;
    m_vLine = nullptr;
}


void Plot2DHolder::clearSelection(){
    if ( m_range ){
        m_range->reset();
        m_plot->replot();
    }
}


void Plot2DHolder::clearSelectionColor(){
    if ( m_rangeColor != nullptr ){
        m_rangeColor->reset();
        m_plot->replot();
    }
}

void Plot2DHolder::_detachData(QList<std::shared_ptr<Plot2D> >& datas ){
    int dataCount = datas.size();
       for ( int i = 0; i < dataCount; i++ ){
           datas[i]->detachFromPlot();
       }
}

std::shared_ptr<Plot2D> Plot2DHolder::_findData( const QString& id, bool primary  ) const {
    std::shared_ptr<Plot2D> data( nullptr );
    if ( primary ){
        data = _findDataPrimary( id );
    }
    else {
        data = _findDataSecondary( id );
    }
    return data;
}

std::shared_ptr<Plot2D> Plot2DHolder::_findDataPrimary( const QString& id  ) const {
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

std::shared_ptr<Plot2D> Plot2DHolder::_findDataSecondary( const QString& id  ) const {
    std::shared_ptr<Plot2D> data( nullptr );
    int dataCount = m_dataSecondary.size();
    for ( int i = 0; i < dataCount; i++ ){
        if ( m_dataSecondary[i]->getId()  == id ){
            data = m_dataSecondary[i];
            break;
        }
    }
    return data;
}

int Plot2DHolder::getAxisExtentY() const {
    return m_plot->getAxisExtentY();
}


QString Plot2DHolder::getAxisUnitsY() const {
    QString unitStr = m_plot->getAxisUnitsY();
    return unitStr;
}

std::pair<double,double> Plot2DHolder::getAxisXRange() const {
    std::pair<double,double> range = m_plot->getAxisScaleX();
    return range;
}

QColor Plot2DHolder::getColor( const QString& id, bool* valid ) const {
    QColor color;
    *valid = false;
    std::shared_ptr<Plot2D> data = _findData( id, true );
    if ( data ){
        *valid = true;
        color = data->getColor();
    }
    return color;
}

void Plot2DHolder::_getDataBounds( double* dataMinX, double* dataMaxX,
        double* dataMinY, double* dataMaxY ) const {
    int dataCount = m_datas.size();
    for ( int i = 0; i < dataCount; i++ ){
        std::pair<double,double> boundsX = m_datas[i]->getBoundsX();
        std::pair<double,double> boundsY = m_datas[i]->getBoundsY();
        if ( boundsX.first < *dataMinX ){
            *dataMinX = boundsX.first;
        }
        if ( boundsX.second > *dataMaxX ){
            *dataMaxX = boundsX.second;
        }
        if ( boundsY.first < *dataMinY ){
            *dataMinY = boundsY.first;
        }
        if ( boundsY.second > *dataMaxY ){
            *dataMaxY = boundsY.second;
        }
    }
}

QPointF Plot2DHolder::getImagePoint(const QPointF& screenPt, bool* valid ) const {
    //Returns a point in image coordinates that may not correspond to
    //an actual data point.
    *valid = false;
    QPointF imagePoint = m_plot->getImagePoint( screenPt );

    //Find the 'closest' data point to the screen point.
    int dataCount = m_datas.size();
    double dataMinX = std::numeric_limits<double>::max();
    double dataMaxX = -1 * dataMinX;
    double dataMinY = std::numeric_limits<double>::max();
    double dataMaxY = -1 * dataMinY;
    _getDataBounds( &dataMinX, &dataMaxX, &dataMinY, &dataMaxY );
    double targetErrorX = _getRelativeError( dataMinX, dataMaxX );
    double targetErrorY = _getRelativeError( dataMinY, dataMaxY );

    //Find the index that yields the smallest error in the x-coordinate withen the
    //acceptable bounds.
    double minErrorX = std::numeric_limits<double>::max();
    for ( int i = 0; i < dataCount; i++ ){
        double xError = std::numeric_limits<double>::max();
        double yError = std::numeric_limits<double>::max();
        std::pair<double,double> closestPt = m_datas[i]->getClosestPoint(imagePoint.x(), imagePoint.y(), &xError,&yError );
        if ( xError < targetErrorX && yError < targetErrorY ){
            if ( xError < minErrorX ){
                minErrorX = xError;
                imagePoint.setX( closestPt.first);
                imagePoint.setY( closestPt.second );
                *valid = true;
            }
        }
    }
    return imagePoint;
}

QString Plot2DHolder::getLegendLocation() const {
    return m_legendPosition;
}

QSize Plot2DHolder::getLegendSize() const {
    return m_plot->getLegendSize();
}


std::pair<double,double>  Plot2DHolder::getPlotBoundsY( const QString& id, bool* valid ) const {
    std::pair<double,double> result;
    *valid = false;
    std::shared_ptr<Plot2D> plotData = _findData(id, true );
    if ( plotData ){
        result = plotData->getBoundsY();
        *valid = true;
    }
    else {
        qDebug() << "Generator could not get bounds for id="<<id;
    }
    return result;
}


QString Plot2DHolder::getPlotTitle() const {
    return m_plot->title().text();
}


std::pair<double,double> Plot2DHolder::getRange( bool* valid ) const {
    std::pair<double,double> result;
    *valid = false;
    if ( m_range ){
        result.first = m_range->getClipMin();
        result.second = m_range->getClipMax();
        *valid = true;
    }
    return result;
}


std::pair<double,double> Plot2DHolder::getRangeColor(bool* valid ) const {
    std::pair<double,double> result;
    *valid = false;
    if ( m_rangeColor ){
        result.first = m_rangeColor->getClipMin();
        result.second = m_rangeColor->getClipMax();
        *valid = true;
    }
    return result;
}

double Plot2DHolder::_getRelativeError( double minValue, double maxValue) const {
    double range = qAbs( maxValue - minValue );
    double error = 0;
    if ( ! isnan( range ) ){
        //Divide by powers of 10 until we get something less than 1.
        if ( range > 1 ){
            int i = 0;
            double powerValue = qPow( 10, i );
            while( range / powerValue > 1 ){
                i++;
                powerValue = qPow( 10, i );
            }
            error = powerValue;
            //Add in arbitrary scaling for more accuracy.
            error = error * 0.005;
        }
        //Multiply by powers of 10 until we get something larger than 1.
        else {
           int i = 0;
           while( range * qPow( 10, i ) < 1 ){
               i++;
           }
           error = 1 / qPow( 10, i );
           error = error * 0.5;
        }
    }
    return error;
}

QPointF Plot2DHolder::getScreenPoint( const QPointF& dataPoint ) const {
    return m_plot->getScreenPoint( dataPoint );
}

QString Plot2DHolder::getTitleAxisX() const {
    return m_plot->getAxisTitleX();
}

QString Plot2DHolder::getTitleAxisY() const {
    return m_plot->getAxisTitleY();
}

double Plot2DHolder::getVLinePosition( bool* valid ) const {
    *valid = false;
    double pos = 0;
    if ( m_vLine ){
        *valid = true;
        pos = m_vLine->getPosition( );
    }
    return pos;
}


double Plot2DHolder::getMarkerLine() const {
    double pos = 0;
    if ( m_vLine ){
        pos = m_vLine->getPosition();
    }
    return pos;
}

QSize Plot2DHolder::getPlotSize() const {
    return m_plot->getPlotSize();
}


QPointF Plot2DHolder::getPlotUpperLeft() const {
    return m_plot->getPlotUpperLeft();
}

QSize Plot2DHolder::getSize() const {
    return m_plot->size();
}


bool Plot2DHolder::isExternalLegend() const {
    return m_legendExternal;
}

bool Plot2DHolder::isLegendVisible() const {
    return m_legendVisible;
}

bool Plot2DHolder::isSelectionOnCanvas( int xPos ) const {
    bool selectionOnCanvas = m_plot->isSelectionOnCanvas( xPos );
    return selectionOnCanvas;
}

bool Plot2DHolder::isMarkerLineVisible() const {
    bool visible = false;
    if ( m_vLine ){
        visible = true;
    }
    return visible;
}


void Plot2DHolder::legendToImage( QPainter* paint, const QRectF& geom ) const {
    m_plot->legendToImage( paint, geom );
}

std::shared_ptr<Plot2D> Plot2DHolder::_makeData() const {
    std::shared_ptr<Plot2D> pData( nullptr );
    if ( m_plotType == PlotType::PROFILE ){
        pData.reset( new Plot2DProfile() );
    }
    else if ( m_plotType == PlotType::HISTOGRAM ){
        pData.reset( new Plot2DHistogram() );
    }
    else {
        qWarning() << "Unrecognized plot type: "<<(int)( m_plotType );
    }
    pData->setLegendLine( m_legendLineShow );
    return pData;
}

void Plot2DHolder::removeData( const QString& dataName ){
    std::shared_ptr<Plot2D> pData = _findData( dataName, true );
    if ( pData ){
        pData->detachFromPlot();
        m_datas.removeOne( pData );
    }
    pData = _findData( dataName, false );
    if ( pData ){
        pData->detachFromPlot();
        m_dataSecondary.removeOne( pData );
    }
}

void Plot2DHolder::setAxisExtentY( int extent ){
    m_plot->setAxisExtentY( extent );
}

void Plot2DHolder::setAxisLocationX( QwtPlot::Axis axis ){
    QwtPlot::Axis oldAxis = m_plot->getAxisLocationX();
    if ( oldAxis != axis ){
        m_plot->setAxisLocationX( axis );
    }
}

void Plot2DHolder::setAxisXRange( double min, double max ){
    m_plot->setAxisScaleX(  min, max );
    m_plot->replot();
}

void Plot2DHolder::setColor( QColor color, const QString& id ){
    if ( id.isEmpty() || id.trimmed().length() == 0 ){
        int dataCount = m_datas.size();
        for ( int i = 0; i < dataCount; i++ ){
            _setColorData( color, m_datas[i] );
        }
        int dataCountSecondary = m_dataSecondary.size();
        for ( int i = 0; i < dataCountSecondary; i++ ){
            _setColorData( color, m_dataSecondary[i] );
        }
    }
    else {
        std::shared_ptr<Plot2D> plotData = _findData( id, true );
        if ( plotData ){
            _setColorData( color, plotData );
           std::shared_ptr<Plot2D> plotDataSecondary = _findData( id, false );
           if ( plotDataSecondary ){
               _setColorData( color, plotDataSecondary );
           }
        }
    }
}

void Plot2DHolder::_setColorData( QColor color, std::shared_ptr<Plot2D> plotData ){
    plotData->detachFromPlot();
    plotData->setColor( color );
    plotData->attachToPlot( m_plot );
}


void Plot2DHolder::setColored( bool colored, const QString& id ){
    if ( id.isEmpty() || id.trimmed().length() == 0 ){
        int dataCount = m_datas.size();
        for ( int i = 0; i < dataCount; i++ ){
            m_datas[i]->setColored( colored );
        }
    }
    else {
        std::shared_ptr<Plot2D> plotData = _findData( id, true );
        if ( plotData ){
            plotData->setColored( colored );
        }
    }
}


void Plot2DHolder::setCurveName( const QString& oldName, const QString& newName ){
    std::shared_ptr<Plot2D> plotData = _findData( oldName, true );
    if ( plotData ){
        plotData->setId( newName );
    }
    plotData = _findData( oldName, false );
    if ( plotData ){
        plotData->setId( newName );
    }
}


void Plot2DHolder::setGridLines( bool showGrid ){
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


void Plot2DHolder::setHLinePosition( double position ){
    if ( m_hLine ){
        m_hLine->setPosition( position );
    }
}


void Plot2DHolder::setHLineVisible( bool visible ){
    if ( visible ){
        if ( !m_hLine ){
            m_hLine = new Plot2DLineHorizontal();
        }
        m_hLine->attach( m_plot );
    }
    else {
        if ( m_hLine ){
            m_hLine->detach();
        }
    }
}


void Plot2DHolder::setLegendExternal( bool externalLegend ){
    if ( m_legendExternal != externalLegend ){
        m_legendExternal = externalLegend;
        _updateLegend();
    }
}


void Plot2DHolder::setLegendLocation( const QString& position ){
    if ( position != m_legendPosition ){
        m_legendPosition = position;
        _updateLegend();
    }
}


void Plot2DHolder::setLegendLine( bool showLegendLine ){
    int dataCount = m_datas.size();
    m_legendLineShow = showLegendLine;
    for ( int i = 0; i < dataCount; i++ ){
        m_datas[i]->setLegendLine( m_legendLineShow );
    }
}


void Plot2DHolder::setLegendVisible( bool visible ){
    if ( m_legendVisible != visible ){
        m_legendVisible = visible;
        _updateLegend();
    }
}


void Plot2DHolder::setLogScale(bool logScale){
    m_logScale = logScale;
    _updateScales();
}


void Plot2DHolder::setMarkerLine( double xPos ){
    if ( m_vLine ){
        m_vLine->setPosition( xPos );
        m_plot->replot();
    }
}

void Plot2DHolder::setMarkerLineVisible( bool visible ){
    if ( visible ){
        if ( !m_vLine ){
            m_vLine = new Plot2DLine();
            m_vLine->setHeight( m_height );
            m_vLine->attach( m_plot );
        }
    }
    else {
        if ( m_vLine ){
            m_vLine->detach();
            delete m_vLine;
            m_vLine = nullptr;
        }
    }
    m_plot->replot();
}



void Plot2DHolder::setMarkedRange( double minY, double maxY ){
    if ( m_rangeMarker ){
        m_rangeMarker->setRange( QwtInterval(minY, maxY) );
    }
}


void Plot2DHolder::setPipeline( std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> pipeline){
    int dataCount = m_datas.size();
    for ( int i = 0; i < dataCount; i++ ){
        m_datas[i]->setPipeline( pipeline );
    }
    m_plot->replot();
}


void Plot2DHolder::setPlotType( PlotType type ){
    if ( m_plotType != type ){
        m_plotType = type;
        clearData();
        _clearMarkers();
        m_range = new Plot2DSelection();
        m_range->attach( m_plot );
        if ( type == PlotType::PROFILE ){
            m_hLine = new Plot2DLineHorizontal();
            m_vLine = new Plot2DLine();
            m_rangeMarker = new Plot2DRangeMarker();
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
            setLegendVisible( false );
        }
    }
}


void Plot2DHolder::setRange(double min, double max){
    if ( m_range ){
        m_range->setClipValues(min, max);
    }
    m_plot->replot();
}


void Plot2DHolder::setRangeColor(double min, double max){
    if ( m_rangeColor ){
        m_rangeColor->setClipValues(min, max);
    }
    m_plot->replot();
}


void Plot2DHolder::setRangeMarkerVisible( bool visible ){
    if ( visible ){
        if ( !m_rangeMarker ){
            m_rangeMarker = new Plot2DRangeMarker();
        }
        m_rangeMarker->attach( m_plot );
    }
    else {
        if ( m_rangeMarker ){
            m_rangeMarker->detach();
        }
    }
    m_plot->replot();
}


void Plot2DHolder::setRangePixels(double min, double max){
    if ( m_range ){
        m_range->setHeight(m_height);
        m_range->setBoundaryValues(min, max);
        m_plot->replot();
    }
}


void Plot2DHolder::setRangePixelsColor(double min, double max){
    if ( m_rangeColor ){
        m_rangeColor->setHeight(m_height);
        m_rangeColor->setBoundaryValues(min, max);
    }
    if ( m_vLine ){
        m_vLine->setPositionPixel( min, max );
    }
    m_plot->replot();
}


void Plot2DHolder::setSelectionMode(bool selection){
    if ( m_range ){
        m_range->setSelectionMode( selection );
    }
}


void Plot2DHolder::setSelectionModeColor( bool selection ){
    if ( m_rangeColor ){
        m_rangeColor->setSelectionMode( selection );
    }
    if ( m_vLine ){
        m_vLine->setSelectionMode( selection );
    }
}


bool Plot2DHolder::setSize( int width, int height ){
    bool newSize = false;
    if ( width != m_width || height != m_height ){
        int minLength = qMin( width, height );
        if ( minLength > 0 ){
            m_width = width;
            m_height = height;
            if ( m_range ){
                m_range->setHeight( m_height );
            }
            if ( m_rangeColor ){
                m_rangeColor->setHeight( m_height );
            }
            if ( m_vLine ){
                m_vLine->setHeight( m_height );
            }
            m_plot->setPlotSize( width, height );
            newSize = true;
        }
        else {
            qWarning() << "Invalid plot dimensions: "<<width<<" x "<< height;
        }
    }
    return newSize;
}

void Plot2DHolder::_setLineStyle( const QString& style, std::shared_ptr<Plot2D> data ){
    //Attach & reattach so the legend icon updates.
    data->detachFromPlot();
    data->setLineStyle( style );
    data->attachToPlot( m_plot );
}


void Plot2DHolder::setLineStyle( const QString& style, const QString& id, bool primary ){
    if ( id.isEmpty() || id.trimmed().length() == 0 ){
        if ( primary ){
            int dataCount = m_datas.size();
            for ( int i = 0; i < dataCount; i++ ){
                _setLineStyle( style, m_datas[i] );
            }
        }
        else {
            int dataCount = m_dataSecondary.size();
            for ( int i = 0; i < dataCount; i++ ){
                _setLineStyle( style, m_dataSecondary[i] );
            }
        }
    }
    else {
        std::shared_ptr<Plot2D> data = _findData( id, primary );
        if ( data ){
            _setLineStyle( style, data );
        }
    }
}


void Plot2DHolder::setStyle( const QString& style, const QString& id ){
    if ( id.isEmpty() || id.trimmed().length() == 0 ){
        int dataCount = m_datas.size();
        for ( int i = 0; i < dataCount; i++ ){
            m_datas[i]->setDrawStyle( style );
        }
    }
    else {
        std::shared_ptr<Plot2D> data = _findData( id, true );
        if ( data ){
            data->setDrawStyle( style );
        }
    }
}


void Plot2DHolder::setTitleAxisX( const QString& title){
    QwtText xTitle( title );
    xTitle.setFont( m_font );
    m_plot->setAxisTitleX( xTitle );
}


void Plot2DHolder::setTitleAxisY( const QString& title){
    QString axisTitle = title;
    if ( m_logScale ){
        axisTitle = "Log " + axisTitle;
    }
    QwtText yTitle( axisTitle );
    yTitle.setFont( m_font );
    m_plot->setAxisTitleY(yTitle);
}


void Plot2DHolder::toImage( QPainter* painter, const QRect& geom ) const {
    QwtPlotRenderer renderer;
    //Draw the legend separately if it is visible and external.
    if ( m_legendVisible && m_legendExternal ){
        renderer.setDiscardFlag( QwtPlotRenderer::DiscardLegend, true );
    }
    renderer.render( m_plot, painter, geom );
}


void Plot2DHolder::_updateLegend(){
    m_plot->setLegendPosition( m_legendVisible, m_legendPosition, m_legendExternal );
}


void Plot2DHolder::_updateScales(){
    int dataCount = m_datas.size();
    if ( dataCount > 0 ){
        std::pair<double,double> firstBounds = m_datas[0]->getBoundsY();
        std::pair<double,double> firstBoundsX = m_datas[0]->getBoundsX();
        double yMin = firstBounds.first;
        double yMax = firstBounds.second;
        double xMin = firstBoundsX.first;
        double xMax = firstBoundsX.second;
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
            std::pair<double,double> plotBoundsX = m_datas[i]->getBoundsX();
            if ( plotBoundsX.first < xMin ){
                xMin = plotBoundsX.first;
            }
            if ( plotBoundsX.second > xMax ){
                xMax = plotBoundsX.second;
            }
        }
        m_plot->setAxisScaleX( xMin, xMax );
        if ( m_logScale ){
            m_plot->setAxisScaleEngineY( new QwtLogScaleEngine());
            m_plot->setAxisScaleY( 1, yMax );
        }
        else {
            m_plot->setAxisScaleEngineY(new QwtLinearScaleEngine());
            m_plot->setAxisScaleY( yMin, yMax );
        }
        m_plot->replot();
    }
}


Plot2DHolder::~Plot2DHolder(){
    clearData();
    _clearMarkers();
    _clearItem( m_range );
    delete m_plot;
}
}
}

