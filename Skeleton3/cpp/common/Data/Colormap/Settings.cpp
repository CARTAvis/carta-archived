#include "Settings.h"
#include "Data/Util.h"
#include "State/UtilState.h"
#include <QDebug>

namespace Carta {

namespace Data {


const QString Settings::CLASS_NAME = "Settings";
const QString Settings::COLOR_MIX = "colorMix";
const QString Settings::COLOR_MODEL = "colorModel";
const QString Settings::COLOR_SCALE = "colorScale";
const QString Settings::COLOR_TRANSFORM = "colorTransform";

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
    m_state.insertValue<bool>( COLOR_MIX, false );
    m_state.insertValue<bool>( COLOR_MODEL, false );
    m_state.insertValue<bool>( COLOR_TRANSFORM, false );
    m_state.insertValue<bool>( COLOR_SCALE, false );

    m_state.flushState();
}

void Settings::_initializeCallbacks(){


    addCommandCallback( "setVisibleColorMix", [=] (const QString & /*cmd*/,
                                            const QString & params, const QString & /*sessionId*/) -> QString {
               bool visible = false;
               bool validBool = _processParams( params, &visible );
               QString result;
               if ( validBool ){
                   setVisibleColorMix( visible );
               }
               else {
                   result = "Color mix setting visibility must be a bool : " + params;
               }
               Util::commandPostProcess( result );
               return result;
        });

    addCommandCallback( "setVisibleColorScale", [=] (const QString & /*cmd*/,
                                                const QString & params, const QString & /*sessionId*/) -> QString {
               bool visible = false;
               bool validBool = _processParams( params, &visible );
               QString result;
               if ( validBool ){
                   setVisibleColorScale( visible );
               }
               else {
                   result = "Color scale setting visibility must be a bool : " + params;
               }
               Util::commandPostProcess( result );
               return result;
        });

    addCommandCallback( "setVisibleColorTransform", [=] (const QString & /*cmd*/,
                                                    const QString & params, const QString & /*sessionId*/) -> QString {
           bool visible = false;
           bool validBool = _processParams( params, &visible );
           QString result;
           if ( validBool ){
               setVisibleColorTransform( visible );
           }
           else {
               result = "Color scale setting visibility must be a bool : " + params;
           }
           Util::commandPostProcess( result );
           return result;
    });

    addCommandCallback( "setVisibleColorModel", [=] (const QString & /*cmd*/,
                                                        const QString & params, const QString & /*sessionId*/) -> QString {
           bool visible = false;
           bool validBool = _processParams( params, &visible );
           QString result;
           if ( validBool ){
               setVisibleColorModel( visible );
           }
           else {
               result = "Color scale setting visibility must be a bool : " + params;
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

void Settings::_setVisibility( const QString& key, bool visible ){
    if ( m_state.getValue<bool>(key) != visible ){
        m_state.setValue<bool>(key, visible );
        m_state.flushState();
    }
}

void Settings::setVisibleColorMix( bool visible ){
    _setVisibility( COLOR_MIX, visible );
}

void Settings::setVisibleColorScale( bool visible ){
    _setVisibility( COLOR_SCALE, visible );
}

void Settings::setVisibleColorTransform( bool visible ){
    _setVisibility( COLOR_TRANSFORM, visible );
}

void Settings::setVisibleColorModel( bool visible ){
    _setVisibility( COLOR_MODEL, visible );
}

Settings::~Settings(){

}
}
}
