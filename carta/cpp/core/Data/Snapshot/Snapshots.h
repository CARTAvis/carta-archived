/***
 * Manages the state of snapshot collections.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "State/UtilState.h"
#include "Snapshot.h"

namespace Carta {

namespace Data {

class ISnapshotsImplementation;

class Snapshots : public Carta::State::CartaObject {

public:

    /**
     * Delete the snapshot with the given identifier.
     * @param sessionId an identifier for a user session.
     * @param saveName an identifier for the snapshot to delete.
     * @return an empty string if the snapshot was deleted; an error message if
     *      there was a problem deleting the snapshot.
     */
    QString deleteSnapshot( const QString& sessionId, const QString& saveName );

    /**
     * Restores the default snapshot if one has been saved.
     */
    void initializeDefaultState();

    /**
      * Read and restore state for a particular sessionId from a string.
      * @param stateStr - a string representation of the state.
      * @param type - the type of state.
      * @return true if the state was read and restored; false otherwise.
      */
    QString restoreSnapshot( const QString& sessionId, const QString& saveName );

    /**
     * Save the current state.
     * @param sessionId - an identifier for a user session.
     * @param saveName - an identifier for the state to be saved.
     * @param saveLayout - true if the layout should be saved; false otherwise.
     * @param savePreferences -true if the preferences should be saved; false otherwise.
     * @param saveData - true if the data should be saved; false otherwise.
     * @param description - notes about the state being saved.
     * @return an error message if there was a problem saving state; an empty string otherwise.
     */
    QString saveSnapshot( const QString& sessionId, const QString& saveName, bool saveLayout,
            bool savePreferences, bool saveData, const QString& description );
    /**
     * Returns a list of available snapshots
     * @param sessionId - an identifier for a user session.
     * @return a list of supported snapshots.
     */
    QList<Snapshot> getSnapshots(const QString& sessionId ) const;

    /**
     * Force a reload of stored snapshots.
     * @param sessionId - an identifier for the user's session.
     */
    void updateSnapshots( const QString& sessionId );

    const static QString CLASS_NAME;
    const static QString SNAPSHOT_SELECTED;
    const static QString FILE_NAME;
    const static QString SAVE_LAYOUT;
    const static QString SAVE_PREFERENCES;
    const static QString SAVE_DATA;
    const static QString SAVE_DESCRIPTION;
    virtual ~Snapshots();

private:

    typedef Carta::State::UtilState UtilState;
    typedef Carta::State::StateInterface StateInterface;

    ISnapshotsImplementation* m_snapImpl;
    const static QString DEFAULT_SNAPSHOT;

    void _initializeCallbacks();
    void _initializeState();

    QString _restoreLayout(const QString& sessionId, const QString& snapName) const;
    QString _restoreSnapshot(const QString& sessionId, CartaObject::SnapshotType snapType, const QString& baseName) const;

    QString _saveData(const QString& sessionId, const QString& baseName);
    QString _saveDescription( const QString& sessionId, const QString& baseName,
            const QString& description );
    QString _saveLayout(const QString& sessionId, const QString& baseName);
    QString _savePreferences(const QString& sessionId, const QString& baseName);
    void _saveSelected( const QString& saveName );

    static bool m_registered;
    Snapshots( const QString& path, const QString& id );
    class Factory;


	Snapshots( const Snapshots& other);
	Snapshots& operator=( const Snapshots& other );
};
}
}
