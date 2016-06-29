#include "Fit1DInfo.h"
#include "CartaLib/CartaLib.h"


namespace Carta {
namespace Lib {

Fit1DInfo::Fit1DInfo(){

}

std::vector<double> Fit1DInfo::getData() const {
    return m_data;
}

int Fit1DInfo::getGaussCount() const {
    return m_gaussCount;
}

std::vector<std::tuple<double,double,double> > Fit1DInfo::getInitialGaussianGuesses() const {
    return m_initGuesses;
}

int Fit1DInfo::getPolyDegree() const {
    return m_polyDegree;
}

QString Fit1DInfo::getId() const {
    return m_id;
}

bool Fit1DInfo::isRandomHeuristics() const {
    return m_randomHeuristics;
}

void Fit1DInfo::setData( const std::vector<double>& data ){
    m_data = data;
}

void Fit1DInfo::setGaussCount( int gaussCount ){
    CARTA_ASSERT( gaussCount >= 0 );
    m_gaussCount = gaussCount;
}

void Fit1DInfo::setInitialGaussianGuesses( const std::vector<std::tuple<double,double,double> >& initialGuesses ){
    m_initGuesses = initialGuesses;
}


void Fit1DInfo::setPolyDegree( int polyDegree ){
    CARTA_ASSERT( polyDegree>= 0 );
    m_polyDegree = polyDegree;
}

void Fit1DInfo::setRandomHeuristics( bool random ){
    m_randomHeuristics = random;
}


void Fit1DInfo::setId( const QString& id ){
    m_id = id;
}



Fit1DInfo::~Fit1DInfo(){

}
} // namespace Lib
} // namespace Carta
