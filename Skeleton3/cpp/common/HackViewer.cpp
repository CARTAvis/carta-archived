/**
 *
 **/

#include "HackViewer.h"
#include "Globals.h"
#include "IConnector.h"
#include "CartaLib/CartaLib.h"
#include "CartaLib/Hooks/ColormapsScalar.h"
#include "CartaLib/Hooks/LoadAstroImage.h"
#include "common/Hacks/MainModel.h"
#include "common/GrayColormap.h"
#include "LinearMap.h"
#include <QPainter>
#include <set>

namespace Hacks
{
namespace StateKey
{
QString MOUSE_X = "mouse_x";
QString MOUSE_Y = "mouse_y";
QString HACKS = "hacks";
QString AUTO_CLIP = "auto_clip";
}

TestView2::TestView2( QString prefix,
                      QString viewName,
                      QColor bgColor,
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
    m_qImageToRender = QImage( 100, 100, QImage::Format_ARGB32_Premultiplied );
}

void
TestView2::scheduleRedraw()
{
    m_connector-> refreshView( this );
}

void
TestView2::registration( IConnector * connector )
{
    m_connector = connector;
}

const QString &
TestView2::name() const
{
    return m_viewName;
}

QSize
TestView2::size()
{
    return m_imageBuffer.size();
}

const QImage &
TestView2::getBuffer()
{
    redrawBuffer();
    return m_imageBuffer;
}

void
TestView2::handleResizeRequest( const QSize & pSize )
{
    QSize size( std::max( pSize.width(), 1 ), std::max( pSize.height(), 1 ) );
    m_imageBuffer = QImage( size, m_imageBuffer.format() );
    scheduleRedraw();

    emit resized();
}

void
TestView2::handleMouseEvent( const QMouseEvent & ev )
{
    QString str;
    QTextStream out( & str );

    m_lastMouse = QPointF( ev.x(), ev.y() );
    scheduleRedraw();

    m_connector-> setState( m_prefix + "/mouse_x", QString::number( ev.x() ) );
    m_connector-> setState( m_prefix + "/mouse_y", QString::number( ev.y() ) );

    emit mouseX( double ( ev.x() ) / size().width() );

    if ( ! m_astroImage ) {
        return;
    }
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

void
TestView2::redrawBuffer()
{
//    QPointF center = m_imageBuffer.rect().center();
//    QPointF diff = m_lastMouse - center;
//    double angle = atan2( diff.x(), diff.y() );
//    angle *= - 180 / M_PI;

//    m_imageBuffer = m_qImageToRender.scaled( m_imageBuffer.size(), Qt::IgnoreAspectRatio,
//                                             Qt::SmoothTransformation );

//    ////                                                 Qt::FastTransformation );

//    {
//        QPainter p( & m_imageBuffer );

//        p.setPen( Qt::NoPen );
//        p.setBrush( QColor( 255, 255, 0, 128 ) );
//        p.drawEllipse( QPoint( m_lastMouse.x(), m_lastMouse.y() ), 10, 10 );
//        p.setPen( QColor( 255, 255, 255 ) );
//        p.drawLine( 0, m_lastMouse.y(), m_imageBuffer.width() - 1, m_lastMouse.y() );
//        p.drawLine( m_lastMouse.x(), 0, m_lastMouse.x(), m_imageBuffer.height() - 1 );

//        p.translate( m_imageBuffer.rect().center() );
//        p.rotate( angle );
//        p.translate( - m_imageBuffer.rect().center() );
//        p.setFont( QFont( "Arial", 20 ) );
//        p.setPen( QColor( "white" ) );
//        p.drawText( m_imageBuffer.rect(), Qt::AlignCenter, m_viewName );
//    }

    // execute the pre-render hook
    Globals::instance()-> pluginManager()
        -> prepare < PreRender > ( m_viewName, & m_imageBuffer )
        .executeAll();
} // redrawBuffer
}
using namespace Hacks;

//static TestView2 * m_testView2 = nullptr;

HackViewer::HackViewer( QString prefix ) :
    QObject( nullptr )
{
    m_statePrefix = prefix;

    // assign a default colormap to the view
    auto rawCmap = std::make_shared < Carta::Core::GrayColormap > ();
    m_rawView2QImageConverter.reset( new Carta::Core::RawView2QImageConverter3 );
    m_rawView2QImageConverter-> setColormap( rawCmap );

    m_wholeImage = QImage( 10, 10, QImage::Format_ARGB32_Premultiplied );
}

QPointF
HackViewer::img2screen( QPointF p )
{
    double icx = m_centeredImagePoint.x();
    double scx = m_testView2->size().width() / 2.0;
    double icy = m_centeredImagePoint.y();
    double scy = m_testView2->size().height() / 2.0;
    LinearMap1D xmap( scx, scx + m_pixelZoom, icx, icx + 1 );
    LinearMap1D ymap( scy, scy + m_pixelZoom, icy, icy + 1 );

    QPointF res;
    res.rx() = xmap.inv( p.x() );
    res.ry() = ymap.inv( p.y() );
    return res;
}

QPointF
HackViewer::screen2img( QPointF p )
{
    double icx = m_centeredImagePoint.x();
    double scx = m_testView2->size().width() / 2.0;
    double icy = m_centeredImagePoint.y();
    double scy = m_testView2->size().height() / 2.0;
    LinearMap1D xmap( scx, scx + m_pixelZoom, icx, icx + 1 );
    LinearMap1D ymap( scy, scy + m_pixelZoom, icy, icy + 1 );

    QPointF res;
    res.rx() = xmap.apply( p.x() );
    res.ry() = ymap.apply( p.y() );
    return res;
}

void
HackViewer::start()
{
    qDebug() << "HackViewer::start() starting";

    auto & globals = * Globals::instance();
    m_connector = globals.connector();

    setState( "auto_clip", "1" );

    auto pm = globals.pluginManager();

    // prepare a list of known colormaps
    {
        // built-in colormaps first (we only have one)
        m_allColormaps.push_back( std::make_shared < Carta::Core::GrayColormap > () );

        // now ask plugins for colormaps
        auto hh =
            pm-> prepare < Carta::Lib::Hooks::ColormapsScalarHook > ();

        auto lam = [this] ( const Carta::Lib::Hooks::ColormapsScalarHook::ResultType & cmaps ) {
            m_allColormaps.insert( m_allColormaps.end(), cmaps.begin(), cmaps.end() );
        };
        hh.forEach( lam );

        qDebug() << "We have" << m_allColormaps.size() << "colormaps:";
        for ( auto & cmap : m_allColormaps ) {
            qDebug() << "    " << cmap-> name();
        }
    }

    // tell clients about available colormaps
    for ( size_t i = 0 ; i < m_allColormaps.size() ; i++ ) {
        setState( QString( "cm-names-%1" ).arg( i ), m_allColormaps[i]-> name() );
    }
    setState( "cm-count", QString::number( m_allColormaps.size() ) );
    setState( "cm-current", "0" );

    {
        auto colormapCB = [this] ( const QString & /*path*/, const QString & value ) {
            qDebug() << "Cmap changed to" << value;
            bool ok;
            int ind = value.toInt( & ok );
            if ( ! ok ) {
                return;
            }
            using namespace Carta::Core;
            if ( ind < 0 || size_t( ind ) >= m_allColormaps.size() ) {
                CARTA_ASSERT( "colormap index out of range!" );
                return;
            }
            m_rawView2QImageConverter-> setColormap( m_allColormaps[ind] );
            scheduleFrameReload();
        };

        addStateCallback( "cm-current", colormapCB );

        addStateCallback(
            "cm-invert",
            [this] ( QString, QString ) {
                scheduleFrameReload();
            }
            );

        addStateCallback(
            "cm-reverse",
            [this] ( QString, QString ) {
                scheduleFrameReload();
            }
            );
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
        m_astroImage = res2.val();

        CARTA_ASSERT( m_astroImage );
        m_coordinateFormatter = m_astroImage-> metaData()-> coordinateFormatter();

        qDebug() << "Pixel type = " << Image::pixelType2int( res2.val()-> pixelType() );

        m_testView2 = new TestView2(
            "/hacks",
            "hackView",
            QColor( "pink" ),
            m_astroImage );

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
        m_pixelZoom = 1.0;
        m_centeredImagePoint.rx() = m_astroImage-> dims()[0] / 2.0;
        m_centeredImagePoint.ry() = m_astroImage-> dims()[1] / 2.0;
        scheduleFrameReload();
    }
    else {
        m_testView2 = new TestView2(
            "/hacks",
            "hackView",
            QColor( "pink" ),
            nullptr );
    }
    m_connector-> registerView( m_testView2 );
    connect( m_testView2, & TestView2::resized, this, & HackViewer::scheduleFrameRepaint );
    connect( m_testView2, & TestView2::mouseX, [ = ] ( double x ) {
                 this->m_pixelZoom = x + 1;
                 this->scheduleFrameRepaint();
             }
             );

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
        pixelCacheSize, [ = ] ( CSR, CSR val ) {
            bool ok;
            m_cmapCacheSize = val.toInt( & ok );
            if ( ! ok || m_cmapCacheSize < 2 ) {
                m_cmapCacheSize = 2;
            }
            setState( pixelCacheSize, QString::number( m_cmapCacheSize ) );
            scheduleFrameReload();
        }
        );

