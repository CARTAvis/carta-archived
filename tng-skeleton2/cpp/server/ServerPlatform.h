#ifndef SERVER_SERVERPLATFORM_H
#define SERVER_SERVERPLATFORM_H

#include "common/IPlatform.h"
#include <QObject>

class ServerPlatform : public IPlatform
{

public:

    ServerPlatform( int argc, char ** argv);

    virtual IConnector *connector() Q_DECL_OVERRIDE;

protected:

    IConnector * m_connector;

};

#endif // SERVER_SERVERPLATFORM_H
