/**
 *
 **/

#include "LayeredViewDemo.h"
#include "core/Globals.h"
#include <QPainter>
#include <QImage>
#include <QTimer>
#include <QTime>
#include <QVector2D>
#include <QJsonArray>
#include <cmath>

class EyesLayer : public Carta::Hacks::ManagedLayerBase
{
    Q_OBJECT

public:

    EyesLayer( Carta::Hacks::ManagedLayerView * mlv, QString layerName )
        : Carta::Hacks::ManagedLayerBase( mlv, layerName )
    {
        onResize( { 100, 100 } );
    }

private:

    virtual void
    onInputEvent( const Carta::Lib::InputEvent & ev ) override
    {
        qDebug() << "eyes layer received event" << ev.json()["type"];
        Carta::Lib::InputEvents::TouchEvent touch( ev );
        if ( touch.valid() ) {
            qDebug() << "eyes touch:" << touch.pos();
            m_center = touch.pos();
            rerender();
        }
    }

    virtual void
    onResize( const QSize & size ) override
    {
        m_clientSize = size;
        m_center = QPointF( size.width() / 2.0, size.height() / 2.0 );
        rerender();
    }

    void
    rerender()
    {
        QImage img( m_clientSize, QImage::Format_ARGB32_Premultiplied );
        img.fill( QColor( 0, 128, 0, 128 ) );
        QPainter p( & img );
        p.setBrush( QColor( "red" ) );
        p.setPen( QPen( QColor( "blue" ), 5 ) );
        p.drawEllipse( m_center,
                       20, 10 );
        p.end();
        setRaster( img );
    }

    QPointF m_center;
    QSize m_clientSize = QSize( 100, 100 );
};

class BouncyLayer : public Carta::Hacks::ManagedLayerBase
{
    Q_OBJECT

public:

    BouncyLayer( Carta::Hacks::ManagedLayerView * mlv, QString layerName )
        : Carta::Hacks::ManagedLayerBase( mlv, layerName )
    {
        m_timer.setInterval( 1000 );
        m_timer.start();
        connect( & m_timer, & QTimer::timeout, this, & BouncyLayer::timerCB );
        onResize( { 100, 100 } );
    }

private:

    virtual void
    onInputEvent( const Carta::Lib::InputEvent & ev ) override
    {
        qDebug() << "Bouncy layer received event" << ev.json()["type"];
        Carta::Lib::InputEvents::TouchEvent touch( ev );
        if ( touch.valid() ) {
            qDebug() << "Bouncy touch:" << touch.pos();
            m_center = touch.pos();
            rerender();
        }
    }

    virtual void
    onResize( const QSize & size ) override
    {
        m_clientSize = size;
        m_center = QPointF( size.width() - m_radius, size.height() - m_radius );
        rerender();
    }

    void
    rerender()
    {
        QTime time = QTime::currentTime();
        QColor shadow = QColor( 0, 0, 0, 192 );
        QImage img( m_clientSize, QImage::Format_ARGB32_Premultiplied );
        img.fill( QColor( 128, 0, 0, 128 ) );
        QPainter p( & img );
        p.setRenderHint( QPainter::Antialiasing, true );
        p.setBrush( QColor( "black" ) );
        p.setPen( QPen( QColor( "white" ), 2 ) );
        p.drawEllipse( m_center, m_radius, m_radius );
        p.setPen( QPen( QColor( "white" ), 2 ) );
        double tickLenght = 5;
        for ( int h = 0 ; h < 12 ; h++ ) {
            double alpha = h * M_PI * 2 / 12;
            p.drawLine( a2p( alpha, m_radius - tickLenght - ( h % 3 == 0 ? 2 : 0 ) ),
                        a2p( alpha, m_radius ) );
        }
        double hour = time.hour() % 12 + time.minute() / 60.0 + time.second() / 3600.0;
        p.setPen( QPen( QColor( "white" ), 4 ) );
        p.drawLine( m_center, a2p( hour / 12.0 * M_PI * 2, m_radius / 2 ) );
        double minute = time.minute() + time.second() / 60.0;
        p.setPen( QPen( shadow, 4 ) );
        p.drawLine( m_center, a2p( minute / 60.0 * M_PI * 2, m_radius * 3 / 4 ) );
        p.setPen( QPen( QColor( "white" ), 3 ) );
        p.drawLine( m_center, a2p( minute / 60.0 * M_PI * 2, m_radius * 3 / 4 ) );
        p.setPen( QPen( shadow, 2 ) );
        p.drawLine( m_center, a2p( time.second() / 60.0 * M_PI * 2, m_radius - 2 ) );
        p.setPen( QPen( QColor( "yellow" ), 1 ) );
        p.drawLine( m_center, a2p( time.second() / 60.0 * M_PI * 2, m_radius - 2 ) );
        p.end();
        setRaster( img );
    } // rerender

