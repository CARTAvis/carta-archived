///**
 //*
 //**/

//#include "SqLitePCache.h"
//#include <QDebug>
//#include <QtSql>

//namespace tCache
//{
//SqLitePCache::SqLitePCache()
//{
    //m_db = QSqlDatabase::addDatabase( "QSQLITE" );
////    m_db.setDatabaseName( "/scratchSD/test.sqlite" );
    //m_db.setDatabaseName( "/scratch/test.sqlite" );
    //bool ok = m_db.open();
    //if ( ! ok ) {
        //qCritical() << "Could not open sqlite database";
    //}

    //QSqlQuery query(m_db);

    //query.prepare( "create table db (key text primary key, "
                   //"val blob)" );
////    qDebug() << "create query" << query.lastQuery();
    //if( ! query.exec()) {
        //qCritical() << "query create table failed"
                    //<<  query.lastError().text();
    //}
//}

//bool
//SqLitePCache::readEntry( const QByteArray & key, QByteArray & val )
//{
    //QSqlQuery query(m_db);
    //query.prepare( "SELECT val FROM db WHERE key = :key" );
    //query.bindValue( ":key", key );
////    qDebug() << "select query" << query.lastQuery();
    //if ( ! query.exec() ) {
        //qWarning() << "query read failed";
        //return false;
    //}
    //if ( query.next() ) {
        //val = query.value( 0 ).toByteArray();
        //return true;
    //}
    //return false;
//}

//void
//SqLitePCache::setEntry( const QByteArray & key, const QByteArray & val, int64_t priority )
//{
    //QSqlQuery query(m_db);
    //query.prepare( "INSERT OR REPLACE INTO db (key, val) VALUES ( :key, :val)" );
////    query.prepare( "INSERT  INTO db (key, val) VALUES ( :key, :val)" );
    //query.bindValue( ":key", key );
    //query.bindValue( ":val", val );
////    qDebug() << "insert query" << query.lastQuery();
    //if( ! query.exec()) {
        //qWarning() << "query insert failed"
                   //<<  query.lastError().text();

    //}
//}
//}
