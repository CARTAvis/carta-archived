#include "Util.h"
#include "Data/Error/ErrorManager.h"
#include "CartaLib/ICoordinateFormatter.h"
#include "CartaLib/IImage.h"
#include <QDebug>
#include <cmath>

namespace Carta {

namespace Data {

const QString Util::ALPHA = "alpha";
const QString Util::APPLY = "apply";
const QString Util::COLOR = "color";
const QString Util::TRUE = "true";
const QString Util::FALSE = "false";
const QString Util::ERROR = "Error";
const QString Util::PREFERENCES = "preferences";
const QString Util::RED = "red";
const QString Util::BLUE = "blue";
const QString Util::GREEN = "green";
const QString Util::ID = "id";
const QString Util::NAME = "name";
const QString Util::WIDTH = "width";
const QString Util::HEIGHT = "height";
const QString Util::TAB_INDEX = "tabIndex";
const QString Util::TYPE = "type";
const QString Util::UNITS = "units";
const QString Util::VISIBLE = "visible";
const QString Util::POINTER_MOVE = "pointer-move";
const QString Util::SELECTED = "selected";
const QString Util::SIGNIFICANT_DIGITS = "significantDigits";
const QString Util::STYLE = "style";
const QString Util::VIEW = "view";
const QString Util::XCOORD = "x";
const QString Util::YCOORD = "y";
const QString Util::ZOOM = "zoom";
const int Util::MAX_COLOR = 255;

Util::Util( ) {

}


void Util::commandPostProcess( const QString& errorMsg){
    if ( errorMsg.trimmed().length() > 0 ){
        ErrorManager* errorMan = Util::findSingletonObject<ErrorManager>();
        errorMan->registerWarning( errorMsg );
    }
}


int Util::getAxisIndex( std::shared_ptr<Carta::Lib::Image::ImageInterface> image,
        Carta::Lib::AxisInfo::KnownType axisType ){
    int index = -1;
    if ( image ){
        std::shared_ptr<CoordinateFormatterInterface> cf(
                               image-> metaData()-> coordinateFormatter()-> clone() );
        int axisCount = cf->nAxes();
        for ( int i = 0; i < axisCount; i++ ){
            Carta::Lib::AxisInfo axisInfo = cf->axisInfo( i );
            if ( axisInfo.knownType() == axisType ){
                index = i;
                break;
            }
        }
    }
    return index;
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


double Util::roundToDigits(double value, int digits){
    if ( value == 0 ) {
        return 0;
    }
    double factor = pow(10.0, digits - ceil(log10(fabs(value))));
    return round(value * factor) / factor;
}


/// convert string to array of doubles
std::vector < double > Util::string2VectorDouble( QString s, bool* error, QString sep ){
    QStringList lst = s.split( sep );
    std::vector < double > res;
    *error = false;
    for ( auto v : lst ) {
        bool ok;
        double val = v.toDouble( & ok );
        if ( ! ok ) {
            *error = true;
            return res;
        }
        res.push_back( val );
    }
    return res;
}


/// convert string to array of doubles
std::vector < int > Util::string2VectorInt( QString s, bool* error, QString sep ){
    QStringList lst = s.split( sep );
    std::vector < int > res;
    *error = false;
    for ( auto v : lst ) {
        bool ok;
        int val = v.toInt( & ok );
        if ( ! ok ) {
            *error = true;
            return res;
        }
        res.push_back( val );
    }
    return res;
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


Util::~Util(){

}
}
}
