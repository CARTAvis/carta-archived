#include "LayerCompositionModes.h"

#include "Data/Util.h"
#include "State/UtilState.h"


#include <QDebug>
#include <set>

namespace Carta {

namespace Data {

const QString LayerCompositionModes::CLASS_NAME = "LayerCompositionModes";
const QString LayerCompositionModes::MODES = "modes";
const QString LayerCompositionModes::NONE = "None";
const QString LayerCompositionModes::PLUS = "Plus";
const QString LayerCompositionModes::ALPHA = "Alpha";


class LayerCompositionModes::Factory : public Carta::State::CartaObjectFactory {

    public:

        Factory():
            CartaObjectFactory( CLASS_NAME ){};

        Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new LayerCompositionModes (path, id);
        }
    };


bool LayerCompositionModes::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new LayerCompositionModes::Factory());


LayerCompositionModes::LayerCompositionModes( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){

    _initializeDefaultState();
    //_initializeCallbacks();
}


QStringList LayerCompositionModes::getLayerCompositionModes() const {
    QStringList buff;
    int modeCount = m_modes.size();
    for ( int i = 0; i < modeCount; i++ ){
        buff << m_modes[i];
    }
    return buff;
}

QString LayerCompositionModes::getDefault() const {
    return NONE;
}

void LayerCompositionModes::_initializeDefaultState(){
    m_modes.push_back(NONE);
    m_modes.push_back(ALPHA);
    m_modes.push_back(PLUS);


    int modeCount = m_modes.size();
    m_state.insertArray( MODES, modeCount );
    for ( int i = 0; i < modeCount; i++ ){
        QString arrayIndexStr = Carta::State::UtilState::getLookup(MODES, QString::number(i));
        m_state.setValue<QString>(arrayIndexStr, m_modes[i]);
    }
    m_state.flushState();
}


bool LayerCompositionModes::isAlphaSupport( const QString& modeName ) const {
    bool alphaSupport = false;
    QString actualModeName;
    bool actualMode = isCompositionMode( modeName, actualModeName );
    if ( actualMode ){
        if ( actualModeName == PLUS || actualModeName == ALPHA ){
            alphaSupport = true;
        }
    }
    return alphaSupport;
}

bool LayerCompositionModes::isColorSupport( const QString& modeName ) const {
    bool colorSupport = false;
    QString actualModeName;
    bool actualMode = isCompositionMode( modeName, actualModeName );
    if ( actualMode ){
        if ( actualModeName == PLUS ){
            colorSupport = true;
        }
    }
    return colorSupport;
}

bool LayerCompositionModes::isCompositionMode( const QString& name, QString& actualName ) const {
    int modeCount = m_modes.size();
    bool validMode = false;
    for ( int i = 0; i < modeCount; i++ ){
        int result = QString::compare( name, m_modes[i], Qt::CaseInsensitive );
        if ( result == 0 ){
           actualName = m_modes[i];
           validMode = true;
           break;
        }
    }
    return validMode;
}


LayerCompositionModes::~LayerCompositionModes(){

}
}
}
