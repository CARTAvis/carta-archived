#include "Data/Snapshots.h"
#include "Data/Selection.h"
#include "CartaLib/CartaLib.h"
#include <QDebug>
#include <QDirIterator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace Carta {

namespace Data {

const QString Snapshots::SNAPSHOT_SELECTED = "selected";
const QString Snapshots::CLASS_NAME = "Snapshots";
const QString Snapshots::DEFAULT_SAVE = "default";
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
}

QString Snapshots::_getRootDir(const QString& /*sessionId*/) const {
    return "/tmp/Sessions";
}

QString Snapshots::getSnapshots( const QString& sessionId ) const {
    QString rootDirName = _getRootDir(sessionId);
    QDir rootDir( rootDirName );
    QString jsonTree;
    QJsonObject rootObj;
    _processDirectory( rootDir, rootObj );
    QJsonDocument document( rootObj );
    QByteArray textArray = document.toJson();
    QString jsonText(textArray);
    return jsonText;
}

void Snapshots::_processDirectory(const QDir& rootDir, QJsonObject& rootObj) const {
    if (!rootDir.exists()) {
        return;
    }

    QJsonArray sessionArray;
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
                _makeFileNode(sessionArray, fileName);
            }
        }
    }
    rootObj.insert("sessions", sessionArray);
}

void Snapshots::_makeFileNode(QJsonArray& parentArray, const QString& fileName) const {
    QJsonObject obj;
    int fileNameLength = fileName.size() - SUFFIX.size();
    QString shortName = fileName.left( fileNameLength );
    QJsonValue fileValue( shortName );
    obj.insert("name", fileValue);
    parentArray.append(obj);
}

void Snapshots::_initializeState(){
    m_state.insertValue( SNAPSHOT_SELECTED, DEFAULT_SAVE );
    m_state.flushState();
}


Snapshots::~Snapshots(){

}
}
}
