#include "RegionTypes.h"
#include "Data/Util.h"
#include "State/UtilState.h"

#include <QDebug>
//#include <set>

namespace Carta {

namespace Data {

const QString RegionTypes::CLASS_NAME = "RegionTypes";
const QString RegionTypes::ELLIPSE = "Ellipse";
const QString RegionTypes::POLYGON = "Polygon";
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

QString RegionTypes::getDefault() const {
	return POLYGON;
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


void RegionTypes::_initializeDefaultState(){
    int regionCount = 2;
    m_state.insertArray( TYPES, regionCount );
    _insertType( POLYGON, 0 );
    _insertType( ELLIPSE, 1 );
    m_state.flushState();
}

void RegionTypes::_insertType( const QString& regionType, int index ){
	QString arrayIndexStr = Carta::State::UtilState::getLookup( TYPES, index );
	m_state.setValue<QString>( arrayIndexStr, regionType );
}



bool RegionTypes::isRegionType( const QString& name, QString& actualName ) const {
    int modeCount = m_state.getArraySize( TYPES );
    bool validMode = false;
    for ( int i = 0; i < modeCount; i++ ){
    	QString arrayIndexStr = Carta::State::UtilState::getLookup( TYPES, i );
    	QString typeStr = m_state.getValue<QString>( arrayIndexStr );
        int result = QString::compare( name, typeStr, Qt::CaseInsensitive );
        if ( result == 0 ){
           actualName = typeStr;
           validMode = true;
           break;
        }
    }
    return validMode;
}


RegionTypes::~RegionTypes(){

}
}
}
