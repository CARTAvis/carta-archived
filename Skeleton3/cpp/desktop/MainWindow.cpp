#include "MainWindow.h"
#include "CustomWebPage.h"
#include "common/misc.h"
#include <QtWidgets>
#include <QtNetwork>
#include <QtWebKitWidgets>
#include <iostream>
#include <QWebInspector>

MainWindow::MainWindow( )
{
    m_progress = 0;

    QNetworkProxyFactory::setUseSystemConfiguration(true);

    m_view = new QWebView(this);
//    m_view-> setPage( new CustomWebPage(this));
    connect(m_view, SIGNAL(loadFinished(bool)), SLOT(adjustLocation()));
    connect(m_view, SIGNAL(titleChanged(QString)), SLOT(adjustTitle()));
    connect(m_view, SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
    connect(m_view, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));

    m_locationEdit = new QLineEdit(this);
    m_locationEdit->setSizePolicy(QSizePolicy::Expanding, m_locationEdit->sizePolicy().verticalPolicy());
    connect(m_locationEdit, SIGNAL(returnPressed()), SLOT(changeLocation()));

    QToolBar *toolBar = addToolBar(tr("Navigation"));
    toolBar->addAction(m_view->pageAction(QWebPage::Back));
    toolBar->addAction(m_view->pageAction(QWebPage::Forward));
    toolBar->addAction(m_view->pageAction(QWebPage::Reload));
    toolBar->addAction(QIcon("://icons/inspector.png"), "Inspector", this, SLOT(showJsConsole()));
    toolBar->addWidget(m_locationEdit);

    m_inspector = new QWebInspector( nullptr);
    m_inspector-> setPage( m_view-> page());
    m_inspector-> resize( 800, 600);

    QMenu *toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(tr("Show JS Console"), this, SLOT(showJsConsole()));

    setCentralWidget(m_view);
    setUnifiedTitleAndToolBarOnMac(true);

    connect( m_view->page()->mainFrame(),
             & QWebFrame::javaScriptWindowObjectCleared,
             this,
             & MainWindow::addToJavaScript );

    m_img = QImage( 500, 500, QImage::Format_RGB888);

}

void MainWindow::loadUrl(const QUrl & url)
{
    m_view-> load( url );
}

void MainWindow::exportToJs(const QString &name, QObject *objPtr)
{
    m_jsExports.push_back( std::make_pair( name, objPtr));

//    m_view->page()->mainFrame()->addToJavaScriptWindowObject( name, objPtr);
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

void MainWindow::adjustLocation()
{
    m_locationEdit->setText(m_view->url().toString());
}

void MainWindow::changeLocation()
{
    QUrl url = QUrl::fromUserInput(m_locationEdit->text());
    m_view->load(url);
    m_view->setFocus();
}

void MainWindow::adjustTitle()
{
    if (m_progress <= 0 || m_progress >= 100)
        setWindowTitle(m_view->title());
    else
        setWindowTitle(QString("%1 (%2%)").arg(m_view->title()).arg(m_progress));
}

void MainWindow::setProgress(int p)
{
    m_progress = p;
    adjustTitle();
}

void MainWindow::finishLoading(bool)
{
    m_progress = 100;
    adjustTitle();
//    view->page()->mainFrame()->evaluateJavaScript( "alert('page loaded');");
    statusBar()-> showMessage( "Page loaded");
    std::cerr << "Page load finished.\n";
}

void MainWindow::showJsConsole()
{
//    m_inspector-> setVisible( true);
    m_inspector-> setVisible( ! m_inspector-> isVisible());
}

// this is called when the global object in javascript is cleared
// we use it to add a bridge to C++

#include "common/Globals.h"
#include "common/IConnector.h"
#include "common/MyQApp.h"
void MainWindow::addToJavaScript()
{
//    m_view->page()->mainFrame()->addToJavaScriptWindowObject("Qt", this);

    for( auto & entry : m_jsExports ) {
        std::cerr << "Exporting " << entry.first << "\n";
        m_view->page()->mainFrame()->addToJavaScriptWindowObject(
                    entry.first, entry.second);
//        m_view->page()->mainFrame()->addToJavaScriptWindowObject("Qt", this);
    }

    defer( []() {
        Globals::instance()-> connector()-> setState( "/pluginList/stamp", "1.2.3");
    });


}
