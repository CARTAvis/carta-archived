/***
 * Stores user preferences as to what settings are visible in the image loader.
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"

namespace Carta {

namespace Data {

class ImageSettings : public Carta::State::CartaObject {

public:

    /**
     * Return a string representing the settings state of a particular type.
     * @param sessionId - an identifier for a user's session.
     * @param type - the type of state needed.
     * @return a QString representing the corresponding settings state.
     */
    virtual QString getStateString( const QString& sessionId, SnapshotType type ) const Q_DECL_OVERRIDE;

    /**
     * Set the grid settings visible.
     * @param visible - true for user grid settings to be visible; false otherwise.
     */
    void setVisibleGrid( bool visible );

    /**
    * Set the font settings visible.
    * @param visible - true for user font settings to be visible; false otherwise.
    */
    void setVisibleFont( bool visible );

    /**
    * Set the color settings visible.
    * @param visible - true for user color settings to be visible; false otherwise.
    */
    void setVisibleColor( bool visible );

    const static QString CLASS_NAME;

    virtual ~ImageSettings();

private:
    void _initializeDefaultState();
    void _initializeCallbacks();

    bool _processParams( const QString& params, bool* value ) const;
    void _setVisibility( const QString& key, bool visible );

    static bool m_registered;

    const static QString GRID_CONTROL;
    const static QString GRID_COLOR;
    const static QString GRID_FONT;

    const static QString VISIBLE;

    ImageSettings( const QString& path, const QString& id );
    class Factory;


	ImageSettings( const ImageSettings& other);
	ImageSettings& operator=( const ImageSettings& other );
};
}
}
