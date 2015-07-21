#include "UtilState.h"
#include "StateInterface.h"
#include <QDebug>

namespace Carta {

namespace State {



UtilState::UtilState( ) {

}

QString UtilState::getLookup( const QString& arrayName, int index ){
    return arrayName + StateInterface::DELIMITER + QString::number( index );
}

QString UtilState::getLookup( const QString& baseName, const QString& subName ){
    return baseName + StateInterface::DELIMITER + subName;
}

std::map < QString, QString >
UtilState::parseParamMap( const QString & paramsToParse, const std::set < QString > & keyList )
{
    std::map < QString, QString > result;
    for ( const auto & entry : paramsToParse.split( ',' ) ) {
        auto keyVal = entry.split( ':' );
        if ( keyVal.size() != 2 ) {
            qWarning() << "bad map format:" << paramsToParse;
            return { };
        }
        auto key = keyVal[0].trimmed();
        auto val = keyVal[1].trimmed();
        auto ind = result.find( key );
        if ( ind != result.end() ) {
            qWarning() << "duplicate key:" << paramsToParse;
            return { };
        }
        result.insert( ind, std::make_pair( key, val ) );
    }

    // make sure every key is in parameters
    for ( const auto & key : keyList ) {
        if ( ! result.count( key ) ) {
            qWarning() << "could not find key=" << key << "in" << paramsToParse;
            return { };
        }

        // make sure parameters don't have unknown keys
    }
    for ( const auto & kv : result ) {
        if ( ! keyList.count( kv.first ) ) {
            qWarning() << "unknown key" << kv.first << "in" << paramsToParse;
            return { };
        }
    }
    return result;
} // parseParamMap



UtilState::~UtilState(){

}
}
}
