/***
 * The purpose of TimeoutMonitor class was to prevent PureWeb from killing our application
 * if the user has not moved the mouse/touched any keys for a long time. So the client was
 * sending fake events to the server, and this class was the one receiving them. This
 * class would also update the clients telling them how long they have before the server
 * would kill the process, so the clients could warn the user...
 *
 * I don't think this is needed anymore.
 */

// TODO: investigate if this class is still useful or if it could be removed

#ifndef TIMEOUTMONITOR_H
#define TIMEOUTMONITOR_H

#include <QObject>
#include <QTime>

#include "PureWeb.h"


class TimeoutMonitor : public QObject, public CSI::PureWeb::Server::IRenderedView
{
    Q_OBJECT
public:
    explicit TimeoutMonitor(QObject *parent = 0);

signals:

public slots:
    void start();

private: // methods

    // following 5 methods are needed to implement IRenderView interface
    virtual void SetClientSize(CSI::PureWeb::Size /*clientSize*/) {}
    virtual CSI::PureWeb::Size GetActualSize() { return CSI::PureWeb::Size(1,1); }
    virtual void RenderView(CSI::PureWeb::Server::RenderTarget /*image*/) { return; }
    virtual void PostKeyEvent(const CSI::PureWeb::Ui::PureWebKeyboardEventArgs& args);
    virtual void PostMouseEvent(const CSI::PureWeb::Ui::PureWebMouseEventArgs& mouseEvent);

    // self timer
    virtual void timerEvent (QTimerEvent *);

    // this is the timeout that should be set to the same value as PureWeb uses
    int pureWebTimeout_;
    // the time when the last event was received
    QTime lastTimeoutReset_;
    // ping counter (to make xml setvalues unique)
    quint64 pingCounter_;

    static constexpr double WarningTime = 30.0;
    static constexpr double DefaultPureWebTimeout = 300;
};

#endif // TIMEOUTMONITOR_H
