#include "IView.h"
#include <QImage>
#include <QColor>

class IConnector;

namespace Carta {
namespace State {
class StateInterface;
}
}

class ImageView: public QObject, public IView {

Q_OBJECT

public:

    ImageView(const QString & viewName, QColor bgColor, QImage img, Carta::State::StateInterface* mouseState);

    void resetImage(QImage img);
    /**
     * Refresh the view.
     */
    void scheduleRedraw();
    virtual void registration(IConnector *connector);
    virtual const QString & name() const;
    virtual QSize size();
    virtual const QImage & getBuffer();
    virtual void handleResizeRequest(const QSize & size);
    virtual void handleMouseEvent(const QMouseEvent & ev);
    virtual void handleKeyEvent(const QKeyEvent & /*event*/) override {}
    virtual void viewRefreshed( qint64 /*id*/) override {}
    static const QString MOUSE;
    static const QString MOUSE_X;
    static const QString MOUSE_Y;
    static const QString VIEW;

signals:
    //Signal the image needs to be resized.
    void resize( const QSize& size );

protected:

    void redrawBuffer();

    QColor m_bgColor;
    QImage m_defaultImage;
    IConnector * m_connector;
    QImage m_qimage;
    QString m_viewName;
    int m_timerId;
    QPointF m_lastMouse;
    Carta::State::StateInterface* m_mouseState;


};

