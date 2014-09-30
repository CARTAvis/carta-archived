#include "Viewer.h"
#include "Globals.h"
#include "IPlatform.h"
#include "IConnector.h"
#include "misc.h"
#include "PluginManager.h"
#include "MainConfig.h"
#include "MyQApp.h"
#include "CmdLine.h"
#include "ScriptedCommandListener.h"
#include <QImage>
#include <QColor>
#include <QPainter>
#include <QDebug>
#include <QCache>
#include <QCoreApplication>
#include <cmath>
#include <iostream>
#include <limits>
#include <functional>

/// compute clip values from the values in a view
template <typename Scalar>
static
typename std::tuple<Scalar,Scalar> computeClips(
        NdArray::TypedView<Scalar> & view,
        double perc
        )
{
    // read in all values from the view into an array
    // we need our own copy because we'll do quickselect on it...
    std::vector<Scalar> allValues;
    view.forEach( [& allValues](const Scalar & val) {
        if( std::isfinite( val)) {
            allValues.push_back( val);
        }
    });

    // indicate bad clip if no finite numbers were found
    if( allValues.size() == 0) {
        return std::make_tuple(
                    std::numeric_limits<Scalar>::quiet_NaN(),
                    std::numeric_limits<Scalar>::quiet_NaN());
    }

    Scalar clip1, clip2;
    Scalar hist = (1.0 - perc) / 2.0;
    int x1 = allValues.size() * hist;
    std::nth_element( allValues.begin(), allValues.begin() + x1, allValues.end());
    clip1 = allValues[x1];
    x1 = allValues.size() - 1 - x1;
    std::nth_element( allValues.begin(), allValues.begin() + x1, allValues.end());
    clip2 = allValues[x1];

    if(0) { // debug
        int64_t count = 0;
        for( auto & x : allValues) {
            if( x >= clip1 && x <= clip2) { count++ ; }
        }
        qDebug() << "autoClip(" << perc << ")=" << count * 100.0 /allValues.size();
    }

    return std::make_tuple( clip1, clip2);
}

/// algorithm for converting an instance of image interface to qimage
class RawView2QImageConverter {
public:
    typedef RawView2QImageConverter & Me;
    typedef float Scalar;

    struct CachedImage {
        QImage img;
        double clip1, clip2, perc;
        bool specificToFrame;

        CachedImage( QImage pImg, double c1, double c2, bool spec, double p) {
            img = pImg; clip1 = c1; clip2 = c2;
            specificToFrame = spec;
            perc = p;
        }
    };

    RawView2QImageConverter() {
        // set cache size (in MB)
        m_cache.setMaxCost( 6 * 1024);
    }

    Me & setView( NdArray::RawViewInterface * rawView) {
        if( ! rawView) {
            throw std::runtime_error( "iimage is null");
        }
        if( rawView->dims().size() < 2) {
            throw std::runtime_error( "need at least 2 dimensions");
        }
        m_rawView = rawView;
        return * this;
    }

    Me & setFrame( int frame) {
        if( frame > 0) {
            int nFrames = 0;
            if( m_rawView->dims().size() > 2) {
                nFrames = m_rawView->dims()[2];
            }
            if( frame >= nFrames) {
                throw std::runtime_error( "not enough frames in this image");
            }
        }
        return * this;
    }

    Me & setAutoClip( double val) {
        m_autoClip = val;
        return * this;
    }