    // add listener for pointer move (using state API for automatic throttling)
    addStateCallback( "views/hackView/pointer-move", [ = ] ( CSR, CSR val ) {
                          qDebug() << "hackView mm" << val;
                      }
                      );

    // string 2 list of doubles
    auto s2vd = [] ( QString s, QString sep = " " ) {
        QStringList lst = s.split( sep );
        std::vector < double > res;
        for ( auto v : lst ) {
            bool ok;
            double val = v.toDouble( & ok );
            if ( ! ok ) {
                return res;
            }
            res.push_back( val );
        }
        return res;
    };

    // add listener for zoom
    m_connector-> addCommandCallback(
        m_statePrefix + "/views/hackView/zoom",
        [ = ] ( CSR, CSR params, CSR ) -> QString {
            try {
                qDebug() << "zoom" << params;

                double x, y, z;
                bool ok;
                QStringList list = params.split( " " );
                if ( list.size() < 3 ) {
                    throw "need 2 entries in list";
                }
                x = list[0].toDouble( & ok );
                if ( ! ok ) {
                    throw "bad x";
                }
                y = list[1].toDouble( & ok );
                if ( ! ok ) {
                    throw "bad y";
                }
                z = list[2].toDouble( & ok );
                if ( ! ok ) {
                    throw "bad z";
                }
                if ( z < 0 ) {
                    this-> m_pixelZoom /= 0.9;
                }
                else {
                    this-> m_pixelZoom *= 0.9;
                }
                this->m_pixelZoom = clamp( this->m_pixelZoom, 0.1, 16.0 );
                this->scheduleFrameRepaint();

                Q_UNUSED( x );
                Q_UNUSED( y );
            }
            catch ( std::string what ) {
                qWarning() << "Error:" << what.c_str();
                return "";
            }
            catch ( ... ) {
                qWarning() << "Other error";
                return "";
            }

            return "";
        }
        );

