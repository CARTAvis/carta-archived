/**
 *
 **/

#include "NetworkAccessManager.h"
#include "NetworkReplyFileq.h"
#include <QDebug>
#include <QNetworkRequest>

namespace Carta
{
namespace Desktop
{
NetworkAccessManager::NetworkAccessManager( QObject * parent )
    : QNetworkAccessManager( parent )
{ }

QNetworkReply *
NetworkAccessManager::createRequest(
    QNetworkAccessManager::Operation op,
    const QNetworkRequest & request,
    QIODevice * outgoingData )
{
    QString scheme = request.url().scheme().toLower();

    // we only override 'fileq' scheme
    if ( ( op == QNetworkAccessManager::GetOperation || op == QNetworkAccessManager::HeadOperation )
         && ( scheme == QLatin1String( "fileq" )
              ) ) {
        qDebug() << "Intercepting" << request.url().toString();
        return new NetworkReplyFileq( this, request, op );
    }

    // all other schemes are handled by the default implementation
    qDebug() << "Not intercepting" << request.url().toString();
    return QNetworkAccessManager::createRequest( op, request, outgoingData );
}
}
}
