#include "CartaLib/CartaLib.h"
#include "CartaLib/Hooks/ColormapsScalar.h"
#include "GrayColormap.h"
#include "Viewer.h"
#include "Globals.h"
#include "IPlatform.h"
#include "State/ObjectManager.h"
#include "Data/ViewManager.h"

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
#include <set>


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
}

void
Viewer::start()
{
    qDebug() << "Viewer::start() starting";

    auto & globals = * Globals::instance();

    // initialize plugin manager
    globals.setPluginManager( new PluginManager );
    auto pm = globals.pluginManager();

    // tell plugin manager where to find plugins
    pm-> setPluginSearchPaths( globals.mainConfig()->pluginDirectories() );

    // find and load plugins
    pm-> loadPlugins();

    qDebug() << "Loading plugins...";
    auto infoList = pm-> getInfoList();
    qDebug() << "List of plugins: [" << infoList.size() << "]";
    for ( const auto & entry : infoList ) {
        qDebug() << "  path:" << entry.json.name;
    }

    // tell all plugins that the core has initialized
    pm-> prepare < Initialize > ().executeAll();



    // ask plugins to load the image
    qDebug() << "======== trying to load image ========";

    //QString fname = Globals::fname();
    QString fname;
    if ( ! Globals::instance()-> platform()-> initialFileList().isEmpty() ) {
        fname = Globals::instance()-> platform()-> initialFileList()[0];
    }



    ObjectManager* objManager = ObjectManager::objectManager();
    QString vmId = objManager->createObject (ViewManager::CLASS_NAME);
    CartaObject* vmObj = objManager->getObject( vmId );
    m_viewManager.reset( dynamic_cast<ViewManager*>(vmObj));

    if ( fname.length() > 0 ) {
        m_viewManager->loadFile( fname );
    }

    qDebug() << "Viewer has started...";
} // start

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
                                       prepare < LoadImage > ( args[1], 0 );
        Nullable < QImage > res = loadImageHookHelper.first();
        if ( res.isNull() ) {
            qDebug() << "Could not find any plugin to load image";
        }
        else {
            qDebug() << "Image loaded: " << res.val().size();
            //testView2->setImage( res.val() );
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


/*void
Viewer::mouseCB( const QString & path, const QString & val )
{
    bool ok;
    double x = m_connector-> getState( StateKey::MOUSE_X, "" ).toDouble( & ok );
    if ( ! ok ) { }
    double y = m_connector-> getState( StateKey::MOUSE_Y, "" ).toDouble( & ok );
    if ( ! ok ) { }
    auto pixCoords = std::vector < double > ( m_image->dims().size(), 0.0 );
    pixCoords[0] = x;
    pixCoords[1] = y;
    if ( pixCoords.size() > 2 ) {
        pixCoords[2] = m_currentFrame;
    }
    auto list = m_coordinateFormatter->formatFromPixelCoordinate( pixCoords );
    qDebug() << "Formatted coordinate:" << list;
    m_connector-> setState( StateKey::CURSOR, "", list.join( "\n" ).toHtmlEscaped() );

}*/ // scriptedCommandCB
