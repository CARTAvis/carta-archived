#include <unistd.h>

#include <QDebug>
#include <QDirIterator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegExp>

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
const QString DataLoader::DIR = "dir";
const QString DataLoader::CRTF = ".crtf";
const QString DataLoader::REG = ".reg";

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
        if ( lastAccessedDir.length() == 0 ){
            lastAccessedDir = getRootDir(sessionId);
        }
        rootDirName = lastAccessedDir;
    }
    else {
        rootDirName = getFile( dirName, sessionId );
    }
    lastAccessedDir = rootDirName;
    QDir rootDir(rootDirName);
    QJsonObject rootObj;

    _processDirectory(rootDir, rootObj);

    if ( securityRestricted ){
        QString baseName = getRootDir( sessionId );
        QString displayName = rootDirName.replace( baseName, DataLoader::fakeRootDirName);
        rootObj.insert(Util::NAME, displayName);
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

QString DataLoader::getShortName( const QString& longName ) const {
    QString rootDir = getRootDir( "" );
    int rootLength = rootDir.length();
    QString shortName;
    if ( longName.contains( rootDir)){
        shortName = longName.right( longName.size() - rootLength - 1);
    }
    else {
        int lastSlashIndex = longName.lastIndexOf( QDir::separator() );
        if ( lastSlashIndex >= 0 ){
            shortName = longName.right( longName.size() - lastSlashIndex - 1);
        }
    }
    return shortName;
}

QStringList DataLoader::getShortNames( const QStringList& longNames ) const {
    QStringList shortNames;
    for ( int i = 0; i < longNames.size(); i++ ){
        QString shortName = getShortName( longNames[i] );
        shortNames.append( shortName );
    }
    return shortNames;
}


QString DataLoader::getLongName( const QString& shortName, const QString& sessionId ) const {
    QString longName = shortName;
    QString potentialLongName = getRootDir( sessionId) + QDir::separator() + shortName;
    QFile file( potentialLongName );
    if ( file.exists() ){
        longName = potentialLongName;
    }
    return longName;
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
    rootObj.insert( Util::NAME, lastPart );

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
            QString rootDirPath = rootDir.absolutePath();
            QString subDirPath = rootDirPath.append("/").append(fileName);

            if ( !_checkSubDir(subDirPath).isNull() ) {
                _makeFileNode( dirArray, fileName, _checkSubDir(subDirPath));
            }
            else {
                _makeFolderNode( dirArray, fileName );
            }
        }
        else if (dit.fileInfo().isFile()) {
            QFile file(lastPart+QDir::separator()+fileName);
            if (file.open(QFile::ReadOnly)) {
                QString dataInfo = file.read(160);
                if (dataInfo.contains("Region", Qt::CaseInsensitive)) {
                    if (dataInfo.contains("DS9", Qt::CaseInsensitive)) {
                        _makeFileNode(dirArray, fileName, "reg");
                    }
                    else if (dataInfo.contains("CRTF", Qt::CaseInsensitive)) {
                        _makeFileNode(dirArray, fileName, "crtf");
                    }
                }
                else if (dataInfo.contains(QRegExp("^SIMPLE *= *T.* BITPIX*")) && !dataInfo.contains(QRegExp("\n"))) {
                    _makeFileNode(dirArray, fileName, "fits");
                }
                file.close();
            }
        }
    }

    rootObj.insert( DIR, dirArray);
}

QString DataLoader::_checkSubDir( QString& subDirPath) const {

    QDir subDir(subDirPath);
    if (!subDir.exists()) {
        QString errorMsg = "Please check that "+subDir.absolutePath()+" is a valid directory.";
        Util::commandPostProcess( errorMsg );
        exit(0);
    }

    QMap< QString, QStringList> filterMap;

    QStringList imageFilters, miriadFilters;
    imageFilters << "table.f0_TSM0" << "table.info";
    miriadFilters << "header" << "image";

    filterMap.insert( "image", imageFilters);
    filterMap.insert( "miriad", miriadFilters);

    //look for the subfiles satisfying a special format
    foreach ( const QString &filter, filterMap.keys()){
        subDir.setNameFilters(filterMap.value(filter));
        if ( subDir.entryList().length() == filterMap.value(filter).length() ) {
            return filter;
        }
    }
    return NULL;
}

void DataLoader::_makeFileNode(QJsonArray& parentArray, const QString& fileName, const QString& fileType) const {
    QJsonObject obj;
    QJsonValue fileValue(fileName);
    obj.insert( Util::NAME, fileValue);
    //use type to represent the format of files
    //the meaning of "type" may differ with other codes
    //can change the string when feeling confused
    obj.insert( Util::TYPE, QJsonValue(fileType));
    parentArray.append(obj);
}

void DataLoader::_makeFolderNode( QJsonArray& parentArray, const QString& fileName ) const {
    QJsonObject obj;
    QJsonValue fileValue(fileName);
    obj.insert( Util::NAME, fileValue);
    QJsonArray arry;
    obj.insert(DIR, arry);
    parentArray.append(obj);
}



DataLoader::~DataLoader(){
}
}
}
