#include "CartaLib/CartaLib.h"
#include "CartaLib/Hooks/ColormapsScalar.h"
#include "GrayColormap.h"
#include "Viewer.h"
#include "Globals.h"
#include "IPlatform.h"
#include "State/ObjectManager.h"
#include "Data/ViewManager.h"
#include "Data/Controller.h"
#include "PluginManager.h"
#include "MainConfig.h"
#include "MyQApp.h"
#include "CmdLine.h"
#include "ScriptedCommandListener.h"

#include <QImage>
#include <QColor>
#include <QPainter>
#include <QDebug>
#include <QCache>
#include <QCoreApplication>

#include <cmath>
#include <iostream>
#include <limits>


Viewer::Viewer() :
    QObject( nullptr ),
    m_viewManager( nullptr)
{
    int port = Globals::instance()->cmdLineInfo()-> scriptPort();
    qDebug() << "Port="<<port;
    if ( port < 0 ) {
        qDebug() << "Not listening to scripted commands.";
    }
    else {
        m_scl = new ScriptedCommandListener( port, this );
        qDebug() << "Listening to scripted commands on port " << port;
        connect( m_scl, & ScriptedCommandListener::command,
                 this, & Viewer::scriptedCommandCB );
    }
    m_devView = false;
}

void
Viewer::start()
{
    qDebug() << "Viewer::start() starting";

    auto & globals = * Globals::instance();

    // tell all plugins that the core has initialized
    globals.pluginManager()-> prepare < Initialize > ().executeAll();

	// ask plugins to load the image
	qDebug() << "======== trying to load image ========";
	QString fname;
	if( ! Globals::instance()-> platform()-> initialFileList().isEmpty()) {
		fname = Globals::instance()-> platform()-> initialFileList() [0];
	}
    Carta::State::ObjectManager* objManager = Carta::State::ObjectManager::objectManager();
    QString vmId = objManager->createObject (Carta::Data::ViewManager::CLASS_NAME);
    Carta::State::CartaObject* vmObj = objManager->getObject( vmId );
    m_viewManager.reset( dynamic_cast<Carta::Data::ViewManager*>(vmObj));
    if ( m_devView ){
       m_viewManager->setDeveloperView();
    }

    if ( fname.length() > 0 ) {
        QString controlId = m_viewManager->getObjectId( Carta::Data::Controller::PLUGIN_NAME, 0);
        m_viewManager->loadFile( controlId, fname );
    }
    qDebug() << "Viewer has been initialized.";
}



void Viewer::setDeveloperView( ){
    m_devView = true;
}

void
Viewer::scriptedCommandCB( QString command )
{
    command = command.simplified();
    qDebug() << "Scripted command received:" << command;

    QStringList args = command.split( ' ', QString::SkipEmptyParts );
    qDebug() << "args=" << args;
    qDebug() << "args.size=" << args.size();
    if ( args.size() == 2 && args[0].toLower() == "load" ) {
        qDebug() << "Trying to load" << args[1];
        auto loadImageHookHelper = Globals::instance()->pluginManager()->
                    prepare < LoadImage >(args[1], 0);
        Nullable <QImage> res = loadImageHookHelper.first();
        if ( res.isNull() ){
            qDebug() << "Could not find any plugin to load image";
        }
        else {
            qDebug() << "Image loaded: " << res.val().size();
//            testView2->setImage( res.val() );
        }
    }
    else if ( args.size() == 1 && args[0].toLower() == "quit" ) {
        qDebug() << "Quitting...";
        MyQApp::exit();
        return;
    }
    else {
        qWarning() << "Sorry, unknown command";
    }
} // scriptedCommandCB

