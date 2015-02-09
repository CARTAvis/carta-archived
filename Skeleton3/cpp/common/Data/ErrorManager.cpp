#include "ErrorManager.h"

#include <cassert>
#include <QDebug>

namespace Carta {

namespace Data {

const QString ErrorManager::CLASS_NAME = "ErrorManager";
const QChar ErrorManager::ERROR_SEPARATOR = '#';
const QString ErrorManager::ERRORS_EXIST = "errorsExist";

class ErrorManager::Factory : public CartaObjectFactory {

    public:

        Factory():
         CartaObjectFactory( CLASS_NAME ){};

        CartaObject * create (const QString & path, const QString & id)
        {
            return new ErrorManager (path, id);
        }
    };

bool ErrorManager::m_registered =
    ObjectManager::objectManager()->registerClass ( CLASS_NAME,
                                                   new ErrorManager::Factory());


ErrorManager::ErrorManager(const QString& path, const QString& id) :
    CartaObject( CLASS_NAME, path, id){
        _initializeState();
        _initializeCallbacks();
}

QString ErrorManager::_commandGetErrors( const QString& /*params*/ ){
    QString errors;
    while ( !errorList.empty() ){
        std::shared_ptr<ErrorReport> report = errorList.front();
        errors.append( report->toString());
        errorList.pop();
        if ( errorList.size() > 0 ){
            errors.append( ERROR_SEPARATOR );
        }
    }
    bool existingErrors = m_state.getValue<bool>( ERRORS_EXIST );
    if ( existingErrors ){
        m_state.setValue<bool>( ERRORS_EXIST, false );
    }
    return errors;
}

void ErrorManager::_initializeCallbacks(){
    addCommandCallback( "getErrors", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) -> QString {
                QString result = _commandGetErrors( params );
                return result;
            });
}

void ErrorManager::_initializeState(){
    m_state.insertValue<bool>( ERRORS_EXIST, false);
    m_state.flushState();
}

void ErrorManager::registerError( const QString& errorMsg ){
    _addReport( errorMsg, ErrorSeverity::ERROR );
    qCritical() << errorMsg;
}

void ErrorManager::registerWarning( const QString& warningMsg ){
    _addReport(warningMsg, ErrorSeverity::WARN );
    qWarning() << warningMsg;
}

void ErrorManager::_addReport( const QString& msg, ErrorSeverity sev){
    assert( msg.trimmed().length() > 0 );
    QString cleanedMsg(msg);
    cleanedMsg.replace(ERROR_SEPARATOR, ' ');
    ErrorReport* report = new ErrorReport(cleanedMsg, sev );
    errorList.push( std::shared_ptr<ErrorReport>(report));
    bool existingErrors = m_state.getValue<bool>( ERRORS_EXIST );
    if ( !existingErrors ){
        m_state.setValue<bool>(ERRORS_EXIST, true );
        m_state.flushState();
    }
}

ErrorManager::~ErrorManager() {

}
}
}
