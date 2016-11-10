
 /* CartaObjectManager.cc
 *
 *  Created on: Oct 3, 2014
 *      Author: jjacobs
 */

#include "ObjectManager.h"
#include "Globals.h"
#include "UtilState.h"
#include "CartaLib/IRemoteVGView.h"
#include <QDebug>
#include <cassert>
#include <set>

using namespace std;

namespace Carta {

namespace State {

QList<QString> CartaObjectFactory::globalIds = {"ChannelUnits",
        "Clips", "Colormaps","ContourGenerateModes","ContourSpacingModes","ContourStyles",
        "CoordinateSystems","DataLoader","ErrorManager",
        "Gamma","GenerateModes","Fonts",
        "LabelFormats","Layout","LayerCompositionModes","LineStyles",
         "PlotStyles", "ProfilePlotStyles",
         "Preferences", "PreferencesSave","ProfileStatistics",
         "RegionTypes", "TransformsImage","TransformsData",
         "Themes",
         "UnitsFrequency","UnitsIntensity","UnitsSpectral","UnitsWavelength",
         "ViewManager"};

QString CartaObject::addIdToCommand (const QString & command) const {
    QString fullCommand = m_path;
    if ( command.size() > 0 ){
        fullCommand = fullCommand + CommandDelimiter + command;
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

void CartaObject::refreshState(){
    m_state.refreshState();
}

int CartaObject::getIndex() const {
    return m_state.getValue<int>(StateInterface::INDEX);
}

QString CartaObject::getSnapType(CartaObject::SnapshotType /*snapType*/) const {
    return m_className;
}

QString CartaObject::getType() const {
    return m_className;
}

void CartaObject::setIndex( int index ){
    CARTA_ASSERT( index >= 0 );
    int oldIndex = m_state.getValue<int>( StateInterface::INDEX );
    if ( oldIndex != index ){
        m_state.setValue<int>(StateInterface::INDEX, index );
        m_state.flushState();
    }
}

void CartaObject::resetState( const QString& state, SnapshotType type ){
    //Make sure the index does not get overwritten, if we are doing
    //a global restore.

    if ( type == SNAPSHOT_DATA){
        resetStateData( state );
    }
    else if ( type == SNAPSHOT_PREFERENCES ){
        int index = getIndex();
        resetState( state );
        setIndex( index );
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
    conn()-> addCommandCallback ( addIdToCommand( rawCommand ), callback);
}


int64_t CartaObject::addStateCallback( const QString& statePath, const IConnector::StateChangedCallback & cb)
{
    return conn()-> addStateCallback( statePath, cb );
}

void CartaObject::registerView( IView * view)
{
    conn()-> registerView( view );
}

void CartaObject::refreshView( IView* view )
{
    conn()-> refreshView( view );
}

void CartaObject::unregisterView()
{
    conn()-> unregisterView( m_path +"/view" );
}

Carta::Lib::LayeredViewArbitrary* CartaObject::makeRemoteView( const QString& path ){
	return new Carta::Lib::LayeredViewArbitrary( conn(), path, NULL );
}

QString CartaObject::getStateLocation( const QString& name ) const
{
    return conn()-> getStateLocation( name );
}

QString
CartaObject::removeId (const QString & commandAndId)
{
    // Command should have the ID as a prefix.
    CARTA_ASSERT(commandAndId.count (CommandDelimiter) == 1);

    // return command without the ID prefix
    return commandAndId.section (CommandDelimiter, 1);
}

IConnector *
CartaObject::conn() {
    static IConnector * conn = Globals::instance()-> connector();
    return conn;
}

CartaObject::~CartaObject () {
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    objMan->removeObject( getId() );
};

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
            //Try to assign by index and matching type.  Note:  May want to remove the assigning by id.
            int targetIndex = obj->getIndex();
            QString targetType = obj->getSnapType( snapType );
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
                //qDebug() << "Unable to restore "<<targetType<<" snapType="<<snapType;
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

void ObjectManager::printObjects(){
    for(map<QString,ObjectRegistryEntry>::iterator it = m_objects.begin(); it != m_objects.end(); ++it) {
        QString firstId = it->first;
        QString classId = it->second.getClassName();
        qDebug() << "id="<<firstId<<" class="<<classId;
    }
}

CartaObject* ObjectManager::removeObject( const QString& id ){
    CartaObject * object = getObject (id);
    if ( object ){
        m_objects.erase (id);
    }
    return object;
}

QString
ObjectManager::destroyObject (const QString & id)
{
    CartaObject* object = removeObject( id );
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
        if ( obj->getIndex() == index && typeStr == obj->getSnapType()){
            target = obj;
            break;
        }
    }
    return target;
}

void
ObjectManager::initialize()
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
    static ObjectManager om;
    return &om;
}

ObjectManager::~ObjectManager (){

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
