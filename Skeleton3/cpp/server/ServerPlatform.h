#ifndef SERVER_SERVERPLATFORM_H
#define SERVER_SERVERPLATFORM_H

#include "ServerConnector.h"
#include "common/IPlatform.h"
#include <QtGlobal>
#include <QStringList>

class ServerPlatform : public IPlatform
{

public:

    /// initialize the platform
    /// also parse the command line arguments
    ServerPlatform();

    /// returns the appropriate connector for this platform
    virtual IConnector *connector() Q_DECL_OVERRIDE;

    /// the list comes from url encoded parameters
    virtual const QStringList & initialFileList() Q_DECL_OVERRIDE;

protected:

    ServerConnector * m_connector;
    QStringList m_initialFileList;

//    MyQApp * m_app;

};

#endif // SERVER_SERVERPLATFORM_H
