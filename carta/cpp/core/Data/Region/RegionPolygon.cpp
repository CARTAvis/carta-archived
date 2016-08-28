#include "RegionPolygon.h"
#include "Data/Util.h"
#include "State/UtilState.h"
#include "CartaLib/CartaLib.h"
#include "CartaLib/Regions/IRegion.h"

#include <QDebug>

namespace Carta {

namespace Data {

const QString RegionPolygon::CLASS_NAME = "RegionPolygon";

class RegionPolygon::Factory : public Carta::State::CartaObjectFactory {
public:

    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new RegionPolygon (path, id);
    }
};

bool RegionPolygon::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new RegionPolygon::Factory());


RegionPolygon::RegionPolygon(const QString& path, const QString& id )
    :Region( CLASS_NAME, path, id ){
    _initializeState();
}


void RegionPolygon::addCorners( const std::vector< std::pair<double,double> >& corners ){
    int cornerCount = corners.size();
    m_state.resizeArray( Carta::Lib::Regions::Polygon::POINTS, cornerCount );
    for ( int i = 0; i < cornerCount; i++ ){
        QString eleLookup = Carta::State::UtilState::getLookup( Carta::Lib::Regions::Polygon::POINTS, i );
        QString xLookup = Carta::State::UtilState::getLookup( eleLookup, Util::XCOORD );
        QString yLookup = Carta::State::UtilState::getLookup( eleLookup, Util::YCOORD );
        m_state.insertValue<double>( xLookup, corners[i].first );
        m_state.insertValue<double>( yLookup, corners[i].second );
    }
    m_state.flushState();
    //User has not set a custom name
    if ( ! m_regionNameSet ){
        //Note the type plus the bounding box is not a unique identifier for a polygon
        //but it may be the best choice for a default name as it is shorter than using
        //all of the corners.
        std::shared_ptr<Carta::Lib::Regions::RegionBase> base = getModel();
        if ( base ){
            QRectF boundingBox = base->outlineBox();
            QPointF topLeft = boundingBox.topLeft();
            QPointF bottomRight = boundingBox.bottomRight();
            QString name = getType() + " [("+topLeft.x()+","+topLeft.y()+
                    "),("+bottomRight.x()+","+bottomRight.y()+")]";
            m_state.setValue<QString>(Util::NAME, name );
        }
    }
}

std::vector<std::pair<double,double> > RegionPolygon::getCorners() const {
    int cornerCount = m_state.getArraySize( Carta::Lib::Regions::Polygon::POINTS );
    std::vector< std::pair<double,double> > corners( cornerCount );
    for( int i = 0; i < cornerCount; i++ ){
        QString eleLookup = Carta::State::UtilState::getLookup( Carta::Lib::Regions::Polygon::POINTS, i );
        QString xLookup = Carta::State::UtilState::getLookup( eleLookup, Util::XCOORD );
        QString yLookup = Carta::State::UtilState::getLookup( eleLookup, Util::YCOORD );
        double xValue = m_state.getValue<double>( xLookup );
        double yValue = m_state.getValue<double>( yLookup );
        corners[i] = std::pair<double,double>( xValue, yValue );
    }
    return corners;
}


std::shared_ptr<Carta::Lib::Regions::RegionBase> RegionPolygon::getModel() const {
    std::shared_ptr<Carta::Lib::Regions::RegionBase> info( new Carta::Lib::Regions::Polygon() );
    QJsonObject jsonObject = toJSON();
    bool jsonValid = info->initFromJson( jsonObject );
    if ( !jsonValid ){
        qWarning()<<"Invalid json string: "<<jsonObject;
    }
    return info;
}




void RegionPolygon::_initializeState(){
    m_state.setValue<QString>( REGION_TYPE, Carta::Lib::Regions::Polygon::TypeName );
    m_state.insertArray( Carta::Lib::Regions::Polygon::POINTS, 0 );
    m_state.flushState();
}

bool RegionPolygon::isPointInside( const QPointF & /*pt*/ ) const {
	return false;
}


void RegionPolygon::_restoreState( const QString& stateStr ){
    Region::_restoreState( stateStr );
    Carta::State::StateInterface dataState( "" );
    dataState.setState( stateStr );
    int cornerCount = dataState.getArraySize( Carta::Lib::Regions::Polygon::POINTS );
    m_state.resizeArray( Carta::Lib::Regions::Polygon::POINTS, cornerCount );
    for ( int i = 0; i < cornerCount; i++ ){
        QString eleLookup = Carta::State::UtilState::getLookup( Carta::Lib::Regions::Polygon::POINTS, i );
        QString xLookup = Carta::State::UtilState::getLookup( eleLookup, Util::XCOORD );
        QString yLookup = Carta::State::UtilState::getLookup( eleLookup, Util::YCOORD );
        double xValue = dataState.getValue<double>( xLookup );
        double yValue = dataState.getValue<double>( yLookup );
        m_state.insertValue<double>( xLookup, xValue );
        m_state.insertValue<double>( yLookup, yValue );
    }
    m_state.flushState();
}


QJsonObject RegionPolygon::toJSON() const {
    QJsonObject descript = Region::toJSON();
    std::vector<std::pair<double,double> > corners = getCorners();
    QJsonArray cornerArray;
    int cornerCount = corners.size();
    if ( cornerCount > 0 ){
        for ( int i = 0; i < cornerCount; i++ ){
            QJsonObject cornerObject;
            cornerObject.insert( Carta::Lib::Regions::Polygon::POINT_X, corners[i].first );
            cornerObject.insert( Carta::Lib::Regions::Polygon::POINT_Y, corners[i].second );
            cornerArray.append( cornerObject );
        }
    }
    descript.insert( Carta::Lib::Regions::Polygon::POINTS, cornerArray );
    return descript;
}


RegionPolygon::~RegionPolygon(){

}
}
}
