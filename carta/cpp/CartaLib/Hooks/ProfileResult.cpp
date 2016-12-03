#include "CartaLib/Hooks/ProfileResult.h"
#include <QDebug>

namespace Carta {
  namespace Lib {
    namespace Hooks {

ProfileResult::ProfileResult( double restFrequency, const QString& restUnits,
      const std::vector< std::pair<double,double> > data){
	m_data = data;
	m_restUnits = restUnits;
	m_restFrequency = restFrequency;
}

std::vector< std::pair<double,double> > ProfileResult::getData() const {
    return m_data;
}

QString ProfileResult::getError() const {
    return m_errorMessage;
}

QString ProfileResult::getRestUnits() const {
    return m_restUnits;
}

double ProfileResult::getRestFrequency() const {
    return m_restFrequency;
}


void ProfileResult::setData( const std::vector< std::pair<double,double> >& data ){
    m_data = data;
}

void ProfileResult::setRestFrequency( double restFreq ){
    m_restFrequency = restFreq;
}

void ProfileResult::setRestUnits( const QString& restUnits ){
    m_restUnits = restUnits;
}

void ProfileResult::setError( const QString& errorMsg ){
    m_errorMessage = errorMsg;
}

QDataStream &operator<<(QDataStream& out, const ProfileResult& result ){
    out << result.getRestUnits()<< result.getRestFrequency();
    std::vector<std::pair<double,double>> data = result.getData();
    int dataCount = data.size();
    out << dataCount;
    for ( int i = 0; i < dataCount; i++ ){
        out << data[i].first << data[i].second;
    }
    return out;
}


QDataStream &operator>>(QDataStream& in, ProfileResult& result ){
    QString name;
    QString unitsX;
    QString unitsY;

    double restFrequency;
    QString restUnits;
    int dataCount;
    in >> restUnits >> restFrequency;
    in >> dataCount;
    std::vector<std::pair<double,double> > data( dataCount );
    for ( int i = 0; i < dataCount; i++ ){
        double firstEle;
        double secondEle;
        in >> firstEle >> secondEle;
        data[i] = std::pair<double,double>( firstEle, secondEle );
    }
    result = ProfileResult( restFrequency, restUnits, data );
    return in;
}

    }
  }
}
