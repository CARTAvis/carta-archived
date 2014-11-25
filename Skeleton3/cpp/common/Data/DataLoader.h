/***
 * Returns Json representing a directory tree of eligible data that can be loaded
 * from a root directory.
 */

#pragma once

#include <QDir>
#include <QJsonArray>

#include <State/ObjectManager.h>
#include <memory>

class DataLoader : public CartaObject {

public:

    /**
     * Returns a QString containing a hierarchical listing of data files that can
     * be loaded.
     * @param selectionParams a filter for choosing specific types of data files.
     * @param sessionId the user's session identifier that may be eventually used to determine
     *        a search directory or URL for files.
     */
    QString getData(const QString& selectionParams,
                    const QString& sessionId);

    /**
     * Returns the name of the file corresponding to the doctored path and session identifier.
     * @param fakePath a QString identifying a file.
     * @param sessionId an identifier for the session.
     * @return the actual path for the file.
     */
    QString getFile( const QString& fakePath, const QString& sessionId ) const;

    static QString fakeRootDirName;
    const static QString CLASS_NAME;

private:

    static bool m_registered;

    class Factory;

    /// Return the top level directory for the data file search.
    QString getRootDir(const QString& sessionId) const;

    //Look for eligible data files in a specific directory (recursive).
    void processDirectory(const QDir& rootDir, QJsonObject& rootArray);

    //Add a file to the list of those available in a given directory.
    void makeFileNode(QJsonArray& parentArray, const QString& fileName);
    DataLoader( const QString& path, const QString& id);
};
