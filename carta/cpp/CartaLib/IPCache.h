/**
 *
 **/

#pragma once

#include <QJsonObject>
#include <QByteArray>
#include <QString>
#include <memory>

namespace Carta
{
namespace Lib
{
class IPCache
{
public:

    /// return maximum storage in bytes
    virtual uint64_t
    maxStorage() = 0;

    /// return used storage in bytes
    virtual uint64_t
    usedStorage() = 0;

    //// return number of entries
    virtual uint64_t
    nEntries() = 0;

    /// remove all entries
    virtual void
    deleteAll() = 0;

    /// read a value of an entry
    /// if entry does not exist, val will be null
    virtual void
    readEntry( const QByteArray & key,
               QByteArray & val ) = 0;

    /// set a value of an entry
    virtual void
    setEntry( const QByteArray & key,
              const QByteArray & val,
              int64_t priority ) = 0;
};
}
}
