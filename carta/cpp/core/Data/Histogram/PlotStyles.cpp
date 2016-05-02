#include "PlotStyles.h"
#include "Data/Util.h"
#include "State/UtilState.h"


#include <QDebug>

namespace Carta {

namespace Data {

const QString PlotStyles::CLASS_NAME = "PlotStyles";
const QString PlotStyles::PLOT_STYLES = "plotStyles";
const QString PlotStyles::PLOT_STYLE_LINE = "Line";
const QString PlotStyles::PLOT_STYLE_OUTLINE = "Outline";
const QString PlotStyles::PLOT_STYLE_FILL = "Fill";

class PlotStyles::Factory : public Carta::State::CartaObjectFactory {
public:
    Factory():
        CartaObjectFactory( CLASS_NAME ){};
    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new PlotStyles (path, id);
    }
};


bool PlotStyles::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new PlotStyles::Factory());


PlotStyles::PlotStyles( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){

    _initializeDefaultState();
    _initializeCallbacks();
}


QString PlotStyles::getActualPlotStyle( const QString& styleStr ) const {
    QString result = "";
    if ( QString::compare( styleStr, PLOT_STYLE_LINE, Qt::CaseInsensitive) == 0 ){
        result = PLOT_STYLE_LINE;
    }
    else if ( QString::compare( styleStr, PLOT_STYLE_OUTLINE, Qt::CaseInsensitive) == 0 ){
        result = PLOT_STYLE_OUTLINE;
    }
    else if ( QString::compare( styleStr, PLOT_STYLE_FILL, Qt::CaseInsensitive) == 0 ){
        result = PLOT_STYLE_FILL;
    }
    return result;
}


QString PlotStyles::getDefault() const {
    return PLOT_STYLE_LINE;
}


QStringList PlotStyles::getPlotStyles() const {
    QStringList buff;
    int styleCount = m_state.getArraySize( PLOT_STYLES);
    for ( int i = 0; i < styleCount; i++ ){
        QString lookup = Carta::State::UtilState::getLookup( PLOT_STYLES, i );
        QString style = m_state.getValue<QString>( PLOT_STYLES );
        buff << style;
    }
    return buff;
}


void PlotStyles::_initializeCallbacks(){
    addCommandCallback( "getPlotStyles", [=] (const QString & /*cmd*/,
                    const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        QStringList styleList = getPlotStyles();
        QString result = styleList.join( ",");
        return result;
     });
}


void PlotStyles::_initializeDefaultState(){
    m_state.insertArray( PLOT_STYLES, 3 );
    QString lookup0 = Carta::State::UtilState::getLookup(PLOT_STYLES, 0);
    m_state.setValue<QString>(lookup0, PLOT_STYLE_LINE);
    QString lookup1 = Carta::State::UtilState::getLookup(PLOT_STYLES, 1);
    m_state.setValue<QString>(lookup1, PLOT_STYLE_OUTLINE);
    QString lookup2 = Carta::State::UtilState::getLookup(PLOT_STYLES, 2);
    m_state.setValue<QString>(lookup2, PLOT_STYLE_FILL);
    m_state.flushState();
}


PlotStyles::~PlotStyles(){

}
}
}
