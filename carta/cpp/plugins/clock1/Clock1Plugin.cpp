#include "Clock1Plugin.h"
#include "CartaLib/Hooks/Initialize.h"
#include <QDebug>
#include <QPainter>
#include <QTime>

typedef Carta::Lib::Hooks::Initialize Initialize;

Clock1Plugin::Clock1Plugin(QObject *parent) :
    QObject(parent)
{
}

bool Clock1Plugin::handleHook(BaseHook &hookData)
{
    if( BaseHook::isHook<Initialize>( hookData)) {
        return true;
    }

    if( BaseHook::isHook<PreRender>( hookData)) {
        PreRender & hook = static_cast<PreRender &>( hookData);

        QPainter p( hook.paramsPtr->imgPtr);
        QString txt = QTime::currentTime().toString();
        QRectF rect = hook.paramsPtr->imgPtr->rect();
        p.setFont( QFont( "Arial", 20));
        rect = p.boundingRect( rect, Qt::AlignRight | Qt::AlignTop, txt);
        p.fillRect( rect, QColor( 0,0,0,128));
        p.setPen( QColor( "yellow"));
        p.drawText( hook.paramsPtr->imgPtr->rect(), Qt::AlignRight | Qt::AlignTop, txt);

        return true;
    }

    qWarning() << "Sorrry, dont' know how to handle this hook" << hookData.hookId();
    return false;
}

std::vector<HookId> Clock1Plugin::getInitialHookList()
{
    return {
        Initialize::staticId,
        PreRender::staticId
    };
}
