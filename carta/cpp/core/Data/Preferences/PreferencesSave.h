/***
 * User PreferencesSave such as menu visibility.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"

namespace Carta {

namespace Data {

class PreferencesSave : public Carta::State::CartaObject {

public:

    /**
     * Return a string representing the histogram state of a particular type.
     * @param sessionId - an identifier for a user's session.
     * @param type - the type of state needed.
     * @return a QString representing the corresponding histogram state.
     */
    virtual QString getStateString( const QString& sessionId, SnapshotType type ) const Q_DECL_OVERRIDE;

    /**
     * Returns the aspect ratio for saves.
     * @return - the aspect ratio that will be used for image saves.
     */
    Qt::AspectRatioMode getAspectRatioMode() const;

    /**
     * Return the target height for saved images.
     * @return - the target height for saved images.
     */
    int getHeight() const;

    /**
     * Return the target width for saved images.
     * @return - the target width for saved images.
     */
    int getWidth() const;


    /**
     * Set the aspect ratio to be used for saving images.
     * @param aspectRatioMode - whether or not to preserve the aspect ratio, etc,
     *      when saving images.
     * @return - an error if there was a problem setting the aspect ratio; an empty
     *      string otherwise.
     */
    QString setAspectRatioMode( const QString& aspectRatioMode );


    /**
     * Set the width for the saved image.
     * @param width - the width in pixels for the saved image.
     * @return - an error string if there was a problem setting the width; an empty
     *      string otherwise.
     */
    QString setWidth( int width );

    /**
     * Set the height for the saved image.
     * @param height - the height in pixels for the saved image.
     * @return - an error string if there was a problem setting the height; an empty
     *      string otherwise.
     */
    QString setHeight( int height );

    const static QString CLASS_NAME;

    virtual ~PreferencesSave();

private:
    void _initializeDefaultState();
    void _initializeCallbacks();
    QString _setDimension( int dim, const QString& key );

    static const QString ASPECT_KEEP;
    static const QString ASPECT_EXPAND;
    static const QString ASPECT_IGNORE;
    static const QString ASPECT_RATIO_MODE;
    static bool m_registered;

    PreferencesSave( const QString& path, const QString& id );
    class Factory;

	PreferencesSave( const PreferencesSave& other);
	PreferencesSave& operator=( const PreferencesSave& other );
};
}
}
