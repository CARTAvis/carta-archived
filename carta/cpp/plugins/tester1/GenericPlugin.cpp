#include "GenericPlugin.h"
#include "CartaLib/Hooks/Initialize.h"
#include <QDebug>
#include <QPainter>

typedef Carta::Lib::Hooks::Initialize Initialize;

GenericPlugin::GenericPlugin(QObject *parent) :
    QObject(parent)
{
}

bool GenericPlugin::handleHook(BaseHook &hookData)
{
    if( hookData.is<Initialize>()) {
        qDebug() << "Woohoo, generic plugin received initialize request.";
        return true;
    }

    if( hookData.hookId() == PreRender::staticId ) {
        return true;
    }


    qDebug() << "Sorrry, dont' know how to handle this hook";
    return false;
}

std::vector<HookId> GenericPlugin::getInitialHookList()
{
    return {
        Initialize::staticId
    };
}
