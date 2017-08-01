#include "MainWindow.h"
#include "NetworkAccessManager.h"
#include "core/Globals.h"
#include "core/MainConfig.h"
#include <QtWidgets>
#include <QtNetwork>
#include <QtWebEngineWidgets>
#include <iostream>
#include <qglobal.h>

MainWindow::MainWindow( )
{
    m_progress = 0;

    QNetworkProxyFactory::setUseSystemConfiguration(true);
    setUnifiedTitleAndToolBarOnMac(true);

    m_view = new QWebEngineView(this);
    connect(m_view, SIGNAL(titleChanged(QString)), SLOT(adjustTitle()));
    connect(m_view, SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
    connect(m_view, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));

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

    setCentralWidget(m_view);
    setUnifiedTitleAndToolBarOnMac(true);

    bool qtDecorationsEnabled = Globals::instance()->mainConfig()->isDeveloperDecorations();
    if( !qtDecorationsEnabled ) {
        menuBar()->setVisible( true);
        // toolBar->setVisible( false);
        statusBar()->setVisible( true);
    } else {
        menuBar()->setVisible( true);
        // toolBar->setVisible( false);
        statusBar()->setVisible( true);
    }

    // 201707, grimmer. This feature is not so important, if we really need this for QtWebEngineWidgets,
    // ref: https://wiki.qt.io/Porting_from_QtWebKit_to_QtWebEngine
    // install 'fileq' protocol handler
    // m_view->page()->setNetworkAccessManager( new Carta::Desktop::NetworkAccessManager(this));
}

void MainWindow::loadUrl(const QUrl & url)
{
    m_view-> load( url );
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
    QString link = "http://localhost:9000/";
    QDesktopServices::openUrl(QUrl(link));
//    m_inspector-> setVisible( ! m_inspector-> isVisible());
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
                    <a href='http://www.gnu.org/licenses/'>http://www.gnu.org/licenses/<a>.</p>"));
}
