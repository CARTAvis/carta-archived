/**
 *
 **/

#include "common/IPlatform.h"

#ifndef DESKTOP_DESKTOPPLATFORM_H
#define DESKTOP_DESKTOPPLATFORM_H

class MainWindow;

class DesktopPlatform : public IPlatform
{
public:
    DesktopPlatform( int argc, char ** argv);

    virtual IConnector * createConnector() Q_DECL_OVERRIDE;

protected:
    int m_argc; // = 0;
    char ** m_argv; // = nullptr;
    MainWindow * m_mainWindow; // = nullptr;
};

#endif // DESKTOP_DESKTOPPLATFORM_H
