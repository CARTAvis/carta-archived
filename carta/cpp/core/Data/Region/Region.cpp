#include "Region.h"
#include "Data/Util.h"
#include "State/UtilState.h"

#include <QDebug>

namespace Carta {

namespace Data {

const QString Region::CLASS_NAME = "Region";
const QString Region::CORNERS = "corners";
const QString Region::REGION_POLYGON = "Polygon";
const QString Region::REGION_ELLIPSE = "Ellipse";
const QString Region::REGION_TYPE = "regionType";
const QString Region::XCOORD = "x";
const QString Region::YCOORD = "y";

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
    _initializeCallbacks();
    _initializeState();
}

void Region::addCorners( const std::vector< std::pair<double,double> >& corners ){
    int cornerCount = corners.size();
    m_state.resizeArray( CORNERS, cornerCount );
    for ( int i = 0; i < cornerCount; i++ ){
        QString eleLookup = Carta::State::UtilState::getLookup( CORNERS, i );
        QString xLookup = Carta::State::UtilState::getLookup( eleLookup, XCOORD );
        QString yLookup = Carta::State::UtilState::getLookup( eleLookup, YCOORD );
        m_state.insertValue<double>( xLookup, corners[i].first );
        m_state.insertValue<double>( yLookup, corners[i].second );
    }
    m_state.flushState();
}

std::shared_ptr<Carta::Lib::RegionInfo> Region::getInfo() const {
    std::shared_ptr<Carta::Lib::RegionInfo> info( new Carta::Lib::RegionInfo() );
    info->setRegionType( getRegionType() );
    int cornerCount = m_state.getArraySize( CORNERS );
    std::vector< std::pair<double,double> > corners( cornerCount );
    for( int i = 0; i < cornerCount; i++ ){
        QString eleLookup = Carta::State::UtilState::getLookup( CORNERS, i );
        QString xLookup = Carta::State::UtilState::getLookup( eleLookup, XCOORD );
        QString yLookup = Carta::State::UtilState::getLookup( eleLookup, YCOORD );
        double xValue = m_state.getValue<double>( xLookup );
        double yValue = m_state.getValue<double>( yLookup );
        corners[i] = std::pair<double,double>( xValue, yValue );
    }
    info->setCorners( corners );
    return info;
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
    m_state.insertArray( CORNERS, 0 );
    m_state.flushState();
}


void Region::_restoreState( const QString& stateStr ){
    Carta::State::StateInterface dataState( "" );
    dataState.setState( stateStr );
    QString regionType = dataState.getValue<QString>(REGION_TYPE);
    m_state.setValue<QString>( REGION_TYPE, regionType);
    int cornerCount = dataState.getArraySize( CORNERS );
    m_state.resizeArray( CORNERS, cornerCount );
    for ( int i = 0; i < cornerCount; i++ ){
        QString eleLookup = Carta::State::UtilState::getLookup( CORNERS, i );
        QString xLookup = Carta::State::UtilState::getLookup( eleLookup, XCOORD );
        QString yLookup = Carta::State::UtilState::getLookup( eleLookup, YCOORD );
        double xValue = dataState.getValue<double>( xLookup );
        double yValue = dataState.getValue<double>( yLookup );
        m_state.insertValue<double>( xLookup, xValue );
        m_state.insertValue<double>( yLookup, yValue );
    }
    m_state.flushState();
}

void Region::setRegionType( Carta::Lib::RegionInfo::RegionType regionType ){
    QString oldRegionTypeStr = m_state.getValue<QString>( REGION_TYPE );
    QString regionTypeStr;
    if ( regionType == Carta::Lib::RegionInfo::RegionType::Polygon ){
        regionTypeStr == REGION_POLYGON;
    }
    else if ( regionType == Carta::Lib::RegionInfo::RegionType::Ellipse ){
        regionTypeStr == REGION_ELLIPSE;
    }
    else {
        qDebug() << "Unrecognized Region type: "<< (int)(regionType);
    }
    if ( !regionTypeStr.isEmpty() && regionTypeStr != oldRegionTypeStr ){
        m_state.setValue<QString>( REGION_TYPE, regionTypeStr );
        m_state.flushState();
    }
}

Region::~Region(){

}
}
}
