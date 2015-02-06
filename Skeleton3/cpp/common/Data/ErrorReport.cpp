/**
 * Encapsulations information to be reported to the user.
 */
#include "ErrorReport.h"

#include <QDebug>
#include <cassert>

namespace Carta {

namespace Data {


ErrorReport::ErrorReport( const QString& msg, ErrorSeverity sev ) :
    message(msg),
    severity(sev){
}

QString ErrorReport::getMessage() const {
    return message;
}

QString ErrorReport::getSeverity() const {
    QString sevStr;
    if ( severity == ErrorSeverity::INFO ){
        sevStr = "Info";
    }
    else if ( severity == ErrorSeverity::WARN ){
        sevStr = "Warn";
    }
    else if ( severity == ErrorSeverity::ERROR ){
        sevStr = "Error";
    }
    else if ( severity == ErrorSeverity::FATAL ){
        sevStr = "Fatal";
    }
    else {
        qDebug() << "Unsupported severity";
    }
   return sevStr;
}

QString ErrorReport::toString(){
    return getSeverity() + ":" + getMessage();
}


ErrorReport::~ErrorReport() {

}
}
}
