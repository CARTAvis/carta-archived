#include "Data/Preferences.h"
#include "Data/Util.h"
#include "State/UtilState.h"
#include <QDebug>

namespace Carta {

namespace Data {


const QString Preferences::CLASS_NAME = "Preferences";
const QString Preferences::SHOW_MENU = "menuVisible";
const QString Preferences::SHOW_TOOLBAR = "toolBarVisible";
const QString Preferences::SHOW_STATUS = "statusVisible";

class Preferences::Factory : public Carta::State::CartaObjectFactory {
    public:

        Factory():
            CartaObjectFactory(CLASS_NAME){
        };

        Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new Preferences (path, id);
        }
    };



bool Preferences::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new Preferences::Factory());

Preferences::Preferences( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){
    _initializeDefaultState();
    _initializeCallbacks();
}

QString Preferences::getStateString( const QString& /*sessionId*/, SnapshotType type ) const{
    QString result("");
    if ( type == SNAPSHOT_PREFERENCES ){
        result = m_state.toString();
    }
    return result;
}

void Preferences::_initializeDefaultState(){
    m_state.insertValue<bool>( SHOW_MENU, true );
    m_state.insertValue<bool>( SHOW_TOOLBAR, true );
    m_state.insertValue<bool>( SHOW_STATUS, true );
    m_state.flushState();
}

void Preferences::_initializeCallbacks(){
    addCommandCallback( "setMenuVisible", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
               std::set<QString> keys = {SHOW_MENU};
               std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
               bool validBool = false;
               bool visible = Util::toBool( dataValues[*keys.begin()], &validBool );
               QString result;
               if ( validBool ){
                   setMenuVisibility( visible );
               }
               else {
                   result = "Menu visibility must be a bool : " + dataValues[0];
               }
               Util::commandPostProcess( result );
               return result;
        });

    addCommandCallback( "setToolBarVisible", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) -> QString {
                   std::set<QString> keys = {SHOW_TOOLBAR};
                   std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                   bool validBool = false;
                   bool visible = Util::toBool( dataValues[*keys.begin()], &validBool );
                   QString result;
                   if ( validBool ){
                       setToolbarVisibility( visible );
                   }
                   else {
                       result = "Toolbar visibility must be a bool : " + dataValues[0];
                   }
                   Util::commandPostProcess( result );
                   return result;
            });

    addCommandCallback( "setStatusVisible", [=] (const QString & /*cmd*/,
                            const QString & params, const QString & /*sessionId*/) -> QString {
       std::set<QString> keys = {SHOW_STATUS};
       std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
       bool validBool = false;
       bool visible = Util::toBool( dataValues[*keys.begin()], &validBool );
       QString result;
       if ( validBool ){
           setStatusVisibility( visible );
       }
       else {
           result = "Status bar visibility must be a bool : " + dataValues[0];
       }
       Util::commandPostProcess( result );
       return result;
});
}

void Preferences::setMenuVisibility( bool visible ){
    if ( m_state.getValue<bool>(SHOW_MENU) != visible ){
        m_state.setValue<bool>(SHOW_MENU, visible );
        m_state.flushState();
    }
}

void Preferences::setStatusVisibility( bool visible ){
    bool oldVisible = m_state.getValue<bool>(SHOW_STATUS);
    if ( oldVisible != visible ){
        m_state.setValue<bool>(SHOW_STATUS, visible );
        m_state.flushState();
    }
}

void Preferences::setToolbarVisibility( bool visible ){
    if ( m_state.getValue<bool>(SHOW_TOOLBAR) != visible ){
        m_state.setValue<bool>(SHOW_TOOLBAR, visible );
        m_state.flushState();
    }
}
Preferences::~Preferences(){

}
}
}
