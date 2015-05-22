#include "Snapshots.h"
#include "SnapshotsFile.h"
#include "Data/LinkableImpl.h"
#include "Data/ILinkable.h"
#include "Data/Layout.h"
#include "Data/Preferences.h"
#include "Data/Util.h"
#include <QDebug>

namespace Carta {

namespace Data {

const QString Snapshots::SNAPSHOT_SELECTED = "selected";
const QString Snapshots::CLASS_NAME = "Snapshots";
const QString Snapshots::DEFAULT_SNAPSHOT = "session_default";

const QString Snapshots::FILE_NAME = "fileName";
const QString Snapshots::SAVE_LAYOUT = "layoutSnapshot";
const QString Snapshots::SAVE_PREFERENCES = "preferencesSnapshot";
const QString Snapshots::SAVE_DATA = "dataSnapshot";
const QString Snapshots::SAVE_DESCRIPTION = "description";

class Snapshots::Factory : public Carta::State::CartaObjectFactory {
    public:
        Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new Snapshots (path, id);
        }
    };

bool Snapshots::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new Snapshots::Factory());

Snapshots::Snapshots( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ),
    m_snapImpl(nullptr){
    _initializeState();
    _initializeCallbacks();
    m_snapImpl = new SnapshotsFile();
}

QString Snapshots::deleteSnapshot( const QString& sessionId, const QString& saveName ){
    QString result;
    if ( saveName.trimmed().length() > 0 ){
        result = m_snapImpl->deleteSnapshot( sessionId, saveName );
        //Successful deletion
        if ( result.isEmpty()){
            updateSnapshots( sessionId );
            //If we deleted the snapshot that is currently being used, we need to select a new one.
            QString usedSnapshot = m_state.getValue<QString>(SNAPSHOT_SELECTED );
            if ( usedSnapshot == saveName ){
                int snapCount = m_state.getArraySize( CLASS_NAME );
                if ( snapCount > 0 ){
                    QString lookup = Carta::State::UtilState::getLookup( CLASS_NAME, 0 );
                    _saveSelected( m_state.getValue<QString>(lookup ));
                }
                else {
                    _saveSelected( DEFAULT_SNAPSHOT );
                }
            }
        }
    }
    else {
        result = "Please specify a name for the snapshot to save.";
    }
    return result;
}

QList<Snapshot> Snapshots::getSnapshots( const QString& sessionId ) const{
    return m_snapImpl->getSnapshots( sessionId );
}


void Snapshots::_initializeState(){
    m_state.insertValue( SNAPSHOT_SELECTED, DEFAULT_SNAPSHOT );
    m_state.insertArray( CLASS_NAME, 0 );
    m_state.flushState();
}

void Snapshots::_initializeCallbacks(){

    addCommandCallback( "deleteSnapshot", [=] (const QString & /*cmd*/,
                                    const QString & params, const QString & sessionId) -> QString {
                std::set<QString> keys = {FILE_NAME};
                std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                QString saveName=dataValues[FILE_NAME];
                QString result = deleteSnapshot( sessionId, saveName );
                Util::commandPostProcess( result );
                return result;
            });

    //Callback for restoring state.
     addCommandCallback( "restoreSnapshot", [=] (const QString & /*cmd*/,
                 const QString & params, const QString & sessionId) -> QString {
         std::set<QString> keys = {Snapshots::FILE_NAME};
         std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
         QString saveName=dataValues[Snapshots::FILE_NAME];
         QString result = restoreSnapshot( sessionId, saveName );
         Util::commandPostProcess( result );
         return result;
     });

    //Callback for saving state.
    addCommandCallback( "saveSnapshot", [=] (const QString & /*cmd*/,
                const QString & params, const QString & sessionId) -> QString {
        std::set<QString> keys = {Snapshots::FILE_NAME,Snapshots::SAVE_LAYOUT,
                Snapshots::SAVE_PREFERENCES,Snapshots::SAVE_DATA, Snapshots::SAVE_DESCRIPTION};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString result;
        QString saveDescription = dataValues[Snapshots::SAVE_DESCRIPTION];
        bool validLayout = false;
        bool saveLayout = Util::toBool( dataValues[Snapshots::SAVE_LAYOUT], &validLayout );
        if ( !validLayout ){
            result = "Error saving state; save layout should be true/false:"+params;
        }
        else {
            bool validPrefs = false;
            bool savePrefs = Util::toBool( dataValues[Snapshots::SAVE_PREFERENCES], & validPrefs );
            if ( !validPrefs ){
                result = "Error saving state; preferences should be true/false:"+params;
            }
            else {
                bool validData = false;
                bool saveData = Util::toBool( dataValues[Snapshots::SAVE_DATA], &validData );
                if ( !validData ){
                    result = "Error saving state: save data should be true/false:"+params;
                }
                else {
                    result = saveSnapshot(sessionId, dataValues[Snapshots::FILE_NAME],
                           saveLayout, savePrefs, saveData, saveDescription );
                }
            }
        }
        Util::commandPostProcess(result);
        return result;
    });
}

