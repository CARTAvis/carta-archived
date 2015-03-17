#include <CartaLib/Hooks/HistogramResult.h>
namespace Carta {
  namespace Lib {
    namespace Hooks {

HistogramResult::HistogramResult( const QString histogramName, const QString units,
	std::vector<std::pair<double,double>> histogramData){

	m_name = histogramName;
	m_data = histogramData;
	m_units = units;
	m_frequencyMin = -1;
	m_frequencyMax = -1;

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

double HistogramResult::getFrequencyMin() const {
    return m_frequencyMin;
}

double HistogramResult::getFrequencyMax() const {
    return m_frequencyMax;
}

void HistogramResult::setFrequencyBounds( double minFreq, double maxFreq ){
    m_frequencyMin = minFreq;
    m_frequencyMax = maxFreq;
}
    }
  }

}
