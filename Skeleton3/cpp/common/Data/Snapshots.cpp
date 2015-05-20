#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <QDebug>
#include <QDirIterator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "Data/Snapshots.h"
#include "Data/Selection.h"
#include "CartaLib/CartaLib.h"

namespace Carta {

namespace Data {

const QString Snapshots::SNAPSHOT_SELECTED = "selected";
const QString Snapshots::CLASS_NAME = "Snapshots";
const QString Snapshots::DEFAULT_SAVE = "default";
const QString Snapshots::DIR_LAYOUT = "layout";
const QString Snapshots::DIR_PREFERENCES = "preferences";
const QString Snapshots::DIR_DATA = "data";
const QString Snapshots::SUFFIX = ".cartaState";
const QString Snapshots::FILE_NAME = "fileName";
const QString Snapshots::SAVE_LAYOUT = "layoutSnapshot";
const QString Snapshots::SAVE_PREFERENCES = "preferencesSnapshot";
const QString Snapshots::SAVE_DATA = "dataSnapshot";

class Snapshots::Factory : public CartaObjectFactory {
    public:

        Factory():
            CartaObjectFactory(CLASS_NAME){
        };

        CartaObject * create (const QString & path, const QString & id)
        {
            return new Snapshots (path, id);
        }
    };



bool Snapshots::m_registered =
    ObjectManager::objectManager()->registerClass ( CLASS_NAME, new Snapshots::Factory());

Snapshots::Snapshots( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){
    _initializeState();
    _initializeCallbacks();
}

QString Snapshots::_getRootDir(const QString& /*sessionId*/) const {
   struct passwd *pw = getpwuid(getuid());
#ifdef DESKTOPVERSION
    return QString("%1/%2/%3").arg(pw->pw_dir, "CARTA", "snapshots");
#else
    return "/scratch/snapshots";
#endif

}

QString Snapshots::getSnapshots( const QString& sessionId ) const {
    QString rootDirName = _getRootDir(sessionId);
    QDir rootDir( rootDirName );
    QStringList snapshotList;
    _processDirectory( rootDir, snapshotList );
    return snapshotList.join( ",");
}

void Snapshots::_processDirectory(const QDir& rootDir, QStringList& snapshotList) const {
    if (!rootDir.exists()) {
        return;
    }

    QDirIterator dit(rootDir.absolutePath(), QDir::NoFilter);
    while (dit.hasNext()) {
        dit.next();
        // skip "." and ".." entries
        if (dit.fileName() == "." || dit.fileName() == "..") {
            continue;
        }

        QString fileName = dit.fileInfo().fileName();
        if (dit.fileInfo().isFile()) {
            if (fileName.endsWith(SUFFIX)) {
                fileName = fileName.left( fileName.length() - SUFFIX.length());
                if ( snapshotList.indexOf( fileName ) < 0 ){
                    snapshotList.append( fileName );
                }
            }
        }
        else if (dit.fileInfo().isDir()){
            QString subDirPath = dit.fileInfo().absoluteFilePath();
            QDir subDir( subDirPath );
            _processDirectory( subDir, snapshotList);
        }
    }
}



void Snapshots::_initializeState(){
    m_state.insertValue( SNAPSHOT_SELECTED, DEFAULT_SAVE );
    m_state.flushState();
}

void Snapshots::_initializeCallbacks(){
    addCommandCallback( "getSnapshots", [=] (const QString & /*cmd*/,
                                const QString & /*params*/, const QString & sessionId) -> QString {
                QString result = getSnapshots( sessionId );
                return result;
            });
}

QString Snapshots::readLayout(const QString& sessionId, const QString& baseName) const {
    QString result = _readSpecific( sessionId, DIR_LAYOUT, baseName);
    return result;
}

QString Snapshots::readPreferences(const QString& sessionId, const QString& baseName) const {
    QString result = _readSpecific( sessionId, DIR_PREFERENCES, baseName);
    return result;
}

QString Snapshots::readData(const QString& sessionId, const QString& baseName) const {
    QString result = _readSpecific( sessionId, DIR_DATA, baseName );
    return result;
}

QString Snapshots::saveLayout(const QString& sessionId, const QString& baseName, const QString& layoutStr) const {
    QString result = _saveSpecific( sessionId, DIR_LAYOUT, baseName, layoutStr );
    return result;
}

QString Snapshots::savePreferences(const QString& sessionId, const QString& baseName, const QString& prefStr) const {
    QString result = _saveSpecific( sessionId, DIR_PREFERENCES, baseName, prefStr );
    return result;
}

QString Snapshots::saveData(const QString& sessionId, const QString& baseName, const QString& dataStr) const {
    QString result = _saveSpecific( sessionId, DIR_DATA, baseName, dataStr );
    return result;
}

QString Snapshots::_readSpecific( const QString& sessionId, const QString& subDirName, const QString& baseName) const {
    QString result;
    QString fullName = baseName;
    if ( !baseName.endsWith( SUFFIX ) ){
        fullName = fullName + SUFFIX;
    }
    QString rootDir = _getRootDir( sessionId );
    QString filePath = rootDir + QDir::separator() + subDirName + QDir::separator() + fullName;

    QFile file( filePath );
    if ( file.exists() ){
        result = _read( filePath);
    }
    return result;
}

QString Snapshots::_saveSpecific( const QString& sessionId, const QString subDirName, const QString& baseName, const QString saveStr ) const {
    QString result;
    QString fullName = baseName;
    if ( !baseName.endsWith( SUFFIX ) ){
        fullName = fullName + SUFFIX;
    }
    QString rootDir = _getRootDir( sessionId );
    QString filePath = rootDir + QDir::separator() + subDirName + QDir::separator() + fullName;
    bool saved = _save( filePath, saveStr );
    if ( !saved ){
        result = "There was a problem saving the "+subDirName;
    }
    return result;
}

bool Snapshots::_save( const QString& fileLocation, const QString& stateStr ) const {
    QFile file( fileLocation );
    bool fileSaved = true;
    if ( !file.open( QIODevice::WriteOnly) ){
        fileSaved = false;
    }
    else {
        QTextStream stream( &file );
        stream << stateStr;
        stream.flush();
        file.close();
    }
    return fileSaved;
}

QString Snapshots::_read( const QString& fileLocation ) const {
    //Read in the file and store it in StateInterface
    QFile file( fileLocation );
    QString state;
    if ( !file.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "Could not open file="<<fileLocation<<" for reading";
    }
    else {
        QTextStream in(&file);
        state = in.readLine();
        file.close();
    }
    return state;
}

Snapshots::~Snapshots(){

}
}
}
