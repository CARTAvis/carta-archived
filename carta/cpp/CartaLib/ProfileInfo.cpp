#include "ProfileInfo.h"

namespace Carta {
namespace Lib {

ProfileInfo::ProfileInfo(){
    m_aggregateType = AggregateType::MEAN;
    m_spectralType = "default";
    m_restFrequency = 0;
    m_restUnit = "";
    m_spectralUnit = "pixel";
}

ProfileInfo::AggregateType ProfileInfo::getAggregateType() const {
    return m_aggregateType;
}

double ProfileInfo::getRestFrequency() const {
    return m_restFrequency;
}

QString ProfileInfo::getRestUnit() const {
    return m_restUnit;
}

QString ProfileInfo::getSpectralType() const {
    return m_spectralType;
}

QString ProfileInfo::getSpectralUnit() const {
    return m_spectralUnit;
}

void ProfileInfo::setAggregateType( const ProfileInfo::AggregateType & knownType ){
    m_aggregateType = knownType;
}

void ProfileInfo::setRestFrequency( double freq ) {
    m_restFrequency = freq;
}


void ProfileInfo::setRestUnit( const QString& unit ){
    m_restUnit = unit;
}

void ProfileInfo::setSpectralType( const QString& specType ){
    m_spectralType = specType;
}

void ProfileInfo::setSpectralUnit( const QString& specUnit ){
    m_spectralUnit = specUnit;
}

ProfileInfo::~ProfileInfo(){

}
} // namespace Lib
} // namespace Carta
