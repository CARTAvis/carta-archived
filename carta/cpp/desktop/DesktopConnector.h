/**
 *
 **/


#ifndef DESKTOP_DESKTOPCONNECTOR_H
#define DESKTOP_DESKTOPCONNECTOR_H

#include <QObject>
#include "core/IConnector.h"
#include "core/CallbackList.h"

#ifndef CARTA_RUNTIME_CHECKS
#define CARTA_RUNTIME_CHECKS 0
#endif

class MainWindow;
class IView;

/// private info we keep with each view
/// unfortunately it needs to live as it's own class because we need to give it slots...
//class ViewInfo;

class DesktopConnector : public QObject, public IConnector
{
    Q_OBJECT
public:

    /// constructor
    explicit DesktopConnector();

    // implementation of IConnector interface
    virtual void initialize( const InitializeCallback & cb) Q_DECL_OVERRIDE;
    //virtual void setState(const QString & path, const QString & newValue) Q_DECL_OVERRIDE;
    //virtual QString getState(const QString &path) Q_DECL_OVERRIDE;
    virtual void setState(const QString& state, const QString & newValue) Q_DECL_OVERRIDE;

    //Return the value of the state with the given key and window id.
    virtual QString getState(const QString&) Q_DECL_OVERRIDE;

    virtual CallbackID addCommandCallback( const QString & cmd, const CommandCallback & cb) Q_DECL_OVERRIDE;
    virtual CallbackID addStateCallback(CSR path, const StateChangedCallback &cb) Q_DECL_OVERRIDE;
    virtual void registerView(IView * view) Q_DECL_OVERRIDE;

    void unregisterView( const QString& viewName ) Q_DECL_OVERRIDE;
    virtual void refreshView(IView *view) Q_DECL_OVERRIDE;
    virtual void removeStateCallback( const CallbackID & id);

    /// Return the location where the state is saved.
    virtual QString getStateLocation( const QString& saveName ) const;

public slots:

    /// javascript calls this to set a state
    void jsSetStateSlot( const QString & key, const QString & value);
    /// javascript calls this to send a command
    void jsSendCommandSlot( const QString & cmd, const QString & parameter);
    /// javascript calls this to let us know js connector is ready
    void jsConnectorReadySlot();
    /// javascript calls this when view is resized
    void jsUpdateViewSlot( const QString & viewName, int width, int height);
    /// javascript calls this on mouse move inside a view
    /// \deprecated
    void jsMouseMoveSlot( const QString & viewName, int x, int y);

    /// this is the callback for stateChangedSignal
    void stateChangedSlot( const QString & key, const QString & value);

signals:

    /// we emit this signal when state is changed (either by c++ or by javascript)
    /// we listen to this signal, and so does javascript
    /// our listener then calls callbacks registered for this value
    /// javascript listener caches the new value and also calls registered callbacks
    void stateChangedSignal( const QString & key, const QString & value);
    /// we emit this signal when command results are ready
    /// javascript listens to it
    void jsCommandResultsSignal( const QString & results);
    /// emitted by c++ when we want javascript to repaint the view
    void jsViewUpdatedSignal( const QString & viewName, const QImage & img);

public:

    typedef std::vector<CommandCallback> CommandCallbackList;
    std::map<QString,  CommandCallbackList> m_commandCallbackMap;

    // list of callbacks
    typedef CallbackList<CSR, CSR> StateCBList;

    /// for each state we maintain a list of callbacks
    std::map<QString, StateCBList *> m_stateCallbackList;

    /// IDs for command callbacks
    CallbackID m_callbackNextId;

    /// private info we keep with each view
    struct ViewInfo;

    /// map of view names to view infos
    std::map< QString, ViewInfo *> m_views;

    ViewInfo * findViewInfo(const QString &viewName);

    virtual void refreshViewNow(IView *view);

    /// @todo move as may of these as possible to protected section

protected:

    InitializeCallback m_initializeCallback;
    std::map< QString, QString > m_state;

};


#endif // DESKTOP_DESKTOPCONNECTOR_H
