#include <CartaLib/Hooks/ProfileResult.h>
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

    }
  }
}
