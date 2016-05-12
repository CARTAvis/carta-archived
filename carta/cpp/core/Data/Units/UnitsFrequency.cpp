#include "UnitsFrequency.h"
#include "CartaLib/CartaLib.h"
#include "State/UtilState.h"
#include <QDebug>
#include <cmath>

namespace Carta {

namespace Data {

const QString UnitsFrequency::UNIT_LIST = "units";
const QString UnitsFrequency::CLASS_NAME = "UnitsFrequency";
const QString UnitsFrequency::UNIT_HZ = "Hz";
const QString UnitsFrequency::UNIT_MHZ = "MHz";
const QString UnitsFrequency::UNIT_GHZ = "GHz";


class UnitsFrequency::Factory : public Carta::State::CartaObjectFactory {
public:

    Factory():
        CartaObjectFactory(CLASS_NAME){
    };

    Carta::State::CartaObject * create (const QString & path, const QString & id){
        return new UnitsFrequency (path, id);
    }
};


bool UnitsFrequency::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new UnitsFrequency::Factory());


UnitsFrequency::UnitsFrequency( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){
    _initializeDefaultState();
}


QString UnitsFrequency::getActualUnits( const QString& unitStr ) const {
    QString actualUnits;
    int dataCount = m_state.getArraySize( UNIT_LIST );
    for ( int i = 0; i < dataCount; i++ ){
        QString key = Carta::State::UtilState::getLookup( UNIT_LIST, i );
        QString unitName = m_state.getValue<QString>( key );
        int result = QString::compare( unitName, unitStr, Qt::CaseInsensitive );
        if ( result == 0 ){
            actualUnits = unitName;
            break;
        }
    }
    return actualUnits;
}


QString UnitsFrequency::getDefault() const {
    return UNIT_HZ;
}


void UnitsFrequency::_initUnit( int * index, const QString& unit ){
    QString key = Carta::State::UtilState::getLookup( UNIT_LIST, *index );
    m_state.setValue<QString>( key, unit );
    *index = *index + 1;
}


void UnitsFrequency::_initializeDefaultState(){
    m_state.insertArray( UNIT_LIST, 3 );
    int i = 0;
    _initUnit( &i, UNIT_HZ );
    _initUnit( &i, UNIT_MHZ );
    _initUnit( &i, UNIT_GHZ );
    m_state.flushState();
}


UnitsFrequency::~UnitsFrequency(){

}
}
}
