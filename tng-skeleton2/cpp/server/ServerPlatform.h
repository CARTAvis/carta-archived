#ifndef SERVER_SERVERPLATFORM_H
#define SERVER_SERVERPLATFORM_H

#include "common/IPlatform.h"

class ServerPlatform : public IPlatform
{
public:
    ServerPlatform( int argc, char ** argv);

    virtual IConnector *createConnector() Q_DECL_OVERRIDE;

protected:
    int m_argc;
    char ** m_argv;
};

#endif // SERVER_SERVERPLATFORM_H
