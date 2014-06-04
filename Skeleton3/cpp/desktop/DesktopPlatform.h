/**
 * Creates the GUI...
 **/

#pragma once

#include "common/IPlatform.h"
#include <QStringList>
#include <QObject>

class MainWindow;
class DesktopConnector;
namespace CmdLine { class ParsedInfo; }

// TODO: do we really need to inherit from QObject?????

class DesktopPlatform : public QObject, public IPlatform
{

    Q_OBJECT

public:

    /// initialize the platform
    /// create a gui displaying the provided url inside the built-in browser
    DesktopPlatform(const CmdLine::ParsedInfo & cmdLineInfo);

    /// returns the appropriate connector for this platform
    virtual IConnector * connector() Q_DECL_OVERRIDE;

    /// return the list of files to load
    virtual const QStringList & initialFileList() Q_DECL_OVERRIDE;

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