    // add listener for center
    m_connector-> addCommandCallback(
        m_statePrefix + "/views/hackView/center",
        [ = ] ( CSR, CSR params, CSR ) -> QString {
            qDebug() << "center" << params;

            auto vals = s2vd( params );
            if ( vals.size() > 1 ) {
                QPointF screenPt( vals[0], vals[1] );
                auto newCenter = screen2img( screenPt );

                qDebug() << "  " << m_centeredImagePoint << "->" << newCenter;

                m_centeredImagePoint = newCenter;

                this->scheduleFrameRepaint();
            }
            qDebug() << "  vals=" << vals;
            return "";
        }
        );

    // newest hacks:
    // =================================================================================

    // initialize hack model
    Hacks::GlobalsH::instance().setPluginManager( pm);

    // new experiment with asynchronous renderer
    m_imageViewController.reset( new Hacks::ImageViewController( m_statePrefix + "/views/IVC7", "7" ) );
    m_imageViewController-> loadImage( "/scratch/mosaic.fits" );
//    m_imageViewController-> loadImage( "/scratch/smallcube.fits" );

    // invert toggle
    addStateCallback(
        "cm-invert",
        [this] ( CSR, CSR val ) {
            m_imageViewController-> setCmapInvert( val == "1" );
        }
        );

    // reverse toggle
    addStateCallback(
        "cm-reverse",
        [this] ( CSR, CSR val ) {
            m_imageViewController-> setCmapReverse( val == "1" );
        }
        );

