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
#include <QCoreApplication>
#include <cmath>
#include <iostream>

/// algorithm for converting an instance of image interface to qimage
class RawView2QImageConverter {
public:
    typedef RawView2QImageConverter & Me;

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

    void computeClips() {
        // read in all values from the view
        std::vector< float> allValues;
        NdArray::TypedView<float> tview( m_rawView, false);
        tview.forEach( [& allValues](const float & val) {
            if( std::isfinite( val)) {
                allValues.push_back( val);
            }
        });

        // indicate bad clip if no finite numbers were found
        if( allValues.size() == 0) {
            m_clip1 = 1.0/0.0;
            return;
        }

        double hist = m_autoClip * 100;
        float clip2;
        {
            hist = (1.0 - hist / 100) / 2.0;
            int x1 = allValues.size() * hist;
            std::nth_element( allValues.begin(), allValues.begin() + x1, allValues.end());
            m_clip1 = allValues[x1];
            x1 = allValues.size() - x1;
            std::nth_element( allValues.begin(), allValues.begin() + x1, allValues.end());
            clip2 = allValues[x1];
        }
        float clipd = clip2 - m_clip1;
        m_clipdinv = 1.0 / clipd; // precalculated linear transformation coeff.
    }

    float m_clip1 = 1.0/0.0, m_clipdinv;

    const QImage & go( bool recomputeClip = true) {

        if( ! std::isfinite(m_clip1) || recomputeClip) {
            computeClips();
        }

        int width = m_rawView->dims()[0];
        int height = m_rawView->dims()[1];

        // make sure there is at least 1 non-nan value, if not, return a black image
        if( ! std::isfinite(m_clip1)) {
            // there were no values.... return a black image
            m_qImage = QImage( width, height, QImage::Format_ARGB32);
            m_qImage.fill( 0);
            return m_qImage;
        }

        // construct image using clips (clipd, clipinv)
        static std::vector < QRgb > outBuffer;
        outBuffer.resize( width * height);
        QRgb * outPtr = & outBuffer[0];

        NdArray::TypedView<float> tview( m_rawView, false);
        tview.forEach( [& outPtr, this](const float & ival) {
            if( std::isfinite( ival)) {
                float val = (ival - m_clip1) * m_clipdinv;
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

        // construct the qimage from the temporary buffer
        m_qImage = QImage( reinterpret_cast<uchar *>( & outBuffer[0]),
                width, height, QImage::Format_ARGB32);

        return m_qImage;
    }
protected:
    QImage m_qImage;
    NdArray::RawViewInterface * m_rawView;
    double m_autoClip = 0.95;
    int m_frame;
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
}

void
Viewer::start()
{
    qDebug() << "Viewer::start() starting";

    auto & globals = * Globals::instance();
    auto connector = globals.connector();

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
    connector-> registerView( new TestView( "view1", QColor( "blue" ) ) );
    connector-> registerView( new TestView( "view2", QColor( "red" ) ) );

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
        qDebug() << "Could not find any plugin to load astroImage";
    }
    else {
        m_image = res2.val();

        // convert the loaded image into QImage
        auto frameSlice = SliceND().next();
        for( size_t i = 2 ; i < m_image->dims().size() ; i ++) {
            frameSlice.next().index(0);
        }
        NdArray::RawViewInterface * frameView = m_image-> getDataSlice( frameSlice);
        RawView2QImageConverter cvt;
        cvt.setView( frameView);
        cvt.setAutoClip( 0.95 /* 95% */);
        QImage qimg = cvt.go();
        delete frameView;
        qDebug() << "Image loaded" << qimg;
        qDebug() << "Pixel type = " << Image::pixelType2int( res2.val()-> pixelType() );

        testView2 = new TestView2( "view3", QColor( "pink" ), qimg );
        connector-> registerView( testView2 );
    }

    if( 0) {
        // some debugging info

        Image::ImageInterface * img = res2.val();

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
            connector->setState( path + "name", entry.json.name );
            connector->setState( path + "description", entry.json.description );
            connector->setState( path + "type", entry.json.typeString );
            connector->setState( path + "version", entry.json.version );

            //            connector-> setState( path + "dirPath", entry.dirPath);
            connector->setState( path + "errors", entry.errors.join( "|" ) );
            ind++;
        }
        connector->setState( "/pluginList/stamp", QString::number( ind ) );
    }

    // everything below is a hack... just to test performance...
    static RawView2QImageConverter cvt;
    static int lastFrame = 0;

    // let's listen to mouseX coordinate and see if we can use it to change the frame...
    connector->addStateCallback( "/movieControl", [this, & cvt](const QString &, const QString & val) {
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
        if( frame == lastFrame) return;

        qDebug() << "switching to frame" << frame;

        // convert the loaded image into QImage
        auto frameSlice = SliceND().next();
        for( size_t i = 2 ; i < m_image->dims().size() ; i ++) {
            frameSlice.next().index( i == 2 ? frame : 0);
        }
        NdArray::RawViewInterface * frameView = m_image-> getDataSlice( frameSlice);
        cvt.setView( frameView);
        cvt.setAutoClip( 0.95 /* 95% */);
        QImage qimg = cvt.go( y < 0.5);
        delete frameView;
        testView2->setImage(qimg);
    });


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
} // scriptedCommandCB
