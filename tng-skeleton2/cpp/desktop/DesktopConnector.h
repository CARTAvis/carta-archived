/**
 *
 **/


#ifndef DESKTOP_DESKTOPCONNECTOR_H
#define DESKTOP_DESKTOPCONNECTOR_H

#include <QObject>
#include "common/IConnector.h"
#include "common/CallbackList.h"

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
    virtual bool initialize() Q_DECL_OVERRIDE;
    virtual void setState(const QString & path, const QString & newValue) Q_DECL_OVERRIDE;
    virtual QString getState(const QString &path) Q_DECL_OVERRIDE;
    virtual CallbackID addCommandCallback( const QString & cmd, const CommandCallback & cb) Q_DECL_OVERRIDE;
    virtual CallbackID addStateCallback(CSR path, const StateChangedCallback &cb) Q_DECL_OVERRIDE;
    virtual void registerView(IView * view) Q_DECL_OVERRIDE;
    virtual void refreshView(IView *view) Q_DECL_OVERRIDE;
    virtual void removeStateCallback( const CallbackID & id);

public slots:
    /// javascript calls this to set a state
    void jsSetStateSlot( const QString & key, const QString & value);
    /// javascript calls this to send a command
    void jsSendCommandSlot( const QString & cmd, const QString & parameter);

    void jsUpdateViewSlot( const QString & viewName, int width, int height);
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

    void jsViewUpdatedSignal( const QString & viewName, const QImage & img);
//    void jsViewUpdatedSignal( const QString & viewName, const QPixmap & img);

public:

    typedef std::vector<CommandCallback> CommandCallbackList;
    std::map<QString,  CommandCallbackList> m_commandCallbackMap;

    // list of callbacks
//    typedef std::vector<StateChangedCallback> StateCBList;
    typedef CallbackList<CSR, CSR> StateCBList;

    // for each state we maintain a list of callbacks
    std::map<QString, StateCBList *> m_stateCallbackList;

    CallbackID m_callbackNextId;

    std::map< QString, QString > m_state;

    /// private info we keep with each view
    struct ViewInfo;

    /// moving outside the class because it needs slots/signals
//    struct ViewInfo;

    /// map of view names to view infos
    std::map< QString, ViewInfo *> m_views;

    ViewInfo * findViewInfo(const QString &viewName);

    /*
    // custom event for view refresh
    class ViewRefreshEvent : public QEvent {
    public:
        ViewRefreshEvent( QEvent::Type type, IView * iview) : QEvent( type) {
            m_iview = iview;
        }
        IView * view() { return m_iview; }
    protected:
        IView * m_iview;
    };

    // custom event type for view refreshes
    QEvent::Type getRefreshViewEventType(){
        static bool done = false;
        static QEvent::Type res;
        if( ! done) {
            done = true;
            res = static_cast< QEvent::Type > ( QEvent::registerEventType());
        }
        return res;
    }

    virtual void customEvent(QEvent * e) Q_DECL_OVERRIDE
    {
        if( e->type() != getRefreshViewEventType()) {
            return;
        }
        e-> accept();
        ViewRefreshEvent * re = static_cast<ViewRefreshEvent *> (e);
        // do the real refresh of the view
        refreshViewNow( re->view());
    }
    */

    virtual void refreshViewNow(IView *view);

    /// TODO: should we move some of these to protected section?

};


#endif // DESKTOP_DESKTOPCONNECTOR_H
