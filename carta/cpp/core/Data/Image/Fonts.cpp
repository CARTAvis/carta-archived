#include "Data/Image/Fonts.h"
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

    m_fontSizes.resize( 28 );
    for ( int i = 3; i <=30; i++ ){
        m_fontSizes[i-3] = i;
    }


    //Find the renderer so we can get the list of fonts (once the code is written!!!!)
//    auto res = Globals::instance()-> pluginManager()
//                   -> prepare < Carta::Lib::Hooks::GetWcsGridRendererHook > ().first();
//    if ( res.isNull() || ! res.val() ) {
//        qWarning( "wcsgrid: Could not find any WCS grid renderers" );
//    }
//    else {
//        const std::shared_ptr<Carta::Lib::IWcsGridRenderService> renderer = res.val();
//    }
    _initializeDefaultState();
    _initializeCallbacks();
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

QStringList Fonts::getFontSizes() const {
    QStringList buff;
    int fontCount = m_fontSizes.size();
    for ( int i = 0; i < fontCount; i++ ){
        buff.append( QString::number(m_fontSizes[i]) );
    }
    return buff;
}

int Fonts::getIndex( const QString& familyName ) const {
    QStringList families = getFontFamilies();
    int index = families.indexOf( familyName );
    return index;
}

void Fonts::_initializeCallbacks(){
    addCommandCallback( "getFontFamilies", [=] (const QString & /*cmd*/,
                    const QString & /*params*/, const QString & /*sessionId*/) -> QString {
            QStringList fontList = getFontFamilies();
            QString result = fontList.join(",");
            return result;
        });
    addCommandCallback( "getFontSizes", [=] (const QString & /*cmd*/,
                        const QString & /*params*/, const QString & /*sessionId*/) -> QString {
                QStringList fontList = getFontSizes();
                QString result = fontList.join(",");
                return result;
            });
}

void Fonts::_initializeDefaultState(){
    int fontCount = m_fontFamilies.size();
    m_state.insertArray( FONT_FAMILY, fontCount );
    for ( int i = 0; i < fontCount; i++ ){
        QString lookup = Carta::State::UtilState::getLookup( FONT_FAMILY, i );
        m_state.setValue<QString>( lookup, m_fontFamilies[i] );
    }
    int fontSizeCount = m_fontSizes.size();

    m_state.insertArray( FONT_SIZE, fontSizeCount );
    for ( int i = 0; i < fontSizeCount; i++ ){
        QString lookup = Carta::State::UtilState::getLookup( FONT_SIZE, i );
        m_state.setValue<int>( lookup, m_fontSizes[i]);
    }
    m_state.flushState();
}

bool Fonts::isFontSize( int fontSize ) const {
    bool validSize = false;
    if ( fontSize >= 0 ){
        int count = m_fontSizes.size();
        for ( int i = 0; i < count; i++ ){
            if ( fontSize == m_fontSizes[i] ){
                validSize = true;
                break;
            }
        }
    }
    return validSize;
}




Fonts::~Fonts(){

}
}
}