    const QImage & go( int frame, bool recomputeClip = true) {

        // let's see if we have the result in cache
        CachedImage * img = m_cache[ frame];
//        img = nullptr;
//        qDebug() << "Cache check #" << m_frame;

        if( img) {
            bool match = false;
            // we have something in cache, let's see if it matches what we want
            if( recomputeClip) {
                if( img->specificToFrame && img->perc == m_autoClip) {
                    match = true;
                }
            }
            else {
                if( img->clip1 == m_clip1 && img->clip2 == m_clip2) {
                    match = true;
                }
            }
            if( match) {
                qDebug() << "Cache hit #" << frame;
                return img->img;
            }
        }
        qDebug() << "Cache miss #" << frame;

        NdArray::TypedView<Scalar> view( m_rawView, false);

        if( ! std::isfinite(m_clip1) || recomputeClip) {
            std::tie( m_clip1, m_clip2) = computeClips<Scalar>( view, m_autoClip);
        }

        int width = m_rawView->dims()[0];
        int height = m_rawView->dims()[1];

        // make sure there is at least 1 finite value, if not, return a black image
        if( ! std::isfinite(m_clip1)) {
            // there were no values.... return a black image
            m_qImage = QImage( width, height, QImage::Format_ARGB32);
            m_qImage.fill( 0);
            return m_qImage;
        }

        // construct image using clips (clipd, clipinv)
        m_qImage = QImage( width, height, QImage::Format_ARGB32);
        if( m_qImage.bytesPerLine() != width * 4) {
            throw std::runtime_error( "qimage does not have consecutive memory...");
        }
        QRgb * outPtr = reinterpret_cast<QRgb *>( m_qImage.bits());

        // precalculate linear transformation coeff.
        float clipdinv = 1.0 / (m_clip2 - m_clip1);

        // apply the clips
        view.forEach( [& outPtr, & clipdinv, this](const float & ival) {
            if( std::isfinite( ival)) {
                float val = (ival - m_clip1) * clipdinv;
                int gray = val * 255;
                if( gray > 255) gray = 255;
                if( gray < 0) gray = 0;
                * outPtr = qRgb( gray, gray, gray);
            }
            else {
                * outPtr = qRgb( 255, 0, 0);
            }
            outPtr ++;
        });

//        // construct image using clips (clipd, clipinv)
//        static std::vector < QRgb > outBuffer;
//        outBuffer.resize( width * height);
//        QRgb * outPtr = & outBuffer[0];

//        // precalculate linear transformation coeff.
//        float clipdinv = 1.0 / (m_clip2 - m_clip1);

//        // apply the clips
//        view.forEach( [& outPtr, & clipdinv, this](const float & ival) {
//            if( std::isfinite( ival)) {
//                float val = (ival - m_clip1) * clipdinv;
//                int gray = val * 255;
//                if( gray > 255) gray = 255;
//                if( gray < 0) gray = 0;
//                * outPtr = qRgb( gray, gray, gray);
//            }
//            else {
//                * outPtr = qRgb( 255, 0, 0);
//            }
//            outPtr ++;
//        });

//        // construct the qimage from the temporary buffer
//        m_qImage = QImage( reinterpret_cast<uchar *>( & outBuffer[0]),
//                width, height, QImage::Format_ARGB32);

        // stick this into the cache so we don't need to recompute it
        m_cache.insert( frame,
                        new CachedImage( m_qImage, m_clip1, m_clip2, recomputeClip, m_autoClip),
                        width * height * 4 / 1024 / 1024);

        qDebug() << "cache total=" << m_cache.totalCost() << "MB";

        return m_qImage;
    }
protected:
    QImage m_qImage;
    NdArray::RawViewInterface * m_rawView = nullptr;
    double m_autoClip = 0.95;
    QCache<int,CachedImage> m_cache;
    float m_clip1 = 1.0/0.0, m_clip2 = 0.0;
};

class TestView : public IView
{
public:
    TestView( const QString & viewName, QColor bgColor )
    {
        m_qimage = QImage( 100, 100, QImage::Format_RGB888 );
        m_qimage.fill( bgColor );

        m_viewName  = viewName;
        m_connector = nullptr;
        m_bgColor   = bgColor;
    }

    virtual void
    registration( IConnector * connector )
    {
        m_connector = connector;
    }

    virtual const QString &
    name() const
    {
        return m_viewName;
    }

    virtual QSize
    size()
    {
        return m_qimage.size();
    }

    virtual const QImage &
    getBuffer()
    {
        redrawBuffer();
        return m_qimage;
    }

    virtual void
    handleResizeRequest( const QSize & pSize )
    {
        QSize size( std::max( pSize.width(), 1), std::max( pSize.height(), 1));
        m_qimage = QImage( size, m_qimage.format() );
        m_connector-> refreshView( this );
    }

    virtual void
    handleMouseEvent( const QMouseEvent & ev )
    {
        m_lastMouse = QPointF( ev.x(), ev.y() );
        m_connector-> refreshView( this );

        m_connector-> setState( "/mouse/x", QString::number( ev.x() ) );
        m_connector-> setState( "/mouse/y", QString::number( ev.y() ) );
    }

    virtual void
    handleKeyEvent( const QKeyEvent & /*event*/ )
    { }

protected:
    QColor m_bgColor;

