#include "CustomWebPage.h"

#include <iostream>

CustomWebPage::CustomWebPage(QObject *parent) :
    QWebPage(parent)
{
}

void CustomWebPage::javaScriptConsoleMessage(
        const QString &message, int lineNumber, const QString & sourceID)
{
    QString logEntry = sourceID + ":"
            + QString::number(lineNumber)
            + ":" + message;
    std::cerr << logEntry.toStdString() << "\n";
}

