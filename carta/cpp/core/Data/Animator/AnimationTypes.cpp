#include "AnimationTypes.h"
#include "Data/Image/Grid/AxisMapper.h"
#include "Data/Selection.h"
#include "CartaLib/AxisInfo.h"
#include <QDebug>

namespace Carta {

namespace Data {

QList<QString> AnimationTypes::m_animations;


AnimationTypes::AnimationTypes(){
}

QStringList AnimationTypes::getAnimations(){
    if ( m_animations.size() == 0 ){
        _init();
    }
    int animCount = m_animations.size();
    QStringList animList;
    for ( int i = 0; i < animCount; i++ ){
        animList.append( m_animations[i] );
    }
    return animList;
}

void AnimationTypes::_init(){
    //Should be an animator for each axis, plus one to animate images
    int axisCount = static_cast<int>(Carta::Lib::AxisInfo::KnownType::OTHER);
    for ( int i = 0; i < axisCount; i++ ){
        QString purpose = AxisMapper::getPurpose( static_cast<Carta::Lib::AxisInfo::KnownType>(i) );
        m_animations.append( purpose );
    }
    m_animations.append( Selection::IMAGE );
}


AnimationTypes::~AnimationTypes(){

}
}
}