    void
    redrawBuffer()
    {
        QPointF center = m_qimage.rect().center();
        QPointF diff   = m_lastMouse - center;
        double angle   = atan2( diff.x(), diff.y() );
        angle *= - 180 / M_PI;

        m_qimage.fill( m_bgColor );
        {
            QPainter p( & m_qimage );
            p.setPen( Qt::NoPen );
            p.setBrush( QColor( 255, 255, 0, 128 ) );
            p.drawEllipse( QPoint( m_lastMouse.x(), m_lastMouse.y() ), 10, 10 );
            p.setPen( QColor( 255, 255, 255 ) );
            p.drawLine( 0, m_lastMouse.y(), m_qimage.width() - 1, m_lastMouse.y() );
            p.drawLine( m_lastMouse.x(), 0, m_lastMouse.x(), m_qimage.height() - 1 );

            p.translate( m_qimage.rect().center() );
            p.rotate( angle );
            p.translate( - m_qimage.rect().center() );
            p.setFont( QFont( "Arial", 20 ) );
            p.setPen( QColor( "white" ) );
            p.drawText( m_qimage.rect(), Qt::AlignCenter, m_viewName );
        }

        // execute the pre-render hook
        Globals::instance()-> pluginManager()
            -> prepare < PreRender > ( m_viewName, & m_qimage )
            .executeAll();
    } // redrawBuffer

    IConnector * m_connector;
    QImage m_qimage;
    QString m_viewName;
    int m_timerId;
    QPointF m_lastMouse;
};

class TestView2 : public IView
{
public:
    TestView2( const QString & viewName, QColor bgColor, QImage img )
    {
        m_defaultImage = img;
        m_qimage       = QImage( 100, 100, QImage::Format_RGB888 );
        m_qimage.fill( bgColor );

        m_viewName  = viewName;
        m_connector = nullptr;
        m_bgColor   = bgColor;
    }

    void
    setImage( const QImage & img )
    {
        m_defaultImage = img;
        m_connector-> refreshView( this );
    }

    virtual void
    registration( IConnector * connector )
    {
        m_connector = connector;
    }

    virtual const QString &
    name() const
    {
        return m_viewName;
    }

    virtual QSize
    size()
    {
        return m_qimage.size();
    }

    virtual const QImage &
    getBuffer()
    {
        redrawBuffer();
        return m_qimage;
    }

    virtual void
    handleResizeRequest( const QSize & pSize )
    {
        QSize size( std::max( pSize.width(), 1), std::max( pSize.height(), 1));
        m_qimage = QImage( size, m_qimage.format() );
        m_connector-> refreshView( this );
    }

    virtual void
    handleMouseEvent( const QMouseEvent & ev )
    {
        m_lastMouse = QPointF( ev.x(), ev.y() );
        m_connector-> refreshView( this );

        m_connector-> setState( "/mouse/x", QString::number( ev.x() ) );
        m_connector-> setState( "/mouse/y", QString::number( ev.y() ) );
    }

    virtual void
    handleKeyEvent( const QKeyEvent & /*event*/ )
    { }

protected:
    QColor m_bgColor;
    QImage m_defaultImage;

    void
    redrawBuffer()
    {
        QPointF center = m_qimage.rect().center();
        QPointF diff   = m_lastMouse - center;
        double angle   = atan2( diff.x(), diff.y() );
        angle *= - 180 / M_PI;

        m_qimage.fill( m_bgColor );
        {
            QPainter p( & m_qimage );
            p.drawImage( m_qimage.rect(), m_defaultImage );
            p.setPen( Qt::NoPen );
            p.setBrush( QColor( 255, 255, 0, 128 ) );
            p.drawEllipse( QPoint( m_lastMouse.x(), m_lastMouse.y() ), 10, 10 );
            p.setPen( QColor( 255, 255, 255 ) );
            p.drawLine( 0, m_lastMouse.y(), m_qimage.width() - 1, m_lastMouse.y() );
            p.drawLine( m_lastMouse.x(), 0, m_lastMouse.x(), m_qimage.height() - 1 );

            p.translate( m_qimage.rect().center() );
            p.rotate( angle );
            p.translate( - m_qimage.rect().center() );
            p.setFont( QFont( "Arial", 20 ) );
            p.setPen( QColor( "white" ) );
            p.drawText( m_qimage.rect(), Qt::AlignCenter, m_viewName );
        }

        // execute the pre-render hook
        Globals::instance()-> pluginManager()
            -> prepare < PreRender > ( m_viewName, & m_qimage )
            .executeAll();
    } // redrawBuffer

