/***
 * File Implementation of the ISnapshotsImplementation interface.
 *
 */

#pragma once

#include "ISnapshotsImplementation.h"
#include <QDir>

namespace Carta {

namespace Data {

class SnapshotsFile : public ISnapshotsImplementation{

public:

    /**
     * Constructor.
     */
    SnapshotsFile( );

    virtual QString deleteSnapshot( const QString& sessionId, const QString& saveName ) Q_DECL_OVERRIDE;

    virtual QList<Snapshot> getSnapshots( const QString& sessionId ) const Q_DECL_OVERRIDE;

    virtual QString save( const QString& sessionId, const QString& snapshotType,
                const QString& snapName, const QString saveStr ) Q_DECL_OVERRIDE;

    virtual QString read(const QString& sessionId, Carta::State::CartaObject::SnapshotType snapshotType,
                const QString& snapName ) const Q_DECL_OVERRIDE;

    virtual ~SnapshotsFile();

private:

    const static QString SUFFIX;
    const static QString BASE_DIR;
    QString _getRootDir(const QString& /*sessionId*/) const;
    void _processDirectory(const QString& sessionId, const QDir& rootDir, QMap<QString,Snapshot>& snapshotList) const;
    QString _read( const QString& fileLocation ) const;
    bool _save( const QString& fileLocation, const QString& stateStr ) const;
    QList<Snapshot> _updateSnapshots( const QString& sessionId );

	SnapshotsFile( const SnapshotsFile& other);
	SnapshotsFile operator=( const SnapshotsFile& other );
};
}
}
