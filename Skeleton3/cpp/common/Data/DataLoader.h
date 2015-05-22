/***
 * Returns Json representing a directory tree of eligible data that can be loaded
 * from a root directory.
 */

#pragma once

#include <QDir>
#include <QJsonArray>

#include <State/ObjectManager.h>
#include <memory>

namespace Carta {

namespace Data {

class DataLoader : public Carta::State::CartaObject {

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

    /**
     * Return the top level directory for the data file search.
     * @param sessionId - an identifier for the user's session.
     * @return the absolute path to the directory containing the user's files.
     */
    QString getRootDir(const QString& sessionId) const;

    static QString fakeRootDirName;
    const static QString CLASS_NAME;

    virtual ~DataLoader(){};

private:

    static bool m_registered;

    class Factory;

    //Look for eligible data files in a specific directory (recursive).
    void processDirectory(const QDir& rootDir, QJsonObject& rootArray);

    //Add a file to the list of those available in a given directory.
    void makeFileNode(QJsonArray& parentArray, const QString& fileName);
    DataLoader( const QString& path, const QString& id);
    DataLoader( const DataLoader& other);
    DataLoader operator=( const DataLoader& other );
};
}
}
