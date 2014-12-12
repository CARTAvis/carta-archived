/**
 *
 **/

#include "HackViewer.h"
#include "Globals.h"
#include "IConnector.h"
#include "CartaLib/Hooks/ColormapsScalar.h"
#include "CartaLib/Hooks/LoadAstroImage.h"
#include "common/GrayColormap.h"
#include <QPainter>
#include <set>

namespace StateKey
{
QString MOUSE_X = "mouse_x";
QString MOUSE_Y = "mouse_y";
QString HACKS = "hacks";
QString AUTO_CLIP = "auto_clip";
}

class TestView2 : public IView
{
public:

    TestView2( QString prefix,
               QString viewName,
               QColor bgColor,

//               QImage img,
               Image::ImageInterface::SharedPtr astroImage )
    {
        // we keep m_imageBuffer sized to the client size
        m_imageBuffer = QImage( 100, 100, QImage::Format_ARGB32_Premultiplied );
        m_imageBuffer.fill( bgColor );

        m_viewName = viewName;
        m_connector = nullptr;
        m_bgColor = bgColor;
        m_astroImage = astroImage;
        m_prefix = prefix + "/" + viewName;
    }

    // image to be rendered:
    QImage m_qImageToRender;

    // accessor to m_qImageToRender
    // if modified, call scheduleRedraw(), which will schedule a redraw
    QImage &
    imageToRender()
    {
        return m_qImageToRender;
    }

//    void
//    setImage( const QImage & img )
//    {
//        m_pixmapToRender.convertFromImage( img );
//        m_connector-> refreshView( this );
//    }

    void
    scheduleRedraw()
    {
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
        return m_imageBuffer.size();
    }

    /// @todo Investigate QPixmap performance vs QImage
    virtual const QImage &
    getBuffer()
    {
        redrawBuffer();
        return m_imageBuffer;
    }

    virtual void
    handleResizeRequest( const QSize & pSize )
    {
        QSize size( std::max( pSize.width(), 1 ), std::max( pSize.height(), 1 ) );
        m_imageBuffer = QImage( size, m_imageBuffer.format() );
        m_connector-> refreshView( this );
    }

    virtual void
    handleMouseEvent( const QMouseEvent & ev )
    {
        QString str;
        QTextStream out( & str );

        m_lastMouse = QPointF( ev.x(), ev.y() );
        m_connector-> refreshView( this );

        m_connector-> setState( m_prefix + "/mouse_x", QString::number( ev.x() ) );
        m_connector-> setState( m_prefix + "/mouse_y", QString::number( ev.y() ) );

        int imgX = ev.x() * m_astroImage-> dims()[0] / m_imageBuffer.width();
        int imgY = ev.y() * m_astroImage-> dims()[1] / m_imageBuffer.height();
        imgY = m_astroImage-> dims()[1] - imgY - 1;

        CoordinateFormatterInterface::SharedPtr cf(
            m_astroImage-> metaData()-> coordinateFormatter()-> clone() );

        std::vector < QString > knownSCS2str {
            "Unknown", "J2000", "B1950", "ICRS", "Galactic",
            "Ecliptic"
        };
        std::vector < KnownSkyCS > css {
            KnownSkyCS::J2000, KnownSkyCS::B1950, KnownSkyCS::Galactic,
            KnownSkyCS::Ecliptic, KnownSkyCS::ICRS
        };
        out << "Default sky cs:" << knownSCS2str[static_cast < int > ( cf-> skyCS() )] << "\n";
        out << "Image cursor:" << imgX << "," << imgY << "\n";

        for ( auto cs : css ) {
            cf-> setSkyCS( cs );
            out << knownSCS2str[static_cast < int > ( cf-> skyCS() )] << ": ";
            std::vector < Carta::Lib::AxisInfo > ais;
            for ( int axis = 0 ; axis < cf->nAxes() ; axis++ ) {
                const Carta::Lib::AxisInfo & ai = cf-> axisInfo( axis );
                ais.push_back( ai );
            }
            std::vector < double > pixel( m_astroImage-> dims().size(), 0.0 );
            pixel[0] = imgX;
            pixel[1] = imgY;
            auto list = cf-> formatFromPixelCoordinate( pixel );
            for ( size_t i = 0 ; i < ais.size() ; i++ ) {
                out << ais[i].shortLabel().html() << ":" << list[i] << " ";
            }
            out << "\n";
        }

        str.replace( "\n", "<br />" );
        m_connector-> setState( "/hacks/cursorText", str );
    } // handleMouseEvent

