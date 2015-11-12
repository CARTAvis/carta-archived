#ifndef DESKTOP_MAINWINDOW_H
#define DESKTOP_MAINWINDOW_H

#include <QMainWindow>

class QWebView;
class QLineEdit;
class QWebInspector;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    /// constructor initializes the GUI
    MainWindow();

    /// loads the given url
    void loadUrl(const QUrl & url);

    /// adds the given QObject to javascript exports
    /// these will be exported automatically on page reload
    /// so call this before calling loadUrl()
    void addJSExport( const QString & name, QObject * objPtr);

signals:

public slots:

protected:

protected slots:

    void adjustLocation();
    void changeLocation();
    void adjustTitle();
    void setProgress(int p);
    void finishLoading(bool);
    void showJsConsole();
    void addToJavaScript();

private:
    QWebView * m_view = nullptr;
    QLineEdit * m_locationEdit = nullptr;
    int m_progress;
    QWebInspector * m_inspector = nullptr; // = nullptr;
    std::vector< std::pair< QString, QObject *> > m_jsExports;
};

#endif
