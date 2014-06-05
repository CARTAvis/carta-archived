#include "Viewer.h"
#include "misc.h"
#include <iostream>
#include <QImage>
#include <QColor>
#include <QPainter>
#include <cmath>

class TestView : public IView
{

public:

    TestView() {
        m_qimage = QImage( 100, 100, QImage::Format_RGB888);
        m_qimage.fill( QColor( "blue"));

        m_viewName = "view1";
        m_connector= nullptr;
    }

    virtual void registration(IConnector *connector)
    {
        m_connector = connector;
    }
    virtual const QString & name() const
    {
        return m_viewName;
    }
    virtual QSize size()
    {
        return m_qimage.size();
    }
    virtual const QImage & getBuffer()
    {
        return m_qimage;
    }
    virtual void handleResizeRequest(const QSize & size)
    {
//        m_qimage = QImage( size, m_qimage.format());
    }
    virtual void handleMouseEvent(const QMouseEvent & ev)
    {
        std::cerr << "Mouse " << ev.x() << "," << ev.y() << "\n";

        m_lastMouse = QPointF( ev.x(), ev.y());


        QPointF center = m_qimage.rect().center();
        QPointF diff = m_lastMouse - center;
        double angle = atan2( diff.x(), diff.y());
        angle *= - 180 / M_PI;

        m_qimage.fill( QColor( "blue"));
        QPainter p( & m_qimage);
        p.setPen( Qt::NoPen);
        p.setBrush( QColor( 255, 255, 0, 128));
        p.drawEllipse( QPoint(ev.x(), ev.y()), 10, 10 );
        p.setPen( QColor( 255, 255, 255));
        p.drawLine( 0, ev.y(), m_qimage.width()-1, ev.y());
        p.drawLine( ev.x(), 0, ev.x(), m_qimage.height()-1);

        p.translate( m_qimage.rect().center());
        p.rotate( angle);
        p.translate( - m_qimage.rect().center());
        p.setFont( QFont( "Arial", 20));
        p.setPen( QColor( "white"));
        p.drawText( m_qimage.rect(), Qt::AlignCenter, m_viewName);

        m_connector-> refreshView( this);
    }
    virtual void handleKeyEvent(const QKeyEvent & /*event*/)
    {
    }


protected:
    IConnector * m_connector;
    QImage m_qimage;
    QString m_viewName;
    int m_timerId;
    QPointF m_lastMouse;
};


Viewer::Viewer( IPlatform * platform) :
    QObject( nullptr)
{
    m_platform = platform;
}

void Viewer::start()
{

    // setup connector
    auto connector = m_platform-> createConnector();

    if( ! connector || ! connector-> initialize()) {
        std::cerr << "Could not initialize connector.\n";
        exit( -1);
    }

    std::cerr << "Viewer::start: connector initialized\n";

    // associate a callback for a command
    connector->addCommandCallback( "debug", [] (const QString & cmd, const QString & params, const QString & sessionId) -> QString {
        std::cerr << "lambda command cb:\n"
                  << " " << cmd << "\n"
                  << " " << params << "\n"
                  << " " << sessionId << "\n";
        return "1";
    });

    // associate a callback for a command
    connector->addCommandCallback( "add", [] (const QString & cmd, const QString & params, const QString & sessionId) -> QString {
        std::cerr << "add command:\n"
                  << params << "\n";
        QStringList lst = params.split(" ");
        double sum = 0;
        for( auto & entry : lst){
            bool ok;
            sum += entry.toDouble( & ok);
            if( ! ok) { sum = -1; break; }
        }
        return QString("add(%1)=%2").arg(params).arg(sum);
    });


    connector-> addStateCallback( "/xyz", [] ( const QString & path, const QString & val) {
        std::cerr << "lambda state cb:\n"
                  << "  path: " << path << "\n"
                  << "  val:  " << val << "\n";
    });

    static const QString varPrefix = "/myVars";
    static int pongCount = 0;
    connector-> addStateCallback(
                varPrefix + "/ping",
                [=] ( const QString & path, const QString & val) {
        std::cerr << "lcb: " << path << "=" << val << "\n";
        QString nv = QString::number( pongCount ++);
        connector-> setState( varPrefix + "/pong", nv);
    });


    connector-> setState( "/xya", "hola");
    connector-> setState( "/xyz", "8");

    // create a view to be rendered on the client side
    TestView * testView = new TestView();

    connector-> registerView( testView);
}
