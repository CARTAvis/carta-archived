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

QPointF Plot::getScreenPoint( const QPointF& dataPoint ) const {
    const QWidget* plotCanvas = canvas();
    QSize canvasSize = plotCanvas->size();
    QSize plotSize = size();

    const QwtScaleWidget* axisWidgetLeft = axisWidget( QwtPlot::yLeft );
    QSize axisSize = axisWidgetLeft->size();
    int leftMargin = axisSize.width();

    double xTrans = transform( QwtPlot::xBottom,dataPoint.x());
    //Figure out the percentage xTrans is compared to canvas width;
    double percentX = xTrans / (canvasSize.width() - ZERO_OFFSET);

    //It will be that percentage of the plot size.
    xTrans = percentX * (plotSize.width() - leftMargin);
    //Add in the left margin.
    xTrans = xTrans +leftMargin;

    //Now do y
    double yTrans = transform( QwtPlot::yLeft, dataPoint.y());
    double percentY = yTrans / canvasSize.height();
    yTrans = plotSize.height() - percentY * plotSize.height();
    return QPointF( xTrans, yTrans );
}

QPointF Plot::getImagePoint(const QPointF& screenPt ) const {
    double x = screenPt.x();
    double y = screenPt.y();
    QSize plotSize = size();
    const QWidget* canvasWidget = canvas();
    QSize canvasSize = canvasWidget->size();
    double xMargin = plotSize.width() - canvasSize.width();

    //Space between edge of canvas and where 0 is.
    int marginSpaceX = xMargin - ZERO_OFFSET;


    double xValue = invTransform( QwtPlot::xBottom, x - marginSpaceX );
    double yValue = invTransform( QwtPlot::yLeft, y );

    QPointF worldPt( xValue, yValue );
    return worldPt;
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
            QwtPlot::LegendPosition legPos = _calculatePosition( legendLocation );
            insertLegend( m_externalLegend, legPos );
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
	
