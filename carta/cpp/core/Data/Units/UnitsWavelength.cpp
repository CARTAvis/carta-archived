#include "UnitsWavelength.h"
#include "CartaLib/CartaLib.h"
#include "State/UtilState.h"
#include <QDebug>
#include <cmath>

namespace Carta {

namespace Data {

const QString UnitsWavelength::UNIT_LIST = "units";
const QString UnitsWavelength::CLASS_NAME = "UnitsWavelength";
const QString UnitsWavelength::UNIT_MM = "mm";
const QString UnitsWavelength::UNIT_UM = "um";
const QString UnitsWavelength::UNIT_NM = "nm";
const QString UnitsWavelength::UNIT_ANGSTROM = "Angstrom";

class UnitsWavelength::Factory : public Carta::State::CartaObjectFactory {
public:

    Factory():
        CartaObjectFactory(CLASS_NAME){
    };

    Carta::State::CartaObject * create (const QString & path, const QString & id){
        return new UnitsWavelength (path, id);
    }
};


bool UnitsWavelength::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new UnitsWavelength::Factory());


UnitsWavelength::UnitsWavelength( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){
    _initializeDefaultState();
}


QString UnitsWavelength::getActualUnits( const QString& unitStr ) const {
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


QString UnitsWavelength::getDefault() const {
    return UNIT_MM;
}


void UnitsWavelength::_initUnit( int * index, const QString& unit ){
    QString key = Carta::State::UtilState::getLookup( UNIT_LIST, *index );
    m_state.setValue<QString>( key, unit );
    *index = *index + 1;
}


void UnitsWavelength::_initializeDefaultState(){
    m_state.insertArray( UNIT_LIST, 4 );
    int i = 0;
    _initUnit( &i, UNIT_MM );
    _initUnit( &i, UNIT_UM );
    _initUnit( &i, UNIT_NM );
    _initUnit( &i, UNIT_ANGSTROM );
    m_state.flushState();
}


UnitsWavelength::~UnitsWavelength(){

}
}
}