    static QVector2D
    a2v( double a )
    {
        return QVector2D( sin( a ), - cos( a ) );
    }

    QPointF
    a2p( double alpha, double radius )
    {
        return ( QVector2D( m_center ) + radius * a2v( alpha ) ).toPointF();
    }

    void
    timerCB()
    {
        rerender();
    }

    QPointF m_center;
    QSize m_clientSize;
    double m_radius = 50;
    QTimer m_timer;
};

class LayeredViewController : public QObject
{
    Q_OBJECT
    CLASS_BOILERPLATE( LayeredViewController );

public:

    LayeredViewController( Carta::Hacks::ManagedLayerView * mlv,
                           QObject * parent = nullptr ) : QObject( parent )
    {
        m_mlv = mlv;

        connect( m_mlv, & Carta::Hacks::ManagedLayerView::layersUpdated,
                 this, & Me::mlvUpdatedCB );

        using namespace std::placeholders;

        Globals::instance()->connector()->addCommandCallback(
                    QString( "/hacks/LayeredViewController/%1/setSelection").arg( m_mlv->viewName()),
                    std::bind(  & Me::setSelectionCB, this, _1, _2, _3));


        mlvUpdatedCB();
    }

    virtual
    ~LayeredViewController() { }

private slots:

    void mlvUpdatedCB() {
        // get the list of layers from the view
        auto & layers = m_mlv-> layers();
        QJsonArray ja;
        for( auto layer : layers) {
            QJsonObject jo;
            jo["name"] = layer-> layerName();
            jo["id"] = layer-> layerID();
            jo["input"] = layer-> hasInput();
            ja.push_back( jo);
        }
        QJsonObject job;
        job["list"] = ja;
        QJsonDocument doc(job);
        QByteArray jstring = doc.toJson();
        qDebug() << "================ JSON ================";
        qDebug() << jstring;
        qDebug() << "======================================";

        Globals::instance()->connector()->setState( QString("/hacks/LayeredViewController/%1").arg( m_mlv-> viewName()), jstring);

    }

private:

    QString setSelectionCB( const QString & cmd, const QString & params, const QString & sessionId){
        QJsonDocument doc =  QJsonDocument::fromJson( params.toLatin1());
        if( ! doc.isArray()) return "";
        QJsonArray arr = doc.array();
        std::vector<Carta::Hacks::ManagedLayerBase::ID> selection;
        for( auto val : arr){
            qDebug() << "Selection" << val.toInt();
            selection.push_back( val.toInt());
        }

        m_mlv->setInputLayers( selection);

        return "";
    }

    Carta::Hacks::ManagedLayerView * m_mlv = nullptr;
};

// hack for declaring qobject classes inside .cpp instead of headers. This will force
// moc to process the .cpp file...
#include "LayeredViewDemo.moc"

namespace Carta
{
namespace Hacks
{
struct LayeredViewDemo::Pimpl {
    LayeredViewController::UniquePtr lvc = nullptr;
    ManagedLayerView::UniquePtr mlv = nullptr;
};

LayeredViewDemo::LayeredViewDemo( QObject * parent ) : QObject( parent )
{
    m_pimpl = new Pimpl;

    m_pimpl-> mlv.reset( new ManagedLayerView( "mlv1", Globals::instance()->connector(), this ) );

    EyesLayer * eyes1 = new EyesLayer( m_pimpl-> mlv.get(), "ellipse1" );
    BouncyLayer * bouncy = new BouncyLayer( m_pimpl-> mlv.get(), "clock" );
    EyesLayer * eyes2 = new EyesLayer( m_pimpl-> mlv.get(), "ellipse2" );

//    m_mlv-> setInputLayers( { eyes->layerID(), bouncy-> layerID() }
    m_pimpl-> mlv-> setInputLayers( { eyes2->layerID() });

    m_pimpl-> lvc.reset( new LayeredViewController( m_pimpl-> mlv.get() ) );
}

LayeredViewDemo::~LayeredViewDemo()
{
    if ( m_pimpl ) {
        delete m_pimpl;
        m_pimpl = nullptr;
    }
}
}
}
