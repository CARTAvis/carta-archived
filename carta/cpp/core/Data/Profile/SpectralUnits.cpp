#include "SpectralUnits.h"
#include "CartaLib/CartaLib.h"
#include "State/UtilState.h"
#include <QDebug>
#include <cmath>

namespace Carta {

namespace Data {

const QString SpectralUnits::UNIT_LIST = "spectralUnits";
const QString SpectralUnits::CLASS_NAME = "SpectralUnits";
const QString SpectralUnits::NAME_VELOCITY_RADIO = "Radio Velocity";
const QString SpectralUnits::NAME_VELOCITY_OPTICAL = "Optical Velocity";
const QString SpectralUnits::NAME_FREQUENCY = "Frequency";
const QString SpectralUnits::NAME_WAVELENGTH = "Wavelength";
const QString SpectralUnits::NAME_CHANNEL = "Channel";
const QString SpectralUnits::UNIT_MS = "m/s";
const QString SpectralUnits::UNIT_KMS = "km/s";
const QString SpectralUnits::UNIT_HZ = "Hz";
const QString SpectralUnits::UNIT_MHZ = "MHz";
const QString SpectralUnits::UNIT_GHZ = "GHz";
const QString SpectralUnits::UNIT_MM = "mm";
const QString SpectralUnits::UNIT_UM = "um";
const QString SpectralUnits::UNIT_NM = "nm";
const QString SpectralUnits::UNIT_ANGSTROM = "Angstrom";

class SpectralUnits::Factory : public Carta::State::CartaObjectFactory {
public:

    Factory():
        CartaObjectFactory(CLASS_NAME){
    };

    Carta::State::CartaObject * create (const QString & path, const QString & id){
        return new SpectralUnits (path, id);
    }
};


bool SpectralUnits::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new SpectralUnits::Factory());


SpectralUnits::SpectralUnits( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){
    _initializeDefaultState();
}


QString SpectralUnits::getActualUnits( const QString& unitStr ) const {
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


QString SpectralUnits::getDefault() const {
    return NAME_CHANNEL;
}


void SpectralUnits::_initUnit( int * index, const QString& name, const QString& unit ){
    QString key = Carta::State::UtilState::getLookup( UNIT_LIST, *index );
    QString value = name;
    if ( unit.length() > 0 ){
        value = value + "("+unit+")";
    }
    m_state.setValue<QString>( key, value );
    *index = *index + 1;
}


void SpectralUnits::_initializeDefaultState(){
    m_state.insertArray( UNIT_LIST, 13 );
    int i = 0;
    _initUnit( &i, NAME_VELOCITY_RADIO, UNIT_MS );
    _initUnit( &i, NAME_VELOCITY_RADIO, UNIT_KMS );
    _initUnit( &i, NAME_VELOCITY_OPTICAL, UNIT_MS );
    _initUnit( &i, NAME_VELOCITY_OPTICAL, UNIT_KMS );
    _initUnit( &i, NAME_FREQUENCY, UNIT_HZ );
    _initUnit( &i, NAME_FREQUENCY, UNIT_MHZ );
    _initUnit( &i, NAME_FREQUENCY, UNIT_GHZ );
    _initUnit( &i, NAME_FREQUENCY, UNIT_HZ );
    _initUnit( &i, NAME_WAVELENGTH, UNIT_MM );
    _initUnit( &i, NAME_WAVELENGTH, UNIT_UM );
    _initUnit( &i, NAME_WAVELENGTH, UNIT_NM );
    _initUnit( &i, NAME_WAVELENGTH, UNIT_ANGSTROM );
    _initUnit( &i, NAME_CHANNEL, "" );
    m_state.flushState();
}


SpectralUnits::~SpectralUnits(){

}
}
}
