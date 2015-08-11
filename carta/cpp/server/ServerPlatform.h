#pragma once

#include "ServerConnector.h"
#include "core/IPlatform.h"
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

    /// return the CARTA Root directory
    virtual QString getCARTADirectory() Q_DECL_OVERRIDE;

    /// returns true so that access to the file system will
    /// be limited, among other things.
    virtual bool isSecurityRestricted() const Q_DECL_OVERRIDE;

protected:

    ServerConnector * m_connector;
    QStringList m_initialFileList;
};

