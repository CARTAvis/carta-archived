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
    QString key = "result";

    /// application commands

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

//    else if ( cmd == "savestate" ) {
//        QString name = args["name"].toString();
//        result = m_scriptFacade->saveState(name);
//    }

    else if ( cmd == "getcolormaps" ) {
        result = m_scriptFacade->getColorMaps();
    }

    /// colormap commands

    else if ( cmd == "setcolormap" ) {
        QString colormapId = args["colormapId"].toString();
        QString colormapName = args["colormapName"].toString();
        result = m_scriptFacade->setColorMap( colormapId, colormapName );
    }

    else if ( cmd == "reversecolormap" ) {
        QString colormapId = args["colormapId"].toString();
        QString reverseString = args["reverseString"].toString().toLower();
        result = m_scriptFacade->reverseColorMap( colormapId, reverseString );
    }

    else if ( cmd == "setcachecolormap" ) {
        QString colormapId = args["colormapId"].toString();
        QString cacheString = args["cacheString"].toString().toLower();
        result = m_scriptFacade->setCacheColormap( colormapId, cacheString );
    }

    else if ( cmd == "setcachesize" ) {
        QString colormapId = args["colormapId"].toString();
        QString size = args["size"].toString();
        result = m_scriptFacade->setCacheSize( colormapId, size );
    }

    else if ( cmd == "setinterpolatedcolormap" ) {
        QString colormapId = args["colormapId"].toString();
        QString interpolatedString = args["interpolatedString"].toString().toLower();
        result = m_scriptFacade->setInterpolatedColorMap( colormapId, interpolatedString );
    }

    else if ( cmd == "invertcolormap" ) {
        QString colormapId = args["colormapId"].toString();
        QString invertString = args["invertString"].toString().toLower();
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

    /// image/controller commands

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

    else if ( cmd == "savefullimage" ) {
        QString imageView = args["imageView"].toString();
        QString filename = args["filename"].toString();
        double scale = args["scale"].toDouble();
        result = m_scriptFacade->saveFullImage( imageView, filename, scale );
        if (result[0] == "false") {
            key = "error";
            result[0] = "Could not save image to " + filename;
        }
    }

    else if ( cmd == "centeronpixel" ) {
        QString imageView = args["imageView"].toString();
        double x = args["xval"].toDouble();
        double y = args["yval"].toDouble();
        result = m_scriptFacade->centerOnPixel( imageView, x, y );
    }

    else if ( cmd == "setzoomlevel" ) {
        QString imageView = args["imageView"].toString();
        double zoomLevel = args["zoomLevel"].toDouble();
        result = m_scriptFacade->setZoomLevel( imageView, zoomLevel );
    }

    else if ( cmd == "getzoomlevel" ) {
        QString imageView = args["imageView"].toString();
        result = m_scriptFacade->getZoomLevel( imageView );
    }

    else if ( cmd == "getimagedimensions" ) {
        QString imageView = args["imageView"].toString();
        result = m_scriptFacade->getImageDimensions( imageView );
    }

    else if ( cmd == "getoutputsize" ) {
        QString imageView = args["imageView"].toString();
        result = m_scriptFacade->getOutputSize( imageView );
    }

    else if ( cmd == "getintensity" ) {
        QString imageView = args["imageView"].toString();
        int frameLow = args["frameLow"].toInt();
        int frameHigh = args["frameHigh"].toInt();
        double percentile = args["percentile"].toDouble();
        result = m_scriptFacade->getIntensity( imageView, frameLow, frameHigh, percentile );
    }
        /*
        result = self.con.cmdTagList("getIntensity", imageView=self.getId(),
                                     frameLow=frameLow, frameHigh=frameHigh,
                                     percentile=percentile)
        */
    /// animator commands

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

    /// histogram commands

    else if ( cmd == "setclipbuffer" ) {
        QString histogramView = args["histogramView"].toString();
        int bufferAmount = args["bufferAmount"].toInt();
        result = m_scriptFacade->setClipBuffer( histogramView, bufferAmount );
    }

    else if ( cmd == "setuseclipbuffer" ) {
        QString histogramView = args["histogramView"].toString();
        QString useBuffer = args["useBuffer"].toString().toLower();
        result = m_scriptFacade->setUseClipBuffer( histogramView, useBuffer );
    }

    else if ( cmd == "setcliprange" ) {
        QString histogramView = args["histogramView"].toString();
        double minRange = args["minRange"].toDouble();
        double maxRange = args["maxRange"].toDouble();
        result = m_scriptFacade->setClipRange( histogramView, minRange, maxRange );
    }

    else if ( cmd == "applyclips" ) {
        QString histogramView = args["histogramView"].toString();
        double clipMinValue = args["clipMinValue"].toDouble();
        double clipMaxValue = args["clipMaxValue"].toDouble();
        QString modeStr = args["modeStr"].toString();
        result = m_scriptFacade->applyClips( histogramView, clipMinValue, clipMaxValue, modeStr );
    }

    else if ( cmd == "setbincount" ) {
        QString histogramView = args["histogramView"].toString();
        int binCount = args["binCount"].toInt();
        result = m_scriptFacade->setBinCount( histogramView, binCount );
    }

    else if ( cmd == "setbinwidth" ) {
        QString histogramView = args["histogramView"].toString();
        double binWidth = args["binWidth"].toDouble();
        result = m_scriptFacade->setBinWidth( histogramView, binWidth );
    }

    else if ( cmd == "setplanemode" ) {
        QString histogramView = args["histogramView"].toString();
        QString planeMode = args["planeMode"].toString();
        result = m_scriptFacade->setPlaneMode( histogramView, planeMode );
    }

    else if ( cmd == "setplanerange" ) {
        QString histogramView = args["histogramView"].toString();
        double minPlane = args["minPlane"].toDouble();
        double maxPlane = args["maxPlane"].toDouble();
        result = m_scriptFacade->setPlaneRange( histogramView, minPlane, maxPlane );
    }

    else if ( cmd == "setchannelunit" ) {
        QString histogramView = args["histogramView"].toString();
        QString unit = args["unit"].toString();
        result = m_scriptFacade->setChannelUnit( histogramView, unit );
    }

    else if ( cmd == "setgraphstyle" ) {
        QString histogramView = args["histogramView"].toString();
        QString graphStyle = args["graphStyle"].toString();
        result = m_scriptFacade->setGraphStyle( histogramView, graphStyle );
    }

    else if ( cmd == "setlogcount" ) {
        QString histogramView = args["histogramView"].toString();
        QString logCount = args["logCount"].toString().toLower();
        result = m_scriptFacade->setLogCount( histogramView, logCount );
    }

    else if ( cmd == "setcolored" ) {
        QString histogramView = args["histogramView"].toString();
        QString colored = args["colored"].toString().toLower();
        result = m_scriptFacade->setColored( histogramView, colored );
    }

    else if ( cmd == "savehistogram" ) {
        QString histogramView = args["histogramView"].toString();
        QString filename = args["filename"].toString();
        int width = args["width"].toInt();
        int height = args["height"].toInt();
        result = m_scriptFacade->saveHistogram( histogramView, filename, width, height );
    }

    /// commands for testing

    else if ( cmd == "fakecommand" ) {
        QString data = args["data"].toString();
        result.append("Fake command received");
    }

    else {
        qDebug() << "Unknown command, sending error back";
        key = "error";
        result.append("Unknown command");
    }

    rjo.insert( key, QJsonValue::fromVariant( result ) );
    JsonMessage rjm = JsonMessage( QJsonDocument( rjo ) );
    m_messageListener->send( rjm.toTagMessage() );
} // tagMessageReceivedCB
}
}
}
