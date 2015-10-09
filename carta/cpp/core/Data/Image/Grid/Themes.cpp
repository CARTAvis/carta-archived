#include "Themes.h"
#include "Data/Util.h"
#include "State/UtilState.h"
#include <QDebug>
#include <set>

namespace Carta {

namespace Data {

const QString Themes::CLASS_NAME = "Themes";
const QString Themes::THEMES = "themes";

class Themes::Factory : public Carta::State::CartaObjectFactory {

    public:

        Factory():
            CartaObjectFactory( CLASS_NAME ){};

        Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new Themes (path, id);
        }
    };


bool Themes::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new Themes::Factory());


Themes::Themes( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){

    m_themes.append( "Light on Dark");

    _initializeDefaultState();
    _initializeCallbacks();
}

QString Themes::getDefaultTheme() const {
    QString name;
    if ( m_themes.size() > 0 ){
        name = m_themes[0];
    }
    return name;
}

QString Themes::getTheme( const QString& theme ) const {
    QString actualTheme;
    int themeCount = m_themes.size();
    for ( int i = 0; i < themeCount; i++ ){
        int compareResult = QString::compare( theme, m_themes[i], Qt::CaseInsensitive);
        if ( compareResult == 0 ){
            actualTheme = m_themes[i];
            break;
        }
    }
    return actualTheme;
}

QStringList Themes::getThemes() const {
    return m_themes;
}

int Themes::getIndex( const QString& themeName ) const {
   return m_themes.indexOf( themeName );
}

void Themes::_initializeCallbacks(){
    addCommandCallback( "getThemes", [=] (const QString & /*cmd*/,
                    const QString & /*params*/, const QString & /*sessionId*/) -> QString {
            QStringList themeList = getThemes();
            QString result = themeList.join(",");
            return result;
        });
}

void Themes::_initializeDefaultState(){
    int themeCount = m_themes.size();
    m_state.insertArray( THEMES, themeCount );
    for ( int i = 0; i < themeCount; i++ ){
        QString lookup = Carta::State::UtilState::getLookup( THEMES, i );
        m_state.setValue<QString>( lookup, m_themes[i] );
    }

    m_state.flushState();
}

Themes::~Themes(){

}
}
}
