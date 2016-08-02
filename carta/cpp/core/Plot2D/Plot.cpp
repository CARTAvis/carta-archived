#include "Plot.h"
#include "Data/Plotter/LegendLocations.h"
#include <QDebug>
#include <qwt_plot_canvas.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_plot_legenditem.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_renderer.h>

namespace Carta {
namespace Plot2D {

using Carta::Data::LegendLocations;

const int Plot::ZERO_OFFSET = 4;;

Plot::Plot( QWidget* parent):
        QwtPlot( parent ),
        m_externalLegend( nullptr ),
        m_legendItem( nullptr ){
    QwtPlotCanvas* canvas = new QwtPlotCanvas();
    canvas->setFocusIndicator( QwtPlotCanvas::CanvasFocusIndicator );
    canvas->setFocusPolicy( Qt::StrongFocus );
    canvas->setPalette( Qt::black );
    canvas->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding);
    setCanvas( canvas );

    m_axisLocationY = QwtPlot::yLeft;
    m_axisLocationX = QwtPlot::xBottom;

    m_external = false;
    m_visible = false;

    setCanvasBackground( Qt::white );
    setAxisAutoScale( m_axisLocationY, false );
    setAutoReplot( false );
}


int Plot::_calculateAlignment( const QString& pos ) const {
    int alignment = 0;
    if ( pos == LegendLocations::LEFT ){
        alignment = Qt::AlignLeft | Qt::AlignVCenter;
    }
    else if ( pos == LegendLocations::RIGHT ){
        alignment = Qt::AlignRight | Qt::AlignVCenter;
    }
    else if ( pos == LegendLocations::BOTTOM ){
        alignment = Qt::AlignBottom | Qt::AlignHCenter;
    }
    else if ( pos == LegendLocations::TOP ){
        alignment = Qt::AlignTop | Qt::AlignHCenter;
    }
    else if ( pos == LegendLocations::TOP_LEFT ){
        alignment = Qt::AlignTop | Qt::AlignLeft;
    }
    else if ( pos == LegendLocations::TOP_RIGHT ){
        alignment = Qt::AlignTop | Qt::AlignRight;
    }
    else if ( pos == LegendLocations::BOTTOM_LEFT ){
        alignment = Qt::AlignBottom | Qt::AlignLeft;
    }
    else if ( pos == LegendLocations::BOTTOM_RIGHT ){
        alignment = Qt::AlignBottom | Qt::AlignRight;
    }
    return alignment;
}


QwtPlot::LegendPosition Plot::_calculatePosition( const QString& pos ) const{
    QwtPlot::LegendPosition legPos = QwtPlot::BottomLegend;
    if ( pos == LegendLocations::RIGHT ){
        legPos = QwtPlot::RightLegend;
    }
    else if ( pos == LegendLocations::LEFT ){
        legPos = QwtPlot::LeftLegend;
    }
    else if ( pos == LegendLocations::TOP_LEFT || pos == LegendLocations::TOP ||
            pos == LegendLocations::TOP_RIGHT ){
        legPos = QwtPlot::TopLegend;
    }
    return legPos;
}

void Plot::clearAxisExtentY(){
    QwtScaleWidget *scaleWidget = axisWidget( m_axisLocationY );
    scaleWidget->scaleDraw()->setMinimumExtent(0);
    replot();
}

int Plot::getAxisExtentY() const {
    const QwtScaleWidget *scaleWidget = axisWidget( m_axisLocationY );
    const QwtScaleDraw *scaleDraw = scaleWidget->scaleDraw();
    int extent = scaleDraw->extent( scaleWidget->font() );
    return extent;
}

QwtPlot::Axis Plot::getAxisLocationX() const {
    return m_axisLocationX;
}

std::pair<double,double> Plot::getAxisScaleX() const {
    QwtScaleDiv scaleDiv = axisScaleDiv(xBottom);
    QwtInterval interval = scaleDiv.interval();
    std::pair<double,double> minMax( interval.minValue(), interval.maxValue());
    return minMax;
}

QString Plot::getAxisTitleX() const {
    QwtText title = axisTitle( m_axisLocationX );
    return title.text();
}

QString Plot::getAxisTitleY() const {
    QwtText title = axisTitle( m_axisLocationY );
    return title.text();
}

QString Plot::getAxisUnitsY() const {
    QString unitStr = axisTitle( m_axisLocationY ).text();
    return unitStr;
}



QPointF Plot::getImagePoint(const QPointF& screenPt ) const {
    //Subtract off non-plotting area from the pixel coordinates.
    QPointF topLeft = getPlotUpperLeft();
    double xVal = screenPt.x() - topLeft.x();
    double yVal = screenPt.y() - topLeft.y();

    //Map the pixel coordinates to image coordinates.
    double xValue = invTransform( m_axisLocationX, xVal );
    double yValue = invTransform( m_axisLocationY, yVal );
    return QPointF( xValue, yValue );
}

QSize Plot::getLegendSize() const {
    QSize legendSize;
    if ( m_externalLegend ){
        legendSize = m_externalLegend->size();
    }
    return legendSize;
}

QSize Plot::getPlotSize() const {
    //Size of plot widget
    /*QSize canvasSize = size();

    //Axis widget sizes
    const QwtScaleWidget* axisWidgetY = axisWidget( m_axisLocationY );
    QSize axisYSize = axisWidgetY->size();
    const QwtScaleWidget* axisWidgetX = axisWidget( m_axisLocationX );
    QSize axisXSize = axisWidgetX->size();

    //Subtract the axis widget sizes from the total size
    int baseWidth = canvasSize.width() - axisYSize.width();
    int baseHeight = canvasSize.height() - axisXSize.height();

    //Subtract legend areas from the total size.
    if ( m_external && m_visible ){
        if ( ( m_legendLocation == QwtPlot::LeftLegend || m_legendLocation == QwtPlot::RightLegend )
                && m_externalLegend ){
            baseWidth = baseWidth - m_externalLegend->size().width();
        }
        else if ( ( m_legendLocation == QwtPlot::BottomLegend || m_legendLocation == QwtPlot::TopLegend )
                && m_externalLegend ){
            baseHeight = baseHeight - m_externalLegend->size().height();
        }
    }
    QSize plotSize( baseWidth, baseHeight );
    return plotSize;*/
    const QWidget* canvasWidget = canvas();
    QSize canvasSize = canvasWidget->size();
    return canvasSize;
}

QPointF Plot::getPlotUpperLeft() const {
    QPointF upperLeft( 0, 0 );
    if ( m_axisLocationY == QwtPlot::yLeft ){
        const QwtScaleWidget* axisWidgetY = axisWidget( m_axisLocationY );
        QSize axisSize = axisWidgetY->size();
        upperLeft.setX( axisSize.width() );
    }
    //Add in sizes of legend.
    if ( m_external && m_visible ){
        if (  m_legendLocation == QwtPlot::LeftLegend  && m_externalLegend ){
            double lWidth = m_externalLegend->size().width();
            upperLeft.setX( upperLeft.x() + lWidth );
        }
        else if ( m_legendLocation == QwtPlot::TopLegend  && m_externalLegend ){
            upperLeft.setY( upperLeft.y() + m_externalLegend->size().height());
        }
    }
    return upperLeft;
}


QPointF Plot::getScreenPoint( const QPointF& dataPoint ) const {
    //Size of entire plotting screen.
    QSize plotSize = size();
    //Size of actual plot area.
    const QWidget* canvasWidget = canvas();
    QSize canvasSize = canvasWidget->size();

    //How much of the left side is taken up by the axis and (left) legend.
    int leftMargin = 0;
    if ( m_axisLocationY == QwtPlot::yLeft ){
        const QwtScaleWidget* axisWidgetLeft = axisWidget( m_axisLocationY );
        QSize axisLeftSize = axisWidgetLeft->size();
        leftMargin = axisLeftSize.width();
    }
    if ( m_visible && m_external && m_legendLocation == QwtPlot::LeftLegend ){
        if ( m_externalLegend ){
            leftMargin = leftMargin + m_externalLegend->size().width();
        }
    }

    //Transform the x-coordinate image point to pixels on the canvas.
    double xTrans = transform( m_axisLocationX,dataPoint.x());
    //What percentage is the pixel point compared to the plot area?
    double plotPercent = xTrans / (canvasSize.width() - ZERO_OFFSET);

    //Add in the left margin and then the same percentage of the total area without the margin.
    xTrans = leftMargin + plotPercent * (plotSize.width()-leftMargin);

    //Transform the y-coordinate image point to pixels on the canvas.
    double yTrans = transform( m_axisLocationY, dataPoint.y());
    return QPointF( xTrans, yTrans );
}


bool Plot::isSelectionOnCanvas( int xPos ) const {
    bool selectionOnCanvas = false;
    if ( xPos >= 0 ){
        //Make sure the point is beyond the left canvas margin
        float plotMargin = axisWidget( m_axisLocationY )->size().width();
        if ( xPos > plotMargin && m_axisLocationY == QwtPlot::yLeft ){
            selectionOnCanvas = true;
        }
        else if ( xPos < canvas()->size().width() && m_axisLocationY == QwtPlot::yRight ){
            selectionOnCanvas = true;
        }
    }
    return selectionOnCanvas;
}


void Plot::legendToImage(QPainter* painter, const QRectF& geom) const{
    if ( m_externalLegend ){
       QwtPlotRenderer renderer;
       const QWidget* aWidget = axisWidget( m_axisLocationY );
       QColor background = aWidget->palette().color( QPalette::Background );
       painter->fillRect( geom, background );
       QObjectList children =m_externalLegend->children();
       renderer.renderLegend( this, painter, geom );
    }
}


void Plot::setAxisExtentY( int extent ){
    QwtScaleWidget *scaleWidget = axisWidget( m_axisLocationY );
    scaleWidget->scaleDraw()->setMinimumExtent( extent );
}

void Plot::setAxisLocationX( QwtPlot::Axis axis ){
    if ( m_axisLocationX != axis ){
        enableAxis( m_axisLocationX, false );
        m_axisLocationX = axis;
        enableAxis( m_axisLocationX, true );
    }
}

void Plot::setAxisScaleX( double min, double max ){
    setAxisScale( QwtPlot::xBottom, min, max );
    setAxisScale( QwtPlot::xTop, min, max );
}

void Plot::setAxisScaleY( double min, double max ){
    setAxisScale( m_axisLocationY, min, max );
}

void Plot::setAxisScaleEngineY(QwtScaleEngine* engine ){
    setAxisScaleEngine( m_axisLocationY, engine );
}


void Plot::setAxisTitleX( const QwtText& title ){
    setAxisTitle( m_axisLocationX, title );
}

void Plot::setAxisTitleY( const QwtText& title ){
    setAxisTitle( m_axisLocationY, title );
}

void Plot::setFont( const QFont& font ){
    QwtScaleWidget* yWidget = axisWidget( m_axisLocationY );
    yWidget->setFont( font );
    yWidget->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding);
    QwtScaleWidget* xWidget = axisWidget( m_axisLocationX );
    xWidget->setFont( font );
}


