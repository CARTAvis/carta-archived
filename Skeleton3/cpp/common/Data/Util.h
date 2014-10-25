/***
 * Utility class.
 */

#pragma once

#include <QVector>

class Util {

public:
    //Utility function that parses a string of the form:  key1:value1,key2:value2,etc for
     //keys contained in the QList and returns a vector containing their corresponding values.
     static QVector<QString> parseParamMap( const QString& params, const QList<QString> & keys );


private:
    Util();
    virtual ~Util();

};
