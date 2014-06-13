#include "CustomWebPage.h"
#include <QDebug>

CustomWebPage::CustomWebPage(QObject *parent) :
    QWebPage(parent)
{
}

void CustomWebPage::javaScriptConsoleMessage(
        const QString & message, int lineNumber, const QString & sourceID)
{
    qDebug() << "cLog " << sourceID + ":"
            + QString::number(lineNumber)
            + ":" + message;
}

