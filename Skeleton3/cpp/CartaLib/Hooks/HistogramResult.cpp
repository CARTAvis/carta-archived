#include <CartaLib/Hooks/HistogramResult.h>
namespace Carta {
  namespace Lib {
    namespace Hooks {

HistogramResult::HistogramResult( const QString histogramName, const QString units,
	std::vector<std::pair<double,double>> histogramData ){

	m_name = histogramName;
	m_data = histogramData;
	m_units = units;

}

QString HistogramResult::getName() const{
	return m_name;
}

std::vector<std::pair<double,double>> HistogramResult::getData() const{
	return m_data;
}

QString HistogramResult::getUnits() const {
    return m_units;
}
    }
  }

}
