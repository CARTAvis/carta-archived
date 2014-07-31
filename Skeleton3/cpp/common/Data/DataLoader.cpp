#include "DataLoader.h"
#include "Globals.h"

#include <QDebug>
#include <QDirIterator>
#include <QJsonDocument>
#include <QJsonObject>

QString DataLoader::getData( const QString& /*selectionParams*/, const QString& sessionId ){
	QString rootDirName = getRootDir( sessionId );
	QDir rootDir(rootDirName);
	QString jsonTree;

    QJsonObject rootObj;
    processDirectory( rootDir, rootObj );

    QJsonDocument document( rootObj );
    QByteArray textArray = document.toJson();
    QString jsonText( textArray);
    return jsonText;
}

void DataLoader::processDirectory(const QDir& rootDir,  QJsonObject& rootObj ){
	if( !rootDir.exists()) {
    	return;
    }

	rootObj.insert("name", rootDir.dirName());

	QJsonArray dirArray;
	QDirIterator dit( rootDir.absolutePath(), QDir::NoFilter);
	while (dit.hasNext()) {
	    dit.next();
	    // skip "." and ".." entries
	    if( dit.fileName() == "." || dit.fileName() == "..") {
	    	continue;
	    }


	    QString fileName = dit.fileInfo().fileName();
	    if( dit.fileInfo().isDir()) {
	    	if ( fileName.endsWith( ".image")){
	    		makeFileNode( dirArray, fileName );
	    	}
	    	else {
	    		QString dirName = dit.fileInfo().absoluteFilePath();
	    		QJsonObject dirObject;
	    		processDirectory (QDir( dirName), dirObject );
	    		dirArray.append( dirObject );
	    	}
	    }
	    else if ( dit.fileInfo().isFile()){
	    	if ( fileName.endsWith( ".fits")){
	    		makeFileNode( dirArray, fileName );
	    	}
	    }
    }

	rootObj.insert( "dir", dirArray );
}

void DataLoader::makeFileNode( QJsonArray& parentArray, const QString& fileName ){
	QJsonObject obj;
	QJsonValue fileValue(fileName);
	obj.insert( "name", fileValue );
	parentArray.append( obj );
}

QString DataLoader::getRootDir( const QString& /*sessionId*/ ){
	return "/scratch";
}
