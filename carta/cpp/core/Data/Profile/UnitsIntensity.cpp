#include "UnitsIntensity.h"
#include "CartaLib/CartaLib.h"
#include "State/UtilState.h"
#include <QDebug>
#include <cmath>

namespace Carta {

namespace Data {

const QString UnitsIntensity::UNIT_LIST = "units";
const QString UnitsIntensity::CLASS_NAME = "UnitsIntensity";
const QString UnitsIntensity::NAME_PEAK = "Fraction of Peak";
const QString UnitsIntensity::NAME_JYBEAM = "Jy/beam";
const QString UnitsIntensity::NAME_JYSR = "MJy/sr";
const QString UnitsIntensity::NAME_JYARCSEC = "Jy/arcsec^2";
const QString UnitsIntensity::NAME_JY = "Jy";
const QString UnitsIntensity::NAME_KELVIN = "Kelvin";


class UnitsIntensity::Factory : public Carta::State::CartaObjectFactory {
public:

    Factory():
        CartaObjectFactory(CLASS_NAME){
    };

    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new UnitsIntensity (path, id);
    }
};



bool UnitsIntensity::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new UnitsIntensity::Factory());

UnitsIntensity::UnitsIntensity( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){
    _initializeDefaultState();
}

QString UnitsIntensity::getActualUnits( const QString& unitStr ) const {
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


QString UnitsIntensity::getDefault() const {
    return NAME_JYBEAM;
}

void UnitsIntensity::_initUnit( int * index, const QString& name ){
    QString key = Carta::State::UtilState::getLookup( UNIT_LIST, *index );
    m_state.setValue<QString>( key, name );
    *index = *index + 1;
}

void UnitsIntensity::_initializeDefaultState(){
    m_state.insertArray( UNIT_LIST, 5 );
    int i = 0;

    _initUnit( &i, NAME_JYBEAM );
    _initUnit( &i, NAME_JYSR );
    _initUnit( &i, NAME_JYARCSEC );
    _initUnit( &i, NAME_JY );
    _initUnit( &i, NAME_KELVIN );

    m_state.flushState();
}


UnitsIntensity::~UnitsIntensity(){

}
}
}
