#include "LegendLocations.h"
#include "CartaLib/CartaLib.h"
#include "State/UtilState.h"
#include <QDebug>
#include <cmath>

namespace Carta {

namespace Data {

const QString LegendLocations::LOCATION_LIST = "locations";
const QString LegendLocations::CLASS_NAME = "LegendLocations";

const QString LegendLocations::RIGHT = "Right";
const QString LegendLocations::BOTTOM = "Bottom";
const QString LegendLocations::LEFT = "Left";
const QString LegendLocations::TOP = "Top";

const QString LegendLocations::TOP_LEFT = "Top Left";
const QString LegendLocations::TOP_RIGHT = "Top Right";
const QString LegendLocations::BOTTOM_LEFT = "Bottom Left";
const QString LegendLocations::BOTTOM_RIGHT = "Bottom Right";

const int LegendLocations::EXTERNAL_COUNT = 4;
const int LegendLocations::INTERNAL_COUNT = 8;


class LegendLocations::Factory : public Carta::State::CartaObjectFactory {
public:
    Carta::State::CartaObject * create (const QString & path, const QString & id){
        return new LegendLocations (path, id);
    }
};


bool LegendLocations::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new LegendLocations::Factory());

LegendLocations::LegendLocations( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){
    _initializeDefaultState();
}

QString LegendLocations::getActualLocation( const QString& locateStr ) const {
    return _getActual( LOCATION_LIST, locateStr );
}


QString LegendLocations::_getActual( const QString& listName, const QString& locateStr ) const {
    QString actualLocation;
    int dataCount = m_state.getArraySize( listName );
    for ( int i = 0; i < dataCount; i++ ){
        QString key = Carta::State::UtilState::getLookup( listName, i );
        QString locateName = m_state.getValue<QString>( key );
        int result = QString::compare( locateName, locateStr, Qt::CaseInsensitive );
        if ( result == 0 ){
            actualLocation = locateName;
            break;
        }
    }
    return actualLocation;
}


QString LegendLocations::getDefaultLocation( bool external ) const {
    QString loc = BOTTOM;
    if ( !external ){
        loc = TOP_LEFT;
    }
    return loc;
}


void LegendLocations::_initValue( int * index, const QString& listName, const QString& name ){
    QString key = Carta::State::UtilState::getLookup( listName, *index );
    m_state.setValue<QString>( key, name );
    *index = *index + 1;
}


void LegendLocations::_initItems(){
    int i = 0;
    _initValue( &i, LOCATION_LIST, LEFT );
    _initValue( &i, LOCATION_LIST, BOTTOM );
    _initValue( &i, LOCATION_LIST, RIGHT );
    _initValue( &i, LOCATION_LIST, TOP );
    int arraySize = m_state.getArraySize( LOCATION_LIST );
    if ( arraySize > i  ){
        _initValue( &i, LOCATION_LIST, TOP_LEFT );
        _initValue( &i, LOCATION_LIST, TOP_RIGHT );
        _initValue( &i, LOCATION_LIST, BOTTOM_LEFT );
        _initValue( &i, LOCATION_LIST, BOTTOM_RIGHT );
    }
    m_state.flushState();
}


void LegendLocations::_initializeDefaultState(){
    m_state.insertArray( LOCATION_LIST, EXTERNAL_COUNT );
    _initItems();
}


void LegendLocations::setAvailableLocations( bool external ){
    if ( external ){
        m_state.resizeArray( LOCATION_LIST, EXTERNAL_COUNT );
    }
    else {
        m_state.resizeArray( LOCATION_LIST, INTERNAL_COUNT );
    }
    _initItems();
}

LegendLocations::~LegendLocations(){

}
}
}
