#include "ProfileInfo.h"

namespace Carta {
namespace Lib {

ProfileInfo::ProfileInfo(){
    m_restFrequency = 0;
    m_restUnit = "GHz";
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


ProfileInfo & ProfileInfo::setAggregateType( const ProfileInfo::AggregateType & knownType ){
    m_aggregateType = knownType;
    return * this;
}

ProfileInfo::~ProfileInfo(){

}
} // namespace Lib
} // namespace Carta
