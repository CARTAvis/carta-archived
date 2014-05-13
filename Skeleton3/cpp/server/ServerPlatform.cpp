#include "ServerPlatform.h"
#include "ServerConnector.h"

ServerPlatform::ServerPlatform( int argc, char ** argv)
{
    m_connector = new ServerConnector( argc, argv);
}

IConnector * ServerPlatform::connector()
{
    return m_connector;
}
