#include "Data/ChannelUnits.h"
#include "CartaLib/CartaLib.h"
#include <QDebug>

namespace Carta {

namespace Data {

const QString ChannelUnits::UNIT_LIST = "channelUnitList";
const QString ChannelUnits::CLASS_NAME = "ChannelUnits";
const QString ChannelUnits::UNIT_COUNT = "channelUnitCount";

class ChannelUnits::Factory : public CartaObjectFactory {
    public:

        Factory():
            CartaObjectFactory(CLASS_NAME){
        };

        CartaObject * create (const QString & path, const QString & id)
        {
            return new ChannelUnits (path, id);
        }
    };



bool ChannelUnits::m_registered =
    ObjectManager::objectManager()->registerClass ( CLASS_NAME, new ChannelUnits::Factory());

ChannelUnits::ChannelUnits( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ),
    m_unitList({ "Hz", "GHz", "MHz"}){
    _initializeDefaultState();
}

double ChannelUnits::convert( const QString& oldUnits, const QString& newUnits, double value ) const {
    int sourceIndex = getIndex( oldUnits );
    int destIndex = getIndex( newUnits );
    double converted = _convert( sourceIndex, destIndex, value );
    return converted;
}

double ChannelUnits::_convert( int oldIndex, int newIndex, double value ) const {
    double convertedValue = value;
    if ( oldIndex >= 0 && newIndex >= 0 ){
        int diff = qAbs( newIndex - oldIndex );
        float power = pow( 10, diff * 3 );
        if ( newIndex > oldIndex ){
            power = 1 / power;
        }
        convertedValue = value * power;
    }
    else {
        qWarning() << "Could not convert "<<oldIndex<<" to "<<newIndex;
    }
    return convertedValue;
}

QString ChannelUnits::getDefaultUnit() const {
    return m_unitList[1];
}

void ChannelUnits::_initializeDefaultState(){
    int unitCount = m_unitList.size();
    m_state.insertValue<int>( UNIT_COUNT, unitCount );
    m_state.insertArray( UNIT_LIST, unitCount );
    for ( int i = 0; i < unitCount; i++ ){
        QString arrayIndexStr = UNIT_LIST + StateInterface::DELIMITER + QString::number(i);
        m_state.setValue<QString>(arrayIndexStr, m_unitList[i] );
    }
    m_state.flushState();
}

QString ChannelUnits::getUnit( int index ) const {
    int unitCount = m_state.getValue<int>(UNIT_COUNT);
    QString unit;
    if ( index < 0 || index  >= unitCount ) {
        CARTA_ASSERT( "unit index out of range!" );
    }
    else {
        unit = m_unitList[index];
    }
    return unit;
}


int ChannelUnits::getIndex( const QString& unit ) const {
    int unitIndex = -1;
    int unitCount = m_unitList.size();
    for ( int i = 0; i < unitCount; i++ ){
        if ( m_unitList[i].toLower() == unit.toLower() ){
            unitIndex = i;
            break;
        }
    }
    if ( unitIndex < 0 ){
        qWarning()<< "Invalid channel unit: "<<unit;
    }
    return unitIndex;
}

ChannelUnits::~ChannelUnits(){

}
}
}
