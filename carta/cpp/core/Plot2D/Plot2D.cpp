#include "Plot2D.h"
#include "Data/Plotter/LineStyles.h"
#include "Data/Histogram/PlotStyles.h"
#include "CartaLib/PixelPipeline/CustomizablePixelPipeline.h"
#include <QDebug>

namespace Carta {
namespace Plot2D {

using Carta::Data::LineStyles;

Plot2D::Plot2D():
    m_defaultColor( "#6699CC" ),
    m_brush( m_defaultColor ){
    m_drawStyle = Carta::Data::PlotStyles::PLOT_STYLE_LINE;
    m_penStyle = Qt::SolidLine;
    m_colored = false;

}

std::pair<double,double> Plot2D::getBoundsX() const {
    return std::pair<double,double>( m_minValueX, m_maxValueX );
}

std::pair<double,double> Plot2D::getBoundsY() const {
    return std::pair<double,double>( m_minValueY, m_maxValueY );
}

std::pair<double,double> Plot2D::getClosestPoint( double targetX, double targetY,
            double* /*xError*/, double* /*yError*/ ) const {
    std::pair<double,double> closest(targetX,targetY);
    return closest;
}

QColor Plot2D::getColor() const {
    return m_defaultColor;
}

QString Plot2D::getId() const {
    return m_id;
}

void Plot2D::setColor( QColor color ){
    m_defaultColor = color;
}

void Plot2D::setId( const QString& id ){
    m_id = id;

}

void Plot2D::setPipeline( std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> pipeline){
    m_pipeline = pipeline;
}

void Plot2D::setColored( bool colored ){
    m_colored = colored;
}

void Plot2D::setDrawStyle( const QString& style ){
    m_drawStyle = style;
}

void Plot2D::setLegendLine( bool /*showLegendLine*/ ){

}

void Plot2D::setLegendVisible( bool /*visible*/ ){

}

void Plot2D::setLineStyle( const QString& style ){
    if ( style == LineStyles::LINE_STYLE_SOLID ){
        m_penStyle = Qt::SolidLine;
    }
    else if ( style == LineStyles::LINE_STYLE_DASH ){
        m_penStyle = Qt::DashLine;
    }
    else if ( style == LineStyles::LINE_STYLE_DOT ){
        m_penStyle = Qt::DotLine;
    }
    else if ( style == LineStyles::LINE_STYLE_DASHDOT ){
        m_penStyle = Qt::DashDotLine;
    }
    else if ( style == LineStyles::LINE_STYLE_DASHDOTDOT ){
        m_penStyle = Qt::DashDotDotLine;
    }
    else {
        qDebug() << "Unrecognized line style: "<<style;
    }
}


Plot2D::~Plot2D(){

}

}
}
	
