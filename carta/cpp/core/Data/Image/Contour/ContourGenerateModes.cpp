#include "ContourGenerateModes.h"
#include "Data/Util.h"
#include "State/UtilState.h"
#include <QDebug>
#include <set>

namespace Carta {

namespace Data {

const QString ContourGenerateModes::CLASS_NAME = "ContourGenerateModes";
const QString ContourGenerateModes::LIMIT_MODE = "generateModes";
const QString ContourGenerateModes::MODE_RANGE = "Range";
const QString ContourGenerateModes::MODE_MINIMUM = "Minimum";
const QString ContourGenerateModes::MODE_PERCENTILE = "Percentile Range";

class ContourGenerateModes::Factory : public Carta::State::CartaObjectFactory {

    public:

        Factory():
            CartaObjectFactory( CLASS_NAME ){}

        Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new ContourGenerateModes (path, id);
        }
    };


bool ContourGenerateModes::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new ContourGenerateModes::Factory());

ContourGenerateModes::ContourGenerateModes( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){

    m_limitModes.resize( 3 );
    m_limitModes[0] = MODE_RANGE;
    m_limitModes[1] = MODE_MINIMUM;
    m_limitModes[2] = MODE_PERCENTILE;

    _initializeDefaultState();
}

QString ContourGenerateModes::getGenerateMethod( const QString& method ){
    QString actualMethod;
    int methodCount = m_limitModes.size();
    for ( int i = 0; i < methodCount; i++ ){
        if ( QString::compare(method, m_limitModes[i], Qt::CaseInsensitive) == 0 ){
            actualMethod = m_limitModes[i];
            break;
        }
    }
    return actualMethod;
}

QString ContourGenerateModes::getModeDefault() const {
    return MODE_RANGE;
}

void ContourGenerateModes::_initializeDefaultState(){
    int limitCount = m_limitModes.size();
    m_state.insertArray( LIMIT_MODE, limitCount );
    for ( int i = 0; i < limitCount; i++ ){
        QString lookup = Carta::State::UtilState::getLookup( LIMIT_MODE, i );
        m_state.setValue<QString>( lookup, m_limitModes[i] );
    }
    m_state.flushState();
}



ContourGenerateModes::~ContourGenerateModes(){

}
}
}