void Snapshots::initializeDefaultState() {
    restoreSnapshot( "", DEFAULT_SNAPSHOT );
}

QString Snapshots::_restoreLayout(const QString& sessionId, const QString& saveName ) const {
    QString msg;
    QString layoutState = m_snapImpl->read( sessionId, CartaObject::SNAPSHOT_LAYOUT, saveName);
    if ( !layoutState.isEmpty() ){

        //First reset the layout
        Carta::State::StateInterface state("");
        state.setState( layoutState );
        Carta::State::CartaObject* obj = Util::findSingletonObject( Layout::CLASS_NAME );
        Layout* layout = dynamic_cast<Layout*>(obj);
        layout->resetState( state );

        //Now go in and reset the links.
        QString stateArrayLookup = Carta::State::ObjectManager::STATE_ARRAY;
        int stateCount = state.getArraySize( stateArrayLookup );
        Carta::State::ObjectManager* objManager = Carta::State::ObjectManager::objectManager();
        for ( int i = 0; i < stateCount; i++ ){
            //Make an object for the source.
            QString stateLookup = UtilState::getLookup( stateArrayLookup, i );
            //Get the index and type of the source and find the corresponding object.
            QString stateIndexLookup = UtilState::getLookup(stateLookup, StateInterface::INDEX );
            int index = state.getValue<int>( stateIndexLookup);
            QString sourceType = state.getValue<QString>(UtilState::getLookup(stateLookup,StateInterface::OBJECT_TYPE ));
            Carta::State::CartaObject* obj = objManager->getObject( index, sourceType );
            ILinkable* linkable = dynamic_cast<ILinkable*>(obj);
            if ( linkable != nullptr ){
                QString linksLookup = Carta::State::UtilState::getLookup( stateLookup, LinkableImpl::LINK );
                int destCount = state.getArraySize( linksLookup );
                for ( int j = 0; j < destCount; j++ ){
                    QString destLookup = UtilState::getLookup( stateLookup,UtilState::getLookup( LinkableImpl::LINK, j ));
                    QString indexLinkLookup = UtilState::getLookup(destLookup, StateInterface::INDEX);
                    QString linkTypeLookup = UtilState::getLookup(destLookup, StateInterface::OBJECT_TYPE);
                    int indexLink = state.getValue<int>( indexLinkLookup );
                    QString linkType = state.getValue<QString>( linkTypeLookup );
                    Carta::State::CartaObject* destObj = objManager->getObject( indexLink, linkType );
                    if ( destObj != nullptr ){
                        linkable->addLink( destObj );
                    }
                }
            }
            else {
                qDebug() << "Null link source!";
            }
        }
    }
    return msg;
}

QString Snapshots::_restoreSnapshot(const QString& sessionId, CartaObject::SnapshotType snapType, const QString& saveName) const {
    QString msg;
    QString stateStr = m_snapImpl->read( sessionId, snapType, saveName );
    Carta::State::ObjectManager* objManager = Carta::State::ObjectManager::objectManager();
    bool snapRestored = objManager->restoreSnapshot( stateStr, snapType );
    //If we did not restore the state && there was a state to restore
    //return an error message.
    if ( !snapRestored && stateStr.length() > 0 ){
        QString snapStr = Snapshot::getNameForType( snapType);
        msg = "Could not restore snapshot "+snapStr;
    }
    return msg;
}


QString Snapshots::restoreSnapshot( const QString& sessionId, const QString& saveName ){
    //Order is important: layout -> preferences -> data
    QString result;
    if ( saveName.trimmed().length() > 0 ){
        QString resultLayout = _restoreLayout( sessionId, saveName );
        QString resultPreferences = _restoreSnapshot( sessionId, CartaObject::SNAPSHOT_PREFERENCES, saveName);
        QString resultData  = _restoreSnapshot( sessionId, CartaObject::SNAPSHOT_DATA, saveName );
        result = resultLayout + resultPreferences+resultData;
    }
    //If we were successful, set the current snapshot name.
    if ( result.isEmpty() || result.trimmed().length() == 0 ){
        _saveSelected( saveName );
    }

    return result;
}