    IConnector * m_connector;
    QImage m_qimage;
    QString m_viewName;
    int m_timerId;
    QPointF m_lastMouse;
};

static TestView2 * testView2 = nullptr;

Viewer::Viewer() :
    QObject( nullptr )
{
    int port = Globals::instance()->cmdLineInfo()-> scriptPort();
    if ( port < 0 ) {
        qDebug() << "Not listening to scripted commands.";
    }
    else {
        m_scl = new ScriptedCommandListener( port, this );
        qDebug() << "Listening to scripted commands on port " << port;
        connect( m_scl, & ScriptedCommandListener::command,
                 this, & Viewer::scriptedCommandCB );
    }

    m_rawView2QImageConverter = std::make_shared<RawView2QImageConverter>();
    m_rawView2QImageConverter-> setAutoClip( 0.95 /* 95% */);
}

void
Viewer::start()
{
    qDebug() << "Viewer::start() starting";

    auto & globals = * Globals::instance();
    m_connector = globals.connector();

    m_connector->setState( "/autoClip", "1");

    // initialize plugin manager
    globals.setPluginManager( new PluginManager );
    auto pm = globals.pluginManager();

    // tell plugin manager where to find plugins
    pm-> setPluginSearchPaths( globals.mainConfig()->pluginDirectories() );

    // find and load plugins
    pm-> loadPlugins();

    qDebug() << "Loading plugins...";
    auto infoList = pm-> getInfoList();
    qDebug() << "List of plugins: [" << infoList.size() << "]";
    for ( const auto & entry : infoList ) {
        qDebug() << "  path:" << entry.json.name;
    }

    // tell all plugins that the core has initialized
    pm-> prepare < Initialize > ()
        .executeAll();

#ifdef DONT_COMPILE

    // associate a callback for a command
    connector->addCommandCallback( "debug",
                                   [] ( const QString & cmd, const QString & params,
                                        const QString & sessionId ) -> QString {
                                       std::cerr << "lambda command cb:\n"
                                                 << " " << cmd << "\n"
                                                 << " " << params << "\n"
                                                 << " " << sessionId << "\n";
                                       return "1";
                                   }
                                   );

    // associate a callback for a command
    connector->addCommandCallback(
        "add",
        [] ( const QString & /*cmd*/, const QString & params,
             const QString & /*sessionId*/ ) -> QString {
            std::cerr << "add command:\n"
                      << params << "\n";
            QStringList lst = params.split( " " );
            double sum = 0;
            for ( auto & entry: lst ) {
                bool ok;
                sum += entry.toDouble( & ok );
                if ( ! ok ) {
                    sum = - 1;
                    break;
                }
            }
            return QString( "add(%1)=%2" ).arg( params ).arg( sum );
        }
        );

//    auto xyzCBid =
    connector-> addStateCallback( "/xyz", [] ( const QString & path, const QString & val ) {
                                      qDebug() << "lambda state cb:\n"
                                               << "  path: " << path << "\n"
                                               << "  val:  " << val;
                                  }
                                  );

//    connector->removeStateCallback(xyzCBid);

    static const QString varPrefix = "/myVars";
    static int pongCount           = 0;
    connector-> addStateCallback(
        varPrefix + "/ping",
        [ = ] ( const QString & path, const QString & val ) {
            std::cerr << "lcb: " << path << "=" << val << "\n";
            QString nv = QString::number( pongCount++ );
            connector-> setState( varPrefix + "/pong", nv );
        }
        );
    connector-> setState( "/xya", "hola" );
    connector-> setState( "/xyz", "8" );

#endif // dont compile

    // create some views to be rendered on the client side
    m_connector-> registerView( new TestView( "view1", QColor( "blue" ) ) );
    m_connector-> registerView( new TestView( "view2", QColor( "red" ) ) );

    // which image are we loading?
    qDebug() << "======== trying to load image ========";
    if ( Globals::instance()-> platform()-> initialFileList().isEmpty() ) {
        qFatal( "No input file given" );
    }
    QString fname = Globals::instance()-> platform()-> initialFileList()[0];



    // ask one of the plugins to load the image
    qDebug() << "Trying to load astroImage...";
    auto res2 =
        Globals::instance()-> pluginManager()
            -> prepare < LoadAstroImage > ( fname )
            .first();
    if ( res2.isNull() ) {
        qFatal( "Could not find any plugin to load astroImage");
    }
    m_image = res2.val();

    qDebug() << "Pixel type:" << Image::pixelType2int( res2.val()-> pixelType() );
    qDebug() << "Title:" << m_image->metaData()->title();
    testView2 = new TestView2( "view3", QColor( "pink" ), QImage(10, 10, QImage::Format_ARGB32) );
    m_connector-> registerView( testView2 );

    // convert the loaded image into QImage
    m_currentFrame = 0;
    reloadFrame( true);

    m_coordinateFormatter = m_image-> metaData()->coordinateFormatter();
    auto pixCoords = std::vector<double>(m_image->dims().size(), 0.0);
    auto list = m_coordinateFormatter->formatFromPixelCoordinate( pixCoords);
    qDebug() << "pixel" << pixCoords << " ==> " << list;

    auto myMouseCb = std::bind(
                         & Viewer::mouseCB, this,
                         std::placeholders::_1, std::placeholders::_2);
    m_connector->addStateCallback( "/mouse/x", myMouseCb);
    m_connector->addStateCallback( "/mouse/y", myMouseCb);

    if( 0) {
        // some debugging info

        Image::ImageInterface::SharedPtr img = res2.val();

        qDebug() << "Dimensions: " << QVector <int>::fromStdVector( img->dims() );
        qDebug() << "Unit: " << img-> getPixelUnit().toStr();
        // let's get a view (the whole image)
        NdArray::RawViewInterface * rawView = img-> getDataSlice( SliceND({Slice1D().step(3)}));
        if( ! rawView) {
            qFatal( "Raw view is null");
        }
        qDebug() << "View dimensions:" << rawView->dims();
        typedef std::vector<int> VI;
        VI pos( img->dims().size(), 0);
        const char * rawPtr = rawView->get( pos);
        for( size_t i = 0 ; i < sizeof( double) ; i ++ ) {
            qDebug() << static_cast<int>(rawPtr[i]);
        }
        NdArray::TypedView<float> fv(rawView, false);
        pos = VI( img->dims().size(), 0);
        qDebug() << "fview @" << pos << "=" << fv.get( pos);
        pos[0] = 1;
        qDebug() << "fview @" << pos << "=" << fv.get( pos);

        {
            // [3:7:2,5:6]
            auto slice = SliceND().start(3).step(2).end(7)
                         .next().start(5).end(6);
            NdArray::RawViewInterface * rawView = img-> getDataSlice( slice);
            NdArray::TypedView<float> fv(rawView, true);
            qDebug().nospace();
            int64_t count = 0;
            fv.forEach([&count]( const float & val) {
                qDebug() << val << " ";
                count ++;
            });
            qDebug() << "foreach processed " << count << "items";
            qDebug().space();
        }

        if( false){
            // [0:5,0:5,0,0]
            auto slice = SliceND().start(0).end(5)
                         .next().start(0).end(5)
                         .next().index(0)
                         .next().index(0);
            NdArray::RawViewInterface * rawView = img-> getDataSlice( slice);
            NdArray::TypedView<float> fv(rawView, true);
            qDebug().nospace();
            int64_t count = 0;
            fv.forEach([&count]( const float & val) {
                qDebug() << val << " ";
                count ++;
            });
            qDebug() << "foreach processed " << count << "items";
            qDebug().space();
        }
    }

    // tell clients about our plugins
    {
        auto pm       = Globals::instance()->pluginManager();
        auto infoList = pm->getInfoList();
        int ind       = 0;
        for ( auto & entry : infoList ) {
            qDebug() << "  path:" << entry.soPath;
            QString path = QString( "/pluginList/p%1/" ).arg( ind );
            m_connector->setState( path + "name", entry.json.name );
            m_connector->setState( path + "description", entry.json.description );
            m_connector->setState( path + "type", entry.json.typeString );
            m_connector->setState( path + "version", entry.json.version );

            //            connector-> setState( path + "dirPath", entry.dirPath);
            m_connector->setState( path + "errors", entry.errors.join( "|" ) );
            ind++;
        }
        m_connector->setState( "/pluginList/stamp", QString::number( ind ) );
    }


    // everything below is a hack... just to test performance for switching frames
    auto movieCallback = [this](const QString &, const QString & val) {
        double x, y;
        QStringList lst = val.split( "_");
        if( lst.size() < 2) return;
        bool ok;
        x = lst[0].toDouble( & ok);
        if( ! ok) return;
        y = lst[1].toDouble( & ok);
        if( ! ok) return;
        x = clamp<double>( x, 0, 1);
        y = clamp<double>( y, 0, 1);

        int nFrames = 1;
        if( m_image->dims().size() >2) { nFrames = m_image->dims()[2]; }
        int frame = x * nFrames;
        frame = clamp( frame, 0, nFrames-1);
        if( frame == m_currentFrame) return;
        m_currentFrame = frame;

        qDebug() << "switching to frame" << frame;

        // convert the loaded image into QImage
        reloadFrame();
    };

    m_connector->addStateCallback( "/movieControl", movieCallback);

    auto autoClipCallback = [this](const QString &, const QString & val) {
        m_clipRecompute = val == "1";
        if( ! m_clipRecompute) {
            return;
        }

        reloadFrame( true);
    };
    m_connector->addStateCallback( "/autoClip", autoClipCallback);

    auto autoClipValueCallback = [this](const QString &, const QString & pval) {
        qDebug() << "xyzxyz" << pval;
        QString val = pval;
        val.chop(1);
        bool ok;
        double d = val.toDouble( & ok);
        if( ! ok) return;
        d = clamp( d/100, 0.001, 1.0);
        qDebug() << "xyzxyz2" << d;
        m_rawView2QImageConverter-> setAutoClip( d);
        reloadFrame( true);
    };
    m_connector->addStateCallback( "/autoClipValue", autoClipValueCallback);



} // start

