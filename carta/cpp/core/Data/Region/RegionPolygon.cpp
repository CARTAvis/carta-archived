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
    _updateName();
    m_state.flushState();

    m_shape->setModel( toJSON() );
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


bool RegionPolygon::setCenter( const QPointF& pt ){
	std::shared_ptr<Carta::Lib::Regions::RegionBase> rBase = getModel();
	QRectF box = rBase->outlineBox();
	bool centerChanged = false;
	QPointF oldCenter = box.center();
	double xMove = pt.x() - oldCenter.x();
	double yMove = pt.y() - oldCenter.y();

	if ( xMove != 0 || yMove != 0 ){
		//Move all the corners by the indicated amount.
		int cornerCount = m_state.getArraySize( Carta::Lib::Regions::Polygon::POINTS);
		if ( cornerCount > 0 ){
			centerChanged = true;
			for ( int i = 0; i < cornerCount; i++ ){
				QString key = Carta::State::UtilState::getLookup( Carta::Lib::Regions::Polygon::POINTS, i );
				QString xKey = Carta::State::UtilState::getLookup( key, Carta::Lib::Regions::RegionBase::POINT_X);
				QString yKey = Carta::State::UtilState::getLookup( key, Carta::Lib::Regions::RegionBase::POINT_Y);
				double cornerX = m_state.getValue<double>( xKey );
				double cornerY = m_state.getValue<double>( yKey );
				cornerX = cornerX + xMove;
				cornerY = cornerY + yMove;
				m_state.setValue<double>( xKey, cornerX );
				m_state.setValue<double>( yKey, cornerY );
			}
			m_shape->setModel( toJSON() );
			_updateName();
		}
	}
	return centerChanged;
}

bool RegionPolygon::setHeight( double height ){
	CARTA_ASSERT( height >= 0 );
	bool heightChanged = false;
	//Get the old height of the outline box and see if there is a change.
	std::shared_ptr<Carta::Lib::Regions::RegionBase> rBase = getModel();
	QRectF box = rBase->outlineBox();
	double oldHeight = box.height();
	if ( oldHeight != height ){
		QPointF centerPt = box.center();
		int cornerCount = m_state.getArraySize( Carta::Lib::Regions::Polygon::POINTS );
		if ( cornerCount > 0 ){
			heightChanged = true;
			for ( int i = 0; i < cornerCount; i++ ){
				QString key = Carta::State::UtilState::getLookup( Carta::Lib::Regions::Polygon::POINTS, i );
				QString yKey = Carta::State::UtilState::getLookup( key, Carta::Lib::Regions::RegionBase::POINT_Y);
				//Move the points proportionally, depending on their distance from the center.
				double cornerY = m_state.getValue<double>( yKey );
				double centerDist = cornerY  - centerPt.y();
				double newAmount = centerDist / oldHeight * height;
				cornerY = centerPt.y() + newAmount;
				m_state.setValue<double>( yKey, cornerY );
			}
			m_shape->setModel( toJSON() );
			_updateName();
		}
	}
	return heightChanged;
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
		_updateName();
	}
}

bool RegionPolygon::setWidth( double width ){
	CARTA_ASSERT( width >= 0 );
	bool widthChanged = false;
	//Get the old width of the outline box and see if there is a change.
	std::shared_ptr<Carta::Lib::Regions::RegionBase> rBase = getModel();
	QRectF box = rBase->outlineBox();
	double oldWidth = box.width();
	if ( oldWidth != width ){
		//Move the points proportionally, depending on their distance from the
		//center.
		QPointF centerPt = box.center();
		int cornerCount = m_state.getArraySize( Carta::Lib::Regions::Polygon::POINTS );
		if ( cornerCount > 0 ){
			widthChanged = true;
			for ( int i = 0; i < cornerCount; i++ ){
				QString key = Carta::State::UtilState::getLookup( Carta::Lib::Regions::Polygon::POINTS, i );
				QString xKey = Carta::State::UtilState::getLookup( key, Carta::Lib::Regions::RegionBase::POINT_X);

				double cornerX = m_state.getValue<double>( xKey );
				double centerDist = cornerX  - centerPt.x();
				double newAmount = centerDist / oldWidth * width;
				cornerX = centerPt.x() + newAmount;
				m_state.setValue<double>( xKey, cornerX );
			}
			m_shape->setModel( toJSON() );
			_updateName();
		}
	}
	return widthChanged;
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
	_updateName();
	emit regionShapeChanged();
}

RegionPolygon::~RegionPolygon(){

}
}
}
