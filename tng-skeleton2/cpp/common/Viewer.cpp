#include "Viewer.h"
#include "IPlatform.h"
#include "IConnector.h"
#include "misc.h"
#include "PluginManager.h"
#include <iostream>
#include <QImage>
#include <QColor>
#include <QPainter>
#include <cmath>
#include <QDebug>

/// shared objects across the application
class Globals {
    PluginManager * m_pluginManager;
    IPlatform * m_platform;
    IConnector * m_connector;

public:
    IConnector *connector() const;
    void setConnector(IConnector *connector);
    IPlatform *platform() const;
    void setPlatform(IPlatform *platform);
    PluginManager *pluginManager() const;
    void setPluginManager(PluginManager *pluginManager);
};

Globals globals;

class TestView : public IView
{

public:

    TestView( const QString & viewName, QColor bgColor) {
        m_qimage = QImage( 100, 100, QImage::Format_RGB888);
        m_qimage.fill( bgColor);

        m_viewName = viewName;
        m_connector= nullptr;
        m_bgColor = bgColor;
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
        redrawBuffer();
        return m_qimage;
    }
    virtual void handleResizeRequest(const QSize & size)
    {
        m_qimage = QImage( size, m_qimage.format());
        m_connector-> refreshView( this);
    }
    virtual void handleMouseEvent(const QMouseEvent & ev)
    {
        m_lastMouse = QPointF( ev.x(), ev.y());
        m_connector-> refreshView( this);

        m_connector-> setState( "/mouse/x", QString::number(ev.x()));
        m_connector-> setState( "/mouse/y", QString::number(ev.y()));
    }
    virtual void handleKeyEvent(const QKeyEvent & /*event*/)
    {
    }

protected:

    QColor m_bgColor;

    void redrawBuffer()
    {
        QPointF center = m_qimage.rect().center();
        QPointF diff = m_lastMouse - center;
        double angle = atan2( diff.x(), diff.y());
        angle *= - 180 / M_PI;

        m_qimage.fill( m_bgColor);
        {
            QPainter p( & m_qimage);
            p.setPen( Qt::NoPen);
            p.setBrush( QColor( 255, 255, 0, 128));
            p.drawEllipse( QPoint(m_lastMouse.x(), m_lastMouse.y()), 10, 10 );
            p.setPen( QColor( 255, 255, 255));
            p.drawLine( 0, m_lastMouse.y(), m_qimage.width()-1, m_lastMouse.y());
            p.drawLine( m_lastMouse.x(), 0, m_lastMouse.x(), m_qimage.height()-1);

            p.translate( m_qimage.rect().center());
            p.rotate( angle);
            p.translate( - m_qimage.rect().center());
            p.setFont( QFont( "Arial", 20));
            p.setPen( QColor( "white"));
            p.drawText( m_qimage.rect(), Qt::AlignCenter, m_viewName);
        }

        // execute the pre-render hook
        globals.pluginManager()-> hookAll2<PreRender>( m_viewName, & m_qimage).executeAll();
    }

    IConnector * m_connector;
    QImage m_qimage;
    QString m_viewName;
    int m_timerId;
    QPointF m_lastMouse;
};


Viewer::Viewer( IPlatform * platform) :
    QObject( nullptr)
{
    globals.setPlatform( platform);
    globals.setConnector( platform->connector());

    m_platform = platform;

    globals.setPluginManager( new PluginManager);

    m_pluginManager = globals.pluginManager();
    m_pluginManager-> loadPlugins();
    // load plugins
    qDebug() << "Loading plugins...";
    auto infoList = m_pluginManager-> getInfoList();
    qDebug() << "List of plugins: [" << infoList.size() << "]";
    for( const auto & entry : infoList) {
        qDebug() << "  path:" << entry-> path;
    }

    // execute a hook
    auto helper = m_pluginManager-> hookAll2<Initialize>();
    helper.executeAll();
}

void Viewer::start()
{
    // setup connector
    auto connector = m_platform-> connector();

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
    connector->addCommandCallback( "add", [] (const QString & /*cmd*/, const QString & params, const QString & /*sessionId*/) -> QString {
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


    auto xyzCBid =
    connector-> addStateCallback( "/xyz", [] ( const QString & path, const QString & val) {
        qDebug() << "lambda state cb:\n"
                 << "  path: " << path << "\n"
                 << "  val:  " << val;
    });

//    connector->removeStateCallback(xyzCBid);

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

    // create view to be rendered on the client side


    connector-> registerView( new TestView( "view1", QColor( "blue")));
    connector-> registerView( new TestView( "view2", QColor( "red")));
}


IPlatform *Globals::platform() const
{
return m_platform;
}

void Globals::setPlatform(IPlatform *platform)
{
m_platform = platform;
}

PluginManager *Globals::pluginManager() const
{
return m_pluginManager;
}

void Globals::setPluginManager(PluginManager *pluginManager)
{
m_pluginManager = pluginManager;
}
IConnector *Globals::connector() const
{
return m_connector;
}

void Globals::setConnector(IConnector *connector)
{
m_connector = connector;
}
