#include "ImageSettings.h"
#include "Data/Util.h"
#include "State/UtilState.h"
#include <QDebug>

namespace Carta {

namespace Data {


const QString ImageSettings::GRID_CONTROL = "grid";
const QString ImageSettings::GRID_FONT = "gridFont";
const QString ImageSettings::GRID_COLOR = "gridColor";
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
    m_state.insertValue<bool>( GRID_CONTROL, false );
    m_state.insertValue<bool>( GRID_FONT, false );
    m_state.insertValue<bool>( GRID_COLOR, true );
    m_state.flushState();
}

void ImageSettings::_initializeCallbacks(){


    addCommandCallback( "setVisibleGrid", [=] (const QString & /*cmd*/,
                                            const QString & params, const QString & /*sessionId*/) -> QString {
               bool visible = false;
               bool validBool = _processParams( params, &visible );
               QString result;
               if ( validBool ){
                   setVisibleGrid( visible );
               }
               else {
                   result = "Grid setting visibility must be a bool : " + params;
               }
               Util::commandPostProcess( result );
               return result;
        });

    addCommandCallback( "setVisibleGridColor", [=] (const QString & /*cmd*/,
                                                const QString & params, const QString & /*sessionId*/) -> QString {
               bool visible = false;
               bool validBool = _processParams( params, &visible );
               QString result;
               if ( validBool ){
                   setVisibleColor( visible );
               }
               else {
                   result = "Grid color setting visibility must be a bool : " + params;
               }
               Util::commandPostProcess( result );
               return result;
        });

    addCommandCallback( "setVisibleGridFont", [=] (const QString & /*cmd*/,
                                                    const QString & params, const QString & /*sessionId*/) -> QString {
           bool visible = false;
           bool validBool = _processParams( params, &visible );
           QString result;
           if ( validBool ){
               setVisibleFont( visible );
           }
           else {
               result = "Grid font setting visibility must be a bool : " + params;
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

void ImageSettings::_setVisibility( const QString& key, bool visible ){
    if ( m_state.getValue<bool>(key) != visible ){
        m_state.setValue<bool>(key, visible );
        m_state.flushState();
    }
}

void ImageSettings::setVisibleGrid( bool visible ){
    _setVisibility( GRID_CONTROL, visible );
}

void ImageSettings::setVisibleColor( bool visible ){
    _setVisibility( GRID_COLOR, visible );
}

void ImageSettings::setVisibleFont( bool visible ){
    _setVisibility( GRID_FONT, visible );
}

ImageSettings::~ImageSettings(){

}
}
}
