#include "CartaLib/CartaLib.h"
#include "State/UtilState.h"
#include <QDebug>
#include <cmath>
#include "UnitsSpectral.h"
#include "UnitsFrequency.h"
#include "UnitsWavelength.h"

namespace Carta {

namespace Data {

const QString UnitsSpectral::UNIT_LIST = "units";
const QString UnitsSpectral::CLASS_NAME = "UnitsSpectral";
const QString UnitsSpectral::NAME_VELOCITY_RADIO = "Radio Velocity";
const QString UnitsSpectral::NAME_VELOCITY_OPTICAL = "Optical Velocity";
const QString UnitsSpectral::NAME_FREQUENCY = "Frequency";
const QString UnitsSpectral::NAME_WAVELENGTH = "Wavelength";
const QString UnitsSpectral::NAME_WAVELENGTH_OPTICAL = "Air Wavelength";
const QString UnitsSpectral::NAME_CHANNEL = "Channel";
const QString UnitsSpectral::UNIT_MS = "m/s";
const QString UnitsSpectral::UNIT_KMS = "km/s";


class UnitsSpectral::Factory : public Carta::State::CartaObjectFactory {
public:

    Factory():
        CartaObjectFactory(CLASS_NAME){
    };

    Carta::State::CartaObject * create (const QString & path, const QString & id){
        return new UnitsSpectral (path, id);
    }
};


bool UnitsSpectral::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new UnitsSpectral::Factory());


UnitsSpectral::UnitsSpectral( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){
    _initializeDefaultState();
}


QString UnitsSpectral::getActualUnits( const QString& unitStr ) const {
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


QString UnitsSpectral::getDefault() const {
    return NAME_CHANNEL;
    // TODO: The default unit had better change to use RADIO_VELOCITY
    // It looks that casa::PixelValueManipulator cannot directly use "km/s" to calculate.
    // return NAME_VELOCITY_RADIO + "(" + UNIT_KMS + ")";
}


void UnitsSpectral::_initUnit( int * index, const QString& name, const QString& unit ){
    QString key = Carta::State::UtilState::getLookup( UNIT_LIST, *index );
    QString value = name;
    if ( unit.length() > 0 ){
        value = value + "("+unit+")";
    }
    m_state.setValue<QString>( key, value );
    *index = *index + 1;
}


void UnitsSpectral::_initializeDefaultState(){
    m_state.insertArray( UNIT_LIST, 16 );
    int i = 0;
    _initUnit( &i, NAME_VELOCITY_RADIO, UNIT_MS );
    _initUnit( &i, NAME_VELOCITY_RADIO, UNIT_KMS );
    _initUnit( &i, NAME_VELOCITY_OPTICAL, UNIT_MS );
    _initUnit( &i, NAME_VELOCITY_OPTICAL, UNIT_KMS );
    _initUnit( &i, NAME_FREQUENCY, UnitsFrequency::UNIT_HZ );
    _initUnit( &i, NAME_FREQUENCY, UnitsFrequency::UNIT_MHZ );
    _initUnit( &i, NAME_FREQUENCY, UnitsFrequency::UNIT_GHZ );
    _initUnit( &i, NAME_WAVELENGTH, UnitsWavelength::UNIT_MM );
    _initUnit( &i, NAME_WAVELENGTH, UnitsWavelength::UNIT_UM );
    _initUnit( &i, NAME_WAVELENGTH, UnitsWavelength::UNIT_NM );
    _initUnit( &i, NAME_WAVELENGTH, UnitsWavelength::UNIT_ANGSTROM );
    _initUnit( &i, NAME_WAVELENGTH_OPTICAL, UnitsWavelength::UNIT_MM );
    _initUnit( &i, NAME_WAVELENGTH_OPTICAL, UnitsWavelength::UNIT_UM );
    _initUnit( &i, NAME_WAVELENGTH_OPTICAL, UnitsWavelength::UNIT_NM );
    _initUnit( &i, NAME_WAVELENGTH_OPTICAL, UnitsWavelength::UNIT_ANGSTROM );
    _initUnit( &i, NAME_CHANNEL, "" );
    m_state.flushState();
}


UnitsSpectral::~UnitsSpectral(){

}
}
}
