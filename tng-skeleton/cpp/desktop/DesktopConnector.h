/**
 *
 **/


#ifndef DESKTOP_DESKTOPCONNECTOR_H
#define DESKTOP_DESKTOPCONNECTOR_H

#include <QObject>
#include "common/IConnector.h"
class MainWindow;

class DesktopConnector : public QObject, public IConnector
{
    Q_OBJECT
public:

    /// constructor
    explicit DesktopConnector();

    // implementation of IConnector interface
    virtual bool initialize() Q_DECL_OVERRIDE;
    virtual void setState(const QString & path, const QString & value) Q_DECL_OVERRIDE;
    virtual QString getState(const QString &path) Q_DECL_OVERRIDE;
    virtual CallbackID addCommandCallback( const QString & cmd, const CommandCallback & cb) Q_DECL_OVERRIDE;
    virtual CallbackID addStateCallback(CSR path, const StateChangedCallback &cb) Q_DECL_OVERRIDE;
    virtual void registerView(IView * view) Q_DECL_OVERRIDE;
    virtual void refreshView(IView *view) Q_DECL_OVERRIDE;

    /// javascript bridge
public slots:
    void jsSetStateSlot( const QString & key, const QString & value);
signals:
    // we emit this signal
    // we listen to this signal, and so does javascript
    void stateChangedSignal( const QString & key, const QString & value);
protected slots:
    // this is the callback for stateChangedSignal
    void stateChangedSlot( const QString & key, const QString & value);

public:

    typedef std::vector<CommandCallback> CommandCallbackList;
    std::map<QString,  CommandCallbackList> m_commandCallbackMap;
    typedef std::vector<StateChangedCallback> StateCBList;
    std::map<QString, StateCBList> m_stateCallbackList;

    std::map< QString, QString > m_state;

};


#endif // DESKTOP_DESKTOPCONNECTOR_H