    virtual void
    handleKeyEvent( const QKeyEvent & /*event*/ )
    { }

signals:

protected:

    void
    redrawBuffer()
    {
        QPointF center = m_imageBuffer.rect().center();
        QPointF diff = m_lastMouse - center;
        double angle = atan2( diff.x(), diff.y() );
        angle *= - 180 / M_PI;

        m_imageBuffer = m_qImageToRender.scaled( m_imageBuffer.size(), Qt::IgnoreAspectRatio,
                                                 Qt::SmoothTransformation );

////                                                 Qt::FastTransformation );

//        m_imageBuffer.fill( m_bgColor );
        {
            QPainter p( & m_imageBuffer );

//            p.drawImage( m_qimage.rect(), m_defaultImage );
//            p.setRenderHint( QPainter::SmoothPixmapTransform);
//            QImage tmpImage = m_defaultImage.scaled( m_qimage.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
//            p.drawImage( m_qimage.rect(), tmpImage);

//            m_pixmapToRender.convertFromImage( m_qImageToRender );

//            QPixmap tmpPixmap = m_pixmapToRender.scaled(
//                m_imageBuffer.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
//            p.drawPixmap( m_imageBuffer.rect(), tmpPixmap );

            p.setPen( Qt::NoPen );
            p.setBrush( QColor( 255, 255, 0, 128 ) );
            p.drawEllipse( QPoint( m_lastMouse.x(), m_lastMouse.y() ), 10, 10 );
            p.setPen( QColor( 255, 255, 255 ) );
            p.drawLine( 0, m_lastMouse.y(), m_imageBuffer.width() - 1, m_lastMouse.y() );
            p.drawLine( m_lastMouse.x(), 0, m_lastMouse.x(), m_imageBuffer.height() - 1 );

            p.translate( m_imageBuffer.rect().center() );
            p.rotate( angle );
            p.translate( - m_imageBuffer.rect().center() );
            p.setFont( QFont( "Arial", 20 ) );
            p.setPen( QColor( "white" ) );
            p.drawText( m_imageBuffer.rect(), Qt::AlignCenter, m_viewName );
        }

        // execute the pre-render hook
        Globals::instance()-> pluginManager()
            -> prepare < PreRender > ( m_viewName, & m_imageBuffer )
            .executeAll();
    } // redrawBuffer

    QColor m_bgColor;
    QPixmap m_pixmapToRender;
    Image::ImageInterface::SharedPtr m_astroImage;
    IConnector * m_connector;
    QImage m_imageBuffer;
    QString m_viewName, m_prefix;
    QPointF m_lastMouse;
};

static TestView2 * testView2 = nullptr;

HackViewer::HackViewer( QString prefix ) :
    QObject( nullptr )
{
    m_statePrefix = prefix;

    m_rawView2QImageConverter.reset( new Carta::Core::RawView2QImageConverter3 );

    // assign a default colormap to the view
    auto rawCmap = std::make_shared < Carta::Core::GrayColormap > ();
    m_rawView2QImageConverter-> setColormap( rawCmap );
}

