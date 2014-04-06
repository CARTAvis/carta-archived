#include "MainWindow.h"
#include "CustomWebPage.h"
#include <QtWidgets>
#include <QtNetwork>
#include <QtWebKitWidgets>
#include <iostream>
#include <QWebInspector>

MainWindow::MainWindow(const QUrl& url)
{
    progress = 0;

    QNetworkProxyFactory::setUseSystemConfiguration(true);

    view = new QWebView(this);
    view-> setPage( new CustomWebPage(this));
    view->load(url);
    connect(view, SIGNAL(loadFinished(bool)), SLOT(adjustLocation()));
    connect(view, SIGNAL(titleChanged(QString)), SLOT(adjustTitle()));
    connect(view, SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
    connect(view, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));

    locationEdit = new QLineEdit(this);
    locationEdit->setSizePolicy(QSizePolicy::Expanding, locationEdit->sizePolicy().verticalPolicy());
    connect(locationEdit, SIGNAL(returnPressed()), SLOT(changeLocation()));

    QToolBar *toolBar = addToolBar(tr("Navigation"));
    toolBar->addAction(view->pageAction(QWebPage::Back));
    toolBar->addAction(view->pageAction(QWebPage::Forward));
    toolBar->addAction(view->pageAction(QWebPage::Reload));
    toolBar->addAction(QIcon("://icons/inspector.png"), "Inspector", this, SLOT(showJsConsole()));
    toolBar->addWidget(locationEdit);

    m_inspector = new QWebInspector( nullptr);
    m_inspector-> setPage( view-> page());
    m_inspector-> resize( 800, 600);
//    m_inspector-> setVisible(true);

//    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
//    QAction* viewSourceAction = new QAction("Page Source", this);
//    connect(viewSourceAction, SIGNAL(triggered()), SLOT(viewSource()));
//    viewMenu->addAction(viewSourceAction);

    QMenu *toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(tr("Show JS Console"), this, SLOT(showJsConsole()));

    setCentralWidget(view);
    setUnifiedTitleAndToolBarOnMac(true);

    connect(view->page()->mainFrame(),
            SIGNAL(javaScriptWindowObjectCleared()), SLOT(addToJavaScript()));

    m_img = QImage( 500, 500, QImage::Format_RGB888);

}

const QImage & MainWindow::getImg() const {
    return m_img;
}

void MainWindow::dbg(const QString &str)
{
    std::cerr << "dbg() called with " << str.toStdString() << "\n";
}

void MainWindow::makeNextImage(const QString &str)
{
    m_img.fill( qRgb(255,0,0));
    QPainter p( & m_img);
    p.setFont( QFont( "Arial", 20));
    p.setPen( QColor( "blue"));
    p.drawText( m_img.rect(), Qt::AlignCenter, str);
}

//void MainWindow::viewSource()
//{
//    QNetworkAccessManager* accessManager = view->page()->networkAccessManager();
//    QNetworkRequest request(view->url());
//    QNetworkReply* reply = accessManager->get(request);
//    connect(reply, SIGNAL(finished()), this, SLOT(slotSourceDownloaded()));
//}

//void MainWindow::slotSourceDownloaded()
//{
//    QNetworkReply* reply = qobject_cast<QNetworkReply*>(const_cast<QObject*>(sender()));
//    QTextEdit* textEdit = new QTextEdit( NULL);
//    textEdit->setAttribute(Qt::WA_DeleteOnClose);
//    textEdit->show();
//    textEdit->setPlainText(reply->readAll());
//    reply->deleteLater();
//}

void MainWindow::adjustLocation()
{
    locationEdit->setText(view->url().toString());
}

void MainWindow::changeLocation()
{
    QUrl url = QUrl::fromUserInput(locationEdit->text());
    view->load(url);
    view->setFocus();
}

void MainWindow::adjustTitle()
{
    if (progress <= 0 || progress >= 100)
        setWindowTitle(view->title());
    else
        setWindowTitle(QString("%1 (%2%)").arg(view->title()).arg(progress));
}

void MainWindow::setProgress(int p)
{
    progress = p;
    adjustTitle();
}

void MainWindow::finishLoading(bool)
{
    progress = 100;
    adjustTitle();
//    view->page()->mainFrame()->evaluateJavaScript( "alert('page loaded');");
    statusBar()-> showMessage( "Page loaded");
    std::cerr << "Page load finished.\n";
}

void MainWindow::showJsConsole()
{
    m_inspector-> setVisible( true);
}

void MainWindow::addToJavaScript()
{
    view->page()->mainFrame()->addToJavaScriptWindowObject("Qt", this);
}

//! [9]

