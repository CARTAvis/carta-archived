#include "Plot2DProfile.h"
#include "Data/Plotter/PlotStyles.h"
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


void Plot2DProfile::drawLines (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF &canvasRect, int from, int to) const{
    p->setPen( m_defaultColor );
    QwtPlotCurve::drawLines( p, xMap, yMap, canvasRect, from, to );
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


Plot2DProfile::~Plot2DProfile(){

}

}
}
	
