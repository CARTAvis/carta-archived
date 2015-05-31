#include "SnapshotsFile.h"
#include <QDebug>
#include <QDateTime>
#include <QDirIterator>

namespace Carta {

namespace Data {


const QString SnapshotsFile::SUFFIX = ".cartaState";
const QString SnapshotsFile::BASE_DIR = "snapshots";


SnapshotsFile::SnapshotsFile(){
}


QString SnapshotsFile::deleteSnapshot( const QString& sessionId, const QString& saveName ){
    QString result;
    QString fullName = saveName;
    if ( !saveName.endsWith( SUFFIX ) ){
        fullName = fullName + SUFFIX;
    }

    QString rootDir = _getRootDir( sessionId );
    QDirIterator it( rootDir, QDirIterator::Subdirectories );
    bool removed = false;
    while ( it.hasNext()){
        QString fileName = it.fileInfo().fileName();
        if ( fileName == fullName ){
            QDir dir;
            dir.remove( it.filePath());
            removed = true;
        }
        it.next();
    }
    if ( !removed ){
        result = "Unable to find snapshot "+saveName+" to remove.";
    }

    return result;
}

QString SnapshotsFile::_getRootDir(const QString& /*sessionId*/) const {
    return "/scratch/snapshots";
}
QList<Snapshot> SnapshotsFile::getSnapshots( const QString& sessionId ) const {
    QString rootDirName = _getRootDir(sessionId);
    QDir rootDir( rootDirName );
    QMap<QString,Snapshot> snapshotList;
    _processDirectory( sessionId, rootDir, snapshotList );
    return snapshotList.values();
}


void SnapshotsFile::_processDirectory(const QString& sessionId, const QDir& rootDir, QMap<QString,Snapshot>& snapshotList) const {
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
                QString strippedFileName = fileName.left( fileName.length() - SUFFIX.length());
                if ( !snapshotList.contains( strippedFileName ) ){
                    snapshotList.insert(strippedFileName, Snapshot(strippedFileName));
                }
                QString dirName = rootDir.dirName();
                if ( dirName == BASE_DIR ){
                    QDateTime lastModify = dit.fileInfo().lastModified();
                    QString lastModifyStr = lastModify.toString("ddd MMMM d yyyy");
                    snapshotList[strippedFileName].setCreatedDate( lastModifyStr );

                    QString description = read( sessionId, Carta::State::CartaObject::SNAPSHOT_INFO, strippedFileName);
                    snapshotList[strippedFileName].setDescription( description );
                }
                else {
                    //Add in the state interfaces
                    QString rootName = rootDir.dirName();
                    snapshotList[strippedFileName].setState(rootName, true);
                }
            }
        }
        else if (dit.fileInfo().isDir()){
            QString subDirPath = dit.fileInfo().absoluteFilePath();
            QDir subDir( subDirPath );
            _processDirectory( sessionId, subDir, snapshotList);
        }
    }
}


QString SnapshotsFile::read(const QString& sessionId, Carta::State::CartaObject::SnapshotType snapshotType,
            const QString& snapName ) const {
    QString result;
    QString fullName = snapName;
    if ( !snapName.endsWith( SUFFIX ) ){
        fullName = fullName + SUFFIX;
    }
    QString rootDir = _getRootDir( sessionId );
    QString filePath = rootDir + QDir::separator();
    QString snapshotDir = Snapshot::getNameForType( snapshotType );
    if ( snapshotDir.length() > 0 ){
        filePath = filePath+ snapshotDir + QDir::separator();
    }
    filePath = filePath + fullName;

    QFile file( filePath );
    if ( file.exists() ){
        result = _read( filePath);
    }
    return result;
}


QString SnapshotsFile::save( const QString& sessionId, const QString& snapshotType,
                const QString& snapName, const QString saveStr ){
    QString result;
    QString fullName = snapName;
    if ( !snapName.endsWith( SUFFIX ) ){
        fullName = fullName + SUFFIX;
    }
    QString rootDir = _getRootDir( sessionId );
    QString filePath = rootDir + QDir::separator();
    if ( snapshotType.length() > 0 ){
        filePath = filePath + snapshotType + QDir::separator();
    }
    filePath = filePath + fullName;
    bool saved = _save( filePath, saveStr );
    if ( !saved ){
        result = "There was a problem saving the "+snapshotType+" snapshot "+snapName;
    }
    return result;
}

bool SnapshotsFile::_save( const QString& fileLocation, const QString& stateStr ) const {
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

QString SnapshotsFile::_read( const QString& fileLocation ) const {
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

QList<Snapshot> SnapshotsFile::_updateSnapshots( const QString& sessionId ){
    QString rootDirName = _getRootDir(sessionId);
    QDir rootDir( rootDirName );
    QMap<QString,Snapshot> snapshotList;
    _processDirectory( sessionId, rootDir, snapshotList );
    return snapshotList.values();
}

SnapshotsFile::~SnapshotsFile(){

}
}
}
