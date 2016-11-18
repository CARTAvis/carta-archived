/*
 * ObjectManager.h
 *
 *  Created on: Oct 3, 2014
 *      Author: jjacobs
 */

#pragma once

#include <map>
#include <QString>
#include <QTextStream>
#include "StateInterface.h"
#include "../IConnector.h"

namespace Carta {
    namespace Lib {
        class LayeredViewArbitrary;
    }
}

namespace Carta {

namespace State {

class CartaObject {

public:

    virtual ~CartaObject ();


    QString addIdToCommand (const QString & commandName) const;
    //Snapshots of state that can be saved.
    typedef enum SnapshotType { SNAPSHOT_INFO, SNAPSHOT_DATA, SNAPSHOT_PREFERENCES, SNAPSHOT_LAYOUT } SnapshotType;

    /**
     * Returns a json representation of this object's state.
     * @param sessionId - an identifier for a user's session.
     * @param type an identifier for the type of state to be returned.
     * @return a QString representing this object's state.
     */
    virtual QString getStateString( const QString& sessionId = "", SnapshotType type = SNAPSHOT_INFO ) const;
    QString getClassName () const;
    QString getId () const;
    QString getPath () const;

    virtual void refreshState();

    /**
     * Reset the state of this object.
     * @param state a QString representing a new state for this object.
     */
    void resetState( const QString& state, SnapshotType type );

    /**
     * Reset the data state of this object.
     * @param state a QString representing the data state of the object.
     */
    //By default; does nothing.  Override for objects containing a data state.
    virtual void resetStateData( const QString& state );

    /**
     * Reset the user preferences for this object.
     * @param state - the user preferences for the object.
     */
    virtual void resetState( const QString& state );

    /**
     * Return the index of the object (withen its type).
     * @return index the index of the object.
     */
    int getIndex() const;

    /**
     * Set the index of the object.
     * @param index - the index of the object withen its type.
     */
    void setIndex( int index );

    /**
     * Return the type of the object.
     *
     * Normally the type of an object will be the class name, but for snapshots such as session snapshots
     * this method may be overriden to append an additional identifier such as 'data' after the class name.
     *
     * @param snapType the type of Snapshot.
     * @return an identifier for the type of the object.
     */
    virtual QString getSnapType(CartaObject::SnapshotType snapType= CartaObject::SnapshotType::SNAPSHOT_INFO) const;

    QString getType() const;

protected:

    CartaObject (const QString & className,
            const QString & path,
            const QString & id);

    void addCommandCallback (const QString & command, IConnector::CommandCallback);

    int64_t addStateCallback( const QString& statePath, const IConnector::StateChangedCallback &);

    /// asks the connector to schedule a redraw of the view
    void refreshView( IView * view);

    /// registers a view with the connector
    void registerView( IView * view);

    /// unregister a view with the connector
    void unregisterView();

    /**
     * Construct a layered view and return it.
     * @param path - a unique identifier for the remote view.
     */
    Carta::Lib::LayeredViewArbitrary* makeRemoteView( const QString& path );

    //Return the full location for the state with the given name.
    QString getStateLocation( const QString& name ) const;


    QString removeId (const QString & commandAndId);

    template <typename Object, typename Method>
    class OnCommand {
        public:
            OnCommand (Object * object, Method method)
                : m_method (method), m_object (object)
                {}

            QString operator() (const QString & commandAndId, const QString & parameters,
                const QString & sessionID){
                QString command = m_object->removeId (commandAndId);
                return (m_object ->* m_method) (command, parameters, sessionID);
            }
        private:

            Method m_method;
            Object * m_object;
    };

    /// helper to get connector
    static IConnector * conn();

protected:
    StateInterface m_state;

private:

    QString m_className;
    QString m_id;
    QString m_path;

    static const char CommandDelimiter = ':';

};

class CartaObjectFactory {

public:

    CartaObjectFactory( const QString& globalId=""):
        m_globalId( globalId ){}
    QString getGlobalId(){
        QString globalId;
        if ( globalIds.contains( m_globalId)){
            globalId = m_globalId;
        }
        return globalId;
    }


    virtual ~CartaObjectFactory (){}

    virtual CartaObject * create (const QString & path, const QString & id) = 0;


private:
    static QList<QString> globalIds;
    QString m_globalId;
};

class ObjectManager {

public:

    ~ObjectManager ();

    /**
     * Create an object of the given class.
     * @param className - the class name of the object.
     * @return the object that was created.
     */
    template<typename T>
    T* createObject (const QString & className){
        // This shouldn't be called until the PW State has been initialized.

        ClassRegistry::iterator i = m_classes.find ( className );
        T* result = nullptr; // nullptr on failure
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

            assert (object != 0);

            // Install the newly created object in the object registry.
            assert (m_objects.find (id) == m_objects.end());
            m_objects [id] = ObjectRegistryEntry ( className, id, path, object);

            result = static_cast<T*>( object);
        }
        return result;
    }

    /**
     * Create an object of the given type.
     * @return the object that was created.
     */
    template <typename T>
    T* createObject(){
        return createObject<T>( T::CLASS_NAME );
    }

    /**
     * Destroy the object with the given identifier.
     * @param id - the unique server side identifier for the object to destroy.
     * @return an string of zero length.
     */
    QString destroyObject (const QString & id);

