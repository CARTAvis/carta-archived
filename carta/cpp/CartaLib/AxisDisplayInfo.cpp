#include "AxisDisplayInfo.h"

#include "CartaLib/CartaLib.h"
#include <QDebug>

namespace Carta {

namespace Lib {

using Carta::Lib::AxisInfo;

bool AxisDisplayInfo::isCelestialPlane( const std::vector<AxisDisplayInfo>& displayInfos ){
    bool celestialPlane = false;
    int infoCount = displayInfos.size();
    int planeCount = 0;
    for ( int i = 0; i < infoCount; i++ ){
        AxisInfo::KnownType type = displayInfos[i].getAxisType();
        if ( type == AxisInfo::KnownType::DIRECTION_LON || type == AxisInfo::KnownType::DIRECTION_LAT ){
            int frame = displayInfos[i].getFrame();
            if ( frame < 0 ){
                planeCount++;
            }
        }
    }
    if ( planeCount == 2 ){
        celestialPlane = true;
    }
    return celestialPlane;
}

bool AxisDisplayInfo::isPermuted( const std::vector<AxisDisplayInfo>& displayInfos){
   int axesCount = displayInfos.size();
   bool actualPerm = false;
   //Decide if we really need to do a permutation based on whether
   //any of the axes have changed from their nominal position.
   for ( int i = 0; i < axesCount; i++ ){
       if ( displayInfos[i].getPermuteIndex() != i ){
           actualPerm = true;
           break;
       }
   }
   return actualPerm;
}

AxisDisplayInfo::AxisDisplayInfo(){
    m_type = AxisInfo::KnownType::OTHER;
    m_frameCount = 0;
    m_frame = 0;
    m_permuteIndex = -1;
}

AxisDisplayInfo::AxisDisplayInfo( const AxisDisplayInfo& other){
    m_type = other.getAxisType();
    m_frameCount = other.getFrameCount();
    m_frame = other.getFrame();
    m_permuteIndex = other.getPermuteIndex();
}



AxisDisplayInfo& AxisDisplayInfo::operator=( const AxisDisplayInfo& other ){
    if ( this != &other ){
        m_type = other.getAxisType();
        m_frameCount = other.getFrameCount();
        m_frame = other.getFrame();
        m_permuteIndex = other.getPermuteIndex();
    }
    return *this;
}

bool AxisDisplayInfo::operator==( const AxisDisplayInfo& other ) const{
    bool equalInfo = false;
    if ( m_type == other.getAxisType() ){
        if ( m_frameCount == other.getFrameCount() ){
            if ( m_frame == other.getFrame() ){
                if ( m_permuteIndex == other.getPermuteIndex() ){
                    equalInfo = true;
                }
            }
        }
    }
    return equalInfo;
}

bool AxisDisplayInfo::operator!=( const AxisDisplayInfo& other ) const {
    return !( *this == other );
}

AxisInfo::KnownType AxisDisplayInfo::getAxisType() const {
    return m_type;
}

int AxisDisplayInfo::getFrameCount() const {
    return m_frameCount;
}

int AxisDisplayInfo::getFrame() const {
    return m_frame;
}

int AxisDisplayInfo::getPermuteIndex() const {
    return m_permuteIndex;
}

void AxisDisplayInfo::setAxisType( const Carta::Lib::AxisInfo::KnownType type ) {
    m_type = type;
}

void AxisDisplayInfo::setFrameCount( int count ){
    CARTA_ASSERT( count >= 0 );
    m_frameCount = count;
}

void AxisDisplayInfo::setFrame( int frame ){
    CARTA_ASSERT( frame >= -1 && frame < m_frameCount );
    m_frame = frame;
}


void AxisDisplayInfo::setPermuteIndex( int index ){
    CARTA_ASSERT( index >= 0 );
    m_permuteIndex = index;
}

QString AxisDisplayInfo::toString() const {
    QString result ("Frame: "+QString::number( m_frame ) + "\n");
    result = result + "Frame Count:" + QString::number( m_frameCount ) + "\n";
    result = result + "Type:" + QString::number( static_cast<int>(m_type) )+ "\n";
    result = result + "Permute:"+ QString::number( m_permuteIndex ) + "\n";
    return result;
}

AxisDisplayInfo::~AxisDisplayInfo(){

}
}
}
