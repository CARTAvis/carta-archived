#include "CartaLib/Hooks/Plot2DResult.h"
namespace Carta {
  namespace Lib {
    namespace Hooks {

Plot2DResult::Plot2DResult( const QString plotTitle,
        const QString unitsX, const QString unitsY,
	std::vector<std::pair<double,double>> plotData){

	m_name = plotTitle;
	m_data = plotData;
	m_unitsX = unitsX;
	m_unitsY = unitsY;
}

QString Plot2DResult::getName() const{
	return m_name;
}

std::vector<std::pair<double,double>> Plot2DResult::getData() const{
	return m_data;
}

QString Plot2DResult::getUnitsX() const {
    return m_unitsX;
}

QString Plot2DResult::getUnitsY() const {
    return m_unitsY;
}

void Plot2DResult::setData( const std::vector<std::pair<double,double> >& result ){
    m_data = result;
}

void Plot2DResult::setName( const QString& name ){
    m_name = name;
}
    }
  }

}
