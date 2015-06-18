#include "Util.h"
#include "State/ObjectManager.h"
#include "Data/Error/ErrorManager.h"
#include <QStringList>
#include <QDebug>
#include <cmath>

namespace Carta {

namespace Data {

const QString Util::TRUE = "true";
const QString Util::FALSE = "false";

Util::Util( ) {

}


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

Carta::State::CartaObject* Util::createObject( const QString& objectName ){
    Carta::State::ObjectManager* objManager = Carta::State::ObjectManager::objectManager();
    QString objectId = objManager->createObject( objectName );
    Carta::State::CartaObject* cartaObj = objManager->getObject( objectId );
    return cartaObj;
}

Carta::State::CartaObject* Util::findSingletonObject( const QString& objectName ){
    Carta::State::ObjectManager* objManager = Carta::State::ObjectManager::objectManager();
    Carta::State::CartaObject* obj = objManager->getObject( objectName );
    if ( obj == NULL ){
        obj = createObject( objectName );
    }
    return obj;
}



void Util::commandPostProcess( const QString& errorMsg){
    if ( errorMsg.trimmed().length() > 0 ){
        Carta::State::CartaObject* obj = Util::findSingletonObject( ErrorManager::CLASS_NAME );
        ErrorManager* errorMan = dynamic_cast<ErrorManager*>(obj);
        errorMan->registerWarning( errorMsg );
    }
}

bool Util::isListMatch( const QStringList& list1, const QStringList& list2 ){
    bool listEqual = true;
    int listSize = list1.size();
    if ( listSize != list2.size() ){
        listEqual = false;
    }
    else {
        for ( int i = 0; i < listSize; i++ ){
            if ( list1[i] != list2[i] ){
                listEqual = false;
                break;
            }
        }
    }
    return listEqual;
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
