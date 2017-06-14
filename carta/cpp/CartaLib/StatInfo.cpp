#include "StatInfo.h"
#include "CartaLib/CartaLib.h"
#include <QDebug>

namespace Carta {
namespace Lib {

QList<StatInfo::StatType> StatInfo::m_regionStatTypes = {
        StatInfo::StatType::Sum,
        StatInfo::StatType::FluxDensity,
        StatInfo::StatType::Mean,
        StatInfo::StatType::RMS,
        StatInfo::StatType::Sigma,
        StatInfo::StatType::SumSq,
        StatInfo::StatType::Min,
        StatInfo::StatType::Max,
        StatInfo::StatType::Blc,
        StatInfo::StatType::Trc,
        StatInfo::StatType::MinPos,
        StatInfo::StatType::MaxPos,
        StatInfo::StatType::Blcf,
        StatInfo::StatType::Trcf,
        StatInfo::StatType::MinPosf,
        StatInfo::StatType::MaxPosf
};


QList<StatInfo::StatType> StatInfo::m_imageStatTypes = {
        StatInfo::StatType::Shape,
        StatInfo::StatType::RestoringBeam,
        StatInfo::StatType::MedianRestoringBeam,
        StatInfo::StatType::RightAscensionRange,
        StatInfo::StatType::DeclinationRange,
        StatInfo::StatType::FrequencyRange,
        StatInfo::StatType::VelocityRange,
        StatInfo::StatType::Frequency,
        StatInfo::StatType::Velocity,
        StatInfo::StatType::Stokes,
        StatInfo::StatType::DirectionType,
        StatInfo::StatType::BrightnessUnit,
        StatInfo::StatType::FrameCount,
        StatInfo::StatType::BeamArea
};

QList<StatInfo::StatType> StatInfo::getStatTypesImage(){
    return m_imageStatTypes;
}

QList<StatInfo::StatType> StatInfo::getStatTypesRegion(){
    return m_regionStatTypes;
}


StatInfo::StatInfo( StatType statType ){
    m_statType = statType;
    m_label = toString( statType);
    m_imageStat = true;
    if ( m_regionStatTypes.contains( m_statType) ){
        m_imageStat = false;
    }
}

QString StatInfo::getLabel() const {
    return m_label;
}

QString StatInfo::getValue() const {
    return m_value;
}

StatInfo::StatType StatInfo::getType() const {
    return m_statType;
}

bool StatInfo::isImageStat() const {
    return m_imageStat;
}

void StatInfo::setImageStat( bool imageStat ){
    m_imageStat = imageStat;
}

void StatInfo::setLabel( const QString& label ){
    m_label = label;
}

void StatInfo::setValue( const QString& value ){
    m_value = value;
}



QString StatInfo::toString( StatType statType){
    QString label( "Unknwon");
    if ( statType == StatType::Name ){
        label = "Name";
    }
    if ( statType == StatType::Shape ){
        label = "Shape";
    }
    else if ( statType == StatType::RestoringBeam ){
        label = "Restoring Beam";
    }
    else if ( statType == StatType::MedianRestoringBeam ){
        label = "Median Restoring Beam";
    }
    else if ( statType == StatType::RightAscensionRange ){
        label = "RA Range";
    }
    else if ( statType == StatType::DeclinationRange ){
        label = "Dec Range";
    }
    else if ( statType == StatType::FrequencyRange ){
        label = "Frequency Range";
    }
    else if ( statType == StatType::VelocityRange ){
        label = "Velocity Range";
    }
    else if ( statType == StatType::Frequency ){
        label = "Frequency";
    }
    else if ( statType == StatType::Velocity ){
        label = "Velocity";
    }
    else if ( statType == StatType::Stokes ){
        label = "Stokes";
    }
    else if ( statType == StatType::DirectionType ){
        label = "Direction Type";
    }
    else if ( statType == StatType::BrightnessUnit ){
        label = "Brightness Unit";
    }
    else if ( statType == StatType::FrameCount ){
        label = "Frame Count";
    }
    else if ( statType == StatType::BeamArea ){
        label = "Beam Area";
    }
    else if ( statType == StatType::Sum ){
        label = "Sum";
    }
    else if ( statType == StatType::FluxDensity ){
        label = "Flux Density";
    }
    else if ( statType == StatType::Mean ){
        label = "Mean";
    }
    else if ( statType == StatType::RMS ){
        label = "RMS";
    }
    else if ( statType == StatType::Sigma ){
        label = "Sigma";
    }
    else if ( statType == StatType::SumSq ){
        label = "Sum of Squares";
    }
    else if ( statType == StatType::Min ){
        label = "Min";
    }
    else if ( statType == StatType::Max ){
        label = "Max";
    }
    else if ( statType == StatType::Blc ){
        label = "BLC";
    }
    else if ( statType == StatType::Trc ){
        label = "TRC";
    }
    else if ( statType == StatType::MinPos ){
        label = "Min Pos";
    }
    else if ( statType == StatType::MaxPos ){
        label = "Max Pos";
    }
    else if ( statType == StatType::Blcf ){
        label = "BLCf";
    }
    else if ( statType == StatType::Trcf ){
        label = "TRCf";
    }
    else if ( statType == StatType::MinPosf ){
        label = "Min Pos f";
    }
    else if ( statType == StatType::MaxPosf ){
        label = "Max Pos f";
    }
    return label;
}


StatInfo::~StatInfo(){

}

}
}


