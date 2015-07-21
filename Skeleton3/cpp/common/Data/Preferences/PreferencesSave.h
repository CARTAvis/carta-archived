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
    Qt::AspectRatioMode getAspectRatioMode() const;
    int getHeight() const;
    int getWidth() const;
    QString setAspectRatioMode( const QString& aspectRatioMode );

    QString setWidth( int width );

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
    static const QString WIDTH;
    static const QString HEIGHT;
    static bool m_registered;

    PreferencesSave( const QString& path, const QString& id );
    class Factory;


	PreferencesSave( const PreferencesSave& other);
	PreferencesSave& operator=( const PreferencesSave& other );
};
}
}
