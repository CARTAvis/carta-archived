#include "GenericPlugin.h"
#include <QDebug>
#include <QPainter>

GenericPlugin::GenericPlugin(QObject *parent) :
    QObject(parent)
{
}

bool GenericPlugin::handleHook(BaseHook &hookData)
{
    //qDebug() << "GenericPlugin is handling hook #" << hookData.hookId();
    if( hookData.is<Initialize>()) {
//        Initialize & initHook = static_cast<Initialize &>( hookData);

        qDebug() << "Woohoo, generic plugin received initialize request.";

//        qDebug() << "You should see debug from Initialize below";
//        initHook.debug();

        return true;
    }

    if( hookData.hookId() == PreRender::staticId ) {
//        PreRender & hook = static_cast<PreRender &>( hookData);

        //qDebug() << "Prerender hook received by generic plugin";
        //qDebug() << "  " << hook.paramsPtr->viewName;
        //qDebug() << "  " << hook.paramsPtr->imgPtr->size();

        /*QPainter p( hook.paramsPtr->imgPtr);
        QString txt = "(C) Generic Plugin";
        QRectF rect = hook.paramsPtr->imgPtr->rect();
        p.setFont( QFont( "Arial", 20));
        rect = p.boundingRect( rect, Qt::AlignRight | Qt::AlignBottom, txt);
        p.fillRect( rect, QColor( 0,0,0,128));
        p.setPen( QColor( "white"));
        p.drawText( hook.paramsPtr->imgPtr->rect(), Qt::AlignRight | Qt::AlignBottom, txt);
*/
        return true;
    }


    qDebug() << "Sorrry, dont' know how to handle this hook";
    return false;
}

std::vector<HookId> GenericPlugin::getInitialHookList()
{
    return {
        Initialize::staticId,
        PreRender::staticId
    };
}
