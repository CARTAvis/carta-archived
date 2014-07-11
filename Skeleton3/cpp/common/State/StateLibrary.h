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
	QString getPath( StateKey stateKey, const QString& id) const;

	/**
	 * Returns whether or not the state represented by the path should
	 * be persisted.
	 * @param path an identifier for a state.
	 */
	bool isPersistent( const QString& path ) const;

	/**
	 * Returns the state key associated associated with the path.
	 */
	StateKey findKey( const QString& path ) const;

	//Root xml element for application state.
	static const QString APP_ROOT;

	//Path separator when creating a state tree.
	static const QString SEPARATOR;

	//Identifier for an animator state.
	static const QString ANIMATOR;

	//Identifier for a channel.
	static const QString CHANNEL;

	//Identifier for mouse information
	static const QString MOUSE;

	//Identifier for plug-ins.
	static const QString PLUGIN_LIST;

private:

	StateLibrary();
	void _initializeStates();
	static StateLibrary* library;
	QMap<StateKey, State> states;
};
