#include "ServerPlatform.h"
#include "ServerConnector.h"

#include "CSI/Standard/CsiStandard.h"
#include "CSI/Standard/CsiThreading.h"
#include "CSI/PureWeb/StateManager/StateManager.h"
#include <QObject>
#include <QEvent>
#include <QApplication>
#include <iostream>


ServerPlatform::ServerPlatform(int argc, char **argv)
{
    m_argc = argc;
    m_argv = argv;
}

IConnector *ServerPlatform::createConnector()
{
    return new ServerConnector( m_argc, m_argv);
}
