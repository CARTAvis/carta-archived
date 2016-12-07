#include "BinData.h"
#include "Data/Util.h"
#include "State/UtilState.h"

#include <QDebug>


namespace Carta {

namespace Data {

const QString BinData::CLASS_NAME = "BinData";

class BinData::Factory : public Carta::State::CartaObjectFactory  {
public:
    Carta::State::CartaObject * create (const QString & path, const QString & id){
        return new BinData (path, id);
    }
};

bool BinData::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new BinData::Factory());


using Carta::State::UtilState;
using Carta::State::StateInterface;

BinData::BinData( const QString& path, const QString& id):
            CartaObject( CLASS_NAME, path, id ){
    _initializeDefaultState();
}


QColor BinData::getColor() const {
    int red = m_state.getValue<int>( Util::RED );
    int green = m_state.getValue<int>( Util::GREEN );
    int blue = m_state.getValue<int>( Util::BLUE );
    return QColor( red, green, blue );
}

Carta::Lib::Hooks::HistogramResult BinData::getHistogramResult() const {
	return m_result;
}


QString BinData::getName() const {
    return m_state.getValue<QString>( Util::NAME );
}


QString BinData::getStateString() const{
    return m_state.toString();
}


void BinData::_initializeDefaultState(){
    m_state.insertValue<QString>( Util::NAME, "");
    m_state.insertValue<QString>( Util::ID, "");
    m_state.insertValue<int>( Util::RED, 255 );
    m_state.insertValue<int>( Util::GREEN, 0 );
    m_state.insertValue<int>( Util::BLUE, 0 );
}


void BinData::setColor( QColor color ){
    m_state.setValue<int>( Util::RED, color.red() );
    m_state.setValue<int>( Util::GREEN, color.green() );
    m_state.setValue<int>( Util::BLUE, color.blue() );
}


void BinData::setHistogramResult( const Carta::Lib::Hooks::HistogramResult& result  ){
	m_result = result;
}

void BinData::setName( const QString& name ){
	m_state.setValue<QString>( Util::NAME, name );
}

BinData::~BinData(){
}
}
}
