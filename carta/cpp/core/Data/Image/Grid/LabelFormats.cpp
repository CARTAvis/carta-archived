#include "LabelFormats.h"
#include "Data/Util.h"
#include "State/UtilState.h"
#include "CartaLib/AxisInfo.h"

#include <QDebug>

namespace Carta {

namespace Data {

const QString LabelFormats::CLASS_NAME = "LabelFormats";
const QString LabelFormats::AXES_FORMATS = "axesFormats";
const QString LabelFormats::LABEL_FORMATS = "labelFormats";
const QString LabelFormats::FORMAT = "format";
const QString LabelFormats::FORMAT_DEFAULT = "Default";
const QString LabelFormats::FORMAT_NONE = "No Label";
const QString LabelFormats::FORMAT_DEG_MIN_SEC = "Deg:Min:Sec";
const QString LabelFormats::FORMAT_DECIMAL_DEG = "Decimal Degrees";
const QString LabelFormats::FORMAT_HR_MIN_SEC = "Hr:Min:Sec";
const QString LabelFormats::EAST = "left";
const QString LabelFormats::WEST = "right";
const QString LabelFormats::NORTH = "top";
const QString LabelFormats::SOUTH = "bottom";

const QString LabelFormats::LABEL_AXES = "axes";

class LabelFormats::Factory : public Carta::State::CartaObjectFactory {

    public:

        Factory():
            CartaObjectFactory( CLASS_NAME ){};

        Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new LabelFormats (path, id);
        }
    };


bool LabelFormats::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new LabelFormats::Factory());


LabelFormats::LabelFormats( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){

    m_labelFormats.resize( 5 );
    m_labelFormats[0] = FORMAT_NONE;
    m_labelFormats[1] = FORMAT_DEFAULT;
    m_labelFormats[2] = FORMAT_DEG_MIN_SEC;
    m_labelFormats[3] = FORMAT_HR_MIN_SEC;
    m_labelFormats[4] = FORMAT_DECIMAL_DEG;

    _initializeDefaultState();
}

QString LabelFormats::getDefaultFormat( const QString& direction ) const {
    QString actualDirection = getDirection( direction );
    QString format = FORMAT_NONE;
    if ( actualDirection == EAST || actualDirection == SOUTH ){
        format = FORMAT_DEFAULT;
    }
    return format;
}

QString LabelFormats::getDefaultFormatForAxis( Carta::Lib::AxisInfo::KnownType axis,
        const Carta::Lib::KnownSkyCS& cs ) const {
    QString defaultFormat = FORMAT_DECIMAL_DEG;
    if ( cs != Carta::Lib::KnownSkyCS::Galactic && cs != Carta::Lib::KnownSkyCS::Ecliptic ){
        if ( axis == Carta::Lib::AxisInfo::KnownType::DIRECTION_LON ){
            defaultFormat = FORMAT_HR_MIN_SEC;
        }
        else if ( axis == Carta::Lib::AxisInfo::KnownType::DIRECTION_LAT ){
            defaultFormat = FORMAT_DEG_MIN_SEC;
        }
    }
    return defaultFormat;
}

QString LabelFormats::getDirection( const QString& direction ) const {
    QString actualDirection;
    if ( QString::compare( direction, EAST, Qt::CaseInsensitive) == 0 ){
        actualDirection = EAST;
    }
    else if ( QString::compare( direction, WEST, Qt::CaseInsensitive) == 0){
        actualDirection = WEST;
    }
    else if ( QString::compare( direction, NORTH, Qt::CaseInsensitive) == 0){
        actualDirection = NORTH;
    }
    else if ( QString::compare( direction, SOUTH, Qt::CaseInsensitive) == 0){
        actualDirection = SOUTH;
    }
    return actualDirection;
}

bool LabelFormats::isDefault( const QString& format ) const {
    bool defaultFormat = false;
    if ( QString::compare( format, FORMAT_DEFAULT, Qt::CaseInsensitive) == 0 ){
        defaultFormat = true;
    }
    return defaultFormat;
}

QString LabelFormats::_addDecimalSeconds( const QString& baseFormat, int decimals ) const {
    QString secondFormat = baseFormat;
    if ( secondFormat.length() > 0 ){
        if ( decimals > 0 ){
            secondFormat = secondFormat + "."+QString::number(decimals);
        }
    }
    return secondFormat;
}


QString LabelFormats::getFormat( const QString& format ) const {
    QString actualFormat;
    int formatCount = m_labelFormats.size();
    for ( int i = 0; i < formatCount; i++ ){
        int compareResult = QString::compare( format, m_labelFormats[i], Qt::CaseInsensitive);
        if ( compareResult == 0 ){
            actualFormat = m_labelFormats[i];
            break;
        }
    }
    return actualFormat;
}

Carta::Lib::AxisLabelInfo::Formats LabelFormats::getAxisLabelFormat( const QString& formatStr ) const {
    QString actualFormat = getFormat( formatStr );
    Carta::Lib::AxisLabelInfo::Formats target = Carta::Lib::AxisLabelInfo::Formats::OTHER;
    if ( actualFormat == FORMAT_DEG_MIN_SEC ){
        target = Carta::Lib::AxisLabelInfo::Formats::DEG_MIN_SEC;
    }
    else if ( actualFormat == FORMAT_DECIMAL_DEG ){
        target = Carta::Lib::AxisLabelInfo::Formats::DECIMAL_DEG;
    }
    else if ( actualFormat == FORMAT_HR_MIN_SEC ){
        target = Carta::Lib::AxisLabelInfo::Formats::HR_MIN_SEC;
    }
    else if ( actualFormat == FORMAT_NONE ){
        target = Carta::Lib::AxisLabelInfo::Formats::NONE;
    }
    return target;
}

