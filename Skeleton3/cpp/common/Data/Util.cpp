#include "Util.h"
#include "State/ObjectManager.h"
#include "Data/ErrorManager.h"
#include <QStringList>
#include <QDebug>
#include <cmath>

namespace Carta {

namespace Data {

const QString Util::TRUE = "true";
const QString Util::FALSE = "false";
const QString Util::STATE_FLUSH = "stateFlush";

Util::Util( ) {

}

std::map < QString, QString >
Util::parseParamMap( const QString & paramsToParse, const std::set < QString > & keyList )
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

bool Util::toBool( const QString str, bool* valid ){
    *valid = false;
    bool result = false;
    if ( str == TRUE ){
        *valid = true;
        result = true;
    }
    else if ( str == FALSE ){
        *valid = true;
    }
    return result;
}

QString Util::toString( bool val ){
    QString result = FALSE;
    if ( val ){
        result = TRUE;
    }
    return result;
}

CartaObject* Util::createObject( const QString& objectName ){
    ObjectManager* objManager = ObjectManager::objectManager();
    QString objectId = objManager->createObject( objectName );
    CartaObject* cartaObj = objManager->getObject( objectId );
    return cartaObj;
}

CartaObject* Util::findSingletonObject( const QString& objectName ){
    ObjectManager* objManager = ObjectManager::objectManager();
    CartaObject* obj = objManager->getObject( objectName );
    if ( obj == NULL ){
        obj = createObject( objectName );
    }
    return obj;
}

QString Util::getLookup( const QString& arrayName, int index ){
    return arrayName + StateInterface::DELIMITER + QString::number( index );
}

void Util::commandPostProcess( const QString& errorMsg){
    if ( errorMsg.trimmed().length() > 0 ){
        CartaObject* obj = Util::findSingletonObject( ErrorManager::CLASS_NAME );
        ErrorManager* errorMan = dynamic_cast<ErrorManager*>(obj);
        errorMan->registerWarning( errorMsg );
    }
}

double Util::roundToDigits(double value, int digits)
{
    double factor = pow(10.0, digits - ceil(log10(fabs(value))));
    return round(value * factor) / factor;
}

/// convert string to array of doubles
std::vector < double > Util::string2VectorDouble( QString s, QString sep){
    QStringList lst = s.split( sep );
    std::vector < double > res;
    for ( auto v : lst ) {
        bool ok;
        double val = v.toDouble( & ok );
        if ( ! ok ) {
            return res;
        }
        res.push_back( val );
    }
    return res;
}

Util::~Util(){

}
}
}
