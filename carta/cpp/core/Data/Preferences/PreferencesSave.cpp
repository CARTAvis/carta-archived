#include "Data/Preferences/PreferencesSave.h"
#include "Data/Util.h"
#include "State/UtilState.h"
#include <QDebug>

namespace Carta {

namespace Data {


const QString PreferencesSave::CLASS_NAME = "PreferencesSave";
const QString PreferencesSave::ASPECT_KEEP = "Keep";
const QString PreferencesSave::ASPECT_EXPAND = "Expand";
const QString PreferencesSave::ASPECT_IGNORE = "Ignore";
const QString PreferencesSave::ASPECT_RATIO_MODE = "aspectMode";

class PreferencesSave::Factory : public Carta::State::CartaObjectFactory {
    public:

        Factory():
            CartaObjectFactory(CLASS_NAME){
        };

        Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new PreferencesSave (path, id);
        }
    };



bool PreferencesSave::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new PreferencesSave::Factory());

PreferencesSave::PreferencesSave( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){
    _initializeDefaultState();
    _initializeCallbacks();
}

Qt::AspectRatioMode PreferencesSave::getAspectRatioMode() const {
    QString aspectStr =  m_state.getValue<QString>(ASPECT_RATIO_MODE);
    Qt::AspectRatioMode mode = Qt::IgnoreAspectRatio;
    if ( aspectStr == ASPECT_KEEP ){
        mode = Qt::KeepAspectRatio;
    }
    else if ( aspectStr == ASPECT_EXPAND ){
        mode = Qt::KeepAspectRatioByExpanding;
    }
    return mode;
}

int PreferencesSave::getHeight() const {
    return m_state.getValue<int>( Util::HEIGHT );
}

int PreferencesSave::getWidth() const {
    return m_state.getValue<int>( Util::WIDTH);
}

QString PreferencesSave::getStateString( const QString& /*sessionId*/, SnapshotType type ) const{
    QString result("");
    if ( type == SNAPSHOT_PREFERENCES ){
        result = m_state.toString();
    }
    return result;
}

void PreferencesSave::_initializeDefaultState(){
    m_state.insertValue<QString>( ASPECT_RATIO_MODE, ASPECT_KEEP );
    m_state.insertValue<int>( Util::WIDTH, 400 );
    m_state.insertValue<int>( Util::HEIGHT, 500 );
    m_state.flushState();
}

void PreferencesSave::_initializeCallbacks(){
    addCommandCallback( "setAspectRatioMode", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
               std::set<QString> keys = {ASPECT_RATIO_MODE};
               std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
               QString result =  setAspectRatioMode( dataValues[ASPECT_RATIO_MODE] );
               Util::commandPostProcess( result );
               return result;
        });


    addCommandCallback( "setWidth", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) -> QString {
                   std::set<QString> keys = {Util::WIDTH};
                   std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                   bool validInt = false;
                   int width = dataValues[Util::WIDTH].toInt(&validInt);
                   QString result;
                   if ( validInt ){
                       setWidth( width );
                   }
                   else {
                       result = "Default image save width must be an integer : " + dataValues[Util::WIDTH];
                   }
                   Util::commandPostProcess( result );
                   return result;
            });

    addCommandCallback( "setHeight", [=] (const QString & /*cmd*/,
                            const QString & params, const QString & /*sessionId*/) -> QString {
                       std::set<QString> keys = {Util::HEIGHT};
                       std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                       bool validInt = false;
                       int height = dataValues[Util::HEIGHT].toInt(&validInt);
                       QString result;
                       if ( validInt ){
                           setHeight( height );
                       }
                       else {
                           result = "Default image save height must be an integer : " + dataValues[Util::HEIGHT];
                       }
                       Util::commandPostProcess( result );
                       return result;
                });

}


QString PreferencesSave::setAspectRatioMode( const QString& mode  ){
    QString result;
    QString actualMode;
    if ( QString::compare( mode, ASPECT_KEEP, Qt::CaseInsensitive ) == 0 ){
        actualMode = ASPECT_KEEP;
    }
    else if (QString::compare( mode, ASPECT_IGNORE, Qt::CaseInsensitive ) == 0 ){
        actualMode = ASPECT_IGNORE;
    }
    else if (QString::compare( mode, ASPECT_EXPAND, Qt::CaseInsensitive ) == 0 ){
        actualMode = ASPECT_EXPAND;
    }
    if ( actualMode.isEmpty()){
        result = "Unrecognized save image aspect mode: "+ mode;
    }
    else if ( m_state.getValue<QString>( ASPECT_RATIO_MODE ) != actualMode ){
        m_state.setValue<QString>( ASPECT_RATIO_MODE, actualMode );
        m_state.flushState();
    }
    return result;
}

QString PreferencesSave::_setDimension( int dim, const QString& key ){
    QString result;
    if ( dim > 0 ){
        int oldDim = m_state.getValue<int>(key);
        if ( oldDim != dim ){
            m_state.setValue<int>(key, dim );
            m_state.flushState();
        }
    }
    else {
        result = "Invalid save image "+key+": "+dim;
    }
    return result;
}


QString PreferencesSave::setWidth( int width ){
    QString result = _setDimension( width, Util::WIDTH );
    return result;
}

QString PreferencesSave::setHeight( int height ){
    QString result = _setDimension( height, Util::HEIGHT );
    return result;
}


PreferencesSave::~PreferencesSave(){

}
}
}
