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
const QString ContourStyles::LINE_DASHED = "Dash";
const QString ContourStyles::LINE_DOTTED = "Dot";
const QString ContourStyles::LINE_DASHED_DOTTED = "Dash-Dot";

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

    m_lineStyles.resize( 4 );
    m_lineStyles[0] = LINE_SOLID;
    m_lineStyles[1] = LINE_DASHED;
    m_lineStyles[2] = LINE_DOTTED;
    m_lineStyles[3] = LINE_DASHED_DOTTED;

    _initializeDefaultState();
}

QString ContourStyles::getLineStyle( const QString& lineStyle ){
    QString actualLineStyle;
    int styleCount = m_lineStyles.size();
    for ( int i = 0; i < styleCount; i++ ){
        int compareResult = QString::compare(lineStyle, m_lineStyles[i], Qt::CaseInsensitive);
        if ( compareResult == 0 ){
            actualLineStyle = m_lineStyles[i];
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
