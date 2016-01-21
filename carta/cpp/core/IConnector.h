#ifndef ICONNECTOR_H
#define ICONNECTOR_H

#include "IView.h"

#include <memory>
#include <functional>
#include <cstdint>
#include <QString>
#include <QMouseEvent>
#include <QKeyEvent>

namespace Carta {
namespace Lib {
class IRemoteVGView;
}
}

/**
 * @brief The IConnector interface offers common API that the C++ code uses
 * to communicate with the JavaScript side. Different implementations are used
 * for desktop vs server-side versions.
 */

class IConnector {
public:

    /// alias for const QString &
    typedef const QString & CSR;

    /// shared pointer type for convenience
    typedef std::shared_ptr< IConnector > SharedPtr;

    /// signature for command callback
    typedef std::function<QString(CSR cmd, CSR params, CSR sessionId)> CommandCallback;

    /// signature for initialization callback
    typedef std::function<void(bool success)> InitializeCallback;

    /// signature for state changed callback
    typedef std::function<void (CSR path, CSR newValue)> StateChangedCallback;

    /// callback ID type (needed to remove callbacks)
    typedef int64_t CallbackID;

    /// establish a connection to the html5 client
    /// callback is executed when connection is established or failed
    /// callback receives a boolean indicating whether connection is valid or not
    virtual void initialize( const InitializeCallback & cb) = 0;

    /// registers a view with the connector
    virtual void registerView( IView * view) = 0;

    /// asks the connector to schedule a redraw of the view
    /// \param view which view to refresh
    /// \return the id for the refresh (always increasing)
    virtual qint64 refreshView( IView * view) = 0;

    /// unregister a view with the connector
    virtual void unregisterView( const QString& viewName ) = 0;

    /// set state to a new value
    virtual void setState( const QString & path,  const QString & value) = 0;

    /// read state
    virtual QString getState( const QString & path) = 0;

    /// add a callback for a command
    virtual CallbackID addCommandCallback( const QString & cmd, const CommandCallback & cb) = 0;

    /// add a callback for a state change event
    virtual CallbackID addStateCallback( CSR path, const StateChangedCallback & cb) = 0;

    /// add a callback for a tree change

    /// remove a callback for a state change event
    /// \todo maybe we can have a universal 'removeCallback' for commands/states
    virtual void removeStateCallback( const CallbackID & id ) = 0;

    /// return filename where state is saved/restored.
    /// \todo this should not be part of connector, as it has nothing to do with communication
    /// between C++ and JavaScript
    virtual QString getStateLocation( const QString& saveName ) const = 0;

    /// create a vector graphics view
//    virtual Carta::Lib::IRemoteVGView::SharedPtr
    virtual Carta::Lib::IRemoteVGView *
    makeRemoteVGView( QString viewName) = 0;

    virtual ~IConnector() {}
};



#endif // ICONNECTOR_H
