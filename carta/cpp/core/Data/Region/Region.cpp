#include "Region.h"
#include "Data/Util.h"
#include "State/UtilState.h"
#include "CartaLib/CartaLib.h"

#include <QDebug>

namespace Carta {

namespace Data {

const QString Region::CLASS_NAME = "Region";
const QString Region::CORNERS = "corners";
const QString Region::REGION_POLYGON = "Polygon";
const QString Region::REGION_ELLIPSE = "Ellipse";
const QString Region::REGION_TYPE = "regionType";

class Region::Factory : public Carta::State::CartaObjectFactory {
public:

    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new Region (path, id);
    }
};

bool Region::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new Region::Factory());


Region::Region(const QString& path, const QString& id )
    :CartaObject( CLASS_NAME, path, id ){
    m_regionNameSet = false;
    _initializeCallbacks();
    _initializeState();
}


void Region::addCorners( const std::vector< std::pair<double,double> >& corners ){
    int cornerCount = corners.size();
    m_state.resizeArray( CORNERS, cornerCount );
    for ( int i = 0; i < cornerCount; i++ ){
        QString eleLookup = Carta::State::UtilState::getLookup( CORNERS, i );
        QString xLookup = Carta::State::UtilState::getLookup( eleLookup, Util::XCOORD );
        QString yLookup = Carta::State::UtilState::getLookup( eleLookup, Util::YCOORD );
        m_state.insertValue<double>( xLookup, corners[i].first );
        m_state.insertValue<double>( yLookup, corners[i].second );
    }
    m_state.flushState();
    //User has not set a custom name
    if ( ! m_regionNameSet ){
        m_state.setValue<QString>(Util::NAME, toString() );
    }
}

std::vector<std::pair<double,double> > Region::getCorners() const {
    int cornerCount = m_state.getArraySize( CORNERS );
    std::vector< std::pair<double,double> > corners( cornerCount );
    for( int i = 0; i < cornerCount; i++ ){
        QString eleLookup = Carta::State::UtilState::getLookup( CORNERS, i );
        QString xLookup = Carta::State::UtilState::getLookup( eleLookup, Util::XCOORD );
        QString yLookup = Carta::State::UtilState::getLookup( eleLookup, Util::YCOORD );
        double xValue = m_state.getValue<double>( xLookup );
        double yValue = m_state.getValue<double>( yLookup );
        corners[i] = std::pair<double,double>( xValue, yValue );
    }
    return corners;
}

Carta::Lib::RegionInfo Region::getInfo() const {
    Carta::Lib::RegionInfo info;
    info.setRegionType( getRegionType() );
    std::vector<std::pair<double,double> > corners = getCorners();
    info.setCorners( corners );
    return info;
}

QString Region::getRegionName() const {
    QString name = m_state.getValue<QString>( Util::NAME );
    return name;
}

Carta::Lib::RegionInfo::RegionType Region::getRegionType( const QString& regionTypeStr ){
    Carta::Lib::RegionInfo::RegionType regionType = Carta::Lib::RegionInfo::RegionType::Unknown;
    int result = QString::compare( regionTypeStr, REGION_POLYGON, Qt::CaseInsensitive );
    if ( result == 0 ){
        regionType = Carta::Lib::RegionInfo::RegionType::Polygon;
    }
    else {
        result = QString::compare( regionTypeStr, REGION_ELLIPSE, Qt::CaseInsensitive );
        if ( result == 0 ){
            regionType = Carta::Lib::RegionInfo::RegionType::Ellipse;
        }
    }
    return regionType;
}

Carta::Lib::RegionInfo::RegionType Region::getRegionType() const {
    QString regionTypeStr = m_state.getValue<QString>( REGION_TYPE );
    Carta::Lib::RegionInfo::RegionType regionType = Region::getRegionType( regionTypeStr );
    return regionType;
}

QString Region::_getStateString() const {
    return m_state.toString();
}

