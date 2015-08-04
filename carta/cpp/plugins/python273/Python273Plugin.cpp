#include "Python273Plugin.h"
#include "PyCppPlugin.h"
#include "CartaLib/Hooks/LoadPlugin.h"
#include <QDebug>
#include <QPainter>


typedef Carta::Lib::Hooks::LoadPlugin LoadPlugin;

Python273Plugin::Python273Plugin(QObject *parent) :
    QObject(parent)
{
}

bool Python273Plugin::handleHook(BaseHook & hookData)
{
    qDebug() << "Python273Plugin is handling hook #" << hookData.hookId();

    if( hookData.hookId() == LoadPlugin::staticId) {
        LoadPlugin & hook = static_cast<LoadPlugin &>( hookData);
        hook.result = nullptr;
        qDebug() << "Python273Plugin is asked to load plugin" << hook.paramsPtr->json.name;
        if( hook.paramsPtr->json.typeString != "python") {
            return false;
        }
        try {
            hook.result = new PyCppPlug( * hook.paramsPtr);
            return true;
        } catch( ...) {
            return false;
        }
    }

    qWarning() << "Sorrry, dont' know how to handle this hook";
    return false;
}

std::vector<HookId> Python273Plugin::getInitialHookList()
{
    return {
        LoadPlugin::staticId
    };
}



void Python273Plugin::initialize(const InitInfo & /*InitInfo*/)
{
}