Carta::Lib::AxisLabelInfo::Locations LabelFormats::getAxisLabelLocation( const QString& locationStr ) const {
    QString actualLocation = getDirection( locationStr );
    Carta::Lib::AxisLabelInfo::Locations target = Carta::Lib::AxisLabelInfo::Locations::OTHER;
    if ( actualLocation == EAST ){
        target = Carta::Lib::AxisLabelInfo::Locations::EAST;
    }
    else if ( actualLocation == WEST ){
        target = Carta::Lib::AxisLabelInfo::Locations::WEST;
    }
    else if ( actualLocation == NORTH ){
        target = Carta::Lib::AxisLabelInfo::Locations::NORTH;
    }
    else if ( actualLocation == SOUTH ){
        target = Carta::Lib::AxisLabelInfo::Locations::SOUTH;
    }
    return target;
}


QStringList LabelFormats::getLabelFormats( Carta::Lib::AxisInfo::KnownType direction ) const {
    QStringList buff;
    if ( direction == Carta::Lib::AxisInfo::KnownType::DIRECTION_LON ){
        buff.append( FORMAT_HR_MIN_SEC );
        buff.append( FORMAT_DECIMAL_DEG );
        buff.append( FORMAT_DEFAULT );
        buff.append( FORMAT_NONE );
    }
    else if ( direction == Carta::Lib::AxisInfo::KnownType::DIRECTION_LAT ){
        buff.append( FORMAT_DEG_MIN_SEC );
        buff.append( FORMAT_DECIMAL_DEG );
        buff.append( FORMAT_DEFAULT );
        buff.append( FORMAT_NONE );
    }
    else {
        buff.append( FORMAT_DEFAULT );
        buff.append( FORMAT_NONE );
    }
    return buff;
}

int LabelFormats::_getIndex( const QString& format ) const {
    int targetIndex = -1;
    int formatCount = m_labelFormats.size();
    for ( int i = 0; i < formatCount; i++ ){
        if ( m_labelFormats[i] == format ){
            targetIndex = i;
            break;
        }
    }
    return targetIndex;
}

std::vector<int> LabelFormats::_getFormatIndices( Carta::Lib::AxisInfo::KnownType axisIndex ) const {
    //Right ascension
    std::vector<int> indices;
    if ( axisIndex == Carta::Lib::AxisInfo::KnownType::DIRECTION_LON ){
        indices.push_back( _getIndex( FORMAT_HR_MIN_SEC ) );
        indices.push_back( _getIndex( FORMAT_DECIMAL_DEG ) );
        indices.push_back( _getIndex( FORMAT_DEFAULT) );
        indices.push_back( _getIndex( FORMAT_NONE ) );
    }
    else if ( axisIndex == Carta::Lib::AxisInfo::KnownType::DIRECTION_LAT ){
        indices.push_back( _getIndex( FORMAT_DEG_MIN_SEC ) );
        indices.push_back( _getIndex( FORMAT_DECIMAL_DEG ) );
        indices.push_back( _getIndex( FORMAT_DEFAULT) );
        indices.push_back( _getIndex( FORMAT_NONE ) );
    }
    else {
        indices.push_back( _getIndex( FORMAT_DEFAULT) );
        indices.push_back( _getIndex( FORMAT_NONE ) );
    }
    return indices;
}

QString LabelFormats::getOppositeSide( const QString& side ) const {
    QString oppSide;
    if ( side == EAST ){
        oppSide = WEST;
    }
    else if ( side == WEST ){
        oppSide = EAST;
    }
    else if ( side == NORTH ){
        oppSide = SOUTH;
    }
    else if ( side == SOUTH ){
        oppSide = NORTH;
    }
    return oppSide;
}


void LabelFormats::_initializeDefaultState(){
    int formatCount = m_labelFormats.size();
    m_state.insertArray( LABEL_FORMATS, formatCount );
    for ( int i = 0; i < formatCount; i++ ){
        QString lookup = Carta::State::UtilState::getLookup( LABEL_FORMATS, i );
        m_state.setValue<QString>( lookup, m_labelFormats[i] );
    }
    int supportedAxisCount = 2;
    m_state.insertArray( AXES_FORMATS, supportedAxisCount );
    for ( int i = 0; i < supportedAxisCount; i++ ){
        QString lookup = Carta::State::UtilState::getLookup( AXES_FORMATS, i );
        std::vector<int> formatIndices = _getFormatIndices( static_cast<Carta::Lib::AxisInfo::KnownType>(i) );
        int formatCount = formatIndices.size();
        m_state.setArray( lookup, formatCount );
        for ( int j = 0; j < formatCount; j++ ){
            QString formatLookup = Carta::State::UtilState::getLookup( lookup, j );
            m_state.setValue<int>( formatLookup, formatIndices[j] );
        }
    }
    m_state.flushState();
}

bool LabelFormats::isVisible( const QString& format ) const {
    bool visibleFormat = true;
    QString actualFormat = getFormat( format );
    if ( actualFormat == FORMAT_NONE ){
        visibleFormat = false;
    }
    return visibleFormat;
}

LabelFormats::~LabelFormats(){

}
}
}
