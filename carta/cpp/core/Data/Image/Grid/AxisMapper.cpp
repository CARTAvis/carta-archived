#include "AxisMapper.h"
#include "Data/Util.h"
#include "State/UtilState.h"
#include "CartaLib/AxisInfo.h"
#include "CartaLib/Hooks/GetWcsGridRenderer.h"
#include "Globals.h"
#include <QDebug>
#include <set>

namespace Carta {

namespace Data {

const QString AxisMapper::AXIS_X = "xAxis";
const QString AxisMapper::AXIS_Y = "yAxis";
const QString AxisMapper::RIGHT_ASCENSION = "Right Ascension";
const QString AxisMapper::LONGITUDE = "Longitude";
const QString AxisMapper::DECLINATION = "Declination";
const QString AxisMapper::LATITUDE = "Latitude";
const QString AxisMapper::SPECTRAL = "Channel";
const QString AxisMapper::LINEAR = "Linear";
const QString AxisMapper::STOKES = "Stokes";
const QString AxisMapper::TABULAR = "Tabular";
const QString AxisMapper::QUALITY = "Quality";


std::multimap<Carta::Lib::AxisInfo::KnownType, QString> AxisMapper::axisMap;

AxisMapper::AxisMapper(){

}

QString AxisMapper::_getAxisRAPurpose( const Carta::Lib::KnownSkyCS& cs ){
    QString name = RIGHT_ASCENSION;
    if ( cs == Carta::Lib::KnownSkyCS::Galactic || cs == Carta::Lib::KnownSkyCS::Ecliptic ){
        name = LONGITUDE;
    }
    return name;
}

QString AxisMapper::_getAxisDECPurpose( const Carta::Lib::KnownSkyCS& cs ){
    QString name = DECLINATION;
    if ( cs == Carta::Lib::KnownSkyCS::Galactic || cs == Carta::Lib::KnownSkyCS::Ecliptic ){
        name = LATITUDE;
    }
    return name;
}

QString AxisMapper::getDefaultPurpose( const QString& axis, const Carta::Lib::KnownSkyCS& cs ){
    QString name;
    if ( axis == AXIS_X ){
        name = _getAxisRAPurpose( cs );
    }
    else if ( axis == AXIS_Y ){
        name = _getAxisDECPurpose( cs );
    }
    return name;
}

QString AxisMapper::getDisplayName( const QString& axisName ){
    QString result;
    if ( QString::compare( axisName, AXIS_X, Qt::CaseInsensitive) == 0 ){
        result = AXIS_X;
    }
    else if ( QString::compare( axisName, AXIS_Y, Qt::CaseInsensitive) == 0 ){
        result = AXIS_Y;
    }
    return result;
}

QStringList AxisMapper::getDisplayNames(){
    QStringList names = {AXIS_X, AXIS_Y};
    return names;
}

QString AxisMapper::getPurpose( Carta::Lib::AxisInfo::KnownType type){
    return (axisMap.find(type) != axisMap.end()) ?
                axisMap.find(type)->second : QString("Undefined");
}

QString AxisMapper::getAnimatorPurpose( Carta::Lib::AxisInfo::KnownType type){
    if(type == Carta::Lib::AxisInfo::KnownType::SPECTRAL){
        return QString("Channel");
    }
    return getPurpose(type);
}

void AxisMapper::cleanAxisMap(){
    axisMap.clear();
}

void AxisMapper::setAxisMap( AxisMapData supplant, QString target ){
    std::multimap<Carta::Lib::AxisInfo::KnownType, QString>::iterator
            iter = axisMap.find(supplant.first);
    // if there is any axis with the same type, check whether
    // we do want replace the name.
    if(iter != axisMap.end() && iter->second == target){
        axisMap.erase(iter);
    }
    axisMap.insert(supplant);
}

Carta::Lib::AxisInfo::KnownType AxisMapper::getType( const QString& purpose ){

    std::multimap<Carta::Lib::AxisInfo::KnownType, QString>::iterator iter;
    // for Animator to display "channel"
    if(purpose == "Channel"){
        return Carta::Lib::AxisInfo::KnownType::SPECTRAL;
    }
    for( iter = axisMap.begin(); iter != axisMap.end(); iter++){
        if(iter->second == purpose){
            return iter->first;
        }
    }
    return Carta::Lib::AxisInfo::KnownType::OTHER;
}


AxisMapper::~AxisMapper(){

}
}
}
