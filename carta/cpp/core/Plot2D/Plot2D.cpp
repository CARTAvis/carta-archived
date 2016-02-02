#include "Plot2D.h"
#include "Data/Plotter/PlotStyles.h"
#include "CartaLib/PixelPipeline/CustomizablePixelPipeline.h"
#include <QDebug>

namespace Carta {
namespace Plot2D {


Plot2D::Plot2D():
    m_defaultColor( "#6699CC" ),
    m_brush( m_defaultColor ){
    m_drawStyle = Carta::Data::PlotStyles::PLOT_STYLE_LINE;
    m_colored = false;
}

std::pair<double,double> Plot2D::getBoundsY() const {
    return std::pair<double,double>( m_minValueY, m_maxValueY );
}

QString Plot2D::getId() const {
    return m_id;
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


Plot2D::~Plot2D(){

}

}
}
	