void
HackViewer::start()
{
    qDebug() << "HackViewer::start() starting";

    auto & globals = * Globals::instance();
    m_connector = globals.connector();

    setState( "auto_clip", "1" );

    auto pm = globals.pluginManager();

    // setup some colormap hacks
    {
        // get all colormaps provided by core
        static std::vector < Carta::Lib::IColormapScalar::SharedPtr > allColormaps;
        allColormaps.push_back( std::make_shared < Carta::Core::GrayColormap > () );

        // ask plugins for colormaps
        auto hh =
            pm-> prepare < Carta::Lib::Hooks::ColormapsScalarHook > ();

        auto lam = [] ( const Carta::Lib::Hooks::ColormapsScalarHook::ResultType & cmaps ) {
            allColormaps.insert( allColormaps.end(), cmaps.begin(), cmaps.end() );
        };
        hh.forEach( lam );

        qDebug() << "We have" << allColormaps.size() << "colormaps:";
        for ( auto & cmap : allColormaps ) {
            qDebug() << "    " << cmap-> name();
        }
        for ( size_t i = 0 ; i < allColormaps.size() ; i++ ) {
            setState( QString( "cm-names-%1" ).arg( i ), allColormaps[i]-> name() );
        }
        setState( "cm-count", QString::number( allColormaps.size() ) );
        setState( "cm-current", "0" );
        auto colormapCB = [this] ( const QString & /*path*/, const QString & value ) {
            qDebug() << "Cmap changed to" << value;
            bool ok;
            int ind = value.toInt( & ok );
            if ( ! ok ) {
                return;
            }
            using namespace Carta::Core;
            if ( ind < 0 || size_t( ind ) >= allColormaps.size() ) {
                CARTA_ASSERT( "colormap index out of range!" );
                return;
            }
            m_rawView2QImageConverter-> setColormap( allColormaps[ind] );
            scheduleFrameReload();
        };

//        m_connector->addStateCallback(
//            "/hacks/cm-current",
//            colormapCB
//            );
        addStateCallback( "cm-current", colormapCB );
    }

    // ask plugins to load the image
    qDebug() << "======== trying to load image ========";

    //QString fname = Globals::fname();
    QString fname;
    if ( ! Globals::instance()-> platform()-> initialFileList().isEmpty() ) {
        fname = Globals::instance()-> platform()-> initialFileList()[0];
    }

    // ask one of the plugins to load the image
    if ( fname.length() > 0 ) {
        qDebug() << "Trying to load astroImage...";
        auto res2 =
            Globals::instance()-> pluginManager()
                -> prepare < Carta::Lib::Hooks::LoadAstroImage > ( fname ).first();
        if ( res2.isNull() ) {
            qFatal( "Could not find any plugin to load astroImage" );
        }
        m_image = res2.val();

        CARTA_ASSERT( m_image );
        m_coordinateFormatter = m_image-> metaData()-> coordinateFormatter();

        qDebug() << "Pixel type = " << Image::pixelType2int( res2.val()-> pixelType() );
        testView2 = new TestView2(
            "/hacks",
            "hackView",
            QColor( "pink" ),

            // QImage( 10, 10, QImage::Format_ARGB32 ),
            m_image );
        m_connector-> registerView( testView2 );

        CoordinateFormatterInterface::VD pixel;
        pixel.resize( m_coordinateFormatter->nAxes(), 0 );
        auto fmt = m_coordinateFormatter-> formatFromPixelCoordinate( pixel );
        auto skycs = KnownSkyCS::Galactic;
        m_coordinateFormatter-> setSkyCS( skycs );
        qDebug() << "set skycs to" << int (skycs)
                 << "now it is" << int ( m_coordinateFormatter-> skyCS() );
        fmt = m_coordinateFormatter-> formatFromPixelCoordinate( pixel );
        qDebug() << "0->" << fmt.join( "|" );

        // convert the loaded image into QImage
        m_currentFrame = 0;
        scheduleFrameReload();
    }

    QString pixelCachingOn = "pixelCacheOn";
    QString pixelCacheSize = "pixelCacheSize";
    QString pixelCacheInterpolationOn = "pixelCacheInterpolationOn";
    setState( pixelCachingOn, m_cmapUseCaching ? "1" : "0" );
    setState( pixelCacheInterpolationOn, m_cmapUseInterpolatedCaching ? "1" : "0" );
    setState( pixelCacheSize, QString::number( m_cmapCacheSize ) );

    typedef const QString & CSR;
    addStateCallback(
        pixelCachingOn, [this] ( CSR, CSR val ) {
            m_cmapUseCaching = val == "1";
            scheduleFrameReload();
        }
        );
    addStateCallback(
        pixelCacheInterpolationOn, [this] ( CSR, CSR val ) {
            m_cmapUseInterpolatedCaching = val == "1";
            scheduleFrameReload();
        }
        );
    addStateCallback(
        pixelCacheSize, [=] ( CSR, CSR val ) {
            bool ok;
            m_cmapCacheSize = val.toInt( & ok );
            if ( ! ok || m_cmapCacheSize < 2 ) {
                m_cmapCacheSize = 2;
            }
            setState( pixelCacheSize, QString::number( m_cmapCacheSize ) );
            scheduleFrameReload();
        }
        );

    qDebug() << "HackViewer has been initialized.";
} // start

