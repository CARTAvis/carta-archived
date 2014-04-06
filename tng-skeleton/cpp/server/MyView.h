/**
 * This is the license for the CyberSKA image viewer.
 *
 **/

#ifndef MYVIEW_H
#define MYVIEW_H

#include <QObject>
#include <QString>
#include <QImage>

#include "CSI/PureWeb/StateManager/StateManager.h"


class MyView : public QObject, CSI::PureWeb::Server::IRenderedView
{
    Q_OBJECT

public:

    MyView(QObject * parent = 0);
    ~MyView();



    // IRenderedView interface
public:
    virtual void SetClientSize(CSI::PureWeb::Size clientSize);
    virtual CSI::PureWeb::Size GetActualSize();
    virtual void RenderView(CSI::PureWeb::Server::RenderTarget target);
    virtual void PostKeyEvent(const CSI::PureWeb::Ui::PureWebKeyboardEventArgs & ev);
    virtual void PostMouseEvent(const CSI::PureWeb::Ui::PureWebMouseEventArgs & ev);

protected:
    static CSI::PureWeb::Server::StateManager& GetStateManager();

    QString m_viewName;
    QImage m_qimage;
    int m_timerId;
    QPointF m_lastMouse;
    // QObject interface
protected:
    virtual void timerEvent(QTimerEvent *);

};

#endif // MYVIEW_H
