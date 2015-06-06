/***
 * Stores user preferences as to what settings are visible in the colormap.
 *
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
     * Set the color mix settings visible.
     * @param visible - true for user color mix settings to be visible; false otherwise.
     */
    void setVisibleColorMix( bool visible );

    /**
    * Set the color color scale settings visible.
    * @param visible - true for user color scale settings to be visible; false otherwise.
    */
    void setVisibleColorScale( bool visible );

    /**
    * Set the color transform settings visible.
    * @param visible - true for user color transform settings to be visible; false otherwise.
    */
    void setVisibleColorTransform( bool visible );

    /**
    * Set the color model settings visible.
    * @param visible - true for user color model settings to be visible; false otherwise.
    */
    void setVisibleColorModel( bool visible );

    const static QString CLASS_NAME;

    virtual ~Settings();

private:
    void _initializeDefaultState();
    void _initializeCallbacks();

    bool _processParams( const QString& params, bool* value ) const;
    void _setVisibility( const QString& key, bool visible );

    static bool m_registered;

    const static QString COLOR_MIX;
    const static QString COLOR_MODEL;
    const static QString COLOR_SCALE;
    const static QString COLOR_TRANSFORM;

    const static QString VISIBLE;

    Settings( const QString& path, const QString& id );
    class Factory;


	Settings( const Settings& other);
	Settings& operator=( const Settings& other );
};
}
}
