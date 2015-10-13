#include "AxisLabelInfo.h"

namespace Carta {
namespace Lib {

AxisLabelInfo::AxisLabelInfo():
    m_format( Formats::OTHER),
    m_location( Locations::OTHER ),
    m_precision (0){

}

int AxisLabelInfo::getPrecision() const {
    return m_precision;
}

void AxisLabelInfo::setPrecision( int decimals ){
    m_precision = decimals;
}

AxisLabelInfo::Formats AxisLabelInfo::getFormat() const {
    return m_format;
}


void AxisLabelInfo::setFormat( AxisLabelInfo::Formats format ){
    m_format = format;
}

AxisLabelInfo::Locations AxisLabelInfo::getLocation() const {
    return m_location;
}

void AxisLabelInfo::setLocation( AxisLabelInfo::Locations location ){
    m_location = location;
}


bool AxisLabelInfo::operator==( const AxisLabelInfo& other ) const {
    bool labelInfoSame = false;
    if ( other.m_location == this->m_location ){
        if ( other.m_format == this->m_format ){
            if ( other.m_precision == this->m_precision ){
                labelInfoSame = true;
            }
        }
    }
    return labelInfoSame;
}


bool AxisLabelInfo::operator!=( const AxisLabelInfo& other ) const {
    return !( *this == other );
}

AxisLabelInfo::~AxisLabelInfo(){
}

} // namespace Lib
} // namespace Carta
