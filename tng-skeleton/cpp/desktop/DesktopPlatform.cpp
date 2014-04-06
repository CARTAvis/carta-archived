/**
 *
 **/

#include "DesktopPlatform.h"
#include "DesktopConnector.h"
#include "MainWindow.h"

#include <QtWidgets>
#include <QWebSettings>


DesktopPlatform::DesktopPlatform(int argc, char **argv)
{
    QUrl url;
    if (argc > 1) {
        url = QUrl::fromUserInput(argv[1]);
    } else {
        url = QUrl("qrc:///html5/desktop/desktopIndex.html");
    }

    // enable web inspector
    QWebSettings::globalSettings()->setAttribute( QWebSettings::DeveloperExtrasEnabled, true);

    m_mainWindow = new MainWindow(url);
    m_mainWindow-> show();
}

IConnector * DesktopPlatform::createConnector()
{
    return new DesktopConnector( m_mainWindow);
}
