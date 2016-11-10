#include "ProfileInfo.h"
#include <cmath>

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

bool ProfileInfo::operator==( const ProfileInfo& rhs ) const {
    bool equalProfiles = false;
    if ( m_aggregateType == rhs.m_aggregateType ){
        if ( m_restUnit == rhs.m_restUnit ){
            if ( m_spectralType == rhs.m_spectralType ){
                if ( m_spectralUnit == rhs.m_spectralUnit ){
                    const double ERROR_MARGIN = 0.000001;
                    if ( fabs( m_restFrequency - rhs.m_restFrequency ) < ERROR_MARGIN ){
                        equalProfiles = true;
                    }
                }
            }
        }
    }
    return equalProfiles;
}

bool ProfileInfo::operator!=( const ProfileInfo& rhs ) {
    return !( *this== rhs );
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
