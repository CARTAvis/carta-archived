#include "Settings.h"
#include "Data/Util.h"
#include "State/UtilState.h"
#include <QDebug>

namespace Carta {

namespace Data {

const QString Settings::SETTINGS = "settings";
const QString Settings::CLASS_NAME = "Settings";
const QString Settings::VISIBLE = "visible";


class Settings::Factory : public Carta::State::CartaObjectFactory {
    public:

        Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new Settings (path, id);
        }
    };



bool Settings::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new Settings::Factory());

Settings::Settings( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){
    _initializeDefaultState();
    _initializeCallbacks();
}

QString Settings::getStateString( const QString& /*sessionId*/, SnapshotType type ) const{
    QString result("");
    if ( type == SNAPSHOT_PREFERENCES ){
        result = m_state.toString();
    }
    return result;
}

void Settings::_initializeDefaultState(){
    m_state.insertValue<bool>( SETTINGS, false );
    m_state.flushState();
}

void Settings::_initializeCallbacks(){
    addCommandCallback( "setSettingsVisible", [=] (const QString & /*cmd*/,
                                            const QString & params, const QString & /*sessionId*/) -> QString {
               bool visible = false;
               bool validBool = _processParams( params, &visible );
               QString result;
               if ( validBool ){
                   setVisible( visible );
               }
               else {
                   result = "Setting visibility must be a bool : " + params;
               }
               Util::commandPostProcess( result );
               return result;
        });
}

bool Settings::_processParams( const QString& params, bool* value ) const {
    bool validValue = false;
    std::set<QString> keys = {VISIBLE};
    std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
    *value = Util::toBool( dataValues[*keys.begin()], &validValue );
    return validValue;
}

void Settings::resetState( const QString& /*stateStr*/ ){
    //Implemented to do nothing so that the owning object can set the state.
}

void Settings::resetStateString( const QString& stateStr ){
    m_state.setState( stateStr );
    m_state.flushState();
}

void Settings::setVisible( bool visible ){
    if ( m_state.getValue<bool>(SETTINGS) != visible ){
        m_state.setValue<bool>(SETTINGS, visible );
        m_state.flushState();
    }
}


Settings::~Settings(){

}
}
}
