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
    qDebug() << "Scripted command received:" << command;

    QStringList args = command.split( ' ', QString::SkipEmptyParts );
    qDebug() << "args=" << args;
    qDebug() << "args.size=" << args.size();

    // command: loadFile
    if ( args.size() == 3 && args[0].toLower() == "loadfile" ) {
        qDebug() << "Trying to load" << args[2] << " into " << args[1];
        m_scriptFacade->loadFile( args[1], "/RootDirectory/" + args[2] );
    }

    // command: loadLocalFile
    if ( args.size() == 3 && args[0].toLower() == "loadlocalfile" ) {
        qDebug() << "Trying to load local file " << args[2] << " into " << args[1];
        m_scriptFacade->loadLocalFile( args[1], args[2] );
    }

    // command: linkAdd
    else if (args.size() == 3 && args[0].toLower() == "linkadd") {
        QString result = m_scriptFacade->linkAdd( args[1], args[2] );
        qDebug() << "linkAdd result: " << result;
    }

    // command: getColorMapId
    else if (args.size() > 0 && args[0].toLower() == "getcolormapid") {
        int index = -1;
        if (args.size() > 1) {
            index = args[1].toInt();
        }
        QString colormapId = m_scriptFacade->getColorMapId( index );
        //cout << qPrintable(colormapId) << endl;
        m_scl->dataTransporter( colormapId );
    }

    // command: getImageViewId
    else if (args.size() > 0 && args[0].toLower() == "getimageviewid") {
        int index = -1;
        if (args.size() > 1) {
            index = args[1].toInt();
        }
        QString imageViewId = m_scriptFacade->getImageViewId( index );
        //cout << qPrintable(imageViewId) << endl;
        m_scl->dataTransporter( imageViewId );
    }

    // command: getImageViews
    else if (args.size() == 1 && args[0].toLower() == "getimageviews") {
        QStringList imageViews = m_scriptFacade->getImageViews();
        //qDebug() << "(JT) imageViews: " << imageViews;
        m_scl->dataTransporter( imageViews.join(',') );
    }

    // command: getColorMapViews
    else if (args.size() == 1 && args[0].toLower() == "getcolormapviews") {
        QStringList colorMapViews = m_scriptFacade->getColorMapViews();
        //qDebug() << "(JT) colorMapViews: " << colorMapViews;
        m_scl->dataTransporter( colorMapViews.join(',') );
    }

    // command: getAnimatorViews
    else if (args.size() == 1 && args[0].toLower() == "getanimatorviews") {
        QStringList animatorViews = m_scriptFacade->getAnimatorViews();
        //qDebug() << "(JT) animatorViews: " << animatorViews;
        m_scl->dataTransporter( animatorViews.join(',') );
    }

    // command: getHistogramViews
    else if (args.size() == 1 && args[0].toLower() == "gethistogramviews") {
        QStringList histogramViews = m_scriptFacade->getHistogramViews();
        //qDebug() << "(JT) histogramViews: " << histogramViews;
        m_scl->dataTransporter( histogramViews.join(',') );
    }

    // command: getStatisticsViews
    else if (args.size() == 1 && args[0].toLower() == "getstatisticsviews") {
        QStringList statisticsViews = m_scriptFacade->getStatisticsViews();
        //qDebug() << "(JT) statisticsViews: " << statisticsViews;
        m_scl->dataTransporter( statisticsViews.join(',') );
    }

    // command: setColorMap
    else if (args.size() == 3 && args[0].toLower() == "setcolormap") {
        m_scriptFacade->setColorMap( args[1], args[2] );
    }

    // command: setFrame
    else if ( args.size() == 3 && args[0].toLower() == "setframe" ) {
        //qDebug() << "(JT) setFrame of " << args[1] << " to " << args[2];
        m_scriptFacade->setFrame( args[1], args[2] );
    }

    // command: setClipValue
    else if ( args.size() == 3 && args[0].toLower() == "setclipvalue" ) {
        //qDebug() << "(JT) setClipValue " << args[1];
        m_scriptFacade->setClipValue( args[1], args[2] );
    }

    // command: getFileList
    else if (args.size() > 0 && args[0].toLower() == "getfilelist") {
        QString fileList = m_scriptFacade->getFileList();
        //qDebug() << "(JT) raw file list: " << fileList;
        QString substring = "";
        if (args.size() > 1) {
            substring = args[1];
            //qDebug() << "substring: " << substring;
        }
        Document fileListJson;
        fileListJson.Parse(fileList.toStdString().c_str());
        const Value& dir = fileListJson["dir"];
        QStringList fileListList = _parseDirectory( dir, "" );
        m_scl->dataTransporter( fileListList.join(',') );
    }

    // command: setAnalysisLayout
    else if (args.size() == 1 && args[0].toLower() == "setanalysislayout") {
        m_scriptFacade->setAnalysisLayout();
    }

    // command: setCustomLayout
    else if (args.size() == 3 && args[0].toLower() == "setcustomlayout") {
        int rows = args[1].toInt();
        int cols = args[2].toInt();
        m_scriptFacade->setCustomLayout( rows, cols );
    }

    // command: setImageLayout
    else if (args.size() == 1 && args[0].toLower() == "setimagelayout") {
        m_scriptFacade->setImageLayout();
    }

    // command: getColorMaps
    else if (args.size() > 0 && args[0].toLower() == "getcolormaps") {
        QString substring = "";
        if (args.size() > 1) {
            substring = args[1];
        }
        QStringList colormaps = m_scriptFacade->getColorMaps();
        if (substring != "") {
            colormaps = colormaps.filter(substring, Qt::CaseInsensitive);
        }
//        for (int i = 0; i < colormaps.size(); ++i) {
//            cout << colormaps.at(i).toLocal8Bit().constData() << endl;
//        }
        m_scl->dataTransporter( colormaps.join(',') );
    }

    // command: setPlugins
    else if (args.size() > 0 && args[0].toLower() == "setplugins") {
        QStringList names;
        for (int i = 1; i < args.size(); i++) {
            names << args[i];
        }
        //qDebug() << "(JT) setplugins: names = " << names;
        m_scriptFacade->setPlugins(names);
    }
    
    // command: quit
    else if ( args.size() == 1 && args[0].toLower() == "quit" ) {
        //qDebug() << "Quitting...";
        MyQApp::exit();
    }

    // command: saveState
    else if ( args.size() == 2 && args[0].toLower() == "savestate" ) {
        bool result = m_scriptFacade->saveState( args[1] );
        //qDebug() << "(JT) saveState result = " << result;
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