void
Viewer::scriptedCommandCB( QString command )
{
    command = command.simplified();
    qDebug() << "Scripted command received:" << command;

    QStringList args = command.split( ' ', QString::SkipEmptyParts );
    qDebug() << "args=" << args;
    qDebug() << "args.size=" << args.size();
    qDebug() << "args[0].tolower=" << args[0].toLower();
    if ( args.size() == 2 && args[0].toLower() == "load" ) {
        qDebug() << "Trying to load" << args[1];
        auto loadImageHookHelper = Globals::instance()-> pluginManager()
                                       -> prepare < LoadImage > ( args[1] );
        Nullable < QImage > res = loadImageHookHelper.first();
        if ( res.isNull() ) {
            qDebug() << "Could not find any plugin to load image";
        }
        else {
            qDebug() << "Image loaded: " << res.val().size();
            testView2-> setImage( res.val() );
        }
    }
    else if ( args.size() == 1 && args[0].toLower() == "quit" ) {
        qDebug() << "Quitting...";
        MyQApp::exit();
        return;
    }
    else {
        qWarning() << "Sorry, unknown command";
    }
}

void Viewer::reloadFrame( bool forceClipRecompute)
{
    auto frameSlice = SliceND().next();
    for( size_t i = 2 ; i < m_image->dims().size() ; i ++) {
        frameSlice.next().index( i == 2 ? m_currentFrame : 0);
    }
    NdArray::RawViewInterface * frameView = m_image-> getDataSlice( frameSlice);
    m_rawView2QImageConverter-> setView( frameView);
    QImage qimg = m_rawView2QImageConverter-> go(
                      m_currentFrame, m_clipRecompute || forceClipRecompute);
    delete frameView;
    testView2->setImage(qimg);

}

void Viewer::mouseCB(const QString & /*path*/, const QString & /*val*/)
{
    bool ok;
    double x = m_connector-> getState( "/mouse/x").toDouble( & ok);
    if( !ok) {}
    double y = m_connector-> getState( "/mouse/y").toDouble( & ok);
    if( !ok) {}
    auto pixCoords = std::vector<double>(m_image->dims().size(), 0.0);
    pixCoords[0] = x;
    pixCoords[1] = y;
    if( pixCoords.size() > 2) {
        pixCoords[2] = m_currentFrame;
    }
    auto list = m_coordinateFormatter->formatFromPixelCoordinate( pixCoords);
    qDebug() << "Formatted coordinate:" << list;
    m_connector-> setState( "/cursor", list.join("\n").toHtmlEscaped());

} // scriptedCommandCB
