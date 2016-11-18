#include <Python.h>
#include "PyCppPlugin.h"
#include "pluginBridge.h"
#include "CartaLib/Hooks/ColormapsScalar.h"
#include <QPainter>
#include <QDebug>
#include <dlfcn.h>
#include <csignal>

static struct sigaction oldSigIntAction;
typedef Carta::Lib::Hooks::LoadPlugin LoadPlugin;

static void mySigintHandler( int sig)
{
    if( sig != SIGINT) return;
    qWarning() << "Hey, you pressed ctrl-c";
    if( oldSigIntAction.sa_handler != SIG_DFL && oldSigIntAction.sa_handler != SIG_IGN) {
        qWarning() << "Calling old handler, stay put...";
        oldSigIntAction.sa_handler(sig);
        qWarning() << "Old handler called, are you still there?";
    }
    qWarning() << "Calling exit(0)";
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
                // << (void *) (oldSigIntAction.sa_restorer)
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

    // test of pyobject interaction
//    PyObject * pyo = pb_testGetObj();
//    qDebug() << "pyo refcnt" << Py_REFCNT(pyo);
//    std::string res = pb_testRunMethod( pyo, 3.5);
//    Py_XDECREF(pyo);
//    QString qres = res.c_str();
//    qDebug() << "res=" << qres;

    // make sure this plugin has name.py module
    QString fname = QString( "%1/%2.py")
                    .arg( params.pluginDir).arg( params.json.name);
    qDebug() << "Asking python to load" << fname;
    std::string cfname = fname.toStdString();
    std::string cfmodname = params.json.name.toStdString();
    m_pyModId = pb_loadModule( cfname, cfmodname);
//    m_pyModId = pb_loadModule( fname.toStdString(), params.json.name.toStdString());
    qDebug() << "m_pyModId=" << m_pyModId;
    if( m_pyModId < 0) {
        throw "Forget it";
    }
    qDebug() << "Do we have prerender hook:" << pb_hasPreRenderHook(m_pyModId);

}


namespace colormap_impl
{

class ColormapHelper : public Carta::Lib::PixelPipeline::IColormapNamed
{
    CLASS_BOILERPLATE( ColormapHelper);

public:
    ColormapHelper( int pluginId, PyObject * obj) {
        m_pluginId = pluginId;
        m_pyObj = obj;
        Py_XINCREF( m_pyObj);
    }

    virtual ~ColormapHelper() {
        Py_XDECREF( m_pyObj);
    }

    PyObject * m_pyObj;
    int m_pluginId;

    virtual QString name() override
    {
        return pb_colormapScalarGetName( m_pyObj).c_str();
    }
    virtual void convert(norm_double val, NormRgb & nrgb) override
    {
        pb_colormapScalarConvert( m_pyObj, val, & nrgb[0]);
    }
};

}

bool PyCppPlug::handleHook(BaseHook & hookData)
{
    qDebug() << "PyCppPlug " << m_params.json.name << " is handling hook #" << hookData.hookId();

    if( hookData.hookId() == PreRender::staticId) {
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

    if( hookData.is<Carta::Lib::Hooks::ColormapsScalarHook>()) {
        Carta::Lib::Hooks::ColormapsScalarHook & hook =
                static_cast<Carta::Lib::Hooks::ColormapsScalarHook &>( hookData);
        // get the list of raw python objects representing the colormaps
        std::vector<PyObject*> rawList = pb_colormapScalarGetColormaps( m_pyModId);
        qDebug() << "found" << rawList.size() << "colormaps";
        // wrap them up
        for( PyObject * pyCmap : rawList) {
            qDebug() << "pycmap refcnt" << Py_REFCNT(pyCmap);
            auto wrappedCmap = std::make_shared<colormap_impl::ColormapHelper>( m_pyModId, pyCmap);
            hook.result.push_back( wrappedCmap);
        }
        return true;
    }
    qWarning() << "PyCppPlug:: Sorrry, don't know how to handle this hook" << hookData.hookId();
    return false;
}

std::vector<HookId> PyCppPlug::getInitialHookList()
{
    // compile the list of hooks
    std::vector<HookId> list;
    if( pb_hasPreRenderHook( m_pyModId)) {
        list.push_back( PreRender::staticId);
    }

    if( pb_hasColormapScalarHook( m_pyModId)) {
        qWarning() << "PyCppPlug: has colormaps";
        list.push_back( Carta::Lib::Hooks::ColormapsScalarHook::staticId);
    }
    else {
        qWarning() << "PyCppPlug: does not have colormaps";
    }

    // return the list
    return list;
}

void PyCppPlug::initialize(const IPlugin::InitInfo & /*InitInfo*/)
{
}
