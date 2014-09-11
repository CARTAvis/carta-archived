#include <Python.h>
#include "PyCppPlugin.h"
#include "pluginBridge.h"
#include <QPainter>
#include <QDebug>
#include <dlfcn.h>

static void initPythonBridgeOnce()
{
    static bool alreadyInitialized = false;
    if( alreadyInitialized) return;
    alreadyInitialized = true;

    dlopen("libpython2.7.so", RTLD_LAZY | RTLD_GLOBAL);

    Py_InitializeEx( 0); // make ctrl-c work?
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


//        std::vector<char> data;
//        for( int row = 0 ; row < img.height() ; row ++) {
//            auto p = img.constScanLine(row);
//            for( int col = 0 ; col < img.width(); col ++) {
//                data.push_back( * p);
//                p ++;
//                data.push_back( * p);
//                p ++;
//                data.push_back( * p);
//                p ++;
//            }
//        }

//        qDebug() << "pixels:" << data.size();
//        qDebug() << "pixel 0 =" << double(data[0]);
//        pb_callPreRenderHook( m_pyModId, img.width(), img.height(), data);
//        qDebug() << "-pixels:" << data.size();
//        qDebug() << "-pixel 0 =" << double(data[0]);

//        size_t ind = 0;
//        for( int row = 0 ; row < img.height() ; row ++) {
//            auto p = img.scanLine(row);
//            for( int col = 0 ; col < img.width(); col ++) {
//                * p = data[ind ++];
//                p ++;
//                * p = data[ind ++];
//                p ++;
//                * p = data[ind ++];
//                p ++;
//            }
//        }

        if( img.width() * 3 != img.bytesPerLine()) {
            qWarning() << "################" << img.width() *3 << img.bytesPerLine();
        }
        qDebug() << "cpp: stride should be = " << img.bytesPerLine();
        pb_callPreRenderHook( m_pyModId, img.width(), img.height(),
                              img.bytesPerLine(), img.bits());


        return true;

        // TODO: we need to pass the image to python, then put the result back into c++
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
