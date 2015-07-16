/**
  * ServerPlatform does not really do much at the moment, other than creating
  * a server connector.
  **/

#include "ServerPlatform.h"
#include "ServerConnector.h"

/// custom Qt message handler
/// it's inside platform implementation because we may want to do different things
/// with debug messages on server than on desktop...
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

ServerPlatform::ServerPlatform()
{
    // install a custom message handler
    qInstallMessageHandler( qtMessageHandler);

    // create the connector
    m_connector = new ServerConnector();
}

IConnector * ServerPlatform::connector()
{
    return m_connector;
}


const QStringList & ServerPlatform::initialFileList()
{
    auto params = m_connector-> urlParams();
    auto it = this-> m_connector-> urlParams().find( "file");
    if( it != this-> m_connector-> urlParams().end()) {
        m_initialFileList << ( * it).second;
    }
    return m_initialFileList;
}

QString ServerPlatform::getCARTADirectory()
{
   return "/scratch/";
}

bool ServerPlatform::isSecurityRestricted() const {
    return true;
}
