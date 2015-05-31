/***
 * User preferences such as menu visibility.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"

namespace Carta {

namespace Data {

class Preferences : public Carta::State::CartaObject {

public:

    /**
     * Return a string representing the histogram state of a particular type.
     * @param sessionId - an identifier for a user's session.
     * @param type - the type of state needed.
     * @return a QString representing the corresponding histogram state.
     */
    virtual QString getStateString( const QString& sessionId, SnapshotType type ) const Q_DECL_OVERRIDE;

    /**
     * Show/hide the menu bar.
     * @param visible true if the menu bar should be shown; false otherwise.
     */
    void setMenuVisibility( bool visible );

    /**
     * Show/hide the status bar.
     * @param visible true if the status bar should be shown; false otherwise.
     */
    void setStatusVisibility( bool visible );

    /**
     * Show/hide the tool bar.
     * @param visible true if the tool bar should be shown; false otherwise.
     */
    void setToolbarVisibility( bool visible );

    const static QString CLASS_NAME;

    virtual ~Preferences();

private:
    void _initializeDefaultState();
    void _initializeCallbacks();

    static const QString SHOW_MENU;
    static const QString SHOW_TOOLBAR;
    static const QString SHOW_STATUS;
    static bool m_registered;

    Preferences( const QString& path, const QString& id );
    class Factory;


	Preferences( const Preferences& other);
	Preferences operator=( const Preferences& other );
};
}
}
