/**
 *
 **/

#include "DesktopPlatform.h"
#include "DesktopConnector.h"
#include "MainWindow.h"
#include "common/CmdLine.h"
#include "common/Globals.h"

#include <QtWidgets>
#include <QWebSettings>


/// custom Qt message handler
static
void qtMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &pmsg)
{
    QString msg = pmsg;
    if( !msg.endsWith( '\n')) {
        msg += '\n';
    }
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s", localMsg.constData());
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
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
//    auto & cmdLineInfo = CmdLine::info();
    auto & cmdLineInfo = * Globals::instance()->cmdLineInfo();
    if( cmdLineInfo.htmlPath().isEmpty()) {
        url = QUrl("qrc:///html5/desktop/desktopIndex.html");
    } else {
        url = QUrl::fromUserInput( cmdLineInfo.htmlPath());
    }

    // get the filename sfrom the command line
    m_initialFileList = cmdLineInfo.fileList();
    if( m_initialFileList.isEmpty()) {
        qFatal( "No input files to open...");
    }

//    Globals::setFname( argv[1]);

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


const QStringList & DesktopPlatform::initialFileList()
{
    return m_initialFileList;
}
