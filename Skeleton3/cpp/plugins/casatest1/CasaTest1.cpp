#include "CasaTest1.h"
#include <QDebug>
#include <QPainter>
#include <QTime>
#include <casa/Exceptions/Error.h>
#include <images/Images/FITSImage.h>
#include <images/Images/ImageExpr.h>
#include <images/Images/ImageExprParse.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Quanta.h>

CasaTest1::CasaTest1(QObject *parent) :
    QObject(parent)
{
}

bool CasaTest1::handleHook(BaseHook &hookData)
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
        QString txt = "casa";
        QRectF rect = hook.paramsPtr->imgPtr->rect();
        p.setFont( QFont( "Arial", 12));
        rect = p.boundingRect( rect, Qt::AlignLeft | Qt::AlignTop, txt);
        p.fillRect( rect, QColor( 0,0,0,128));
        p.setPen( QColor( "white"));
        p.drawText( hook.paramsPtr->imgPtr->rect(), Qt::AlignLeft | Qt::AlignTop, txt);

        return true;
    }

    if( hookData.hookId() == LoadImage::StaticHookId) {
        LoadImage & hook = static_cast<LoadImage &>( hookData);
        auto fname = hook.paramsPtr->fileName;
        qDebug() << "casatest plugin trying to load image: " << fname;

        // try to open the image using casacore's fits implementation
        casa::ImageInterface<casa::Float> * img = nullptr;
        try {
            img = new casa::FITSImage( fname.toStdString());
        } catch (...) {
        }

        // if we succeeded, we are done
        if( ! img) {
            return false;
        }


        qDebug() << "Image opened using casacore\n";

        //            virtual double operator ()(const std::vector<int> &cursor)
        //            {
        //                casa::IPosition casaCursor = cursor;
        //                return m_casaImage-> operator ()( casaCursor);
        //            }
        int width, height;
        {
            auto shape = img-> shape();
            width = shape[0];
            height = shape[1];
        }

        QImage tmpImg( width, height, QImage::Format_ARGB32);
        tmpImg.fill( QColor( "pink"));
        hook.result = tmpImg;
        return true;

    }


    qDebug() << "Sorrry, dont' know how to handle this hook";
    return false;
}

std::vector<HookId> CasaTest1::getInitialHookList()
{
    return {
        Initialize::StaticHookId,
                PreRender::StaticHookId,
                LoadImage::StaticHookId
    };
}
