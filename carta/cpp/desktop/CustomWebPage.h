/// subclass of QWebPage is needed to redefine javaScriptConsoleMessage

#pragma once

#include <QWebPage>

class CustomWebPage : public QWebPage
{
    Q_OBJECT

public:

    explicit CustomWebPage(QObject *parent = 0);

protected:

    void javaScriptConsoleMessage(const QString& message, int lineNumber, const QString& sourceID);

signals:

public slots:

};