void Snapshots::_saveSelected( const QString& saveName ){
    if ( !saveName.isEmpty() && saveName.trimmed().length() > 0 ){
        QString oldSaveName = m_state.getValue<QString>(SNAPSHOT_SELECTED );
        if ( oldSaveName != saveName ){
            m_state.setValue<QString>(SNAPSHOT_SELECTED, saveName );
            m_state.flushState();
        }
    }
}

QString Snapshots::_saveLayout(const QString& sessionId, const QString& snapName){
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    //Layout state consists of both the layout and the links.
    Carta::State::CartaObject* obj = Util::findSingletonObject( Layout::CLASS_NAME );
    Layout* layout = dynamic_cast<Layout*>(obj);
    QString layoutState = layout->getStateString();
    QString linkState =objMan->getStateString( sessionId, LinkableImpl::LINK, SNAPSHOT_LAYOUT);

    Carta::State::StateInterface state( "");
    state.setState( linkState );
    state.insertObject( Layout::CLASS_NAME, layoutState);
    QString layoutStr = state.toString();
    QString result = m_snapImpl->save( sessionId, Snapshot::DIR_LAYOUT, snapName, layoutStr );
    return result;
}

QString Snapshots::_savePreferences(const QString& sessionId, const QString& snapName){
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    QString prefStr = objMan->getStateString( sessionId, Preferences::CLASS_NAME,SNAPSHOT_PREFERENCES );
    QString result = m_snapImpl->save( sessionId, Snapshot::DIR_PREFERENCES, snapName, prefStr );
    return result;
}

QString Snapshots::_saveData(const QString& sessionId, const QString& baseName){
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    QString dataStr = objMan->getStateString( sessionId, "Data", SNAPSHOT_DATA );
    QString result = m_snapImpl->save( sessionId, Snapshot::DIR_DATA, baseName, dataStr );
    return result;
}

QString Snapshots::_saveDescription( const QString& sessionId, const QString& baseName,
        const QString& description ){
    QString result = m_snapImpl->save( sessionId, "", baseName, description );
    return result;
}

QString Snapshots::saveSnapshot( const QString& sessionId, const QString& saveName,
        bool saveLayout, bool savePreferences, bool saveData, const QString& description ){
    QString result;
    if ( !savePreferences && !saveData && !saveLayout ){
        result = "Please select the type of state to save.";
        return result;
    }
    if ( saveName.isEmpty() || saveName.trimmed().size() == 0 ){
        result = "Please specify a name for the state to save.";
        return result;
    }
    //Because we want to replace the state with the new one, we delete the state if
    //it already exists.
    deleteSnapshot( sessionId, saveName );
    result = _saveDescription( sessionId, saveName, description );
    if ( saveLayout ){
        result = _saveLayout( sessionId, saveName);
    }
    if ( savePreferences ){
        result = _savePreferences( sessionId, saveName);
    }
    if ( saveData ){
        result = _saveData( sessionId, saveName);
    }
    updateSnapshots( sessionId );
    return result;
}


void Snapshots::updateSnapshots( const QString& sessionId ){
    QList<Snapshot> snapshotList = m_snapImpl->getSnapshots( sessionId );
    int count = snapshotList.size();
    bool stateChanged = false;
    int oldCount = m_state.getArraySize( CLASS_NAME );
    if ( oldCount != count ){
        m_state.resizeArray( CLASS_NAME, count, Carta::State::StateInterface::PreserveAll );
        stateChanged = true;
    }
    for ( int i = 0; i < count; i++ ){
        QString lookup = Carta::State::UtilState::getLookup( CLASS_NAME, i );
        QString oldValue = "";
        if ( i < oldCount ){
            oldValue = m_state.getValue<QString>( lookup );
        }
        if ( oldValue != snapshotList[i].getName()){
            m_state.setValue<QString>( lookup, snapshotList[i].toString());
            stateChanged = true;
        }
    }
    if ( stateChanged ){
        m_state.flushState();
    }
}

Snapshots::~Snapshots(){
    delete m_snapImpl;
}
}
}
