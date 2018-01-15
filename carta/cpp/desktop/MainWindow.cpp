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
#include <qglobal.h>

MainWindow::MainWindow( )
{
    m_progress = 0;

    QNetworkProxyFactory::setUseSystemConfiguration(true);
    setUnifiedTitleAndToolBarOnMac(true);

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

#ifdef Q_OS_LINUX
    // add Carta option
    QMenu *cartaMenu = menuBar()->addMenu(tr("&CARTA"));
    cartaMenu->addAction(tr("Copyright and License"), this, SLOT(cartaLicense()));
    cartaMenu->addAction(tr("Version 0.9"));
#else
    // add Carta option
    QMenu *cartaMenu = menuBar()->addMenu(tr("&CARTA"));
    //cartaMenu->addAction(tr("&About"), this, SLOT(cartaLicense()));

    QAction *aboutCopyright = new QAction(tr("Copyright and License"), this);
    connect(aboutCopyright, SIGNAL(triggered()), this, SLOT(cartaLicense()));
    aboutCopyright->setMenuRole(QAction::ApplicationSpecificRole);
    cartaMenu->addAction(aboutCopyright);

    QAction *aboutVersion = new QAction(tr("Version 0.9"), this);
    aboutVersion->setMenuRole(QAction::ApplicationSpecificRole);
    cartaMenu->addAction(aboutVersion);
#endif

    // add Tool option
    QMenu *toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(tr("Show JS Console"), this, SLOT(showJsConsole()));

    // add Help option
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("GitHub Home"), this, SLOT(helpUrlGitHubHome()));
    helpMenu->addAction(tr("GitHub Wiki"), this, SLOT(helpUrlGitHubWiki()));
    helpMenu->addAction(tr("GitHub Issues"), this, SLOT(helpUrlGitHubIssues()));
    helpMenu->addAction(tr("Release"), this, SLOT(helpReleaseNote()));
    helpMenu->addAction(tr("Manual"), this, SLOT(helpManual()));
    helpMenu->addAction(tr("Helpdesk"), this, SLOT(helpUrlHelpdesk()));
    
    setCentralWidget(m_view);
    setUnifiedTitleAndToolBarOnMac(true);

    connect( m_view->page()->mainFrame(),
             & QWebFrame::javaScriptWindowObjectCleared,
             this,
             & MainWindow::addToJavaScript );

    // set visibilities of window bars
    menuBar()->setVisible( true);
    toolBar->setVisible( false);
    statusBar()->setVisible( true);

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

void MainWindow::helpUrlGitHubHome()
{
    QDesktopServices::openUrl(QUrl("https://github.com/CARTAvis/carta", QUrl::TolerantMode));
}

void MainWindow::helpUrlGitHubWiki()
{
    QDesktopServices::openUrl(QUrl("https://github.com/CARTAvis/carta/wiki", QUrl::TolerantMode));
}

void MainWindow::helpUrlGitHubIssues()
{
    QDesktopServices::openUrl(QUrl("https://github.com/CARTAvis/carta/issues", QUrl::TolerantMode));
}

void MainWindow::helpReleaseNote()
{
    QDesktopServices::openUrl(QUrl("https://github.com/CARTAvis/carta/releases", QUrl::TolerantMode));
}

void MainWindow::helpManual()
{
    QDesktopServices::openUrl(QUrl("https://cartavis.github.io/manual/", QUrl::TolerantMode));
}

void MainWindow::helpUrlHelpdesk()
{
    QDesktopServices::openUrl(QUrl("mailto:carta_helpdesk@asiaa.sinica.edu.tw", QUrl::TolerantMode));
}

void MainWindow::cartaLicense()
{
    QMessageBox::about(this, tr("Copyright and License"),
                 tr("<p>This program is free software: you can redistribute it and/or modify \
                    it under the terms of the GNU General Public License as published by \
                    the Free Software Foundation, either version 3 of the License, or \
                    (at your option) any later version.</p>\
                    <p>This program is distributed in the hope that it will be useful, \
                    but WITHOUT ANY WARRANTY; without even the implied warranty of \
                    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the \
                    GNU General Public License for more details.</p>\
                    <p>You should have received a copy of the GNU General Public License \
                    along with this program.  If not, see \
                    <a href='http://www.gnu.org/licenses/'>http://www.gnu.org/licenses/<a>.</p> \
                    <p>Third party list: <a href='https://github.com/cartavis/carta#third-party-libraries'>\
                    https://github.com/cartavis/carta#third-party-libraries<a>.</p>"));
}
