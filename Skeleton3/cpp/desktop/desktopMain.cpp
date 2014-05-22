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
void handler(QtMsgType type, const QMessageLogContext &context, const QString &pmsg)
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
} // handler

int main(int argc, char ** argv)
{

        qInstallMessageHandler( handler);

        // setup Qt
        MyQApp app(argc, argv);

        // setup platform
        Globals::setPlatform( new DesktopPlatform( argc, argv));

        // run the viewer with this platorm
        Viewer viewer( Globals::platform());
        viewer.start();

        return app.exec();
}

//#include "common/CallbackList.h"

//static void testCallbacks()
//{
//    CallbackList<int,QString> list;

//    list.add( [] (int x, QString s) {
//        qDebug() << "Closure x=" << x << " s=" << s;

//    });

//    list.callEveryone( 1.1, "Hello");
//}
