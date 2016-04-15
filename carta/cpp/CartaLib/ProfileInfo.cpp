#include "ProfileInfo.h"

namespace Carta {
namespace Lib {

ProfileInfo::AggregateType ProfileInfo::getAggregateType() const {
    return m_aggregateType;
}

ProfileInfo & ProfileInfo::setAggregateType( const ProfileInfo::AggregateType & knownType ){
    m_aggregateType = knownType;
    return * this;
}



ProfileInfo& ProfileInfo::setUnit( const QString & suffix ){
    m_unit = suffix;
    return * this;
}


} // namespace Lib
} // namespace Carta
