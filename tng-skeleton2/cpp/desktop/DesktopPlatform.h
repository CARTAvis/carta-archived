/**
 *
 **/

#include "common/IPlatform.h"
#include <QObject>

#ifndef DESKTOP_DESKTOPPLATFORM_H
#define DESKTOP_DESKTOPPLATFORM_H

class MainWindow;
class DesktopConnector;

class DesktopPlatform : public QObject, public IPlatform
{
    Q_OBJECT

public:

    DesktopPlatform( int argc, char ** argv);

    virtual IConnector * connector() Q_DECL_OVERRIDE;

public slots:

    void goFullScreen();

protected:

    int m_argc; // = 0;
    char ** m_argv; // = nullptr;
    MainWindow * m_mainWindow; // = nullptr;
    DesktopConnector * m_connector; // = nullptr;

};

#endif // DESKTOP_DESKTOPPLATFORM_H
