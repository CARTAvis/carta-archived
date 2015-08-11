/***
 * Encapsulates error information.
 */

#pragma once

#include <QString>
namespace Carta {

namespace Data {

enum class ErrorSeverity {INFO,WARN,ERROR,FATAL};

class ErrorReport {

public:
    /**
     * Constructor.
     * @param msg {QString} user information about the error.
     * @param sev the severity lever of the error.
     */
    ErrorReport( const QString& msg, ErrorSeverity sev );

    /**
     * Returns user information about the error.
     * @return user information about the error.
     */
    QString getMessage() const;

    /**
     * Returns the severity level of the error.
     * @return the severity level of the error.
     */
    QString getSeverity() const;

    /**
     * Returns a summary of the error report.
     * @return a summary of the error.
     */
    QString toString();

    /**
     * Equality operator.
     * @param other an ErrorReport to compare this one to.
     * @return true if the ErrorReports are the same; false otherwise.
     */
    bool operator==( const ErrorReport& other ) const;
    virtual ~ErrorReport();
private:
    QString message;
    ErrorSeverity severity;
};
}
}
