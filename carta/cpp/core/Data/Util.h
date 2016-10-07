/***
 * Utility class.
 */

#pragma once

#include <State/ObjectManager.h>
#include <CartaLib/AxisInfo.h>
#include <QStringList>
#include <vector>



namespace Carta {
namespace State {
class CartaObject;
}

namespace Lib {
namespace Image {
class ImageInterface;
}
}

namespace Data {

class Util {

public:

    /**
     * Posts the error message, if one exists, and returns the last valid value, if one exists
     * in the case of an error.
     * @param errorMsg {QString} an error message if one occurred; otherwise an empty string.
     * @param revertValue {QString} a string representation of the last valid value
     */
    static void commandPostProcess( const QString& errorMsg );

     /**
      * Returns the singleton object of the given typed class.
      * @return the singleton object with the given typed class.
      */
     template <typename T>
     static T* findSingletonObject( ){
         Carta::State::ObjectManager* objManager = Carta::State::ObjectManager::objectManager();
         T* obj = dynamic_cast<T*>(objManager->getObject( T::CLASS_NAME ));
         if ( obj == NULL ){
             obj = objManager->createObject<T>();
         }
         return obj;
     }

     /**
      * Return the index of the axis of the given type in the image.
      * @param image - an image.
      * @param axisType - an identifier for the type of axis.
      * @return - the (0-based) index of the axis type in the image or -1 if no such
      *     axis type exists in the image.
      */
     static int getAxisIndex( std::shared_ptr<Carta::Lib::Image::ImageInterface> image,
             Carta::Lib::AxisInfo::KnownType axisType );


     /**
      * Returns true if the lists have the same length and elements; false otherwise.
      * @param list1 - {QStringList} the first list to compare.
      * @param list2 - {QStringList} the second list to compare.
      * @return true if the lists are equal; false otherwise.
      */
     static bool isListMatch( const QStringList& list1, const QStringList& list2 );

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
      * @param error {boolean} - true if there was an error converting to doubles; false otherwise.
      * @return a vector of doubles.
      */
     static std::vector < double > string2VectorDouble( QString s, bool* error, QString sep = " " );

     /**
       * Converts a string containing integers with a separator between them to a vector.
       * @param sep {QString} the separator.
       * @param s {QString} a string containing integers separated by a delimiter.
       * @param error {boolean} - true if there was an error converting to integers; false otherwise.
       * @return a vector of integers.
       */
     static std::vector < int > string2VectorInt( QString s, bool* error, QString sep = " " );

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

     static const QString PREFERENCES;
     static const QString ALPHA;
     static const QString COLOR;
     static const QString APPLY;
     static const QString RED;
     static const QString GREEN;
     static const QString BLUE;
     static const QString ERROR;
     static const QString ID;
     static const QString NAME;
     static const QString WIDTH;
     static const QString HEIGHT;
     static const QString POINTER_MOVE;
     static const QString SELECTED;
     static const QString SIGNIFICANT_DIGITS;
     static const QString STYLE;
     static const QString TAB_INDEX;
     static const QString TYPE;
     static const QString UNITS;
     static const QString VISIBLE;
     static const QString VIEW;
     static const QString XCOORD;
     static const QString YCOORD;
     static const QString ZOOM;
     static const int MAX_COLOR;

private:
    Util();
    virtual ~Util();
    Util( const Util& other);
    Util& operator=( const Util& other );
    static const QString TRUE;
    static const QString FALSE;

};
}
}
