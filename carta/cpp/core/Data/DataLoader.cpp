#include <unistd.h>

#include <QDebug>
#include <QDirIterator>
#include <QJsonDocument>
#include <QJsonObject>

#include "DataLoader.h"
#include "Util.h"
#include "Globals.h"
#include "IPlatform.h"
#include "State/UtilState.h"

#include <set>


namespace Carta {

namespace Data {

class DataLoader::Factory : public Carta::State::CartaObjectFactory {

public:

    Factory():
        CartaObjectFactory( "DataLoader" ){};

    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new DataLoader (path, id);
    }
};

QString DataLoader::fakeRootDirName = "RootDirectory";
const QString DataLoader::CLASS_NAME = "DataLoader";
const QString DataLoader::ROOT_NAME = "name";
const QString DataLoader::DIR = "dir";

bool DataLoader::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME,
                                                   new DataLoader::Factory());


DataLoader::DataLoader( const QString& path, const QString& id ):
    CartaObject( CLASS_NAME, path, id ){
    _initCallbacks();
}


QString DataLoader::getData(const QString& dirName, const QString& sessionId) {
    QString rootDirName = dirName;
    bool securityRestricted = isSecurityRestricted();
    //Just get the default if the user is trying for a directory elsewhere and
    //security is restricted.
    if ( securityRestricted && !dirName.startsWith( DataLoader::fakeRootDirName) ){
        rootDirName = "";
    }

    if ( rootDirName.length() == 0 || dirName == DataLoader::fakeRootDirName){
        rootDirName = getRootDir(sessionId);
    }
    else {
        rootDirName = getFile( dirName, sessionId );
    }
    QDir rootDir(rootDirName);
    QJsonObject rootObj;

    _processDirectory(rootDir, rootObj);

    if ( securityRestricted ){
        QString baseName = getRootDir( sessionId );
        QString displayName = rootDirName.replace( baseName, DataLoader::fakeRootDirName);
        rootObj.insert(ROOT_NAME, displayName);
    }

    QJsonDocument document(rootObj);
    QByteArray textArray = document.toJson();
    QString jsonText(textArray);
    return jsonText;
}

QString DataLoader::getFile( const QString& bogusPath, const QString& sessionId ) const {
    QString path( bogusPath );
    QString fakePath( DataLoader::fakeRootDirName );
    if( path.startsWith( fakePath )){
        QString rootDir = getRootDir( sessionId );
        QString baseRemoved = path.remove( 0, fakePath.length() );
        path = QString( "%1%2").arg( rootDir).arg( baseRemoved);
    }
    return path;
}

QString DataLoader::getRootDir(const QString& /*sessionId*/) const {
    return Globals::instance()-> platform()-> getCARTADirectory().append("Images");
}

QStringList DataLoader::getShortNames( const QStringList& longNames ) const {
    QString sessionId( "");
    QString rootDir = getRootDir( sessionId );
    int rootLength = rootDir.length();
    QStringList shortNames;
    for ( int i = 0; i < longNames.size(); i++ ){
        QString shortName = longNames[i].right( longNames[i].size() - rootLength - 1);
        shortNames.append( shortName );
    }
    return shortNames;
}

void DataLoader::_initCallbacks(){

    //Callback for returning a list of data files that can be loaded.
    addCommandCallback( "getData", [=] (const QString & /*cmd*/,
            const QString & params, const QString & sessionId) -> QString {
        std::set<QString> keys = { "path" };
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString dir = dataValues[*keys.begin()];
        QString xml = getData( dir, sessionId );
        return xml;
    });

    addCommandCallback( "isSecurityRestricted", [=] (const QString & /*cmd*/,
                const QString & /*params*/, const QString & /*sessionId*/) -> QString {
            bool securityRestricted = isSecurityRestricted();
            QString result = "false";
            if ( securityRestricted ){
                result = true;
            }
            return result;
        });
}

bool DataLoader::isSecurityRestricted() const {
    bool securityRestricted = Globals::instance()-> platform()-> isSecurityRestricted();
    return securityRestricted;
}

void DataLoader::_processDirectory(const QDir& rootDir, QJsonObject& rootObj) const {

    if (!rootDir.exists()) {
        QString errorMsg = "Please check that "+rootDir.absolutePath()+" is a valid directory.";
        Util::commandPostProcess( errorMsg );
        return;
    }

    QString lastPart = rootDir.absolutePath();
    rootObj.insert( ROOT_NAME, lastPart );

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
                _makeFileNode(dirArray, fileName);
            }
            else {
                _makeFolderNode( dirArray, fileName );
            }
        }
        else if (dit.fileInfo().isFile()) {
            if (fileName.endsWith(".fits")) {
                _makeFileNode(dirArray, fileName);
            }
        }
    }

    rootObj.insert( DIR, dirArray);
}

void DataLoader::_makeFileNode(QJsonArray& parentArray, const QString& fileName) const {
    QJsonObject obj;
    QJsonValue fileValue(fileName);
    obj.insert(ROOT_NAME, fileValue);
    parentArray.append(obj);
}

void DataLoader::_makeFolderNode( QJsonArray& parentArray, const QString& fileName ) const {
    QJsonObject obj;
    QJsonValue fileValue(fileName);
    obj.insert(ROOT_NAME, fileValue);
    QJsonArray arry;
    obj.insert(DIR, arry);
    parentArray.append(obj);
}



DataLoader::~DataLoader(){
}
}
}
