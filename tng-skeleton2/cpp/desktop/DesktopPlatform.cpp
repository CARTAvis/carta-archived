/**
 *
 **/

#include "DesktopPlatform.h"
#include "DesktopConnector.h"
#include "MainWindow.h"

#include <QtWidgets>
#include <QWebSettings>

DesktopPlatform::DesktopPlatform(int argc, char **argv)
    : QObject( nullptr)
{
    QUrl url;
    if (argc > 1) {
        url = QUrl::fromUserInput(argv[1]);
    } else {
        url = QUrl("qrc:///html5/desktop/desktopIndex.html");
    }

    // create the connector
    m_connector = new DesktopConnector();

    // enable web inspector
    QWebSettings::globalSettings()->setAttribute( QWebSettings::DeveloperExtrasEnabled, true);

    // create main window
    m_mainWindow = new MainWindow();

    // add platform and connector to JS exports
    m_mainWindow->exportToJs( "QtPlatform", this);
    m_mainWindow->exportToJs( "QtConnector", m_connector);

    // load the url
    m_mainWindow->loadUrl( url);

    // display the window
    m_mainWindow->show();
}

IConnector * DesktopPlatform::connector()
{
//    if( ! m_connector) {
//        m_connector = new DesktopConnector();
//    }
    return m_connector;
}

void DesktopPlatform::goFullScreen()
{
    m_mainWindow->showFullScreen();
}
