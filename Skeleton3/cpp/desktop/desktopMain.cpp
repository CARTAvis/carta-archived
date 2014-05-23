/*
 * This is the desktop main
 */

#include <QApplication>
#include <iostream>

#include "DesktopPlatform.h"
#include "common/Viewer.h"
#include "common/MyQApp.h"
#include "common/Globals.h"

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
} // qtMessageHandler

int main(int argc, char ** argv)
{
        qInstallMessageHandler( qtMessageHandler);

        // setup Qt
        MyQApp app(argc, argv);

        // hack for now (to get filename)
        if( argc < 2) {
            qFatal( "Must give me filename");
        }

        Globals::setFname( argv[1]);

        // run the viewer with the proper platorm
        Viewer viewer( new DesktopPlatform( argc, argv));
        viewer.start();

        return app.exec();
} // main

