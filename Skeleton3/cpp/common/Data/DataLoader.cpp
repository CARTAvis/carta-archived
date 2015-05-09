#include "DataLoader.h"

#include <QDebug>
#include <QDirIterator>
#include <QJsonDocument>
#include <QJsonObject>

namespace Carta {

namespace Data {

class DataLoader::Factory : public CartaObjectFactory {

public:

    Factory():
        CartaObjectFactory( "DataLoader" ){};

    CartaObject * create (const QString & path, const QString & id)
    {
        return new DataLoader (path, id);
    }
};

QString DataLoader::fakeRootDirName = "RootDirectory";
const QString DataLoader::CLASS_NAME = "DataLoader";
bool DataLoader::m_registered =
    ObjectManager::objectManager()->registerClass ( CLASS_NAME,
                                                   new DataLoader::Factory());


DataLoader::DataLoader( const QString& path, const QString& id ):
    CartaObject( CLASS_NAME, path, id ){
    //Callback for returning a list of data files that can be loaded.
    addCommandCallback( "getData", [=] (const QString & /*cmd*/,
            const QString & params, const QString & sessionId) -> QString {
        QString xml = getData( params, sessionId );
        return xml;
    });
}

QString DataLoader::getFile( const QString& bogusPath, const QString& sessionId ) const {
    QString path( bogusPath );
    QString fakePath( QDir::separator() + DataLoader::fakeRootDirName );
    if( path.startsWith( fakePath )){
        QString rootDir = getRootDir( sessionId );
        QString baseRemoved = path.remove( 0, fakePath.length() );
        path = QString( "%1%2").arg( rootDir).arg( baseRemoved);
    }
    else {
        /// security issue...
        //qDebug() << "Security issue, filePath="<<path;
    }
    return path;
}

QString DataLoader::getData(const QString& /*selectionParams*/,
        const QString& sessionId) {
    QString rootDirName = getRootDir(sessionId);
    QDir rootDir(rootDirName);

    QJsonObject rootObj;
    processDirectory(rootDir, rootObj);
    // replace the entry for the root object with a fake, for two reasons:
    // root directory could contain multiple directories (e.g. /scratch/Images ...)
    // for little added security
    rootObj.insert("name", fakeRootDirName);

    QJsonDocument document(rootObj);
    QByteArray textArray = document.toJson();
    QString jsonText(textArray);
    return jsonText;
}

void DataLoader::processDirectory(const QDir& rootDir, QJsonObject& rootObj) {
    if (!rootDir.exists()) {
        return;
    }

    rootObj.insert("name", rootDir.dirName());

    QJsonArray dirArray;
    QDirIterator dit(rootDir.absolutePath(), QDir::NoFilter);
    while (dit.hasNext()) {
        dit.next();
        // skip "." and ".." entries
        if (dit.fileName() == "." || dit.fileName() == "..") {
            continue;
        }

        QString fileName = dit.fileInfo().fileName();
        if (dit.fileInfo().isDir()) {
            if (fileName.endsWith(".image")) {
                makeFileNode(dirArray, fileName);
            }
            else {
                QString dirName = dit.fileInfo().absoluteFilePath();
                QJsonObject dirObject;
                processDirectory(QDir(dirName), dirObject);
                dirArray.append(dirObject);
            }
        }
        else if (dit.fileInfo().isFile()) {
            if (fileName.endsWith(".fits")) {
                makeFileNode(dirArray, fileName);
            }
        }
    }

    rootObj.insert("dir", dirArray);
}

void DataLoader::makeFileNode(QJsonArray& parentArray,
        const QString& fileName) {
    QJsonObject obj;
    QJsonValue fileValue(fileName);
    obj.insert("name", fileValue);
    parentArray.append(obj);
}

QString DataLoader::getRootDir(const QString& /*sessionId*/) const {
    return "/scratch/Images";
}
}
}
