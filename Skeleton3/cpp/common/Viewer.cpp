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
#include "ScriptFacade.h"

#include <QImage>
#include <QColor>
#include <QPainter>
#include <QDebug>
#include <QCache>
#include <QCoreApplication>

#include <cmath>
#include <iostream>
#include <limits>

#include <rapidjson/document.h>

using namespace rapidjson;

const QString Viewer::SOCKET_DELIMITER("\n");

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
    ObjectManager* objManager = ObjectManager::objectManager();
    QString vmId = objManager->createObject (Carta::Data::ViewManager::CLASS_NAME);
    CartaObject* vmObj = objManager->getObject( vmId );
    m_viewManager.reset( dynamic_cast<Carta::Data::ViewManager*>(vmObj));
    if ( m_devView ){
       m_viewManager->setDeveloperView();
    }

    if ( fname.length() > 0 ) {
        QString controlId = m_viewManager->getObjectId( Carta::Data::Controller::PLUGIN_NAME, 0);
        m_viewManager->loadFile( controlId, fname );
    }

    m_scriptFacade = ScriptFacade::getInstance();

    qDebug() << "Viewer has been initialized.";
}



void Viewer::setDeveloperView( ){
    m_devView = true;
}

void
Viewer::scriptedCommandCB( QString command )
{
    command = command.simplified();

    QStringList args = command.split( ' ', QString::SkipEmptyParts );

    // command: loadFile
    if ( args.size() == 3 && args[0].toLower() == "loadfile" ) {
        QString output = m_scriptFacade->loadFile( args[1], "/RootDirectory/" + args[2] );
        m_scl->sendTypedMessage( "1", output.toLocal8Bit() );
    }

    // command: loadLocalFile
    if ( args.size() == 3 && args[0].toLower() == "loadlocalfile" ) {
        QString output = m_scriptFacade->loadLocalFile( args[1], args[2] );
        m_scl->sendTypedMessage( "1", output.toLocal8Bit() );
    }

    // command: addLink
    else if (args.size() == 3 && args[0].toLower() == "addlink") {
        QString output = m_scriptFacade->addLink( args[1], args[2] );
        m_scl->sendTypedMessage( "1", output.toLocal8Bit() );
    }

    // command: removeLink
    else if (args.size() == 3 && args[0].toLower() == "removelink") {
        QString output = m_scriptFacade->removeLink( args[1], args[2] );
        m_scl->sendTypedMessage( "1", output.toLocal8Bit() );
    }

    // command: getImageViews
    else if (args.size() == 1 && args[0].toLower() == "getimageviews") {
        QStringList imageViews = m_scriptFacade->getImageViews();
        m_scl->sendTypedMessage( "1", imageViews.join(Viewer::SOCKET_DELIMITER).toLocal8Bit() );
    }

    // command: getColorMapViews
    else if (args.size() == 1 && args[0].toLower() == "getcolormapviews") {
        QStringList colorMapViews = m_scriptFacade->getColorMapViews();
        m_scl->sendTypedMessage( "1", colorMapViews.join(Viewer::SOCKET_DELIMITER).toLocal8Bit() );
    }

    // command: getAnimatorViews
    else if (args.size() == 1 && args[0].toLower() == "getanimatorviews") {
        QStringList animatorViews = m_scriptFacade->getAnimatorViews();
        m_scl->sendTypedMessage( "1", animatorViews.join(Viewer::SOCKET_DELIMITER).toLocal8Bit() );
    }

    // command: getHistogramViews
    else if (args.size() == 1 && args[0].toLower() == "gethistogramviews") {
        QStringList histogramViews = m_scriptFacade->getHistogramViews();
        m_scl->sendTypedMessage( "1", histogramViews.join(Viewer::SOCKET_DELIMITER).toLocal8Bit() );
    }

    // command: getStatisticsViews
    else if (args.size() == 1 && args[0].toLower() == "getstatisticsviews") {
        QStringList statisticsViews = m_scriptFacade->getStatisticsViews();
        m_scl->sendTypedMessage( "1", statisticsViews.join(Viewer::SOCKET_DELIMITER).toLocal8Bit() );
    }

    // command: setColorMap
    else if (args.size() == 3 && args[0].toLower() == "setcolormap") {
        QString output = m_scriptFacade->setColorMap( args[1], args[2] );
        m_scl->sendTypedMessage( "1", output.toLocal8Bit() );
    }

    // command: reverseColorMap
    else if (args.size() == 3 && args[0].toLower() == "reversecolormap") {
        QString output = m_scriptFacade->reverseColorMap( args[1], args[2].toLower() );
        m_scl->sendTypedMessage( "1", output.toLocal8Bit() );
    }

    // command: setCacheColormap
    else if (args.size() == 3 && args[0].toLower() == "setcachecolormap") {
        QString output = m_scriptFacade->setCacheColormap( args[1], args[2] );
        m_scl->sendTypedMessage( "1", output.toLocal8Bit() );
    }

    // command: setCacheSize
    else if (args.size() == 3 && args[0].toLower() == "setcachesize") {
        QString output = m_scriptFacade->setCacheSize( args[1], args[2] );
        m_scl->sendTypedMessage( "1", output.toLocal8Bit() );
    }

    // command: setInterpolatedColorMap
    else if (args.size() == 3 && args[0].toLower() == "setinterpolatedcolormap") {
        QString output = m_scriptFacade->setInterpolatedColorMap( args[1], args[2].toLower() );
        m_scl->sendTypedMessage( "1", output.toLocal8Bit() );
    }

    // command: invertColorMap
    else if (args.size() == 3 && args[0].toLower() == "invertcolormap") {
        QString output = m_scriptFacade->invertColorMap( args[1], args[2].toLower() );
        m_scl->sendTypedMessage( "1", output.toLocal8Bit() );
    }

    // command: setColorMix
    else if (args.size() == 5 && args[0].toLower() == "setcolormix") {
        QString percentString;
        percentString = "redPercent:" + args[2] + ",greenPercent:" + args[3] + ",bluePercent:" + args[4];
        QString output = m_scriptFacade->setColorMix( args[1], percentString );
        m_scl->sendTypedMessage( "1", output.toLocal8Bit() );
    }

    // command: setGamma
    else if (args.size() == 3 && args[0].toLower() == "setgamma") {
        QString output = m_scriptFacade->setGamma( args[1], args[2].toDouble() );
        m_scl->sendTypedMessage( "1", output.toLocal8Bit() );
    }

    // command: setDataTransform
    else if (args.size() > 1 && args[0].toLower() == "setdatatransform") {
        QString transformString;
        for (int i = 2; i < args.size(); i++) {
            transformString += args[i] + " ";
        }
        QString output = m_scriptFacade->setDataTransform( args[1], transformString.trimmed() );
        m_scl->sendTypedMessage( "1", output.toLocal8Bit() );
    }

    // command: setFrame
    else if ( args.size() == 3 && args[0].toLower() == "setframe" ) {
        QString output = m_scriptFacade->setFrame( args[1], args[2] );
        m_scl->sendTypedMessage( "1", output.toLocal8Bit() );
    }

    // command: setClipValue
    else if ( args.size() == 3 && args[0].toLower() == "setclipvalue" ) {
        QString output = m_scriptFacade->setClipValue( args[1], args[2] );
        m_scl->sendTypedMessage( "1", output.toLocal8Bit() );
    }

    // command: getFileList
    else if (args.size() > 0 && args[0].toLower() == "getfilelist") {
        QString fileList = m_scriptFacade->getFileList();
        Document fileListJson;
        fileListJson.Parse(fileList.toStdString().c_str());
        const Value& dir = fileListJson["dir"];
        QStringList fileListList = _parseDirectory( dir, "" );
        m_scl->sendTypedMessage( "1", fileListList.join(Viewer::SOCKET_DELIMITER).toLocal8Bit() );
    }

    // command: setAnalysisLayout
    else if (args.size() == 1 && args[0].toLower() == "setanalysislayout") {
        QString output = m_scriptFacade->setAnalysisLayout();
        m_scl->sendTypedMessage( "1", output.toLocal8Bit() );
    }

    // command: setCustomLayout
    else if (args.size() == 3 && args[0].toLower() == "setcustomlayout") {
        int rows = args[1].toInt();
        int cols = args[2].toInt();
        QString output = m_scriptFacade->setCustomLayout( rows, cols );
        m_scl->sendTypedMessage( "1", output.toLocal8Bit() );
    }

    // command: setImageLayout
    else if (args.size() == 1 && args[0].toLower() == "setimagelayout") {
        QString output = m_scriptFacade->setImageLayout();
        m_scl->sendTypedMessage( "1", output.toLocal8Bit() );
    }

    // command: getColorMaps
    else if (args.size() > 0 && args[0].toLower() == "getcolormaps") {
        QStringList colormaps = m_scriptFacade->getColorMaps();
        m_scl->sendTypedMessage( "1", colormaps.join(Viewer::SOCKET_DELIMITER).toLocal8Bit() );
    }

    // command: setPlugins
    else if (args.size() > 0 && args[0].toLower() == "setplugins") {
        QStringList names;
        for (int i = 1; i < args.size(); i++) {
            names << args[i];
        }
        QString output = m_scriptFacade->setPlugins(names);
        m_scl->sendTypedMessage( "1", output.toLocal8Bit() );
    }
    
    // command: quit
    else if ( args.size() == 1 && args[0].toLower() == "quit" ) {
        MyQApp::exit();
    }

    // command: saveState
    else if ( args.size() == 2 && args[0].toLower() == "savestate" ) {
        QString output = m_scriptFacade->saveState( args[1] );
        m_scl->sendTypedMessage( "1", output.toLocal8Bit() );
    }

    // command: getLinkedColorMaps
    else if ( args.size() == 2 && args[0].toLower() == "getlinkedcolormaps" ) {
        QStringList linkedColorMaps = m_scriptFacade->getLinkedColorMaps( args[1] );
        m_scl->sendTypedMessage( "1", linkedColorMaps.join(Viewer::SOCKET_DELIMITER).toLocal8Bit() );
    }

    // command: getLinkedAnimators
    else if ( args.size() == 2 && args[0].toLower() == "getlinkedanimators" ) {
        QStringList linkedAnimators = m_scriptFacade->getLinkedAnimators( args[1] );
        m_scl->sendTypedMessage( "1", linkedAnimators.join(Viewer::SOCKET_DELIMITER).toLocal8Bit() );
    }

    // command: getLinkedHistograms
    else if ( args.size() == 2 && args[0].toLower() == "getlinkedhistograms" ) {
        QStringList linkedHistograms = m_scriptFacade->getLinkedHistograms( args[1] );
        m_scl->sendTypedMessage( "1", linkedHistograms.join(Viewer::SOCKET_DELIMITER).toLocal8Bit() );
    }

    // command: getLinkedStatistics
    else if ( args.size() == 2 && args[0].toLower() == "getlinkedstatistics" ) {
        QStringList linkedStatistics = m_scriptFacade->getLinkedStatistics( args[1] );
        m_scl->sendTypedMessage( "1", linkedStatistics.join(Viewer::SOCKET_DELIMITER).toLocal8Bit() );
    }

    else {
        qWarning() << "Sorry, unknown command";
    }
} // scriptedCommandCB

QStringList Viewer::_parseDirectory( const Value& dir, QString prefix )
{
    QStringList fileList;
    for (rapidjson::SizeType i = 0; i < dir.Size(); i++)
    {
        const Value& name = dir[i];
        QString filename = QString::fromStdString(name["name"].GetString());
        if (name.HasMember("dir")) {
            const Value& subdir = name["dir"];
            QStringList subFileList = _parseDirectory( subdir, prefix + "/" + filename );
            fileList.append( subFileList );
        }
        else {
            if (prefix != "")
            {
                filename = prefix + "/" + filename;
            }
            fileList.append(filename);
            //const char *printableName = filename.toLocal8Bit().constData();
            //printf("%s \n", printableName);
        }
    }
    //return fileList.join(',');
    return fileList;
}
