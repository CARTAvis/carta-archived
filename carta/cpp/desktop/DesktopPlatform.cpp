/**
 *
 **/

#include "DesktopPlatform.h"
#include "DesktopConnector.h"
#include "MainWindow.h"
#include "core/CmdLine.h"
#include "core/Globals.h"

#include <QtWidgets>
#include <QWebSettings>
#include <QDesktopWidget>
#include <unistd.h>

std::string warningColor, criticalColor, fatalColor, resetColor;
static void initializeColors() {
    static bool initialized = false;
    if( initialized) return;
    initialized = true;
    if( isatty(3)) return;
    warningColor = "\033[1m\033[36m";
    criticalColor = "\033[31m";
    fatalColor = "\033[41m";
    resetColor = "\033[0m";
}

static const int m_isatty = isatty(3);

/// custom Qt message handler
static
void qtMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &pmsg)
{
    initializeColors();

    QString msg = pmsg;
    if( ! msg.endsWith( '\n')) {
        msg += '\n';
    }
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s", localMsg.constData());
        break;
    case QtWarningMsg:
        fprintf(stderr, "%sWarning: %s (%s:%u, %s)%s\n",
                warningColor.c_str(),
                localMsg.constData(), context.file, context.line, context.function,
                resetColor.c_str());
        break;
    case QtCriticalMsg:
        fprintf(stderr, "%sCritical: %s (%s:%u, %s)%s\n",
                criticalColor.c_str(),
                localMsg.constData(), context.file, context.line, context.function,
                resetColor.c_str());
        break;
    case QtFatalMsg:
        fprintf(stderr, "%sFatal: %s (%s:%u, %s)%s\n",
                fatalColor.c_str(),
                localMsg.constData(), context.file, context.line, context.function,
                resetColor.c_str());
        abort();
    }

} // qtMessageHandler

DesktopPlatform::DesktopPlatform()
    : QObject( nullptr)
{
    // install a custom message handler
    qInstallMessageHandler( qtMessageHandler);
    
    // figure out which url to use to load the html5 component
    // by default it's the locally compiled filesystem, but we let the developer
    // override it for debugging purposes
    QUrl url;
    auto & cmdLineInfo = * Globals::instance()->cmdLineInfo();
    if( cmdLineInfo.htmlPath().isEmpty()) {
        // check if we have index in qrc
        if( QFileInfo(":/html/desktopIndexRelease.html").exists()) {
            url = "qrc:///html/desktopIndexRelease.html";
        } else {
            url = "http://www.google.com";
        }
    } else {
        url = QUrl::fromUserInput( cmdLineInfo.htmlPath());
    }

    // get the filename sfrom the command line
    m_initialFileList = cmdLineInfo.fileList();

    // create the connector
    m_connector = new DesktopConnector();

    // enable web inspector
    QWebSettings::enablePersistentStorage( "/tmp/xyz");
    QWebSettings::globalSettings()->setAttribute( QWebSettings::DeveloperExtrasEnabled, true);
//    QWebSettings::globalSettings()->setAttribute( QWebSettings::Accelerated2dCanvasEnabled, true);

    // create main window
    m_mainWindow = new MainWindow();
    m_mainWindow-> resize( 1280, 800);

    // center the main window on the screen
    QDesktopWidget wid;
    int screenWidth = wid.screen()->width();
    int screenHeight = wid.screen()->height();
    int panelWidth = m_mainWindow->frameGeometry().width();
    int panelHeight = m_mainWindow->frameGeometry().height();
    m_mainWindow->setGeometry( (screenWidth/2)-(panelWidth/2), (screenHeight/2)-(panelHeight/2), panelWidth, panelHeight);

    // add platform and connector to JS exports
    m_mainWindow->addJSExport( "QtPlatform", this);
    m_mainWindow->addJSExport( "QtConnector", m_connector);

    // load the url
    m_mainWindow->loadUrl( url);

    // display the window
    m_mainWindow->show();
}

IConnector * DesktopPlatform::connector()
{
    return m_connector;
}

void DesktopPlatform::goFullScreen()
{
    m_mainWindow->showFullScreen();
}


const QStringList & DesktopPlatform::initialFileList()
{
    return m_initialFileList;
}

bool DesktopPlatform::isSecurityRestricted() const {
    return false;
}

QString DesktopPlatform::getCARTADirectory()
{
	return QDir::homePath().append("/CARTA/");
}
