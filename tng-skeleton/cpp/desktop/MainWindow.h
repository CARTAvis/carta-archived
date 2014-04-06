#ifndef DESKTOP_MAINWINDOW_H
#define DESKTOP_MAINWINDOW_H

#include <QtWidgets>

class QWebView;
QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

class QWebInspector;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(QImage img READ getImg)

public:
    MainWindow(const QUrl& url);
    const QImage & getImg() const;

public slots:
    void dbg( const QString & str);
    void makeNextImage( const QString & str);

protected slots:

    void adjustLocation();
    void changeLocation();
    void adjustTitle();
    void setProgress(int p);
    void finishLoading(bool);

//    void viewSource();
//    void slotSourceDownloaded();

    void showJsConsole();

    void addToJavaScript();


private:
    QWebView *view;
    QLineEdit *locationEdit;
    int progress;
    QImage m_img;
    QWebInspector * m_inspector; // = nullptr;
};

#endif