void Plot::setLegendPosition( bool visible,
        const QString& legendLocation, bool external ){
    m_legendLocation = _calculatePosition( legendLocation );
    m_external = external;
    m_visible = visible;
    if ( visible ){
        if ( !external ){
            //Make an internal legend
            _removeLegendExternal();
            if ( m_legendItem == nullptr ){
                m_legendItem = new QwtPlotLegendItem( );
                m_legendItem->attach( this );
            }
            Qt::Alignment alignment= static_cast<Qt::Alignment>(_calculateAlignment( legendLocation ));
            m_legendItem->setAlignment( alignment );
            updateLegend();
        }
        else {
            //Make an external legend.
            _removeLegendInternal();
            if ( m_externalLegend == nullptr ){
                m_externalLegend = new QwtLegend( this );
            }
            insertLegend( m_externalLegend, m_legendLocation );
            updateLegend();
        }
    }
    else {
        _removeLegendInternal();
        _removeLegendExternal();
    }
    replot();
}

void Plot::_removeLegendExternal(){
    if ( m_externalLegend ){
        delete m_externalLegend;
        m_externalLegend = nullptr;
    }
}


void Plot::_removeLegendInternal(){
    if ( m_legendItem != nullptr ){
        delete m_legendItem;
        m_legendItem = nullptr;
    }
}

void Plot::setPlotSize( int width, int height ){
    setFixedSize( width, height );
    updateLayout();
}

Plot::~Plot(){
    _removeLegendInternal();
    _removeLegendExternal();
}

}
}
	
