/**
  * This is the class that actually implements the fileq protocol.
  *
  * Most of the code here was copied from QNetworkReplyFileImpl
  *
  **/

#pragma once

#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QFile>
#include <QFileInfo>

namespace Carta
{
namespace Desktop
{
class NetworkReplyFileq : public QNetworkReply
{
    Q_OBJECT

public:

    NetworkReplyFileq( QObject * parent,
                    const QNetworkRequest & req,
                    const QNetworkAccessManager::Operation op );
    ~NetworkReplyFileq();
    virtual void
    abort() override;

    virtual void
    close() override;

    virtual qint64
    bytesAvailable() const override;

    virtual bool
    isSequential() const override;

    qint64
    size() const override;

    virtual qint64
    readData( char * data, qint64 maxlen ) override;

private:

    bool m_isClosed = true;
    qint64 m_offset = 0;
    QFileInfo m_fileInfo;
};
}
}
