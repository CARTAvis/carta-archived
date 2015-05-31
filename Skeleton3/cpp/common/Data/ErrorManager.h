/***
 * Stores errors for client reporting.
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "Data/ErrorReport.h"
#include <queue>
#include <memory>

namespace Carta {

namespace Data {


class ErrorManager : public Carta::State::CartaObject {

public:

    /**
     * Add an error.
     * @param errorMsg {QString} user information about the error.
     */
    void registerError( const QString& errorMsg );
    /**
     * Add a warning.
     * @param warningMsg {QString} user information about the warning.
     */
    void registerWarning( const QString& warningMsg );
    virtual ~ErrorManager();

    const static QString CLASS_NAME;
    const static QChar ERROR_SEPARATOR;

private:

    /**
     * Constructor.
     * @param the base path for state identification.
     * @param id the particular id for this object.
     */
    ErrorManager(const QString& path, const QString& id );

    void _addReport( const QString& msg, ErrorSeverity sev);
    bool _addReport( ErrorReport* errorReport );

    class Factory;

    QString _commandGetErrors( const QString& params );

    void _initializeState();
    void _initializeCallbacks();

    static bool m_registered;
    const static QString ERRORS_EXIST;
    std::vector< std::shared_ptr<ErrorReport> > errorList;
    ErrorManager( const ErrorManager& other);
    ErrorManager operator=( const ErrorManager& other );

};
}
}
