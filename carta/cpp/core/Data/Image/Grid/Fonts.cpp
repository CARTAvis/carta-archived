#include "Fonts.h"
#include "Data/Util.h"
#include "State/UtilState.h"
#include "CartaLib/Hooks/GetWcsGridRenderer.h"
#include "Globals.h"
#include <QDebug>
#include <set>

namespace Carta {

namespace Data {

const QString Fonts::CLASS_NAME = "Fonts";
const QString Fonts::FONT_FAMILY = "family";
const QString Fonts::FONT_SIZE = "size";
const QString Fonts::FONT_SIZE_MIN = "fontSizeMin";
const QString Fonts::FONT_SIZE_MAX = "fontSizeMax";

class Fonts::Factory : public Carta::State::CartaObjectFactory {

    public:

        Factory():
            CartaObjectFactory( CLASS_NAME ){};

        Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new Fonts (path, id);
        }
    };


bool Fonts::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new Fonts::Factory());


Fonts::Fonts( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){

    m_fontFamilies.resize( 3 );
    m_fontFamilies[0] = "Helvetica";
    m_fontFamilies[1] = "Times New Roman";
    m_fontFamilies[2] = "Courier New";



    //Find the renderer so we can get the list of fonts (once the code is written!!!!)
    auto res = Globals::instance()-> pluginManager()
                   -> prepare < Carta::Lib::Hooks::GetWcsGridRendererHook > ().first();
    if ( res.isNull() || ! res.val() ) {
        qWarning( "wcsgrid: Could not find any WCS grid renderers" );
    }
    else {
        const std::shared_ptr<Carta::Lib::IWcsGridRenderService> renderer = res.val();
    }
    _initializeDefaultState();
}

QString Fonts::getDefaultFamily() const {
    QString family;
    if ( m_fontFamilies.size() > 0 ){
        family = m_fontFamilies[0];
    }
    return family;
}

int Fonts::getDefaultSize() const {
    int fontSize = 12;
    return fontSize;
}

QString Fonts::getFontFamily( const QString& fontFamily ) const {
    QString actualFont;
    int familyCount = m_fontFamilies.size();
    for ( int i = 0; i < familyCount; i++ ){
        int compareResult = QString::compare( fontFamily, m_fontFamilies[i], Qt::CaseInsensitive);
        if ( compareResult == 0 ){
            actualFont = m_fontFamilies[i];
            break;
        }
    }
    return actualFont;
}


QStringList Fonts::getFontFamilies() const {
    QStringList buff;
    int fontCount = m_fontFamilies.size();
    for ( int i = 0; i < fontCount; i++ ){
        buff.append( m_fontFamilies[i] );
    }
    return buff;
}

int Fonts::getIndex( const QString& familyName ) const {
    QStringList families = getFontFamilies();
    int index = families.indexOf( familyName );
    return index;
}



void Fonts::_initializeDefaultState(){
    int fontCount = m_fontFamilies.size();
    m_state.insertArray( FONT_FAMILY, fontCount );
    for ( int i = 0; i < fontCount; i++ ){
        QString lookup = Carta::State::UtilState::getLookup( FONT_FAMILY, i );
        m_state.setValue<QString>( lookup, m_fontFamilies[i] );
    }

    m_state.insertValue<int>(FONT_SIZE_MIN, 0 );
    m_state.insertValue<int>(FONT_SIZE_MAX, 20 );
    m_state.flushState();
}

bool Fonts::isFontSize( int fontSize ) const {
    bool validSize = false;
    if ( fontSize >= m_state.getValue<int>(FONT_SIZE_MIN) ){
        if ( fontSize <= m_state.getValue<int>(FONT_SIZE_MAX )){
            validSize = true;
        }
    }
    return validSize;
}




Fonts::~Fonts(){

}
}
}