    auto colormapCB2 = [this] ( const QString & /*path*/, const QString & value ) {
        qDebug() << "Cmap2 changed to" << value;
        bool ok;
        int ind = value.toInt( & ok );
        if ( ! ok ) {
            return;
        }
        using namespace Carta::Core;
        if ( ind < 0 || size_t( ind ) >= m_allColormaps.size() ) {
            CARTA_ASSERT( "colormap index out of range!" );
            return;
        }
        m_imageViewController-> setColormap( m_allColormaps[ind] );
    };

    addStateCallback( pixelCachingOn, [this] ( CSR, CSR val ) {
        auto set = m_imageViewController-> getPPCsettings();
        set.enabled = val == "1";
        m_imageViewController-> setPPCsettings( set);
    });
    addStateCallback( pixelCacheInterpolationOn, [this] ( CSR, CSR val ) {
        auto set = m_imageViewController-> getPPCsettings();
        set.interpolated = val == "1";
        m_imageViewController-> setPPCsettings( set);
    });
    addStateCallback( pixelCacheSize, [ = ] ( CSR, CSR val ) {
        auto set = m_imageViewController-> getPPCsettings();
        bool ok;
        int size = val.toInt( & ok );
        if ( ! ok || size < 2 ) {
            size = 2;
        }
        set.size = size;
        m_imageViewController-> setPPCsettings( set);
    });


    addStateCallback( "cm-current", colormapCB2 );

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
    CARTA_ASSERT( m_astroImage );

    // if reload is already pending, do nothing
    if ( m_reloadFrameQueued ) {
        return;
    }
    m_reloadFrameQueued = true;
    QMetaObject::invokeMethod( this, "_reloadFrameNow", Qt::QueuedConnection );
}

void
HackViewer::_reloadFrameNow()
{
    CARTA_ASSERT( m_astroImage );

    m_reloadFrameQueued = false;

    qDebug() << "reloadFrame ppCacheSize" << m_cmapCacheSize;

    // prepare slice description corresponding to the entire frame [:,:,frame,0,0,...0]
    auto frameSlice = SliceND().next();
    for ( size_t i = 2 ; i < m_astroImage->dims().size() ; i++ ) {
        frameSlice.next().index( i == 2 ? m_currentFrame : 0 );
    }

    // get a view of the data using the slice description
    NdArray::RawViewInterface::UniquePtr frameView( m_astroImage-> getDataSlice( frameSlice ) );

    // setup the pipeline with this view
    m_rawView2QImageConverter-> setView( frameView.get() );

    // recompute clips
    m_rawView2QImageConverter-> computeClips( 0.95 );

    // update options for the pipeline
    m_rawView2QImageConverter-> setPixelPipelineCacheSize( m_cmapCacheSize );
    m_rawView2QImageConverter-> setPixelPipelineInterpolation( m_cmapUseInterpolatedCaching );
    m_rawView2QImageConverter-> setPixelPipelineCacheEnabled( m_cmapUseCaching );
    m_rawView2QImageConverter-> setInvert( getState( "cm-invert" ) == "1" );
    m_rawView2QImageConverter-> setReverse( getState( "cm-reverse" ) == "1" );

    // convert the data to image
    m_rawView2QImageConverter-> convert( m_wholeImage );

    setState( "cm-preview", m_rawView2QImageConverter-> getCmapPreview( 300 ) );

    _repaintFrameNow();
} // scheduleFrameReload

void
HackViewer::scheduleFrameRepaint()
{
    CARTA_ASSERT( m_astroImage );

    // if reload is already pending, do nothing
    if ( m_repaintFrameQueued ) {
        return;
    }
    else {
//        qDebug() << "Scheduling repaint";
    }
    m_repaintFrameQueued = true;
    QMetaObject::invokeMethod( this, "_repaintFrameNow", Qt::QueuedConnection );
}

void
HackViewer::_repaintFrameNow()
{
    m_repaintFrameQueued = false;

    m_testView2->getBufferRW().fill( QColor( "pink" ) );
    QPainter p( & m_testView2->getBufferRW() );
    double w = m_wholeImage.width();
    double h = m_wholeImage.height();

    // figure out rectf according to pixelzoom, centerx and centery
    QPointF p1 = img2screen( QPointF( 0, 0 ) );
    QPointF p2 = img2screen( QPointF( w, h ) );
    QRectF rectf( p1, p2 );

    p.drawImage( rectf, m_wholeImage );

    // schedule a redraw for the client
    m_testView2-> scheduleRedraw();
} // _repaintFrame

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
