#include "NetworkReplyFileq.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QDebug>

namespace Carta
{
namespace Desktop
{
NetworkReplyFileq::~NetworkReplyFileq()
{ }

void
NetworkReplyFileq::close()
{
    m_isClosed = true;
}

void
NetworkReplyFileq::abort()
{
    m_isClosed = true;
}

qint64
NetworkReplyFileq::bytesAvailable() const
{
    if ( m_isClosed ) {
        return QNetworkReply::bytesAvailable();
    }
    return QNetworkReply::bytesAvailable() + m_fileInfo.size() - m_offset;
}

bool
NetworkReplyFileq::isSequential() const
{
    return true;
}

qint64
NetworkReplyFileq::size() const
{
    return m_fileInfo.size();
}

qint64
NetworkReplyFileq::readData( char * data, qint64 maxlen )
{
    if ( m_isClosed ) {
        return - 1;
    }

    QFile file( m_fileInfo.filePath() );
    if ( ! file.open( QIODevice::ReadOnly ) ) {
        return - 1;
    }
    if ( ! file.seek( m_offset ) ) {
        return - 1;
    }
    qint64 ret = file.read( data, maxlen );

    if ( ret >= 0 && file.pos() != m_offset + ret ) {
        qWarning( "offset problems?" );
    }
    m_offset = file.pos();

    file.close();
    return ret;
} // readData

NetworkReplyFileq::NetworkReplyFileq( QObject * parent,
                                const QNetworkRequest & req,
                                const QNetworkAccessManager::Operation op )
    : QNetworkReply( parent )
{
    setRequest( req );
    setUrl( req.url() );
    setOperation( op );
    setFinished( true );
    QNetworkReply::open( QIODevice::ReadOnly );

    QUrl url = req.url();
    if ( url.host() == QLatin1String( "localhost" ) ) {
        url.setHost( QString() );
    }

#if ! defined ( Q_OS_WIN )

    // do not allow UNC paths on Unix
    if ( ! url.host().isEmpty() ) {
        // we handle only local files
        QString msg = QCoreApplication::translate( "QNetworkAccessFileBackend",
                                                   "Request for opening non-local file %1" ).arg(
             url.toString() );
        setError( QNetworkReply::ProtocolInvalidOperationError, msg );
        QMetaObject::invokeMethod( this, "error", Qt::QueuedConnection,
                                   Q_ARG( QNetworkReply::NetworkError,
                                          QNetworkReply::ProtocolInvalidOperationError ) );
        QMetaObject::invokeMethod( this, "finished", Qt::QueuedConnection );
        return;
    }
#endif
    if ( url.path().isEmpty() ) {
        url.setPath( QLatin1String( "/" ) );
    }
    setUrl( url );

    QString fileName = url.toLocalFile();
    if ( fileName.isEmpty() ) {
        if ( url.scheme() == QLatin1String( "qrc" ) ) {
            fileName = QLatin1Char( ':' ) + url.path();
        }
        else {
#if defined ( Q_OS_ANDROID )
            if ( url.scheme() == QLatin1String( "assets" ) ) {
                fileName = QLatin1String( "assets:" ) + url.path();
            }
            else
#endif
            fileName = url.toString(
                QUrl::RemoveScheme | QUrl::RemoveAuthority | QUrl::RemoveFragment |
                QUrl::RemoveQuery );
        }
    }

    m_fileInfo.setFile( fileName );
    if ( m_fileInfo.isDir() ) {
        QString msg = QCoreApplication::translate( "QNetworkAccessFileBackend",
                                                   "Cannot open %1: Path is a directory" ).arg(
             url.toString() );
        setError( QNetworkReply::ContentOperationNotPermittedError, msg );
        QMetaObject::invokeMethod( this, "error", Qt::QueuedConnection,
                                   Q_ARG( QNetworkReply::NetworkError,
                                          QNetworkReply::ContentOperationNotPermittedError ) );
        QMetaObject::invokeMethod( this, "finished", Qt::QueuedConnection );
        return;
    }

    bool canOpen = m_fileInfo.isReadable();

    // could we open the file?
    if ( ! canOpen ) {
        QString msg = QCoreApplication::translate( "QNetworkAccessFileBackend",
                                                   "Error opening %1: %2" )
                          .arg( fileName, "cannot open file" );

        if ( m_fileInfo.exists() ) {
            setError( QNetworkReply::ContentAccessDenied, msg );
            QMetaObject::invokeMethod( this, "error", Qt::QueuedConnection,
                                       Q_ARG( QNetworkReply::NetworkError,
                                              QNetworkReply::ContentAccessDenied ) );
        }
        else {
            setError( QNetworkReply::ContentNotFoundError, msg );
            QMetaObject::invokeMethod( this, "error", Qt::QueuedConnection,
                                       Q_ARG( QNetworkReply::NetworkError,
                                              QNetworkReply::ContentNotFoundError ) );
        }
        QMetaObject::invokeMethod( this, "finished", Qt::QueuedConnection );
        return;
    }

    setHeader( QNetworkRequest::LastModifiedHeader, m_fileInfo.lastModified() );
    setHeader( QNetworkRequest::ContentLengthHeader, m_fileInfo.size() );

    QMetaObject::invokeMethod( this, "metaDataChanged", Qt::QueuedConnection );
    QMetaObject::invokeMethod( this, "downloadProgress", Qt::QueuedConnection,
                               Q_ARG( qint64, m_fileInfo.size() ), Q_ARG( qint64, m_fileInfo.size() ) );
    QMetaObject::invokeMethod( this, "readyRead", Qt::QueuedConnection );
    QMetaObject::invokeMethod( this, "finished", Qt::QueuedConnection );

    m_isClosed = false;
}
}
}

