#include "ImageSettings.h"
#include "Data/Util.h"
#include "State/UtilState.h"
#include <QDebug>

namespace Carta {

namespace Data {

const QString ImageSettings::SETTINGS = "settings";
const QString ImageSettings::CLASS_NAME = "ImageSettings";
const QString ImageSettings::VISIBLE = "visible";


class ImageSettings::Factory : public Carta::State::CartaObjectFactory {
    public:

        Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new ImageSettings (path, id);
        }
    };



bool ImageSettings::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new ImageSettings::Factory());

ImageSettings::ImageSettings( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){
    _initializeDefaultState();
    _initializeCallbacks();
}

QString ImageSettings::getStateString( const QString& /*sessionId*/, SnapshotType type ) const{
    QString result("");
    if ( type == SNAPSHOT_PREFERENCES ){
        result = m_state.toString();
    }
    return result;
}

void ImageSettings::_initializeDefaultState(){
    m_state.insertValue<bool>( SETTINGS, true );
    m_state.flushState();
}

void ImageSettings::_initializeCallbacks(){


    addCommandCallback( "setSettingsVisible", [=] (const QString & /*cmd*/,
                                            const QString & params, const QString & /*sessionId*/) -> QString {
               bool visible = false;
               bool validBool = _processParams( params, &visible );
               QString result;
               if ( validBool ){
                   setVisible( visible );
               }
               else {
                   result = "Grid setting visibility must be a bool : " + params;
               }
               Util::commandPostProcess( result );
               return result;
        });


}

bool ImageSettings::_processParams( const QString& params, bool* value ) const {
    bool validValue = false;
    std::set<QString> keys = {VISIBLE};
    std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
    *value = Util::toBool( dataValues[*keys.begin()], &validValue );
    return validValue;
}

void ImageSettings::setVisible( bool visible ){
    if ( m_state.getValue<bool>(SETTINGS) != visible ){
        m_state.setValue<bool>(SETTINGS, visible );
        m_state.flushState();
    }
}


ImageSettings::~ImageSettings(){

}
}
}
