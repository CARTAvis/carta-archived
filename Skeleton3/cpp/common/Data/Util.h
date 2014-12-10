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
    //Utility function that parses a string of the form:  key1:value1,key2:value2,etc for
     //keys contained in the QList and returns a map of key value pairs.
     static std::map < QString, QString > parseParamMap( const QString & paramsToParse,
             const std::set < QString > & keyList );
     static bool toBool( const QString str, bool* valid );
     static CartaObject* createObject( const QString& objectName );

private:
    Util();
    virtual ~Util();

};
