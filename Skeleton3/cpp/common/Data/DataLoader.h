/***
 * Returns Json representing a directory tree of eligible data that can be loaded
 * from a root directory.
 */

#pragma once

#include <QDir>
#include <QJsonArray>

class DataLoader {

public:

	/**
	 * Returns a QString containing a hierarchical listing of data files that can
	 * be loaded.
	 * @param selectionParams a filter for choosing specific types of data files.
	 * @param sessionId the user's session identifier that may be eventually used to determine
	 * 		a search directory or URL for files.
	 */
    static QString getData( const QString& selectionParams, const QString& sessionId );

private:
    //Return the top level directory for the data file search.
    static QString getRootDir( const QString& sessionId );

    //Look for eligible data files in a specific directory (recursive).
    static void processDirectory(const QDir& rootDir,  QJsonObject& rootArray );

    //Add a file to the list of those available in a given directory.
    static void makeFileNode( QJsonArray& parentArray, const QString& fileName );
    DataLoader();

};
