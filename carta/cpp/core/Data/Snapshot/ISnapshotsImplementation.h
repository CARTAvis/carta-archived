/***
 * Interface for classes that can store/save snapshots.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "Snapshot.h"

namespace Carta {

namespace Data {

class ISnapshotsImplementation {

public:

    /**
     * Delete the snapshot with the given identifier.
     * @param sessionId an identifier for a user session.
     * @param saveName an identifier for the snapshot to delete.
     * @return an empty string if the snapshot was deleted; an error message if
     *      there was a problem deleting the snapshot.
     */
    virtual QString deleteSnapshot( const QString& sessionId, const QString& saveName ) = 0;

    /**
     * Returns a list of supported snapshots
     * @param sessionId an identifier for a user session.
     * @return a list of supported snapshots.
     */
    virtual QList<Snapshot> getSnapshots( const QString& sessionId ) const = 0;

    /**
     * Save a snapshot of a specific type.
     * @param sessionId - an identifier for a user session.
     * @param snapshotType - an identifier for the type of snapshot to save.
     * @param snapName - an identifier for the snapshot.
     * @param saveStr - the contents to save.
     */
    virtual QString save( const QString& sessionId, const QString& snapshotType,
            const QString& snapName, const QString saveStr ) = 0;

    /**
     * Read a snapshot of a specific type.
     * @param sessionId - an identifier for a user session.
     * @param snapshotType - an identifier for the type of snapshot to save.
     * @param snapName - an identifier for the snapshot.
     * @param msg - a place holder for an error message if there is one.
     * @return the contents of the snapshot.
     */
    virtual QString read(const QString& sessionId, Carta::State::CartaObject::SnapshotType snapshotType,
            const QString& snapName/*, QString& msg*/ ) const = 0;


    virtual ~ISnapshotsImplementation(){};


};
}
}
