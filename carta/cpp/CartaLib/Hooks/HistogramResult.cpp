#include <CartaLib/Hooks/HistogramResult.h>
namespace Carta {
  namespace Lib {
    namespace Hooks {

HistogramResult::HistogramResult( const QString histogramName,
        const QString unitsX, const QString unitsY,
	std::vector<std::pair<double,double>> histogramData):
	    Plot2DResult( histogramName, unitsX, unitsY, histogramData ){
	m_frequencyMin = -1;
	m_frequencyMax = -1;
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
