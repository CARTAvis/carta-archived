#include "Util.h"
#include "Data/Error/ErrorManager.h"
#include <QDebug>
#include <cmath>

namespace Carta {

namespace Data {

const QString Util::TRUE = "true";
const QString Util::FALSE = "false";
const QString Util::PREFERENCES = "preferences";

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


void Util::commandPostProcess( const QString& errorMsg){
    if ( errorMsg.trimmed().length() > 0 ){
        ErrorManager* errorMan = Util::findSingletonObject<ErrorManager>();
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
    if ( value == 0 ) {
        return 0;
    }
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
