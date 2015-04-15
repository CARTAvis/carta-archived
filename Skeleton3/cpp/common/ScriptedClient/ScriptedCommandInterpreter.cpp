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
    auto args = jo["args"].toObject();
    QJsonObject rjo;
    QStringList result;

    if ( cmd == "getcolormapviews" ) {
        result = m_scriptFacade->getColorMapViews();
    }

    else if ( cmd == "getimageviews" ) {
        result = m_scriptFacade->getImageViews();
    }

    else if ( cmd == "getanimatorviews" ) {
        result = m_scriptFacade->getAnimatorViews();
    }

    else if ( cmd == "gethistogramviews" ) {
        result = m_scriptFacade->getHistogramViews();
    }

    else if ( cmd == "getstatisticsviews" ) {
        result = m_scriptFacade->getStatisticsViews();
    }

    else if ( cmd == "setanalysislayout" ) {
        result = m_scriptFacade->setAnalysisLayout();
    }

    else if ( cmd == "setimagelayout" ) {
        result = m_scriptFacade->setImageLayout();
    }

    else if ( cmd == "setcustomlayout" ) {
        int rows = args["nrows"].toInt();
        int columns = args["ncols"].toInt();
        result = m_scriptFacade->setCustomLayout(rows, columns);
    }

    else if ( cmd == "setplugins" ) {
        QString plugins = args["plugins"].toString();
        QStringList pluginsList = plugins.split(' ');
        result = m_scriptFacade->setPlugins(pluginsList);
    }

    else if ( cmd == "addlink" ) {
        QString source = args["sourceView"].toString();
        QString dest = args["destView"].toString();
        result = m_scriptFacade->addLink(source, dest);
    }

    else if ( cmd == "removelink" ) {
        QString source = args["sourceView"].toString();
        QString dest = args["destView"].toString();
        result = m_scriptFacade->removeLink(source, dest);
    }

    else if ( cmd == "savestate" ) {
        QString name = args["name"].toString();
        result = m_scriptFacade->saveState(name);
    }

    else if ( cmd == "setcolormap" ) {
        QString colormapId = args["colormapId"].toString();
        QString colormapName = args["colormapName"].toString();
        result = m_scriptFacade->setColorMap( colormapId, colormapName );
    }

    else if ( cmd == "reversecolormap" ) {
        QString colormapId = args["colormapId"].toString();
        QString reverseString = args["reverseString"].toString();
        result = m_scriptFacade->reverseColorMap( colormapId, reverseString );
    }

    else if ( cmd == "setcachecolormap" ) {
        QString colormapId = args["colormapId"].toString();
        QString cacheString = args["cacheString"].toString();
        result = m_scriptFacade->setCacheColormap( colormapId, cacheString );
    }

    else if ( cmd == "setcachesize" ) {
        QString colormapId = args["colormapId"].toString();
        QString size = args["size"].toString();
        result = m_scriptFacade->setCacheSize( colormapId, size );
    }

    else if ( cmd == "setinterpolatedcolormap" ) {
        QString colormapId = args["colormapId"].toString();
        QString interpolatedString = args["interpolatedString"].toString();
        result = m_scriptFacade->setInterpolatedColorMap( colormapId, interpolatedString );
    }

    else if ( cmd == "invertcolormap" ) {
        QString colormapId = args["colormapId"].toString();
        QString invertString = args["invertString"].toString();
        result = m_scriptFacade->invertColorMap( colormapId, invertString );
    }

    else if ( cmd == "setcolormix" ) {
        QString colormapId = args["colormapId"].toString();
        QString red = args["red"].toString();
        QString green = args["green"].toString();
        QString blue = args["blue"].toString();
        QString percentString;
        percentString = "redPercent:" + red + ",greenPercent:" + green + ",bluePercent:" + blue;
        result = m_scriptFacade->setColorMix( colormapId, percentString );
    }

    else if ( cmd == "setgamma" ) {
        QString colormapId = args["colormapId"].toString();
        double gamma = args["gammaValue"].toDouble();
        result = m_scriptFacade->setGamma( colormapId, gamma );
    }

    else if ( cmd == "setdatatransform" ) {
        QString colormapId = args["colormapId"].toString();
        QString transform = args["transform"].toString();
        result = m_scriptFacade->setDataTransform( colormapId, transform );
    }

    else if ( cmd == "loadfile" ) {
        QString imageView = args["imageView"].toString();
        QString fileName = args["fname"].toString();
        result = m_scriptFacade->loadFile( imageView, fileName );
    }

    else if ( cmd == "loadlocalfile" ) {
        QString imageView = args["imageView"].toString();
        QString fileName = args["fname"].toString();
        result = m_scriptFacade->loadLocalFile( imageView, fileName );
    }

    else if ( cmd == "getlinkedcolormaps" ) {
        QString imageView = args["imageView"].toString();
        result = m_scriptFacade->getLinkedColorMaps( imageView );
    }

    else if ( cmd == "getlinkedanimators" ) {
        QString imageView = args["imageView"].toString();
        result = m_scriptFacade->getLinkedAnimators( imageView );
    }

    else if ( cmd == "getlinkedhistograms" ) {
        QString imageView = args["imageView"].toString();
        result = m_scriptFacade->getLinkedHistograms( imageView );
    }

    else if ( cmd == "getlinkedstatistics" ) {
        QString imageView = args["imageView"].toString();
        result = m_scriptFacade->getLinkedStatistics( imageView );
    }

    else if ( cmd == "setclipvalue" ) {
        QString imageView = args["imageView"].toString();
        QString clipValue = args["clipValue"].toString();
        result = m_scriptFacade->setClipValue( imageView, clipValue );
    }

    else if ( cmd == "saveimage" ) {
        QString imageView = args["imageView"].toString();
        QString filename = args["filename"].toString();
        result = m_scriptFacade->saveImage( imageView, filename );
    }

    else if ( cmd == "setchannel" ) {
        QString animatorView = args["animatorView"].toString();
        int channel = args["channel"].toInt();
        result = m_scriptFacade->setChannel( animatorView, channel );
    }

    else if ( cmd == "setimage" ) {
        QString animatorView = args["animatorView"].toString();
        int image = args["image"].toInt();
        result = m_scriptFacade->setImage( animatorView, image );
    }

    else if ( cmd == "showimageanimator" ) {
        QString animatorView = args["animatorView"].toString();
        result = m_scriptFacade->showImageAnimator( animatorView );
    }

//    else if ( cmd == "getfilelist" ) {
//        QString fileList = m_scriptFacade->getFileList();
//    }

    else if ( cmd == "getcolormaps" ) {
        result = m_scriptFacade->getColorMaps();
    }

    else if ( cmd == "fakecommand" ) {
        QString data = args["data"].toString();
        result.append("Fake command received");
    }

    else {
        qDebug() << "Unknown command, sending error back";
        result.append("Unknown command");
    }

    if (result[0] == "Unknown command") {
        qDebug() << "(JT) this was an unknown command.";
        rjo.insert( "error", QJsonValue::fromVariant( result ) );
    }

    else {
        rjo.insert( "result", QJsonValue::fromVariant( result ) );
    }

    JsonMessage rjm = JsonMessage( QJsonDocument( rjo ) );
    m_messageListener->send( rjm.toTagMessage() );
} // tagMessageReceivedCB
}
}
}
