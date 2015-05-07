
 /* CartaObjectManager.cc
 *
 *  Created on: Oct 3, 2014
 *      Author: jjacobs
 */

#include "ObjectManager.h"
#include "Globals.h"
#include "UtilState.h"
#include <QDebug>
#include <cassert>
#include <set>

using namespace std;

namespace Carta {

namespace State {

QList<QString> CartaObjectFactory::globalIds = {"AnimationTypes","ChannelUnits",
        "Clips", "Colormaps","DataLoader","TransformsImage","TransformsData",
        "ErrorManager","Layout","Preferences","ViewManager"};

QString CartaObject::addIdToCommand (const QString & command) const {
    QString fullCommand = m_path;
    if ( command.size() > 0 ){
        fullCommand = fullCommand + m_Delimiter + command;
    }
    return fullCommand;
}

QString CartaObject::getStateString( const QString& /*sessionId*/, SnapshotType /*type*/ ) const {
    return "";
}



QString
CartaObject::getClassName () const
{
    return m_className;
}

QString
CartaObject::getId () const
{
    return m_id;
}

QString
CartaObject::getPath () const
{
    return m_path;
}

CartaObject::CartaObject (const QString & className,
                          const QString & path,
                          const QString & id)
:
  m_state( path, className ),
  m_className (className),
  m_id (id),
  m_path (path){
    }

int CartaObject::getIndex() const {
    return m_state.getValue<int>(StateInterface::INDEX);
}

QString CartaObject::getType(CartaObject::SnapshotType /*snapType*/) const {
    return m_className;
}

void CartaObject::setIndex( int index ){
    CARTA_ASSERT( index >= 0 );
    m_state.setValue<int>(StateInterface::INDEX, index );
}

void CartaObject::resetState( const QString& state, SnapshotType type ){
    if ( type == SNAPSHOT_DATA){
        resetStateData( state );
    }
    else if ( type == SNAPSHOT_PREFERENCES ){
        resetState( state );
    }
    else {
        qDebug() << "Unsupport resetState type="<<type;
    }
}

void CartaObject::resetState( const QString& state ){
    m_state.setState( state );
    m_state.flushState();
}

void CartaObject::resetStateData( const QString& /*state*/ ){
}

void
CartaObject::addCommandCallback (const QString & rawCommand, IConnector::CommandCallback callback)
{
    IConnector * connector = Globals::instance()->connector();
    QString actualCmd = addIdToCommand( rawCommand );
    connector->addCommandCallback ( actualCmd, callback);
}


int64_t CartaObject::addStateCallback( const QString& statePath, const IConnector::StateChangedCallback & cb){
    IConnector * connector = Globals::instance()->connector();
    return connector->addStateCallback( statePath, cb );
}

void CartaObject::registerView( IView * view){
    IConnector * connector = Globals::instance()->connector();
    connector->registerView( view );
}

void CartaObject::refreshView( IView* view ){
    IConnector * connector = Globals::instance()->connector();
    connector->refreshView( view );
}

void CartaObject::unregisterView(){
    IConnector * connector = Globals::instance()->connector();
    QString viewId = m_path +"/view";
    connector->unregisterView( viewId );
}

QString CartaObject::getStateLocation( const QString& name ) const {
    IConnector * connector = Globals::instance()->connector();
    return connector->getStateLocation( name );
}

QString
CartaObject::removeId (const QString & commandAndId){
    // Command should have the ID as a prefix.  Strip off
    // that part plus the delimiter and return the rest.

    assert (commandAndId.count (m_Delimiter) == 1);

    return commandAndId.section (m_Delimiter, 1);
}



const QString ObjectManager::CreateObject = "CreateObject";
const QString ObjectManager::ClassName = "ClassName";
const QString ObjectManager::DestroyObject = "DestroyObject";

const QString ObjectManager::STATE_ARRAY = "states";
const QString ObjectManager::STATE_ID = "id";
const QString ObjectManager::STATE_VALUE = "state";

ObjectManager::ObjectManager ()
:       m_root( "CartaObjects"),
        m_sep( "/"),
    m_nextId (0){

}

QString ObjectManager::getRootPath() const {
    return  m_sep + m_root;
}

QString ObjectManager::getRoot() const {
    return m_root;
}

bool ObjectManager::restoreSnapshot(const QString stateStr, CartaObject::SnapshotType snapType ) const {
    bool stateRestored = false;
    if ( !stateStr.isEmpty() && stateStr.length() > 0 ){
        StateInterface state("");
        state.setState( stateStr );
        int stateCount = state.getArraySize( STATE_ARRAY );
        for(map<QString,ObjectRegistryEntry>::const_iterator it = m_objects.begin(); it != m_objects.end(); ++it) {
            CartaObject* obj = it->second.getObject();

            //Try to assign by (row,col) and matching type.  Note:  May want to remove the assigning by id.
            int targetIndex = obj->getIndex();
            QString targetType = obj->getType( snapType );
            bool restored = false;
            for ( int j = 0; j < stateCount; j++ ){
                QString stateLookup = UtilState::getLookup( STATE_ARRAY, j );
                int objIndex = state.getValue<int>( UtilState::getLookup(stateLookup, StateInterface::INDEX ) );
                QString objType = state.getValue<QString>( UtilState::getLookup(stateLookup, StateInterface::OBJECT_TYPE ));
                if ( objType == targetType && objIndex == targetIndex ){
                    restored = true;
                    QString stateVal = state.toString( stateLookup );
                    obj->resetState( stateVal, snapType );
                    break;
                }
            }

            if ( !restored ){
                //We lower our standard and just use the first object with matching type, assuming
                //we can find one.
                for ( int j = 0; j < stateCount; j++ ){
                    QString stateLookup = UtilState::getLookup( STATE_ARRAY, j );
                    QString typeLookup = UtilState::getLookup(stateLookup,StateInterface::OBJECT_TYPE);
                    QString objType = state.getValue<QString>( typeLookup );
                    if ( objType == targetType ){
                        restored = true;
                        QString stateVal = state.toString( stateLookup );
                        obj->resetState( stateVal, snapType );
                        break;
                    }
                }
            }
            if ( !restored ){
                qDebug() << "Unable to restore "<<targetType<<" snapType="<<snapType;
            }
        }
        stateRestored = true;
    }
    return stateRestored;
}


QString ObjectManager::getStateString( const QString& sessionId, const QString& rootName, CartaObject::SnapshotType type ) const {
    StateInterface state( rootName );
    int stateCount = m_objects.size();
    state.insertArray( STATE_ARRAY, stateCount );
    int arrayIndex = 0;
    //Create an array of object with each object having an id and state.
    for(map<QString,ObjectRegistryEntry>::const_iterator it = m_objects.begin(); it != m_objects.end(); ++it) {
        CartaObject* obj = it->second.getObject();
        QString objState = obj->getStateString( sessionId, type );
        if ( !objState.isEmpty() && objState.trimmed().length() > 0){
           QString lookup = UtilState::getLookup(STATE_ARRAY, arrayIndex );
           state.setObject( lookup, objState );
           arrayIndex++;
        }
    }
    //Because some of the objects may not support a snapshot of type,
    //the original array that was created may be too large.  We resize
    //it according to actual contents.
    state.resizeArray( STATE_ARRAY, arrayIndex, StateInterface::PreserveAll );
    return state.toString();
}


QString ObjectManager::parseId( const QString& path ) const {
    QString basePath = m_sep + m_root + m_sep;
    int rootIndex = path.indexOf( basePath );
    QString id;
    if ( rootIndex >= 0 ){
        id = path.right( path.length() - basePath.length());
    }
    return id;
}

QString
ObjectManager::createObject (const QString & className)
{
    // This shouldn't be called until the PW State has been initialized.

    ClassRegistry::iterator i = m_classes.find (className);

    QString result; // empty string on failure

    if (i != m_classes.end()){

        // Generate the object's id and path
        // Create the object
        CartaObjectFactory* factory = i->second.getFactory();
        QString id = factory->getGlobalId();
        if ( id.length() == 0 ){
            m_nextId ++;
            id = "c"+QString::number( m_nextId);
        }
        QString path (m_sep + m_root + m_sep + id);


        CartaObject* object = factory->create( path, id );
        //CartaObject * object = i->second.getFactory() (path, id);

        assert (object != 0);

        // Install the newly created object in the object registry.

        assert (m_objects.find (id) == m_objects.end());
        m_objects [id] = ObjectRegistryEntry (className, id, path, object);

        result = id;

    }

    return result;
}

QString
ObjectManager::destroyObject (const QString & id)
{
    CartaObject * object = getObject (id);

    assert (object != 0);

    m_objects.erase (id);

    delete object;

    return "";
}



CartaObject *
ObjectManager::getObject (const QString & id)
{
    ObjectRegistry::iterator i = m_objects.find (id);

    CartaObject * result = 0;

    if (i != m_objects.end()){
        result = i->second.getObject();
    }

    return result;
}

CartaObject* ObjectManager::getObject( int index, const QString & typeStr ){
    CartaObject* target = nullptr;

    for( ObjectRegistry::iterator i = m_objects.begin(); i != m_objects.end(); ++i){
        CartaObject* obj = i->second.getObject();
        if ( obj->getIndex() == index && typeStr == obj->getType()){
            target = obj;
            break;
        }
    }
    return target;
}

void
ObjectManager::initialize ()
{

    // Register command handler(s) for create, etc.

    IConnector * connector = Globals::instance()->connector();
    connector->addCommandCallback (QString (CreateObject), OnCreateObject (this));
    connector->addCommandCallback (QString (DestroyObject), OnDestroyObject (this));

}

ObjectManager *
ObjectManager::objectManager ()
{
    // Implements a singleton pattern

    static ObjectManager * om = new ObjectManager ();

    return om;
}



bool
ObjectManager::registerClass (const QString & className, CartaObjectFactory * factory)
{
    assert (m_classes.find (className) == m_classes.end());

    m_classes [className] = ClassRegistryEntry (className, factory);

    return true; // failure gets "handled" by the assert for now
}

bool ExampleCartaObject::m_registered =
    ObjectManager::objectManager()->registerClass ("edu.nrao.carta.ExampleCartaObject",
                                                   new ExampleCartaObject::Factory());

const QString ExampleCartaObject::DoSomething ("DoSomething");

ExampleCartaObject::ExampleCartaObject (const QString & path, const QString & id)
: CartaObject (path, id, "edu.nrao.carta.ExampleCartaObject")
{
    //addCommandCallback (DoSomething, OnCommand (this, & ExampleCartaObject::doSomething));
    //addCommandCallback (DoSomething, wrapCommandHandler (this, & ExampleCartaObject::doSomething));
}

}
}
