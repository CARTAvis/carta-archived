#ifndef DESKTOP_MAINWINDOW_H
#define DESKTOP_MAINWINDOW_H

#include <QMainWindow>

class QWebView;
class QLineEdit;
class QWebInspector;

class MainWindow : public QMainWindow
{
    Q_OBJECT
//    Q_PROPERTY(QImage img READ getImg)

public:
    /// constructor initializes the GUI
    MainWindow();

    /// loads the given url
    void loadUrl(const QUrl & url);

    /// adds the given QObject to javascript exports
    /// these will be exported automatically on page reload
    /// so call this before calling loadUrl()
    void exportToJs( const QString & name, QObject * objPtr);

//    const QImage & getImg() const;

signals:

public slots:

protected:

//    Q_INVOKABLE QString getState( const QString & key) {
//        return QString( "ok " + key + ".");
//    }

protected slots:

    void adjustLocation();
    void changeLocation();
    void adjustTitle();
    void setProgress(int p);
    void finishLoading(bool);
    void showJsConsole();
    void addToJavaScript();

private:
    QWebView * m_view;
    QLineEdit * m_locationEdit;
    int m_progress;
//    QImage m_img;
    QWebInspector * m_inspector; // = nullptr;
    std::vector< std::pair< QString, QObject *> > m_jsExports;
};

#endif