    /**
     * Remove the object from storage without destroying it.
     * @param id - the unique server-side identifier for the object to remove.
     * @return a point to the object that was removed.
     */
    CartaObject* removeObject( const QString& id );

    /**
     * Return the object with the given identifier or a nullptr if none exists.
     * @param id - an identifier for a CartaObject.
     * @return the corresponding CartaObject or a nullptr if none exists.
     */
    CartaObject * getObject (const QString & id);

    /**
     * Return the object of the given type an index if one exists; otherwise return a nullptr.
     * @param index - the index of the object withen its type.
     * @param typeStr - an identifier for the type of object.
     * @return the corresponding CartaObject or a nullptr if none exists.
     */
    CartaObject* getObject( int index, const QString & typeStr );

    /**
     * Returns a string containing the state of all managed objects as JSON array of strings.
     * @param sessionId - an identifier for a user's session.
     * @param snapName - the name of the snapshot.
     * @param type - the type of state needed.
     * @return a QString containing the entire state of managed objects.
     */
    QString getStateString( const QString& sessionId, const QString& snapName, CartaObject::SnapshotType type ) const;


    void initialize();

    void printObjects();
    bool registerClass (const QString & className, CartaObjectFactory * factory);

    /**
     * Restore a snapshot of the application state.
     * @param stateStr - a string representation of the state to restore.
     * @param snapType - the type of application state to restore.
     * @return true if the state was successfully restored; false otherwise.
     */
    bool restoreSnapshot(const QString stateStr, CartaObject::SnapshotType snapType ) const;

    /**
     * Returns the singleton instance of the object manager.
     * @return a pointer to the object manager.
     */
    // Singleton accessor
    static ObjectManager * objectManager ();
    QString getRootPath() const;
    QString getRoot() const;

    /**
     * Parses the full path of a CartaObject to return the id portion.
     * @param path a full path to a CartaObject;
     * @return the id of the CartaObject.
     */
    QString parseId( const QString& path ) const;
    static const QString STATE_ARRAY;

    class OnCreateObject{
    public:
        OnCreateObject (ObjectManager * objectManager) : m_objectManager (objectManager) {}

        QString operator() (const QString & /*command*/, const QString & parameters,
                const QString & /*sessionId*/)
        {
            CartaObject * object = m_objectManager -> createObject<CartaObject> (parameters);
            QString id;
            if ( object != nullptr ){
                id= object->getId();
            }
            return id;
        }

    private:

        ObjectManager * m_objectManager;
    };

    class OnDestroyObject{
    public:
        OnDestroyObject (ObjectManager * objectManager) : m_objectManager (objectManager) {}

        QString operator() (const QString & /*command*/, const QString & parameters,
                const QString & /*sessionId*/)
        {
            return m_objectManager -> destroyObject (parameters);
        }

    private:

        ObjectManager * m_objectManager;
    };

    static const QString CreateObject;
    static const QString ClassName;
    static const QString DestroyObject;


    static const QString STATE_ID;
    static const QString STATE_VALUE;

private:



    /// stores a pair< QString, CartaObjectFactory >
    class ClassRegistryEntry {

    public:

        ClassRegistryEntry () : m_factory (nullptr) {}

        ClassRegistryEntry (const QString & className, CartaObjectFactory * factory)
        : m_className (className), m_factory (factory)
        {}

        QString getClassName () const;
        CartaObjectFactory * getFactory () const {
            return m_factory;
        }

    private:

        QString m_className;
        CartaObjectFactory * m_factory;
    };

    /// stores a tuple<QString, QString, QString, CartaObject>
    class ObjectRegistryEntry {

    public:

        ObjectRegistryEntry () : m_object(nullptr) {}

        ObjectRegistryEntry (const QString & className,
                const QString & id,
                const QString & path,
                CartaObject * object)
        : m_className (className),
          m_id (id),
          m_object (object),
          m_path (path)
        {}

        const QString & getClassName () const {
            return m_className;
        }
        const QString & getId () const;
        CartaObject * getObject () const {
            return m_object;
        }
        const QString & getPath () const;

    private:

        QString m_className;
        QString m_id;
        CartaObject * m_object;
        QString m_path;

    };

    ObjectManager (); // for use of singleton only

    ObjectManager (const ObjectManager & other); // do not implement
    ObjectManager & operator= (const ObjectManager & other); // do not implement

    // Looks up factory information using class name

    typedef std::map <QString, ClassRegistryEntry> ClassRegistry;

    // note m_classes[name].getClassName() == name
    ClassRegistry m_classes;

    const QString m_root;
    const QString m_sep;

    int m_nextId;

    // Looks up existing objects using their ID

    typedef std::map <QString, ObjectRegistryEntry> ObjectRegistry;

    ObjectRegistry m_objects;

};

class ExampleCartaObject : public CartaObject {

public:

    static const QString DoSomething;

    QString doSomething (const QString & command, const QString & parameters,
            const QString & sessionId);


private:

    ExampleCartaObject (const QString & path, const QString & id);

    class Factory : public CartaObjectFactory {

    public:

        CartaObject * create (const QString & path, const QString & id)
        {
            return new ExampleCartaObject (path, id);
        }
    };

    static bool m_registered;

};
}
}
