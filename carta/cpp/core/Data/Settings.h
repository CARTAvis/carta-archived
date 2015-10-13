/***
 * Stores whether or not settings are visible.
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"

namespace Carta {

namespace Data {

class Settings : public Carta::State::CartaObject {

public:

    /**
     * Return a string representing the settings state of a particular type.
     * @param sessionId - an identifier for a user's session.
     * @param type - the type of state needed.
     * @return a QString representing the corresponding settings state.
     */

    virtual QString getStateString( const QString& sessionId, SnapshotType type ) const Q_DECL_OVERRIDE;

    /**
     * Restore the state from a string representation.
     * @param stateStr- a json representation of state.
     */
    void resetStateString( const QString& stateStr );

    /**
     * Restore the state from a string representation.
     * @param stateStr- a json representation of state.
     */
    //Implemented to do nothing so the state can be set from the owning object.
    virtual void resetState( const QString& stateStr ) Q_DECL_OVERRIDE;

    /**
     * Set the settings visible.
     * @param visible - true for user settings to be visible; false otherwise.
     */
    void setVisible( bool visible );

    static const QString SETTINGS;
    const static QString CLASS_NAME;

    virtual ~Settings();

private:
    void _initializeDefaultState();
    void _initializeCallbacks();

    bool _processParams( const QString& params, bool* value ) const;


    static bool m_registered;

    Settings( const QString& path, const QString& id );
    class Factory;


	Settings( const Settings& other);
    Settings& operator=( const Settings& other );
};
}
}
