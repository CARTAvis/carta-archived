/***
 * Utility class.
 */

#pragma once
#include <set>
#include <map>
#include <QString>
#include <vector>



namespace Carta {
namespace State {
class CartaObject;
}

namespace Data {

class Util {

public:

     /**
      * Converts the a string of the form true/false into a bool.
      * @param str the string to convert.
      * @param valid a bool whose value will be set to false if the string is not a valid bool.
      * @return the bool value of the str.
      */
     static bool toBool( const QString str, bool* valid );

     /**
      * Converts a bool to a string representation.
      * @param val a bool to convert;
      * @return a QString representation of the bool.
      */
     static QString toString( bool val );

     /**
      * Creates an object of the given class.
      * @param objectName the class name of the object to create.
      * @return the object that was created.
      */
     static Carta::State::CartaObject* createObject( const QString& objectName );

     /**
      * Returns the singleton object of the given class or null if there is no such object.
      * @param objectName the class name of the object to return.
      * @return the singleton object with the corresponding name or null if there is no
      *     such object.
      */
     static Carta::State::CartaObject* findSingletonObject( const QString& objectName );

     /**
      * Posts the error message, if one exists, and returns the last valid value, if one exists
      * in the case of an error.
      * @param errorMsg {QString} an error message if one occurred; otherwise an empty string.
      * @param revertValue {QString} a string representation of the last valid value
      */
     static void commandPostProcess( const QString& errorMsg );



     /**
      * Round the value to the given number of significant digits.
      * @param value the value to round.
      * @param digits the number of significant digits.
      * @return the rounded value.
      */
     static double roundToDigits(double value, int digits);

     /**
      * Converts a string containing doubles with a separator between them to a vector.
      * @param sep {QString} the separator.
      * @param s {QString} a string containing doubles separated by a delimiter.
      * @return a vector of doubles.
      */
     static std::vector < double > string2VectorDouble( QString s, QString sep = " " );

     static const QString STATE_FLUSH;
private:
    Util();
    virtual ~Util();
    static const QString TRUE;
    static const QString FALSE;

};
}
}
