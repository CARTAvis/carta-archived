#include "PCacheLevelDB.h"
#include "CartaLib/Hooks/GetPersistentCache.h"
#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include "leveldb/db.h"

typedef Carta::Lib::Hooks::GetPersistentCache GetPersistentCacheHook;

///
/// Implementation of IPCache using LevelDB
///
class LevelDBPCache : public Carta::Lib::IPCache
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
        if ( ! p_db ) {
            return;
        }
        
        leveldb::Iterator* it = p_db->NewIterator(p_readOptions);
        for (it->SeekToFirst(); it->Valid(); it->Next()) {
            auto status = p_db->Delete(p_writeOptions, it->key());
            if ( ! status.ok() ) {
                qWarning() << "query delete failed:" << status.ToString().c_str();
            }
        }
    } // deleteAll

    virtual bool
    readEntry( const QByteArray & key, QByteArray & val ) override
    {
        
        if ( ! p_db ) {
            return false;
        }
        std::string tmpVal;
        auto status = p_db-> Get( p_readOptions, key.constData(), & tmpVal );
        if ( ! status.ok() ) {
            //qWarning() << "query read failed:" << status.ToString().c_str();
            return false;
        }
        val = QByteArray( tmpVal.data(), tmpVal.size());
        return true;
    } // readEntry

    virtual void
    setEntry( const QByteArray & key, const QByteArray & val, int64_t priority ) override
    {
        if( ! p_db) return;
        auto status = p_db-> Put( p_writeOptions,
                    leveldb::Slice( key.constData(), key.size()),
                    leveldb::Slice( val.constData(), val.size()));
        if ( ! status.ok() ) {
            qWarning() << "query insert failed:" << status.ToString().c_str();
        }

        Q_UNUSED( priority);
        // \todo we'll have to embed  priority into the value, e.g. first 8 bytes?
    } // setEntry

    static
    Carta::Lib::IPCache::SharedPtr
    getCacheSingleton( QString dirPath)
    {
        if ( m_cachePtr ) {
            qCritical() << "PCacheLevelDBPlugin::Calling GetPersistentCacheHook multiple times!!!";
        }
        else {
            m_cachePtr.reset( new LevelDBPCache( dirPath) );
        }
        return m_cachePtr;
    }

    //~LevelDBPCache()
    //{
        // I don't think a destructor is required; leaving this as a placeholder for now
    //}

private:

    LevelDBPCache( QString dirPath)
    {
        p_readOptions = leveldb::ReadOptions();
        p_writeOptions = leveldb::WriteOptions();
        p_writeOptions.sync = false;

        // Set up database connection information and open database
        leveldb::DB * db;
        leveldb::Options options;

        options.create_if_missing = true;
        
        leveldb::Status status = leveldb::DB::Open( options, dirPath.toStdString(), & db );

        if ( false == status.ok() ) {
            qDebug() << "Unable to open/create database '" << dirPath.toStdString().c_str() << "':" << status.ToString().c_str();
            return;
        }

        p_db.reset( db );
    }

    std::unique_ptr< leveldb::DB > p_db;
    leveldb::ReadOptions p_readOptions;
    leveldb::WriteOptions p_writeOptions;
    static Carta::Lib::IPCache::SharedPtr m_cachePtr; //  = nullptr;
};

Carta::Lib::IPCache::SharedPtr LevelDBPCache::m_cachePtr = nullptr;

PCacheLevelDBPlugin::PCacheLevelDBPlugin( QObject * parent ) :
    QObject( parent )
{ }

bool
PCacheLevelDBPlugin::handleHook( BaseHook & hookData )
{
    // we only handle one hook: get the cache object
    if ( hookData.is < GetPersistentCacheHook > () ) {
        // decode hook data
        GetPersistentCacheHook & hook = static_cast < GetPersistentCacheHook & > ( hookData );

        // if no dbdir was specified, refuse to work :)
        if( m_dbPath.isNull()) {
            hook.result.reset();
            return false;
        }

        // try to create the database
        hook.result = LevelDBPCache::getCacheSingleton( m_dbPath);

        // return true if result is not null
        return hook.result != nullptr;
    }

    qWarning() << "PCacheLevelDBPlugin: Sorrry, dont' know how to handle this hook";
    return false;
} // handleHook

void
PCacheLevelDBPlugin::initialize( const IPlugin::InitInfo & initInfo )
{
    qDebug() << "PCacheLevelDBPlugin::initialized";
    QJsonDocument doc( initInfo.json );
    qDebug() << doc.toJson();

    // extract the location of the database from carta.config
    m_dbPath = initInfo.json.value( "dbPath").toString();
    if( m_dbPath.isNull()) {
        qCritical() << "No dbPath specified for PCacheLevelDB plugin!!!";
    }
    else {
        // convert this to absolute path just in case
        m_dbPath = QDir(m_dbPath).absolutePath();
    }
}

std::vector < HookId >
PCacheLevelDBPlugin::getInitialHookList()
{
    return {
               GetPersistentCacheHook::staticId
    };
}
