/***
 * Utility class for processing state information.
 */

#pragma once
#include <set>
#include <map>
#include <QString>

class CartaObject;

namespace Carta {

namespace State {

class UtilState {

public:

    /**
      * Return the lookup key for an array element with a given name and index.
      * @param arrayName - the name of a state array.
      * @param index - the array index.
      * @return the lookup key for the array element.
      */
     static QString getLookup( const QString& arrayName, int index );

     /**
      * Return the lookup key for a child element based on a parent name.
      * @param baseName - a lookup for a parent element.
      * @param subName - an identifier for a child of the parent element.
      * @return the lookup key for the child element.
      */
     static QString getLookup( const QString& baseName, const QString& subName );

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

private:
    UtilState();
    virtual ~UtilState();
};
}
}