// prefixed setState
void
HackViewer::setState( const QString & path, const QString & value )
{
    m_connector->setState( m_statePrefix + "/" + path, value );
}

// prefixed getState
QString
HackViewer::getState( const QString & path )
{
    return m_connector->getState( m_statePrefix + "/" + path );
}

IConnector::CallbackID
HackViewer::addStateCallback( QString path, IConnector::StateChangedCallback cb )
{
    return m_connector-> addStateCallback( m_statePrefix + "/" + path, cb );
}

void
HackViewer::scheduleFrameReload()
{
    CARTA_ASSERT( m_image );

    // if reload is already pending, do nothing
    if( m_reloadFrameQueued) {
        qDebug() << "XXX saved reload";
        return;
    }
    m_reloadFrameQueued = true;
    QMetaObject::invokeMethod( this, "_reloadFrameNow", Qt::QueuedConnection);
}


void
HackViewer::_reloadFrameNow()
{
    CARTA_ASSERT( m_image );

    m_reloadFrameQueued = false;

    qDebug() << "reloadFrame ppCacheSize" << m_cmapCacheSize;

    // prepare slice description corresponding to the entire frame [:,:,frame,0,0,...0]
    auto frameSlice = SliceND().next();
    for ( size_t i = 2 ; i < m_image->dims().size() ; i++ ) {
        frameSlice.next().index( i == 2 ? m_currentFrame : 0 );
    }

    // get a view of the data using the slice description
    NdArray::RawViewInterface::UniquePtr frameView( m_image-> getDataSlice( frameSlice ) );

    // setup the pipeline with this view
    m_rawView2QImageConverter-> setView( frameView.get() );

    // recompute clips
    m_rawView2QImageConverter-> computeClips( 0.95 );

    // update options for the pipeline
    m_rawView2QImageConverter-> setPixelPipelineCacheSize( m_cmapCacheSize );
    m_rawView2QImageConverter-> setPixelPipelineInterpolation( m_cmapUseInterpolatedCaching );
    m_rawView2QImageConverter-> setPixelPipelineCacheEnabled( m_cmapUseCaching );

    // do the conversion directly into the view's qimage buffer
    m_rawView2QImageConverter-> convert( testView2-> imageToRender() );

    // schedule a redraw
    testView2-> scheduleRedraw();
} // scheduleFrameReload



/// experiment, currently unused
///

/*
static
std::map < QString, QString >
parseParamMap2( const QString & paramsToParse, const std::set < QString > & keyList )
{
    std::map < QString, QString > result;
    for ( const auto & entry : paramsToParse.split( ',' ) ) {
        auto keyVal = entry.split( ':' );
        if ( keyVal.size() != 2 ) {
            qWarning() << "bad map format:" << paramsToParse;
            return { };
        }
        auto key = keyVal[0].trimmed();
        auto val = keyVal[1].trimmed();
        auto ind = result.find( key );
        if ( ind != result.end() ) {
            qWarning() << "duplicate key:" << paramsToParse;
            return { };
        }
        result.insert( ind, std::make_pair( key, val ) );
    }

    // make sure every key is in parameters
    for ( const auto & key : keyList ) {
        if ( ! result.count( key ) ) {
            qWarning() << "could not find key=" << key << "in" << paramsToParse;
            return { };
        }

        // make sure parameters don't have unknown keys
    }
    for ( const auto & kv : result ) {
        if ( ! keyList.count( kv.first ) ) {
            qWarning() << "unknown key" << kv.first << "in" << paramsToParse;
            return { };
        }
    }
    return result;
} // parseParamMap2
*/
