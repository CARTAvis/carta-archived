#include "PCacheSqlite3.h"
#include "CartaLib/Hooks/GetPersistentCache.h"
#include <QDebug>
#include <QtSql>
#include <QDir>

typedef Carta::Lib::Hooks::GetPersistentCache GetPersistentCacheHook;

///
/// Implementation of IPCache using sqlite
///
class SqLitePCache : public Carta::Lib::IPCache
{
public:

    virtual uint64_t
    maxStorage() override
    {
        return 1;
    }

    virtual uint64_t
    usedStorage() override
    {
        return 1;
    }

    virtual uint64_t
    nEntries() override
    {
        return 1;
    }

    virtual void
    deleteAll() override
    {
        // not implemented
    }

    virtual bool
    readEntry( const QByteArray & key, QByteArray & val ) override
    {
        if ( ! m_db.isOpen() ) {
            return false;
        }
        QSqlQuery query( m_db );
        query.prepare( "SELECT val FROM db WHERE key = :key" );
        query.bindValue( ":key", key );

        //    qDebug() << "select query" << query.lastQuery();
        if ( ! query.exec() ) {
            qWarning() << "query read failed";
            return false;
        }
        if ( query.next() ) {
            val = query.value( 0 ).toByteArray();
            return true;
        }
        return false;
    } // readEntry

    virtual void
    setEntry( const QByteArray & key, const QByteArray & val, int64_t priority ) override
    {
        if ( ! m_db.isOpen() ) {
            return;
        }

        Q_UNUSED( priority );
        QSqlQuery query( m_db );
        query.prepare( "INSERT OR REPLACE INTO db (key, val) VALUES ( :key, :val)" );

        //    query.prepare( "INSERT  INTO db (key, val) VALUES ( :key, :val)" );
        query.bindValue( ":key", key );
        query.bindValue( ":val", val );

        //    qDebug() << "insert query" << query.lastQuery();
        if ( ! query.exec() ) {
            qWarning() << "query insert failed"
                       << query.lastError().text();
        }
    } // setEntry

    static
    Carta::Lib::IPCache::SharedPtr
    getCacheSingleton( QString dirPath)
    {
        if ( m_cachePtr ) {
            qCritical() << "PCacheSQlite3Plugin::Calling GetPersistentCacheHook multiple times!!!";
        }
        else {
            // stupid c++ won't allow this
            // m_cachePtr = std::make_shared < SqLitePCache > ();
            m_cachePtr.reset( new SqLitePCache( dirPath) );
        }
        return m_cachePtr;
    }

    ~SqLitePCache()
    {
        qDebug() << "Destroying SqLitePCache";
        qDebug() << "Possible segmentation fault below, it's a bug and we'll try to fix it.";
        m_db.close();
        qDebug() << "Ok, no segmentation fault occurred, whew :)";
    }

private:

    SqLitePCache( QString dirPath)
    {
        m_db = QSqlDatabase::addDatabase( "QSQLITE" );

        QString fname = dirPath + "/pcache.sqlite";

        //    m_db.setDatabaseName( "/scratchSD/test.sqlite" );
        m_db.setDatabaseName( fname );
        bool ok = m_db.open();
        if ( ! ok ) {
            qCritical() << "Could not open sqlite database";
            qCritical() << "  - at location:" + fname;
        }

        QSqlQuery query( m_db );

        query.prepare( "create table db (key text primary key, "
                       "val blob)" );

        //    qDebug() << "create query" << query.lastQuery();
        if ( ! query.exec() ) {
            qCritical() << "query create table failed"
                        << query.lastError().text();
        }
    }

private:

    QSqlDatabase m_db;
    static Carta::Lib::IPCache::SharedPtr m_cachePtr; //  = nullptr;
};

Carta::Lib::IPCache::SharedPtr SqLitePCache::m_cachePtr = nullptr;

PCacheSQlite3Plugin::PCacheSQlite3Plugin( QObject * parent ) :
    QObject( parent )
{ }

bool
PCacheSQlite3Plugin::handleHook( BaseHook & hookData )
{
    // we only handle one hook: get the cache object
    if ( hookData.is < GetPersistentCacheHook > () ) {
        // decode hook data
        GetPersistentCacheHook & hook = static_cast < GetPersistentCacheHook & > ( hookData );

        // if no dbdir was specified, refuse to work :)
        if( m_dbDir.isNull()) {
            hook.result.reset();
            return false;
        }

        // try to create the database
        hook.result = SqLitePCache::getCacheSingleton( m_dbDir);

        // return true if result is not null
        return hook.result != nullptr;
    }

    qWarning() << "PCacheSQlite3Plugin: Sorrry, dont' know how to handle this hook";
    return false;
} // handleHook

void
PCacheSQlite3Plugin::initialize( const IPlugin::InitInfo & initInfo )
{
    qDebug() << "PCacheSQlite3Plugin::initialized";
    QJsonDocument doc( initInfo.json );
    qDebug() << doc.toJson();

    // extract the location of the database from carta.config
    m_dbDir = initInfo.json.value( "dbDir").toString();
    if( m_dbDir.isNull()) {
        qCritical() << "No dbDir specified for PCacheSqlite3 plugin!!!";
    }
    else {
        // convert this to absolute path just in case
        m_dbDir = QDir(m_dbDir).absolutePath();
    }
}

std::vector < HookId >
PCacheSQlite3Plugin::getInitialHookList()
{
    return {
               GetPersistentCacheHook::staticId
    };
}
