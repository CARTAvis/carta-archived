#include "CyberSKAplugin.h"
#include "CartaLib/Hooks/GetInitialFileList.h"
#include "CartaLib/Hooks/Initialize.h"
#include <QEventLoop>
#include <QJsonDocument>
#include <QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QTimer>

CyberSKAplugin::CyberSKAplugin( QObject * parent ) :
    QObject( parent )
{ }

bool
CyberSKAplugin::handleHook( BaseHook & hookData )
{
    qDebug() << "CyberSKAplugin is handling hook #" << hookData.hookId();
    if ( hookData.is < Carta::Lib::Hooks::Initialize > () ) {
        // we may need to initialize some qt formats here in the future...
        return true;
    }
    else if ( hookData.is < Carta::Lib::Hooks::GetInitialFileList > () ) {
        Carta::Lib::Hooks::GetInitialFileList & hook =
            static_cast < Carta::Lib::Hooks::GetInitialFileList & > ( hookData );
        QString key = hook.paramsPtr->urlParams["key"];
        qDebug() << "key" << key;
        hook.result = getParamsFromVizMan( key);
        // return success if we made a list of at least one filename
        return ! hook.result.isEmpty();
    }

    qWarning() << "CyberSKAplugin: Sorrry, dont' know how to handle this hook";
    return false;
} // handleHook

std::vector < HookId >
CyberSKAplugin::getInitialHookList()
{
    return {
               Carta::Lib::Hooks::Initialize::staticId,
               Carta::Lib::Hooks::GetInitialFileList::staticId
    };
}

void
CyberSKAplugin::initialize( const IPlugin::InitInfo & info )
{
    qDebug() << "CyberSKA plugin initialized";
//    QJsonDocument doc( info.json );
//    QString docs = doc.toJson();
//    qDebug() << docs;

//    m_urlPattern = doc.object()["vizmanUrl"].toString();
//    m_timeout = doc.object().value( "timeout" ).toDouble( 5.0 );

    m_urlPattern = info.json.value( "vizmanUrl").toString();
    m_timeout = info.json.value( "timeout").toDouble( 5.0);


    qDebug() << "vizmanUrl" << m_urlPattern;
    qDebug() << "timeout" << m_timeout;
}

QStringList
CyberSKAplugin::getParamsFromVizMan( QString sessionKey )
{
    QStringList res;

    if ( m_urlPattern.isEmpty() ) {
        qWarning( "CyberSKA plugin: vizMan.urlPattern cannot be empty in config" );
        return res;
    }

    // insert session key into the pattern
    QString urlString = QString( m_urlPattern ).replace( "%%", sessionKey );

    // try to validate the url
    QUrl qUrl( urlString );
    if ( ! qUrl.isValid() ) {
        qWarning() << "cyberska plugin: bad url:" << qUrl.errorString();
        return res;
    }

    // get the json
    std::unique_ptr < QNetworkAccessManager > networkMgr( new QNetworkAccessManager() );
    std::unique_ptr < QNetworkReply > reply(
        networkMgr->get( QNetworkRequest( qUrl ) ) );

    // do this asynchronously, but with a timeout
    QEventLoop loop;
    QTimer::singleShot( m_timeout * 1000, & loop, SLOT( quit() ) );
    QObject::connect( reply.get(), & QNetworkReply::finished,
                      & loop, & QEventLoop::quit );

    QObject::connect( reply.get(), SIGNAL( finished() ), & loop, SLOT( quit() ) );
    loop.exec();

    // if there was an error, return invalid paramters
    if ( reply->error() != QNetworkReply::NoError ) {
        qWarning() << "Network error: " << reply-> errorString() << "(" << reply->error() << ")\n";
        return res;
    }

    // did the reply finish?
    qDebug() << "reply finished = " << reply-> isFinished() << "\n";
    qDebug() << "reply running = " << reply-> isRunning() << "\n";

    // if the reply was not finished, it means the timeout kicked in
    if ( reply->isRunning() ) {
        qWarning() << QString( "Timeout - no reply within %1 sec" ).arg( m_timeout );
        return res;
    }

    // read the response
    QByteArray jsonRaw = reply->readAll();
    qDebug() << "Raw JSON below:\n"
             << jsonRaw << "\n"
             << "---------------end of json---------------\n";
    reply->deleteLater();

    // try to parse this into JSON
    QJsonParseError errors;
    QJsonDocument jsonDoc = QJsonDocument::fromJson( jsonRaw, & errors );
    if ( jsonDoc.isNull() ) {
        qWarning() << "Could not parse reply";
        qWarning() << "Errors below:";
        qWarning() << errors.errorString();
        return res;
    }
    QJsonObject json = jsonDoc.object();

    // get the filename out of it
    QString filename = json.value("filename").toString();
    if( filename.isEmpty()) {
        qWarning() << "Could not get filename from reply";
        return res;
    }

    res << filename;

    return res;
} // getParamsFromVizMan
