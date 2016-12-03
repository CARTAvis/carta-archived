#include "CartaLib/Hooks/HistogramResult.h"
#include <QDebug>

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


QDataStream &operator<<(QDataStream& out, const HistogramResult& result ){
    out << result.getName()<< result.getUnitsX() << result.getUnitsY();
    std::vector<std::pair<double,double>> data = result.getData();
    int dataCount = data.size();
    out << dataCount;
    for ( int i = 0; i < dataCount; i++ ){
        out << data[i].first << data[i].second;
    }
    return out;
}


QDataStream &operator>>(QDataStream& in, HistogramResult& result ){
    QString name;
    QString unitsX;
    QString unitsY;
    int dataCount;
    in >> name >> unitsX >> unitsY;
    in >> dataCount;
    std::vector<std::pair<double,double> > data( dataCount );
    for ( int i = 0; i < dataCount; i++ ){
        double firstEle;
        double secondEle;
        in >> firstEle >> secondEle;
        data[i] = std::pair<double,double>( firstEle, secondEle );
    }
    result = HistogramResult( name, unitsX, unitsY, data );
    return in;
}


    }
  }

}
