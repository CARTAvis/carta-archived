/***
 * List of available snapshots as well as the currently selected one.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
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
    void _makeFileNode(QJsonArray& parentArray, const QString& fileName) const;
    void _processDirectory(const QDir& rootDir, QJsonObject& rootObj) const;
    void _initializeState();

    static bool m_registered;
    Snapshots( const QString& path, const QString& id );
    class Factory;


	Snapshots( const Snapshots& other);
	Snapshots operator=( const Snapshots& other );
};
}
}
