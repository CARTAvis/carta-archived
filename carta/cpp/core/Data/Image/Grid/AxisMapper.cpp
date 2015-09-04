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
const QString AxisMapper::AXIS_Z = "zAxis";
const QString AxisMapper::RIGHT_ASCENSION = "Right Ascension";
const QString AxisMapper::DECLINATION = "Declination";
const QString AxisMapper::SPECTRAL = "Frequency";
const QString AxisMapper::STOKES = "Stokes";
const QString AxisMapper::TABULAR = "Tabular";
const QString AxisMapper::QUALITY = "Quality";

const QList<QString> AxisMapper::m_purposes( {RIGHT_ASCENSION, DECLINATION, SPECTRAL,
    STOKES, TABULAR, QUALITY});

AxisMapper::AxisMapper(){

}

QString AxisMapper::getDefaultPurpose( const QString& axis ){
    QString name;
    if ( axis == AXIS_X ){
        name = RIGHT_ASCENSION;
    }
    else if ( axis == AXIS_Y ){
        name = DECLINATION;
    }
    else if ( axis == AXIS_Z ){
        name = SPECTRAL;
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
    else if ( QString::compare( axisName, AXIS_Z, Qt::CaseInsensitive) == 0 ){
        result = AXIS_Z;
    }
    return result;
}

QStringList AxisMapper::getDisplayNames(){
    QStringList names = {AXIS_X, AXIS_Y, AXIS_Z};
    return names;
}

QString AxisMapper::getPurpose( Carta::Lib::AxisInfo::KnownType type ){
    QString name;
    int typeIndex = static_cast<int>( type );
    if ( typeIndex < m_purposes.size() ){
        name = m_purposes[typeIndex];
    }
    return name;
}

QString AxisMapper::getPurpose( const QString& purpose ){
    QString actualPurpose;
    int purposeCount = m_purposes.size();
    for ( int i = 0; i < purposeCount; i++ ){
        if ( QString::compare( purpose, m_purposes[i], Qt::CaseInsensitive) == 0 ){
            actualPurpose = m_purposes[i];
            break;
        }
    }
    return actualPurpose;
}

Carta::Lib::AxisInfo::KnownType AxisMapper::getType( const QString& purpose ){
    Carta::Lib::AxisInfo::KnownType target = Carta::Lib::AxisInfo::KnownType::OTHER;
    int purposeCount = m_purposes.size();
    for ( int i = 0; i < purposeCount; i++ ){
        if ( QString::compare( purpose, m_purposes[i], Qt::CaseInsensitive) == 0 ){
            target = static_cast<Carta::Lib::AxisInfo::KnownType>(i);
            break;
        }
    }
    return target;
}


AxisMapper::~AxisMapper(){

}
}
}
