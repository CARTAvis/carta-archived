#pragma once

/// \todo move this all into carta::core namespace

#include <QApplication>
#include <functional>
#include "IConnector.h"

/// The only code that should go into this class is Qt specific stuff that needs
/// access to the internals of QApplication.
///
/// @todo For server side this should be QGuiApplication
class MyQApp : public QApplication
{
    Q_OBJECT

public:

    explicit
    MyQApp( int & argc, char * * argv );

#if CARTA_RUNTIME_CHECKS > 0

    /// we override notify() to be able to report exceptions that are thrown
    /// inside slots
    virtual bool
    notify( QObject *, QEvent * ) override;

#endif

protected:
};

/// convenience function for deferred function calls
/// execute the the given function later (essentially at the next noop of the QtMainLoop)
/// \param function to execute
/// \note if you need to execute function with some parameters, use std::bind
void
defer( const std::function < void() > & function );

/// \brief Internal class to implement defer()
/// \internal
///
/// \note It lives in this include file because it's easier to convince MOC to
/// process it (since it's QObject)
class DeferHelper : public QObject
{
    Q_OBJECT

public:

    typedef std::function < void ( ) > VoidFunc;

    void
    queue( const VoidFunc & func );

public slots:

    void
    execute( const VoidFunc & func );
};
