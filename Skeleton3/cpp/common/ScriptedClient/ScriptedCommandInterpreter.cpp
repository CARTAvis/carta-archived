/**
 *
 **/

#include "Listener.h"
#include "ScriptedCommandInterpreter.h"

namespace Carta
{
namespace Core
{
namespace ScriptedClient
{
ScriptedCommandInterpreter::ScriptedCommandInterpreter( int port, QObject * parent )
    : QObject( parent )
{
    qDebug() << "ScriptedCommandInterpreter starting on port:" << port;

    m_messageListener.reset( new MessageListener( port, this ) );

    connect( m_messageListener.get(), & MessageListener::received,
             this, & ScriptedCommandInterpreter::tagMessageReceivedCB );
}

/// this is just a quick demo how to listen to TagMessage, convert them to Json,
/// extract info from Json, and pack results back into Json, then to TagMessage...
///
/// it could probably use some error checking, like asserts etc...
void
ScriptedCommandInterpreter::tagMessageReceivedCB( TagMessage tm )
{
    m_scriptFacade = ScriptFacade::getInstance();
    if ( tm.tag() != "json" ) {
        qWarning() << "I don't handle tag" << tm.tag();
        return;
    }
    JsonMessage jm = JsonMessage::fromTagMessage( tm );
    if ( ! jm.doc().isObject() ) {
        qWarning() << "Received json is not object...";
        return;
    }
    QJsonObject jo = jm.doc().object();
    QString cmd = jo["cmd"].toString().toLower();
    QJsonObject rjo;
    auto args = jo["args"].toObject();

    if ( cmd == "ls" ) {
        QString dir = args["dir"].toString();
        if ( dir.isEmpty() ) {
            dir = "/";
        }
        QStringList list = QDir( dir ).entryList();
        rjo.insert( "result", QJsonArray::fromStringList( list ) );
    }

    else if ( cmd == "add" ) {
        double a = args["a"].toDouble();
        double b = args["b"].toDouble();
        rjo.insert( "result", a + b );
    }

    else if ( cmd == "getcolormapviews" ) {
        QStringList colormapViews = m_scriptFacade->getColorMapViews();
        rjo.insert( "result", QJsonValue::fromVariant( colormapViews ) );
    }

    else if ( cmd == "getimageviews" ) {
        QStringList imageViews = m_scriptFacade->getImageViews();
        rjo.insert( "result", QJsonValue::fromVariant( imageViews ) );
    }

    else if ( cmd == "getanimatorviews" ) {
        QStringList animatorViews = m_scriptFacade->getAnimatorViews();
        rjo.insert( "result", QJsonValue::fromVariant( animatorViews ) );
    }

    else if ( cmd == "gethistogramviews" ) {
        QStringList histogramViews = m_scriptFacade->getHistogramViews();
        rjo.insert( "result", QJsonValue::fromVariant( histogramViews ) );
    }

    else if ( cmd == "getstatisticsviews" ) {
        QStringList statisticsViews = m_scriptFacade->getStatisticsViews();
        rjo.insert( "result", QJsonValue::fromVariant( statisticsViews ) );
    }

    else if ( cmd == "setanalysislayout" ) {
        QString result = m_scriptFacade->setAnalysisLayout();
        rjo.insert( "result", QJsonValue::fromVariant( result ) );
    }

    else if ( cmd == "setimagelayout" ) {
        QString result = m_scriptFacade->setImageLayout();
        rjo.insert( "result", QJsonValue::fromVariant( result ) );
    }

    else if ( cmd == "setcustomlayout" ) {
        int rows = args["nrows"].toInt();
        int columns = args["ncols"].toInt();
        QString result = m_scriptFacade->setCustomLayout(rows, columns);
        rjo.insert( "result", QJsonValue::fromVariant( result ) );
    }

    else if ( cmd == "setplugins" ) {
        QString plugins = args["plugins"].toString();
        QStringList pluginsList = plugins.split(' ');
        QString result = m_scriptFacade->setPlugins(pluginsList);
        rjo.insert( "result", QJsonValue::fromVariant( result ) );
    }

    else if ( cmd == "addlink" ) {
        QString source = args["sourceView"].toString();
        QString dest = args["destView"].toString();
        QString result = m_scriptFacade->addLink(source, dest);
        rjo.insert( "result", QJsonValue::fromVariant( result ) );
    }

    else if ( cmd == "removelink" ) {
        QString source = args["sourceView"].toString();
        QString dest = args["destView"].toString();
        QString result = m_scriptFacade->removeLink(source, dest);
        rjo.insert( "result", QJsonValue::fromVariant( result ) );
    }

    else if ( cmd == "savestate" ) {
        QString name = args["name"].toString();
        QString result = m_scriptFacade->saveState(name);
        rjo.insert( "result", QJsonValue::fromVariant( result ) );
    }

    else if ( cmd == "setcolormap" ) {
        QString colormapId = args["colormapId"].toString();
        QString colormapName = args["colormapName"].toString();
        QString result = m_scriptFacade->setColorMap( colormapId, colormapName );
        rjo.insert( "result", result );
    }

    else if ( cmd == "reversecolormap" ) {
        QString colormapId = args["colormapId"].toString();
        QString reverseString = args["reverseString"].toString();
        QString result = m_scriptFacade->reverseColorMap( colormapId, reverseString );
        rjo.insert( "result", result );
    }

    else if ( cmd == "setcachecolormap" ) {
        QString colormapId = args["colormapId"].toString();
        QString cacheString = args["cacheString"].toString();
        QString result = m_scriptFacade->setCacheColormap( colormapId, cacheString );
        rjo.insert( "result", result );
    }

    else if ( cmd == "setcachesize" ) {
        QString colormapId = args["colormapId"].toString();
        QString size = args["size"].toString();
        QString result = m_scriptFacade->setCacheSize( colormapId, size );
        rjo.insert( "result", result );
    }

    else if ( cmd == "setinterpolatedcolormap" ) {
        QString colormapId = args["colormapId"].toString();
        QString interpolatedString = args["interpolatedString"].toString();
        QString result = m_scriptFacade->setInterpolatedColorMap( colormapId, interpolatedString );
        rjo.insert( "result", result );
    }

    else if ( cmd == "invertcolormap" ) {
        QString colormapId = args["colormapId"].toString();
        QString invertString = args["invertString"].toString();
        QString result = m_scriptFacade->invertColorMap( colormapId, invertString );
        rjo.insert( "result", result );
    }

    else if ( cmd == "setcolormix" ) {
        QString colormapId = args["colormapId"].toString();
        QString red = args["red"].toString();
        QString green = args["green"].toString();
        QString blue = args["blue"].toString();
        QString percentString;
        percentString = "redPercent:" + red + ",greenPercent:" + green + ",bluePercent:" + blue;
        QString result = m_scriptFacade->setColorMix( colormapId, percentString );
        rjo.insert( "result", result );
    }

    else if ( cmd == "setgamma" ) {
        QString colormapId = args["colormapId"].toString();
        double gamma = args["gammaValue"].toDouble();
        QString result = m_scriptFacade->setGamma( colormapId, gamma );
        rjo.insert( "result", result );
    }

    else if ( cmd == "setdatatransform" ) {
        QString colormapId = args["colormapId"].toString();
        QString transform = args["transform"].toString();
        QString result = m_scriptFacade->setDataTransform( colormapId, transform );
        rjo.insert( "result", result );
    }

    else if ( cmd == "loadfile" ) {
        QString imageView = args["imageView"].toString();
        QString fileName = args["fname"].toString();
        QString result = m_scriptFacade->loadFile( imageView, fileName );
        rjo.insert( "result", result );
    }

    else if ( cmd == "loadlocalfile" ) {
        QString imageView = args["imageView"].toString();
        QString fileName = args["fname"].toString();
        QString result = m_scriptFacade->loadLocalFile( imageView, fileName );
        rjo.insert( "result", result );
    }

    else if ( cmd == "getlinkedcolormaps" ) {
        QString imageView = args["imageView"].toString();
        QStringList result = m_scriptFacade->getLinkedColorMaps( imageView );
        rjo.insert( "result", QJsonValue::fromVariant( result ) );
    }

    else if ( cmd == "getlinkedanimators" ) {
        QString imageView = args["imageView"].toString();
        QStringList result = m_scriptFacade->getLinkedAnimators( imageView );
        rjo.insert( "result", QJsonValue::fromVariant( result ) );
    }

    else if ( cmd == "getlinkedhistograms" ) {
        QString imageView = args["imageView"].toString();
        QStringList result = m_scriptFacade->getLinkedHistograms( imageView );
        rjo.insert( "result", QJsonValue::fromVariant( result ) );
    }

    else if ( cmd == "getlinkedstatistics" ) {
        QString imageView = args["imageView"].toString();
        QStringList result = m_scriptFacade->getLinkedStatistics( imageView );
        rjo.insert( "result", QJsonValue::fromVariant( result ) );
    }

    else if ( cmd == "setclipvalue" ) {
        QString imageView = args["imageView"].toString();
        QString clipValue = args["clipValue"].toString();
        QString result = m_scriptFacade->setClipValue( imageView, clipValue );
        rjo.insert( "result", result );
    }

    else if ( cmd == "setchannel" ) {
        QString animatorView = args["animatorView"].toString();
        int channel = args["channel"].toInt();
        QString result = m_scriptFacade->setChannel( animatorView, channel );
        rjo.insert( "result", result );
    }

    else if ( cmd == "setimage" ) {
        QString animatorView = args["animatorView"].toString();
        int image = args["image"].toInt();
        QString result = m_scriptFacade->setImage( animatorView, image );
        rjo.insert( "result", result );
    }

    else if ( cmd == "showimageanimator" ) {
        QString animatorView = args["animatorView"].toString();
        QString result = m_scriptFacade->showImageAnimator( animatorView );
        rjo.insert( "result", result );
    }

//    else if ( cmd == "getfilelist" ) {
//        QString fileList = m_scriptFacade->getFileList();
//        rjo.insert( "result", QJsonValue::fromVariant( fileList ) );
//    }

    else if ( cmd == "getcolormaps" ) {
        QStringList colorMaps = m_scriptFacade->getColorMaps();
        rjo.insert( "result", QJsonValue::fromVariant( colorMaps ) );
    }

    else if ( cmd == "fakecommand" ) {
        QString data = args["data"].toString();
        QString result = "Fake command received";
        rjo.insert( "result", result );
    }

    else {
        qDebug() << "Unknown command, sending error back";
        QString result = "Unknown command";
        rjo.insert( "error", result );
    }

    JsonMessage rjm = JsonMessage( QJsonDocument( rjo ) );
    m_messageListener->send( rjm.toTagMessage() );
} // tagMessageReceivedCB
}
}
}
