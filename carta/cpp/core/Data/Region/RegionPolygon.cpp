#include "RegionPolygon.h"
#include "Data/Util.h"
#include "State/UtilState.h"
#include "CartaLib/CartaLib.h"
#include "CartaLib/Regions/IRegion.h"
#include "Shape/ShapePolygon.h"

#include <QDebug>

namespace Carta {

namespace Data {

const QString RegionPolygon::CLASS_NAME = "RegionPolygon";

class RegionPolygon::Factory : public Carta::State::CartaObjectFactory {
public:
    Carta::State::CartaObject * create (const QString & path, const QString & id){
        return new RegionPolygon (path, id);
    }
};

bool RegionPolygon::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new RegionPolygon::Factory());


RegionPolygon::RegionPolygon(const QString& path, const QString& id )
    :Region( CLASS_NAME, path, id ){
	m_shape.reset( new Shape::ShapePolygon());
	connect( m_shape.get(), SIGNAL(shapeChanged( const QJsonObject&)),
				this, SLOT(_updateStateFromJson( const QJsonObject&)));
    _initializeState();
    _updateShapeFromState();
}

void RegionPolygon::addCorner( const QPointF& pt ){
	Shape::ShapePolygon* poly = dynamic_cast<Shape::ShapePolygon*>( m_shape.get());
	if ( !poly->isCorner( pt ) ){
		std::vector<QPointF> corners(1);
		corners[0] = pt;
		_addCorners( corners );
	}
}

void RegionPolygon::_addCorners( const std::vector< QPointF >& corners ){
    int cornerCount = corners.size();
    int oldArraySize = m_state.getArraySize( Carta::Lib::Regions::Polygon::POINTS );
    int newArraySize = cornerCount + oldArraySize;
    m_state.resizeArray( Carta::Lib::Regions::Polygon::POINTS, newArraySize,
    		Carta::State::StateInterface::PreserveAll );
    for ( int i = oldArraySize; i < newArraySize; i++ ){
    	_insertCorner( i, corners[i-oldArraySize] );
    }
    m_state.flushState();

    m_shape->setModel( toJSON() );

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

void RegionPolygon::_closePolygon(){
	bool validCorner = false;
	if ( m_shape ){
		Shape::ShapePolygon* poly = dynamic_cast<Shape::ShapePolygon*>(m_shape.get());
		if ( !poly->isClosed() ){
			QPointF firstPoint = getCorner( 0, &validCorner );
			if ( validCorner ){
				//Circumvent the uniqueness check when adding the last corner
				//by calling _addCorners instead of _addCorner
				std::vector<QPointF> corners(1);
				corners[0] = firstPoint;
				_addCorners( corners );

				emit editDone();
			}
		}
	}
}

QPointF RegionPolygon::getCorner( int index, bool* valid ) const {
    int cornerCount = m_state.getArraySize( Carta::Lib::Regions::Polygon::POINTS );
    QPointF corner;
    if ( index >= 0 && index < cornerCount ){
    	*valid = true;
        QString eleLookup = Carta::State::UtilState::getLookup( Carta::Lib::Regions::Polygon::POINTS, index );
        QString xLookup = Carta::State::UtilState::getLookup( eleLookup, Util::XCOORD );
        QString yLookup = Carta::State::UtilState::getLookup( eleLookup, Util::YCOORD );
        double xValue = m_state.getValue<double>( xLookup );
        double yValue = m_state.getValue<double>( yLookup );
        corner = QPointF( xValue, yValue );
    }
    else {
    	*valid = false;
    }
    return corner;
}


std::vector< QPointF > RegionPolygon::getCorners() const {
    int cornerCount = m_state.getArraySize( Carta::Lib::Regions::Polygon::POINTS );
    std::vector< QPointF > corners( cornerCount );
    for( int i = 0; i < cornerCount; i++ ){
        QString eleLookup = Carta::State::UtilState::getLookup( Carta::Lib::Regions::Polygon::POINTS, i );
        QString xLookup = Carta::State::UtilState::getLookup( eleLookup, Util::XCOORD );
        QString yLookup = Carta::State::UtilState::getLookup( eleLookup, Util::YCOORD );
        double xValue = m_state.getValue<double>( xLookup );
        double yValue = m_state.getValue<double>( yLookup );
        corners[i] = QPointF( xValue, yValue );
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

void RegionPolygon::_insertCorner( int index, const QPointF& corner ){
	QString eleLookup = Carta::State::UtilState::getLookup( Carta::Lib::Regions::Polygon::POINTS, index );
	QString xLookup = Carta::State::UtilState::getLookup( eleLookup, Util::XCOORD );
	QString yLookup = Carta::State::UtilState::getLookup( eleLookup, Util::YCOORD );
	m_state.insertValue<double>( xLookup, corner.x() );
	m_state.insertValue<double>( yLookup, corner.y() );
}

void RegionPolygon::handleTouch( const QPointF& pt ){
	bool tapped = false;
	if ( isEditMode() ){
		//Add a new corner point
		addCorner( pt );
	}
	else {
		if ( m_shape->isPointInside( pt ) ){
			tapped = true;
		}
		setSelected( tapped );
	}
}

void RegionPolygon::handleTapDouble( const QPointF& pt ){
	if ( isEditMode() ){
		//Add a new corner point
		addCorner( pt );
		//Close the polygon
		_closePolygon();
	}
}


void RegionPolygon::setModel( Carta::Lib::Regions::RegionBase* model ){
	if ( model ){
		QString regionType = model->typeName();
		CARTA_ASSERT( regionType == Carta::Lib::Regions::Polygon::TypeName );

		QJsonObject obj = model->toJson();
		Carta::Lib::Regions::Polygon* polyRegion = dynamic_cast<Carta::Lib::Regions::Polygon*>( model );
		QPolygonF poly = polyRegion->qpolyf();
		int cornerCount = poly.size();
		for ( int i = 0; i < cornerCount; i++ ){
			addCorner( poly.value(i) );
		}
		_closePolygon();
	}
}


QJsonObject RegionPolygon::toJSON() const {
    QJsonObject descript = Region::toJSON();
    std::vector<QPointF> corners = getCorners();
    QJsonArray cornerArray;
    int cornerCount = corners.size();
    if ( cornerCount > 0 ){
        for ( int i = 0; i < cornerCount; i++ ){
            QJsonObject cornerObject;
            cornerObject.insert( Carta::Lib::Regions::RegionBase::POINT_X, corners[i].x() );
            cornerObject.insert( Carta::Lib::Regions::RegionBase::POINT_Y, corners[i].y() );
            cornerArray.append( cornerObject );
        }
    }
    descript.insert( Carta::Lib::Regions::Polygon::POINTS, cornerArray );
    return descript;
}

void RegionPolygon::_updateStateFromJson( const QJsonObject& json ){
	if ( !json[Carta::Lib::Regions::Polygon::POINTS].isArray() ){
		return;
	}
	QJsonArray corners = json[Carta::Lib::Regions::Polygon::POINTS].toArray();
	int cornerCount = corners.size();
	m_state.resizeArray( Carta::Lib::Regions::Polygon::POINTS, cornerCount );
	for ( int i = 0; i < cornerCount; i++ ){
		QJsonValue cornerValue = corners.at( i );
		if ( cornerValue.isObject() ){
			QJsonObject cornerObj = cornerValue.toObject();
			if ( cornerObj[Util::XCOORD].isDouble() && cornerObj[Util::YCOORD].isDouble() ){
				QPointF corner( cornerObj[Util::XCOORD].toDouble(), cornerObj[Util::YCOORD].toDouble() );
				_insertCorner( i, corner );
			}
		}
	}
}

RegionPolygon::~RegionPolygon(){

}
}
}
