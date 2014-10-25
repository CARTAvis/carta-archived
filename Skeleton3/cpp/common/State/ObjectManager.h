/*
 * ObjectManager.h
 *
 *  Created on: Oct 3, 2014
 *      Author: jjacobs
 */

#ifndef OBJECTMANAGER_H_
#define OBJECTMANAGER_H_

#include <map>
#include <QString>

#include "../IConnector.h"


class CartaObject {

public:

    virtual ~CartaObject () {}

    QString addIdToCommand (const QString & commandName) const;
    QString getClassName () const;
    QString getId () const;
    QString getPath () const;

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
    /*template <typename Object, typename Method>
        OnCommand<Object, Method>
        wrapCommandHandler (Object * object, Method method){
            return OnCommand<Object, Method> (object, method);
        }*/

private:

    QString m_className;
    QString m_id;
    QString m_path;

    static const char m_Delimiter = ':';

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

    QString createObject (const QString & className);
    QString destroyObject (const QString & id);
    CartaObject * getObject (const QString & id);
    void initialize ();
    bool registerClass (const QString & className, CartaObjectFactory * factory);
    ///Initialize the state variables that were persisted.
    bool readState( const QString& fileName );
    bool saveState( const QString& fileName );
    static ObjectManager * objectManager (); // singleton accessor
    QString getRootPath() const;

    class OnCreateObject{
    public:
        OnCreateObject (ObjectManager * objectManager) : m_objectManager (objectManager) {}

        QString operator() (const QString & /*command*/, const QString & parameters,
                            const QString & /*sessionId*/)
        {
            return m_objectManager -> createObject (parameters);
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

private:

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

        const QString & getClassName () const;
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

    ClassRegistry m_classes;

    const QString m_RootPath;

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



#endif /* OBJECTMANAGER_H_ */
