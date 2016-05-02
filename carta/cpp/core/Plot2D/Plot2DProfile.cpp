#include "Plot2DProfile.h"
#include "Data/Profile/ProfilePlotStyles.h"
#include <qwt_painter.h>
#include "CartaLib/PixelPipeline/CustomizablePixelPipeline.h"
#include <QDebug>

namespace Carta {
namespace Plot2D {


Plot2DProfile::Plot2DProfile(){

    setStyle(QwtPlotCurve::Lines);
}


void Plot2DProfile::attachToPlot( QwtPlot* plot ){
    attach( plot );
}


void Plot2DProfile::detachFromPlot(){
    detach();
}


void Plot2DProfile::drawLines (QPainter *painter, const QwtScaleMap &xMap,
        const QwtScaleMap &yMap,
        const QRectF &canvasRect, int from, int to) const{
    QPen curvePen( m_defaultColor );
    curvePen.setStyle( m_penStyle );
    painter->setPen( curvePen );

    QwtPlotCurve::drawLines( painter, xMap, yMap, canvasRect, from, to );
}

void Plot2DProfile::drawSteps (QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF &canvasRect, int from, int to) const {
    QPen curvePen( m_defaultColor );
    curvePen.setStyle( m_penStyle );
    painter->setPen( curvePen );

    QwtPlotCurve::drawSteps( painter, xMap, yMap, canvasRect, from, to );
}

QwtGraphic Plot2DProfile::legendIcon( int /*index*/, const QSizeF& iconSize ) const {
    if ( iconSize.isEmpty() ){
        return QwtGraphic();
    }
    QwtGraphic icon;
    icon.setDefaultSize( iconSize );
    icon.setRenderHint( QwtGraphic::RenderPensUnscaled, true );

    QPainter painter( &icon );
    QPen pen( m_defaultColor );
    pen.setStyle( m_penStyle );
    pen.setWidth( 3 );
    const double y = 0.5 * iconSize.height();
    QPainterPath path;
    path.moveTo( 0.0, y );
    path.lineTo( iconSize.width(), y );
    painter.strokePath( path, pen );
    return icon;
}

void Plot2DProfile::setData ( std::vector<std::pair<double,double> > datas ){
    int dataCount = datas.size();
    m_datasX.resize( dataCount );
    m_datasY.resize( dataCount );
    if ( dataCount > 0 ){
        m_maxValueY = -1 * std::numeric_limits<double>::max();
        m_minValueY = std::numeric_limits<double>::max();
        for ( int i = 0; i < dataCount; i++ ){
            m_datasX[i] = datas[i].first;
            m_datasY[i] = datas[i].second;
            if ( m_datasY[i] > m_maxValueY ){
                m_maxValueY = m_datasY[i];
            }
            if ( m_datasY[i] < m_minValueY ){
                m_minValueY = m_datasY[i];
            }
        }
    }
    //No data so just use bogus bounds
    else {
        m_maxValueY = 1;
        m_minValueY = 0;
    }
    setRawSamples( m_datasX.data(), m_datasY.data(), dataCount );
}


void Plot2DProfile::setBaseLine( double /*val*/ ){
}

void Plot2DProfile::setDrawStyle( const QString& style ){
    Plot2D::setDrawStyle( style );
    if ( m_drawStyle == Carta::Data::ProfilePlotStyles::PLOT_STYLE_LINE ){
        setStyle( QwtPlotCurve::Lines );
    }
    else if ( m_drawStyle == Carta::Data::ProfilePlotStyles::PLOT_STYLE_STEP ){
        setStyle( QwtPlotCurve::Steps );
    }
    else {
        qDebug() << "Unrecognized draw style";
    }
}



void Plot2DProfile::setId( const QString& id ){
    Plot2D::setId( id );
    QwtText dataTitle = title();
    dataTitle.setText( id );
    setTitle( dataTitle );
}

void Plot2DProfile::setLegendLine( bool showLegendLine ){
    QwtText text = title();
    if ( showLegendLine ){
        setLegendIconSize(QSize(36,8));
        text.setColor( "black");
    }
    else {
        setLegendIconSize(QSize(0,0));
        text.setColor( m_defaultColor );
    }
    setTitle( text );
}


Plot2DProfile::~Plot2DProfile(){

}

}
}
	
