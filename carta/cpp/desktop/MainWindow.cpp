#include "MainWindow.h"
#include "CustomWebPage.h"
#include "NetworkAccessManager.h"
#include "core/Globals.h"
#include "core/MainConfig.h"
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
    bool qtDecorationsEnabled = Globals::instance()->mainConfig()->isDeveloperDecorations();
    if( !qtDecorationsEnabled ) {
        menuBar()->setVisible( false);
        toolBar->setVisible( false);
        statusBar()->setVisible( false);
    }

    // install 'fileq' protocol handler
    m_view->page()->setNetworkAccessManager( new Carta::Desktop::NetworkAccessManager(this));
}

void MainWindow::loadUrl(const QUrl & url)
{
    m_view-> load( url );
}

void MainWindow::addJSExport(const QString &name, QObject *objPtr)
{
    m_jsExports.push_back( std::make_pair( name, objPtr));
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
    qDebug() << "Loading page progress: " << p << "/100";
    m_progress = p;
    adjustTitle();
}

void MainWindow::finishLoading(bool)
{
    qDebug() << "Page loaded.";
    m_progress = 100;
    adjustTitle();
    statusBar()-> showMessage( "Page loaded");
}

void MainWindow::showJsConsole()
{
    m_inspector-> setVisible( ! m_inspector-> isVisible());
}

// This method is called when the global javascript object is cleared
// (i.e. when a new page is loaded). We use it to setup bridge JS <--> C++.
void MainWindow::addToJavaScript()
{
    for( auto & entry : m_jsExports ) {
        qDebug() << "Exporting " << entry.first;
        m_view->page()->mainFrame()->addToJavaScriptWindowObject(
                    entry.first, entry.second);
    }
}
