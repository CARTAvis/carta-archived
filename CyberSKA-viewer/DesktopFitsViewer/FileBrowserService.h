#ifndef FILEBROWSERSERVICE_H
#define FILEBROWSERSERVICE_H

#include <QObject>
#include <QDir>
//#include <CSI/Typeless/Typeless.h>
#include "PureWeb.h"
#include "FitsViewerLib/common.h"

class FileBrowserService : public QObject {

    Q_OBJECT

public:
    FileBrowserService( QObject * parent = 0, QString rootDir = QString());

signals:
    void openFile( QString);
protected:

    typedef enum { Reset, Directory, FileFits, FileIQU, FileCasa, FileUnknown } EntryType;
    struct Entry {
        EntryType type;
        QString fname;
        QString description;
        qint64 size;
    };
    struct Instance {
        std::vector < Entry > entries;
        QString currDir;
        QList<QString> pathState;
        QString id;
        qint64 stateNumber;
        int pathStateIndex;

        Instance() : stateNumber(0), pathStateIndex(0) {;}
    };

    void onPureWebCmd(CSI::Guid sessionid, CSI::Typeless command, CSI::Typeless responses);
    void reset( Instance & instance);
    void sendState( Instance & instance);
    static std::string type2str(EntryType t);


    typedef std::map< QString, Instance > InstanceMap;
    InstanceMap m_instances;

    QString m_rootDir;

    Q_DISABLE_COPY( FileBrowserService)
};

/*
class FileBrowserService : public QObject {

    Q_OBJECT
public:
    FileBrowserService( const QString & id);
    void processCommand( CSI::Typeless command);
protected:
    QDir m_currDir;
    QString m_id;
};
*/

#endif // FILEBROWSERSERVICE_H
