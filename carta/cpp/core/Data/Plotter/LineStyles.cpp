#include "LineStyles.h"
#include "Data/Util.h"
#include "State/UtilState.h"


#include <QDebug>

namespace Carta {

namespace Data {

const QString LineStyles::CLASS_NAME = "LineStyles";
const QString LineStyles::LINE_STYLES = "lineStyles";
const QString LineStyles::LINE_STYLE_SOLID = "Solid";
const QString LineStyles::LINE_STYLE_DASH = "Dashed";
const QString LineStyles::LINE_STYLE_DOT = "Dot";
const QString LineStyles::LINE_STYLE_DASHDOT = "DashDot";
const QString LineStyles::LINE_STYLE_DASHDOTDOT = "DashDotDot";


class LineStyles::Factory : public Carta::State::CartaObjectFactory {
public:
    Factory():
        CartaObjectFactory( CLASS_NAME ){};
    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new LineStyles (path, id);
    }
};


bool LineStyles::m_registered =
    Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new LineStyles::Factory());


LineStyles::LineStyles( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){
    _initializeDefaultState();
}


QString LineStyles::getActualLineStyle( const QString& styleStr ) const {
    QString result = "";
    if ( QString::compare( styleStr, LINE_STYLE_DASH, Qt::CaseInsensitive) == 0 ){
        result = LINE_STYLE_DASH;
    }
    else if ( QString::compare( styleStr, LINE_STYLE_DOT, Qt::CaseInsensitive) == 0 ){
        result = LINE_STYLE_DOT;
    }
    else if ( QString::compare( styleStr, LINE_STYLE_DASHDOT, Qt::CaseInsensitive) == 0 ){
        result = LINE_STYLE_DASHDOT;
    }
    else if ( QString::compare( styleStr, LINE_STYLE_SOLID, Qt::CaseInsensitive) == 0 ){
        result = LINE_STYLE_SOLID;
    }
    else if ( QString::compare( styleStr, LINE_STYLE_DASHDOTDOT, Qt::CaseInsensitive) == 0 ){
        result = LINE_STYLE_DASHDOTDOT;
    }
    return result;
}


QString LineStyles::getDefault() const {
    return LINE_STYLE_SOLID;
}

QString LineStyles::getDefaultSecondary() const {
    return LINE_STYLE_DASH;
}

QStringList LineStyles::getLineStyles() const {
    QStringList buff;
    int styleCount = m_state.getArraySize( LINE_STYLES);
    for ( int i = 0; i < styleCount; i++ ){
        QString lookup = Carta::State::UtilState::getLookup( LINE_STYLES, i );
        QString style = m_state.getValue<QString>( lookup);
        buff << style;
    }
    return buff;
}



void LineStyles::_initValue( int * index, const QString& name ){
    QString key = Carta::State::UtilState::getLookup( LINE_STYLES, *index );
    m_state.setValue<QString>( key, name );
    *index = *index + 1;
}


void LineStyles::_initializeDefaultState(){
    m_state.insertArray( LINE_STYLES, 5 );
    int i = 0;
    _initValue( &i, LINE_STYLE_SOLID );
    _initValue( &i, LINE_STYLE_DOT );
    _initValue( &i, LINE_STYLE_DASH );
    _initValue( &i, LINE_STYLE_DASHDOT );
    _initValue( &i, LINE_STYLE_DASHDOTDOT );
    m_state.flushState();
}


LineStyles::~LineStyles(){

}
}
}
