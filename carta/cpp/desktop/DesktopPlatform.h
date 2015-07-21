/**
 * Creates the GUI...
 **/

#pragma once

#include "core/IPlatform.h"
#include <QStringList>
#include <QObject>

class MainWindow;
class DesktopConnector;

// TODO: do we really need to inherit from QObject?????

class DesktopPlatform : public QObject, public IPlatform
{

    Q_OBJECT

public:

    /// initialize the platform
    /// create a gui displaying the provided url inside the built-in browser
    DesktopPlatform();

    /// returns the appropriate connector for this platform
    virtual IConnector * connector() Q_DECL_OVERRIDE;

    /// return the list of files to load
    virtual const QStringList & initialFileList() Q_DECL_OVERRIDE;

    /// return the CARTA Root directory
    virtual QString getCARTADirectory() Q_DECL_OVERRIDE;

    /// Returns false so the user, among other things can have access to
    /// the entire file system of the computer.
    virtual bool isSecurityRestricted() const Q_DECL_OVERRIDE;

public slots:

    void goFullScreen();

protected:

    int m_argc; // = 0;
    char ** m_argv; // = nullptr;
    MainWindow * m_mainWindow; // = nullptr;
    DesktopConnector * m_connector; // = nullptr;
    QStringList m_initialFileList;

//    MyQApp * m_app;


};
