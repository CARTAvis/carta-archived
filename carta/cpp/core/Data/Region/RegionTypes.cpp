#include "RegionTypes.h"
#include "Data/Util.h"
#include "State/UtilState.h"
#include "CartaLib/Regions/IRegion.h"
#include "CartaLib/Regions/Ellipse.h"
#include "CartaLib/Regions/Point.h"
#include "CartaLib/Regions/Rectangle.h"

#include <QDebug>

namespace Carta {

namespace Data {

const QString RegionTypes::CLASS_NAME = "RegionTypes";
const QString RegionTypes::ELLIPSE = "Ellipse";
const QString RegionTypes::POLYGON = "Polygon";
const QString RegionTypes::RECTANGLE = "Rectangle";
const QString RegionTypes::POINT = "Point";
const QString RegionTypes::TYPES = "types";


class RegionTypes::Factory : public Carta::State::CartaObjectFactory {

    public:

        Factory():
            CartaObjectFactory( CLASS_NAME ){};

        Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new RegionTypes (path, id);
        }
    };


bool RegionTypes::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new RegionTypes::Factory());


RegionTypes::RegionTypes( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){

    _initializeDefaultState();
}

QString RegionTypes::getActualType( const QString& name ) const {
    int modeCount = m_state.getArraySize( TYPES );
    QString actualType;
    for ( int i = 0; i < modeCount; i++ ){
    	QString arrayIndexStr = Carta::State::UtilState::getLookup( TYPES, i );
    	QString typeStr = m_state.getValue<QString>( arrayIndexStr );
        int result = QString::compare( name, typeStr, Qt::CaseInsensitive );
        if ( result == 0 ){
           actualType = typeStr;
           break;
        }
    }
    return actualType;
}

QString RegionTypes::getDefault() const {
	return RECTANGLE;
}

QStringList RegionTypes::getRegionTypes() const {
    QStringList buff;
    int typeCount = m_state.getArraySize( TYPES );
    for ( int i = 0; i < typeCount; i++ ){
    	QString lookup = Carta::State::UtilState::getLookup( TYPES, i );
    	buff << m_state.getValue<QString>( lookup );
    }
    return buff;
}

QString RegionTypes::getModelType( const QString& userType ) const {
	QString modelType;
	QString actualType = getActualType( userType );
	if ( !actualType.isEmpty() ){
		if ( actualType == RECTANGLE ){
			modelType = Carta::Lib::Regions::Rectangle::TypeName;
		}
		else if ( actualType == POLYGON ){
			modelType = Carta::Lib::Regions::Polygon::TypeName;
		}
		else if ( actualType == POINT ){
			modelType = Carta::Lib::Regions::Point::TypeName;
		}
		else if ( actualType == ELLIPSE ){
			modelType = Carta::Lib::Regions::Ellipse::TypeName;
		}
		else {
			CARTA_ASSERT( false );
		}
	}
	return modelType;
}


void RegionTypes::_initializeDefaultState(){
    int regionCount = 4;
    m_state.insertArray( TYPES, regionCount );
    int i = 0;
    _insertType( RECTANGLE, i );
    i++;
    _insertType( POLYGON, i );
    i++;
    _insertType( ELLIPSE, i );
    i++;
    _insertType( POINT, i );
    m_state.flushState();
}

void RegionTypes::_insertType( const QString& regionType, int index ){
	QString arrayIndexStr = Carta::State::UtilState::getLookup( TYPES, index );
	m_state.setValue<QString>( arrayIndexStr, regionType );
}

RegionTypes::~RegionTypes(){

}
}
}
