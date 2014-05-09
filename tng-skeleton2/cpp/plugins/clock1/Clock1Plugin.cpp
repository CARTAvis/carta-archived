#include "Clock1Plugin.h"
#include <QDebug>
#include <QPainter>
#include <QTime>

Clock1Plugin::Clock1Plugin(QObject *parent) :
    QObject(parent)
{
}

bool Clock1Plugin::handleHook(BaseHook &hookData)
{
    qDebug() << "ClockPlugin is handling hook #" << hookData.hookId();
    if( hookData.hookId() == Initialize::StaticHookId ) {
        Initialize & initHook = static_cast<Initialize &>( hookData);

        qDebug() << "Woohoo, clock plugin received initialize request.";

        qDebug() << "You should see debug from Initialize below";
        initHook.debug();

        return true;
    }

    if( hookData.hookId() == PreRender::StaticHookId ) {
        PreRender & hook = static_cast<PreRender &>( hookData);

        qDebug() << "Prerender hook received by clock plugin";
        qDebug() << "  " << hook.paramsPtr->viewName;
        qDebug() << "  " << hook.paramsPtr->imgPtr->size();

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


    qDebug() << "Sorrry, dont' know how to handle this hook";
    return false;
}

std::vector<HookId> Clock1Plugin::getInitialHookList()
{
    return {
        Initialize::StaticHookId,
        PreRender::StaticHookId
    };
}
