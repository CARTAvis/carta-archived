/***
 * List of available snapshots as well as the currently selected one.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "Snapshot.h"
#include <QDir>

namespace Carta {

namespace Data {

class Snapshots : public CartaObject{

public:

    /**
     * Returns a list of supported snapshots
     * @param sessionId an identifier for a user session.
     * @return a list of supported snapshots.
     */
    QString getSnapshots(const QString& sessionId ) const;
    QString readLayout(const QString& sessionId, const QString& baseName) const;
    QString readPreferences(const QString& sessionId, const QString& baseName) const;
    QString readData(const QString& sessionId, const QString& baseName) const;
    QString saveLayout(const QString& sessionId, const QString& baseName, const QString& layoutStr) const;
    QString savePreferences(const QString& sessionId, const QString& baseName, const QString& prefStr) const;
    QString saveData(const QString& sessionId, const QString& baseName, const QString& dataStr) const;
    const static QString CLASS_NAME;
    const static QString SNAPSHOT_SELECTED;
    const static QString FILE_NAME;
    const static QString SAVE_LAYOUT;
    const static QString SAVE_PREFERENCES;
    const static QString SAVE_DATA;
    virtual ~Snapshots();

private:
    const static QString DEFAULT_SAVE;

    const static QString SUFFIX;

    QString _getRootDir(const QString& /*sessionId*/) const;
    void _processDirectory(const QDir& rootDir, QMap<QString,Snapshot>& snapshotList) const;
    void _initializeCallbacks();
    void _initializeState();
    QString _read( const QString& fileLocation ) const;
    QString _readSpecific( const QString& sessionId, const QString& subDirName, const QString& baseName ) const;
    bool _save( const QString& fileLocation, const QString& stateStr ) const;
    QString _saveSpecific( const QString& sessionId, const QString subDirName, const QString& baseName, const QString saveStr ) const;

    static bool m_registered;
    Snapshots( const QString& path, const QString& id );
    class Factory;


	Snapshots( const Snapshots& other);
	Snapshots operator=( const Snapshots& other );
};
}
}
