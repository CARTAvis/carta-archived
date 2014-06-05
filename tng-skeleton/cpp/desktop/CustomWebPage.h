#ifndef DESKTOP_CUSTOMWEBPAGE_H
#define DESKTOP_CUSTOMWEBPAGE_H

#include <QWebPage>

/// subclass of QWebPage is needed to redefine javaScriptConsoleMessage
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

#endif // DESKTOP_CUSTOMWEBPAGE_H
