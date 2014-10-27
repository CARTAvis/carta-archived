/***
 * Main class that manages the data state for the views.
 *
 */

#pragma once

#include "State/StateInterface.h"
#include "State/ObjectManager.h"


class Animator;
class Controller;
class DataLoader;
class Layout;
class ViewPlugins;

class ViewManager : public CartaObject {

public:

    virtual ~ViewManager(){}

    /**
     * Convenience method for loading initial data with Viewer start-up.
     * @param fileName a locater for the data to load.
     */
    void loadFile( QString fileName );

    static const QString CLASS_NAME;

private:
    ViewManager( const QString& path, const QString& id);
    class Factory : public CartaObjectFactory {

    public:
        Factory():
          CartaObjectFactory( "ViewManager" ){};
        CartaObject * create (const QString & path, const QString & id)
        {
            return new ViewManager (path, id);
        }
    };
    void _initCallbacks();

    void _initializeExistingAnimationLinks( int index );
    //Sets up a default set of states for constructing the UI if the user
    //has not saved one.
    void _initializeDefaultState();
    void _initializeDataLoader();

    QString _makeAnimator();
    QString _makePluginList();
    QString _makeController();

    //A list of Controllers requested by the client.
    QList <std::shared_ptr<Controller> > m_controllers;

    //A list of Animators requested by the client.
    QList < std::shared_ptr<Animator> > m_animators;

    static bool m_registered;
    std::shared_ptr<Layout> m_layout;
    std::shared_ptr<DataLoader> m_dataLoader;
    std::shared_ptr<ViewPlugins> m_pluginsLoaded;
    //StateInterface m_state;
	ViewManager( const ViewManager& other);
	ViewManager operator=( const ViewManager& other );
};
