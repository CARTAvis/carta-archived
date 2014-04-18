/*
 * This is the desktop main
 */

#include <QApplication>
#include <iostream>

#include "DesktopPlatform.h"
#include "common/Viewer.h"


static
void handler(QtMsgType type, const QMessageLogContext &context, const QString &pmsg)
{
    QString msg = pmsg;
    if( ! msg.endsWith( '\n')) {
        msg += '\n';
    }
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "%s", localMsg.constData());
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
}

int main(int argc, char ** argv)
{
    qInstallMessageHandler( handler);

    // setup Qt
    QApplication app(argc, argv);

    std::cerr << "Command line args:\n";
    for( int i = 0 ; i < argc ; i ++ ) {
        std::cerr << "   " << i << ".) " << argv[i] << "\n";
    }

    // create a platform
    IPlatform * platform = new DesktopPlatform( argc, argv);

    // run the viewer with this platorm
    Viewer viewer( platform);
    viewer.start();

    return app.exec();
}
