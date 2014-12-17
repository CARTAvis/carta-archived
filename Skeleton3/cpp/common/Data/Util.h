/***
 * Utility class.
 */

#pragma once
#include <set>
#include <map>
#include <QString>

class CartaObject;

class Util {

public:
     /**
      * Parses a string of the form:  key1:value1,key2:value2,etc for
      * keys contained in the QList and returns a map of key value pairs.
      * @param paramsToParse the string to parse.
      * @param keyList a set containing the expected keys in the string.
      * @return a map containing the (key,value) pairs in the string.  An empty map will
      *     be returned is the keys in the string do not match those in the keyList.
      */
     static std::map < QString, QString > parseParamMap( const QString & paramsToParse,
             const std::set < QString > & keyList );

     /**
      * Converts the a string of the form true/false into a bool.
      * @param str the string to convert.
      * @param valid a bool whose value will be set to false if the string is not a valid bool.
      * @return the bool value of the str.
      */
     static bool toBool( const QString str, bool* valid );

     /**
      * Creates an object of the given class.
      * @param objectName the class name of the object to create.
      * @return the object that was created.
      */
     static CartaObject* createObject( const QString& objectName );

     /**
      * Returns the singleton object of the given class or null if there is no such object.
      * @param objectName the class name of the object to return.
      * @return the singleton object with the corresponding name or null if there is no
      *     such object.
      */
     static CartaObject* findSingletonObject( const QString& objectName );
private:
    Util();
    virtual ~Util();

};
