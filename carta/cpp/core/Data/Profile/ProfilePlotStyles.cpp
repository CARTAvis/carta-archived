#include "ProfilePlotStyles.h"
#include "Data/Util.h"
#include "State/UtilState.h"


#include <QDebug>

namespace Carta {

namespace Data {

const QString ProfilePlotStyles::CLASS_NAME = "ProfilePlotStyles";
const QString ProfilePlotStyles::PLOT_STYLES = "plotStyles";
const QString ProfilePlotStyles::PLOT_STYLE_LINE = "Line";
const QString ProfilePlotStyles::PLOT_STYLE_STEP = "Step";

class ProfilePlotStyles::Factory : public Carta::State::CartaObjectFactory {
public:
    Factory():
        CartaObjectFactory( CLASS_NAME ){};
    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new ProfilePlotStyles (path, id);
    }
};


bool ProfilePlotStyles::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new ProfilePlotStyles::Factory());


ProfilePlotStyles::ProfilePlotStyles( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){

    _initializeDefaultState();
}


QString ProfilePlotStyles::getActualStyle( const QString& styleStr ) const {
    QString result = "";
    if ( QString::compare( styleStr, PLOT_STYLE_LINE, Qt::CaseInsensitive) == 0 ){
        result = PLOT_STYLE_LINE;
    }
    else if ( QString::compare( styleStr, PLOT_STYLE_STEP, Qt::CaseInsensitive) == 0 ){
        result = PLOT_STYLE_STEP;
    }
    return result;
}


QString ProfilePlotStyles::getDefault() const {
    return PLOT_STYLE_LINE;
}


QStringList ProfilePlotStyles::getProfilePlotStyles() const {
    QStringList buff;
    int styleCount = m_state.getArraySize( PLOT_STYLES);
    for ( int i = 0; i < styleCount; i++ ){
        QString lookup = Carta::State::UtilState::getLookup( PLOT_STYLES, i );
        QString style = m_state.getValue<QString>( PLOT_STYLES );
        buff << style;
    }
    return buff;
}


void ProfilePlotStyles::_initializeDefaultState(){
    m_state.insertArray( PLOT_STYLES, 2 );
    QString lookup0 = Carta::State::UtilState::getLookup(PLOT_STYLES, 0);
    m_state.setValue<QString>(lookup0, PLOT_STYLE_LINE);
    QString lookup1 = Carta::State::UtilState::getLookup(PLOT_STYLES, 1);
    m_state.setValue<QString>(lookup1, PLOT_STYLE_STEP);
    m_state.flushState();
}


ProfilePlotStyles::~ProfilePlotStyles(){

}
}
}
