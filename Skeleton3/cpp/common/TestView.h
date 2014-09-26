//#include "Viewer.h"
#include "IView.h"
//#include "Globals.h"
//#include "IPlatform.h"
//#include "IConnector.h"
//#include "misc.h"
//#include "PluginManager.h"
//#include "Data/DataController.h"
//#include <iostream>
#include <QImage>
#include <QColor>
//#include <QPainter>
//#include <cmath>
//#include <QCoreApplication>

class IConnector;

class TestView: public IView {

public:

    TestView(const QString & viewName, QColor bgColor, QImage img);

    void resetImage(QImage img);
    virtual void registration(IConnector *connector);
    virtual const QString & name() const;
    virtual QSize size();
    virtual const QImage & getBuffer();
    virtual void handleResizeRequest(const QSize & size);
    virtual void handleMouseEvent(const QMouseEvent & ev);
    virtual void handleKeyEvent(const QKeyEvent & /*event*/);

protected:

    void redrawBuffer();

    QColor m_bgColor;
    QImage m_defaultImage;
    IConnector * m_connector;
    QImage m_qimage;
    QString m_viewName;
    int m_timerId;
    QPointF m_lastMouse;
};