void Region::_initializeCallbacks(){
    addCommandCallback( "shapeChanged", [=] (const QString & /*cmd*/,
                                    const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = { "info"};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        //resetStateData( dataValues[ *keys.begin()]);
        return "";
    });
}

void Region::_initializeState(){
    m_state.insertValue<QString>( REGION_TYPE, REGION_POLYGON );
    m_state.insertValue<bool>( Util::SELECTED, true );
    m_state.insertArray( CORNERS, 0 );
    m_state.insertValue<QString>( Util::NAME, "");
    m_state.flushState();
}

void Region::_restoreState( const QString& stateStr ){
    Carta::State::StateInterface dataState( "" );
    dataState.setState( stateStr );
    QString regionType = dataState.getValue<QString>(REGION_TYPE);
    m_state.setValue<QString>( REGION_TYPE, regionType);
    QString name = dataState.getValue<QString>( Util::NAME );
    m_state.setValue<QString>(Util::NAME, name );
    int cornerCount = dataState.getArraySize( CORNERS );
    m_state.resizeArray( CORNERS, cornerCount );
    for ( int i = 0; i < cornerCount; i++ ){
        QString eleLookup = Carta::State::UtilState::getLookup( CORNERS, i );
        QString xLookup = Carta::State::UtilState::getLookup( eleLookup, Util::XCOORD );
        QString yLookup = Carta::State::UtilState::getLookup( eleLookup, Util::YCOORD );
        double xValue = dataState.getValue<double>( xLookup );
        double yValue = dataState.getValue<double>( yLookup );
        m_state.insertValue<double>( xLookup, xValue );
        m_state.insertValue<double>( yLookup, yValue );
    }
    m_state.flushState();
}

QString Region::regionTypeToString( Carta::Lib::RegionInfo::RegionType regionType ) const {
    QString regionTypeStr;
    if ( regionType == Carta::Lib::RegionInfo::RegionType::Polygon ){
        regionTypeStr = REGION_POLYGON;
    }
    else if ( regionType == Carta::Lib::RegionInfo::RegionType::Ellipse ){
        regionTypeStr = REGION_ELLIPSE;
    }
    else {
        qDebug() << "Unrecognized Region type: "<< (int)(regionType);
    }
    return regionTypeStr;
}

QString Region::setRegionName( const QString& name ){
    QString result;
    if ( name.trimmed().length() > 0 ){
        QString oldName = m_state.getValue<QString>( Util::NAME );
        if ( oldName != name ){
            m_regionNameSet = true;
            m_state.setValue<QString>( Util::NAME, name );
            m_state.flushState();
        }
    }
    return result;
}

void Region::setRegionType( Carta::Lib::RegionInfo::RegionType regionType ){
    QString oldRegionTypeStr = m_state.getValue<QString>( REGION_TYPE );
    QString regionTypeStr = regionTypeToString( regionType );
    if ( !regionTypeStr.isEmpty() && regionTypeStr != oldRegionTypeStr ){
        m_state.setValue<QString>( REGION_TYPE, regionTypeStr );
        if ( !m_regionNameSet ){
            m_state.setValue<QString>(Util::NAME, toString() );
        }
        m_state.flushState();
    }
}

QString Region::toString() const {
    Carta::Lib::RegionInfo::RegionType regionType = getRegionType();
    QString descript( regionTypeToString( regionType ) );
    std::vector<std::pair<double,double> > corners = getCorners();
    int cornerCount = corners.size();
    if ( cornerCount > 0 ){
        descript = descript + "[";
        for ( int i = 0; i < cornerCount; i++ ){
            descript = descript + "(" + QString::number(corners[i].first) +
                    ","+QString::number(corners[i].second) + ")";
            if ( i < cornerCount - 1 ){
                descript = descript + ",";
            }
        }
        descript = descript + "]";
    }
    return descript;
}

Region::~Region(){

}
}
}
