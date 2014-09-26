/**
 *	Manages the collection of valid application states.
 **/

#pragma once
#include "State.h"
#include "StateKey.h"
#include <QString>
#include <QMap>

class StateLibrary {

public:

    /**
     * Returns the singleton instance.
     */
    static StateLibrary* instance();

    /**
     * Returns the path associated with the given key and identifier.
     * @param stateKey a StateKey.
     * @param id an identifier for a UI window.  Base states can have multiple
     * 		views associated with them.
     */
    QString getPath(StateKey stateKey, const QString& id = "") const;

    /**
     * Returns whether or not the state represented by the path should
     * be persisted.
     * @param path an identifier for a state.
     */
    bool isPersistent(const QString& path) const;

    /**
     * Returns the state key associated associated with the path.
     */
    StateKey findKey(const QString& path) const;

    //Root xml element for application state.
    static const QString APP_ROOT;

    //Path separator when creating a state tree.
    static const QString SEPARATOR;

    //Identifier for an animator state.
    static const QString ANIMATOR;

    //Image clipping
    static const QString AUTO_CLIP;
    static const QString CLIP_VALUE;

    //Identifier for a channel.
    static const QString CHANNEL;
    static const QString CONTROLLER;

    //Identifier for file system data
    static const QString ID;

    //Number of data sources that have been loaded.
    static const QString DATA_COUNT;

    //Path to a data item
    static const QString DATA;

    //Identifier for an image.
    static const QString IMAGE;

    static const QString LINK;
    static const QString LINK_COUNT;

    //Number of rows and columns in a grid.
    static const QString LAYOUT;
    static const QString LAYOUT_ROWS;
    static const QString LAYOUT_COLS;

    //Name of the plugin located at a particular location in the grid.
    static const QString LAYOUT_PLUGIN;

    //Identifier for mouse information
    static const QString MOUSE;

    //Identifier for plug-ins.
    static const QString PLUGIN_LIST;

private:

    StateLibrary();
    void _initializeStates();
    QString _getLookupPath( const QString& path ) const;
    static StateLibrary* library;
    QMap<StateKey, State> states;
};
