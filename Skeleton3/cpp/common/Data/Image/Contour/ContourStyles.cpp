#include "ContourStyles.h"
#include "Data/Util.h"
#include "State/UtilState.h"

#include <QDebug>
#include <set>

namespace Carta {

namespace Data {

const QString ContourStyles::CLASS_NAME = "ContourStyles";
const QString ContourStyles::LINE_STYLES = "lineStyles";
const QString ContourStyles::LINE_SOLID = "Solid";
const QString ContourStyles::LINE_DASHED = "Dashed";

class ContourStyles::Factory : public Carta::State::CartaObjectFactory {

    public:

        Factory():
            CartaObjectFactory( CLASS_NAME ){};

        Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new ContourStyles (path, id);
        }
    };


bool ContourStyles::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new ContourStyles::Factory());


ContourStyles::ContourStyles( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){

    m_lineStyles.resize( 2 );
    m_lineStyles[0] = LINE_SOLID;
    m_lineStyles[1] = LINE_DASHED;

    _initializeDefaultState();
}

QString ContourStyles::getLineStyle( const QString& lineStyle ){
    QString actualLineStyle;
    int styleCount = m_lineStyles.size();
    for ( int i = 0; i < styleCount; i++ ){
        if ( QString::compare(lineStyle, m_lineStyles[i], Qt::CaseInsensitive) == 0 ){
            actualLineStyle== m_lineStyles[i];
            break;
        }
    }
    return actualLineStyle;
}

QString ContourStyles::getLineStyleDefault() const {
    return LINE_SOLID;
}

void ContourStyles::_initializeDefaultState(){
    int styleCount = m_lineStyles.size();
    m_state.insertArray( LINE_STYLES, styleCount );
    for ( int i = 0; i < styleCount; i++ ){
        QString lookup = Carta::State::UtilState::getLookup( LINE_STYLES, i );
        m_state.setValue<QString>( lookup, m_lineStyles[i] );
    }
    m_state.flushState();
}



ContourStyles::~ContourStyles(){

}
}
}
