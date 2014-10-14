#include <Python.h>
#include "PyCppPlugin.h"
#include "pluginBridge.h"
#include <QPainter>
#include <QDebug>
#include <dlfcn.h>
#include <csignal>

static struct sigaction oldSigIntAction;

static void mySigintHandler( int sig)
{
    if( sig != SIGINT) return;
    qWarning() << "Hey, you pressed ctrl-c";
    if( oldSigIntAction.sa_handler != SIG_DFL && oldSigIntAction.sa_handler != SIG_IGN) {
        qWarning() << "Calling old handler, stay put...";
        oldSigIntAction.sa_handler(sig);
        qWarning() << "Old handler called, are you still there?";
    }
    exit(0);
}

static void enableCtrlC()
{
    struct sigaction newSigIntAction;

    oldSigIntAction.sa_handler = SIG_DFL;

    newSigIntAction.sa_handler = mySigintHandler;
    sigemptyset (& newSigIntAction.sa_mask);
    newSigIntAction.sa_flags = 0;

    if( ! sigaction (SIGINT, 0, & oldSigIntAction)) {
        QStringList maskList;
        for( int i = 0 ; i < 32 ; i ++) {
            if( sigismember( & oldSigIntAction.sa_mask, i)) {
                maskList.append( strsignal(i));
            }
        }
        qDebug() << "old sigint:" << (void *)(oldSigIntAction.sa_handler)
                 << (void *) (oldSigIntAction.sa_sigaction)
                 << (void *) (oldSigIntAction.sa_restorer)
                 << oldSigIntAction.sa_flags
                 << maskList.join(",");
    }

    // retrieve the old action and set the new action...
    if( sigaction (SIGINT, & newSigIntAction, 0)) {
        qWarning() << "Could not install ctrl-c handler!";
    }
    else {
        qWarning() << "Ctrl-c should work now";
    }
    qDebug() << "old sigint:" << (void *)(oldSigIntAction.sa_handler);
}

/// initializes the python bridge
/// only does the initialization on the first call, subsequent calls are ignored
static void initPythonBridgeOnce()
{
    static bool alreadyInitialized = false;
    if( alreadyInitialized) return;
    alreadyInitialized = true;

    dlopen("libpython2.7.so", RTLD_LAZY | RTLD_GLOBAL);

    Py_InitializeEx( 0); // make ctrl-c work?

    // try to enable ctrl-c...
    enableCtrlC();

    // call cython generated code (pluginBridge.pyx)
    initpluginBridge();
}

PyCppPlug::PyCppPlug(const LoadPlugin::Params & params)
    : QObject( nullptr)
    , m_params( params)
{
    qDebug() << "PyCppPlug constructing from dir:" << m_params.pluginDir;

    initPythonBridgeOnce();

    // make sure this plugin has name.py module
    QString fname = QString( "%1/%2.py")
                    .arg( params.pluginDir).arg( params.json.name);
    qDebug() << "Asking python to load" << fname;
    m_pyModId = pb_loadModule( fname.toStdString(), params.json.name.toStdString());
    qDebug() << "m_pyModId=" << m_pyModId;
    if( m_pyModId < 0) {
        throw "Forget it";
    }
    qDebug() << "Do we have prerender hook:" << pb_hasPreRenderHook(m_pyModId);
}

bool PyCppPlug::handleHook(BaseHook & hookData)
{
    qDebug() << "PyCppPlug " << m_params.json.name << " is handling hook #" << hookData.hookId();

    if( hookData.hookId() == PreRender::StaticHookId) {
        PreRender & hook = static_cast<PreRender &>( hookData);

        qDebug() << "Prerender hook received by PyCppPlug plugin";

        QPainter p( hook.paramsPtr->imgPtr);
        QString txt = "Py" + m_params.json.name;
        QRectF rect = hook.paramsPtr->imgPtr->rect();
        p.setFont( QFont( "Arial", 20));
        rect = p.boundingRect( rect, Qt::AlignLeft | Qt::AlignTop, txt);
        p.fillRect( rect, QColor( 0,0,0,128));
        p.setPen( QColor( "yellow"));
        p.drawText( hook.paramsPtr->imgPtr->rect(), Qt::AlignLeft | Qt::AlignTop, txt);

        QImage & img = * (hook.paramsPtr->imgPtr);
        pb_callPreRenderHook( m_pyModId, img.width(), img.height(),
                              img.bytesPerLine(), img.bits());

        return true;
    }
    qWarning() << "Sorrry, dont' know how to handle this hook";
    return false;
}

std::vector<HookId> PyCppPlug::getInitialHookList()
{
    // TODO: this list should be based on what is implemented in python
    if( pb_hasPreRenderHook( m_pyModId)) {
        return {
            PreRender::StaticHookId
        };
    }
    else {
        return {};
    }
}

void PyCppPlug::initialize(const IPlugin::InitInfo & /*InitInfo*/)
{
}
