/**
 * This is the license for the CyberSKA image viewer.
 *
 **/

#include "StartupParameters.h"

#include "FitsViewerLib/common.h"
#include <QCoreApplication>
#include <iostream>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QEventLoop>
#include <QTimer>
#include <QScriptEngine>
#include <QDebug>
#include <memory>
#include "GlobalSettings.h"

VisualizationStartupParameters::VisualizationStartupParameters() {
    error = true;
    errorString = "Unknown error";
}

VisualizationStartupParameters &VisualizationStartupParameters::setError(const QString &str) {
    error = true;
    errorString = str;
    return * this;
}

VisualizationStartupParameters &VisualizationStartupParameters::clearError() {
    error = false;
    errorString = "Success";
    return * this;
}

static
VisualizationStartupParameters getParamsFromVizMan(QString sessionKey)
{
    // default paramters are invalid (i.e. with an unknown error)
    VisualizationStartupParameters res;

    QString urlPattern = GlobalSettings::VizMan::urlPattern();
    dbg(1) << "url pattern = " << urlPattern << "\n";
    double timeout = GlobalSettings::VizMan::timeout();
    dbg(1) << "timeout = " << timeout << "\n";

    if( urlPattern.isEmpty()) {
        res.setError( "vizMan.urlPattern cannot be empty in config");
        return res;
    }

    // insert session key into the pattern
    QString urlString = QString(urlPattern).replace( "%%", sessionKey);
    dbg(1) << "url = " << urlString << "\n";

    // try to validate the url
    QUrl qUrl( urlString);
    if( ! qUrl.isValid()) {
        res.setError( "Bad url: " + qUrl.errorString());
        return res;
    }

    // get the json
    std::unique_ptr< QNetworkAccessManager> networkMgr ( new QNetworkAccessManager());
    std::unique_ptr< QNetworkReply> reply (
                networkMgr->get( QNetworkRequest( qUrl)));
    // do this asynchronously, but with a timeout
    QEventLoop loop;
    QTimer::singleShot( timeout * 1000, & loop, SLOT(quit()));
    QObject::connect(reply.get(), SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    dbg(1) << "Network error: " << reply-> errorString() << "(" << reply->error() << ")\n";
    // if there was an error, return invalid paramters
    if( reply->error() != QNetworkReply::NoError) {
        res.setError( "Network error - " + reply-> errorString());
        return res;
    }
    // did the reply finish?
    dbg(1) << "reply finished = " << reply-> isFinished() << "\n";
    dbg(1) << "reply running = " << reply-> isRunning() << "\n";
    // if the reply was not finished, it means the timeout kicked in
    if( reply->isRunning()) {
        res.setError( QString("Timeout - no reply within %1 sec").arg( timeout));
        return res;
    }

    // read the response
    QString jsonRaw = reply->readAll();
    dbg(1) << "Raw JSON below:\n"
              << jsonRaw << "\n"
              << "---------------end of json---------------\n";
    reply->deleteLater();

    // try to parse json (use qt's javascript, LOL)
    // TODO: fix this to use proper JSON parser included with Qt 5.0+
    QScriptValue value;
    QScriptEngine engine;
    value = engine.evaluate("(" + jsonRaw + ")");
    if( engine.hasUncaughtException()) {
        dbg(1) << "uncaugh exception while parsing json\n";
        dbg(1) << ":: " << engine.uncaughtException().toString() << "\n";
        res.setError( "Could not parse JSON - " + engine.uncaughtException().toString());
        return res;
    }
    if( ! value.isValid()) {
        dbg(1) << "Could not parse json, sorry\n";
        res.setError( "Could not parse JSON!");
        return res;
    }
    qDebug() << value.toVariant();
    dbg(1) << "\nvariant[" << value.toString() << "] --variant\n";

    // extract the values we need
    res.title = value.property( "title").toString();
    res.id = value.property( "fileGUID").toString();
    res.path = value.property( "filename").toString();
    res.stamp = value.property( "timestamp").toString();

    if( res.id.isEmpty()) return res.setError( "Invalid id");
    if( res.path.isEmpty()) return res.setError( "Invalid path");
    if( res.stamp.isEmpty()) return res.setError( "Invalid stamp");

    //    QScriptValue v = value.property( "title");
    //    if( ! v.isValid()) std::cout << "invalid v\n";
    //    if( ! v.isString()) std::cout << "invalids v\n";
    //    std::cout << "v = " << v.toString() << "\n";

    //    if(value.property("timetable").isObject()) {
    //        QScriptValue out = value.property("timetable").toObject();
    //        qDebug() << out.toVariant();
    //    } else {
    //        qDebug()<<"Invalid response:\n";
    //    }


    return res.clearError();
}

static
VisualizationStartupParameters
getParamsFromUrl(
        std::map<QString, QString> pureWebParams)
{
    ScopedDebug( "getParamsFromUrl() running");
    VisualizationStartupParameters res;

    res.stamp = pureWebParams[ "timestamp"];
    res.path = pureWebParams[ "filepath"];
    res.title = pureWebParams["title"];
    res.id = pureWebParams["fileid"];
    if( res.stamp.isEmpty() || res.path.isEmpty() || res.id.isEmpty()) {
        res.setError( "url encoded stamp, path or title cannot be null");
        return res;
    }
    res.clearError();
    return res;
}

VisualizationStartupParameters
determineStartupParameters(
        const std::map<QString, QString> & pureWebParams )
{
    ScopedDebug dbg__( "determineStartupParameters()");

    VisualizationStartupParameters res;

    // if 'key'  parameter is present, try to do the REST call
    auto key = pureWebParams.find( "key");
    if( key != pureWebParams.end()) {
        dbg(1) << "key = " << key-> second << "\n";
        dbg(1) << "trying rest call\n";
        res = getParamsFromVizMan( key-> second);
        if( res.error) {
            dbg(0) << ConsoleColors::warning()
                   << "Failed to get parameters from vizman...\n"
                   << "  --> " << res.errorString
                   << ConsoleColors::resetln();
            return res;
        }
    }
    if( res.error) {
        dbg(1) << "No key found or REST call to vizman failed\n"
               << "trying embeded parameters....\n";
        res = getParamsFromUrl( pureWebParams);
    }

    return res;
}


#ifdef DONT_COMPILE

static int mainTest(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    VisualizationStartupParameters vsp;

    vsp = getParamsFromVizMan( "1.json", "testDir/test.ini");

    std::cout << "Parameters:\n"
              << "title: " << vsp.title << "\n"
              << "path: " << vsp.path << "\n"
              << "id: " << vsp.id << "\n"
              << "stamp: " << vsp.stamp << "\n"
              << "error: " << (vsp.error ? "Yes" : "No") << "\n"
              << "errorString: " << vsp.errorString << "\n";

    return 0;
}

#endif
