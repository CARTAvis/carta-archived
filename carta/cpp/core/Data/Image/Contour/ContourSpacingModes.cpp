#include "ContourSpacingModes.h"
#include "Data/Util.h"
#include "State/UtilState.h"
#include <QDebug>

#include <set>

namespace Carta {

namespace Data {

const QString ContourSpacingModes::CLASS_NAME = "ContourSpacingModes";
const QString ContourSpacingModes::SPACING_MODE = "spacingModes";
const QString ContourSpacingModes::MODE_LINEAR = "Linear";
const QString ContourSpacingModes::MODE_LOGARITHM = "Logarithmic";
const QString ContourSpacingModes::MODE_PERCENTILE = "Percentile";

class ContourSpacingModes::Factory : public Carta::State::CartaObjectFactory {

    public:

        Factory():
            CartaObjectFactory( CLASS_NAME ){}

        Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new ContourSpacingModes (path, id);
        }
    };


bool ContourSpacingModes::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new ContourSpacingModes::Factory());


ContourSpacingModes::ContourSpacingModes( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){

    m_spacingModes.resize( 3 );
    m_spacingModes[0] = MODE_LINEAR;
    m_spacingModes[1] = MODE_LOGARITHM;
    m_spacingModes[2] = MODE_PERCENTILE;

    _initializeDefaultState();
}

QString ContourSpacingModes::getSpacingMethod( const QString& method ){
    QString actualMethod;
    int methodCount = m_spacingModes.size();
    qDebug()<<"Method count="<<methodCount;
    for ( int i = 0; i < methodCount; i++ ){
        int compareResult = QString::compare(method, m_spacingModes[i], Qt::CaseInsensitive);
        qDebug() << "i="<<i<<" mode="<<m_spacingModes[i]<<" result="<<compareResult;
        if ( compareResult == 0 ){
            actualMethod = m_spacingModes[i];
            break;
        }
    }
    return actualMethod;
}

QString ContourSpacingModes::getModeDefault() const {
    return MODE_LINEAR;
}

void ContourSpacingModes::_initializeDefaultState(){
    int modeCount = m_spacingModes.size();
    m_state.insertArray( SPACING_MODE, modeCount );
    for ( int i = 0; i < modeCount; i++ ){
        QString lookup = Carta::State::UtilState::getLookup( SPACING_MODE, i );
        m_state.setValue<QString>( lookup, m_spacingModes[i] );
    }
    m_state.flushState();
}



ContourSpacingModes::~ContourSpacingModes(){

}
}
}
