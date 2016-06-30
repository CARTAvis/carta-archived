#include "Plot.h"
#include "Data/Plotter/LegendLocations.h"
#include <QDebug>
#include <qwt_plot_canvas.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_plot_legenditem.h>
#include <qwt_plot_layout.h>

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

    setCanvasBackground( Qt::white );
    setAxisAutoScale( QwtPlot::yLeft, false );
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

QSize Plot::getPlotSize() const {
    //Size of plot widget
    QSize canvasSize = size();

    //Axis widget sizes
    const QwtScaleWidget* axisWidgetLeft = axisWidget( QwtPlot::yLeft );
    QSize axisLeftSize = axisWidgetLeft->size();
    const QwtScaleWidget* axisWidgetBottom = axisWidget( QwtPlot::xBottom );
    QSize axisBottomSize = axisWidgetBottom->size();

    //Subtract the axis widget sizes from the total size
    int baseWidth = canvasSize.width() - axisLeftSize.width();
    int baseHeight = canvasSize.height() - axisBottomSize.height();

    //Subtract legend areas from the total size.
    if ( m_showExternalLegend ){
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
    return plotSize;
}

QPointF Plot::getPlotUpperLeft() const {
    const QwtScaleWidget* axisWidgetLeft = axisWidget( QwtPlot::yLeft );
    QSize axisSize = axisWidgetLeft->size();
    QPointF upperLeft( axisSize.width(), 0 );
    //Add in sizes of legend.
    if ( m_showExternalLegend ){
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

    //How much of thee left side is taken up by the axis and (left) legend.
    const QwtScaleWidget* axisWidgetLeft = axisWidget( QwtPlot::yLeft );
    QSize axisLeftSize = axisWidgetLeft->size();
    int leftMargin = axisLeftSize.width();
    if ( m_showExternalLegend && m_legendLocation == QwtPlot::LeftLegend ){
        if ( m_externalLegend ){
            leftMargin = leftMargin + m_externalLegend->size().width();
        }
    }

    //Transform the x-coordinate image point to pixels on the canvas.
    double xTrans = transform( QwtPlot::xBottom,dataPoint.x());
    //What percentage is the pixel point compared to the plot area?
    double plotPercent = xTrans / (canvasSize.width() - ZERO_OFFSET);

    //Add in the left margin and then the same percentage of the total area without the margin.
    xTrans = leftMargin + plotPercent * (plotSize.width()-leftMargin);

    //Transform the y-coordinate image point to pixels on the canvas.
    double yTrans = transform( QwtPlot::yLeft, dataPoint.y());
    return QPointF( xTrans, yTrans );
}

QPointF Plot::getImagePoint(const QPointF& screenPt ) const {
    //Subtract off non-plotting area from the pixel coordinates.
    QPointF topLeft = getPlotUpperLeft();
    double xVal = screenPt.x() - topLeft.x();
    double yVal = screenPt.y() - topLeft.y();

    //Sizes of plot widget and actual plotting area.
    QSize plotSize = size();
    const QWidget* canvasWidget = canvas();
    QSize canvasSize = canvasWidget->size();

    //What percentage does the x pixel coordinate represent compared to
    //the total plot widget size?
    double plotPercentX = xVal / (plotSize.width() - topLeft.x());
    //Get a new x pixel coordinate based on that percentage and the actual
    //plotting area size.
    double canvasXVal = plotPercentX * canvasSize.width();

    //Map the pixel coordinates to image coordinates.
    double xValue = invTransform( QwtPlot::xBottom, canvasXVal );
    double yValue = invTransform( QwtPlot::yLeft, yVal );
    return QPointF( xValue, yValue );
}


void Plot::setFont( const QFont& font ){
    QwtScaleWidget* leftWidget = axisWidget( QwtPlot::yLeft );
    leftWidget->setFont( font );
    leftWidget->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding);
    QwtScaleWidget* bottomWidget = axisWidget( QwtPlot::xBottom );
    bottomWidget->setFont( font );
}


void Plot::setLegendPosition( bool visible,
        const QString& legendLocation, bool external ){

    m_legendLocation = _calculatePosition( legendLocation );
    if ( visible && external ){
        m_showExternalLegend = true;
    }
    else {
        m_showExternalLegend = false;
    }

    if ( m_externalLegend ){
        m_externalLegend->hide();
        delete m_externalLegend;
        m_externalLegend = nullptr;
    }
    if ( visible ){
        m_externalLegend = new QwtLegend( this );
        if ( !external ){
            connect( this, SIGNAL(legendDataChanged( const QVariant&, const QList<QwtLegendData>&)),
                    m_externalLegend, SLOT(updateLegend( const QVariant&, const QList<QwtLegendData>&)));
            updateLegend();
            m_externalLegend->show();
            if ( m_legendItem == nullptr ){
               m_legendItem = new QwtPlotLegendItem( );
               m_legendItem->attach( this );
            }
            Qt::Alignment alignment= static_cast<Qt::Alignment>(_calculateAlignment( legendLocation ));
            m_legendItem->setAlignment( alignment );
        }
        else {
            if ( m_legendItem != nullptr ){
                m_legendItem->detach();
                delete m_legendItem;
                m_legendItem = nullptr;
            }
            insertLegend( m_externalLegend, m_legendLocation );
        }
    }
    else {
        if ( m_legendItem != nullptr ){
            m_legendItem->detach();
            delete m_legendItem;
            m_legendItem = nullptr;
        }
    }
    replot();
}


Plot::~Plot(){
    delete m_externalLegend;
}

}
}
	
