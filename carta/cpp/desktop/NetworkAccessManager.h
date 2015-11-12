/**
 * Custom network access manager implementation that allows new protocol implementation.
 *
 * The new protocol is 'fileq:///', which behaves exactly the same as file:///, except it
 * minimizes the number of simultaneous open files.
 *
 * The way Qt handles file:/// requests is not implemented very well, as it implements no
 * throttling on the number of files opened simultaneously. For example, a qooxdoo application
 * compiled in 'source-all' mode will often need 1k+ files opened simultaneously. One solution
 * is to increase ulimit... Another one is this one here :)
 *
 **/

#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace Carta
{
namespace Desktop
{
class NetworkAccessManager
    : public QNetworkAccessManager
{
    Q_OBJECT

public:

    NetworkAccessManager( QObject * parent );

protected:

    /// we only need to override this method
    virtual QNetworkReply *
    createRequest( Operation op, const QNetworkRequest & request,
                   QIODevice * outgoingData ) override;
};
}
}
