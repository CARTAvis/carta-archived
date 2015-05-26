/**
 *
 **/

#include "ImageViewController.h"
#include "../Globals.h"
#include "../IConnector.h"
#include "../PluginManager.h"
#include "Algorithms/quantileAlgorithms.h"
#include "CartaLib/Hooks/LoadAstroImage.h"
#include "GrayColormap.h"
#include <QPainter>
#include <QTime>
#include <functional>

namespace Impl
{
/// convert string to array of doubles
std::vector < double >
s2vd( QString s, QString sep = " " )
{
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
}
}

namespace Hacks
{
ImageViewController::ImageViewController( QString statePrefix, QString viewName, QObject * parent )
    : QObject( parent )
{
    m_statePrefix = statePrefix;
    m_viewName = viewName;
    m_rawViewName = "IVC" + viewName;

    auto & globals = * Globals::instance();
    m_connector = globals.connector();

    // create an image render service and connect it to ourselves
    m_renderService.reset( new Carta::Core::ImageRenderService::Service() );
    connect( m_renderService.get(), & Carta::Core::ImageRenderService::Service::done,
             this, & ImageViewController::irsDoneSlot,
             Qt::QueuedConnection );

    // create a new wcs grid renderer (take the first one that plugins provide), and
    // connect it
    {
        qDebug() << "Looking for wcs grid renderer";
        auto res = Globals::instance()-> pluginManager()
                       -> prepare < Carta::Lib::Hooks::GetWcsGridRendererHook > ().first();
        if ( res.isNull() || ! res.val() ) {
            qWarning( "wcsgrid: Could not find any WCS grid renderers" );
            m_wcsGridRenderer = nullptr;
        }
        else {
            m_wcsGridRenderer = res.val();

            qDebug() << "wcsgrid: connecting wcs grid renderer to core";
            auto conn2 = connect(
                m_wcsGridRenderer.get(), & Carta::Lib::IWcsGridRenderService::done,
                this, & Me::wcsGridSlot,
                Qt::QueuedConnection );
            if ( ! conn2 ) {
                qWarning() << "wcsgrid: wcs grid renderer failed to connect!!!";

                // get rid of the renderer as it's unusable
                m_wcsGridRenderer = nullptr;
            }

            if ( m_wcsGridRenderer ) {
                qDebug( "wcsgrid: WCS grid renderer activated" );
            }
        }
    }

    // initialize pixel pipeline
    m_pixelPipeline = std::make_shared < Carta::Lib::PixelPipeline::CustomizablePixelPipeline > ();
    m_pixelPipeline-> setInvert( false );
    m_pixelPipeline-> setReverse( false );
    m_pixelPipeline-> setColormap( std::make_shared < Carta::Core::GrayColormap > () );
    m_pixelPipeline-> setMinMax( 0, 1 );
    m_renderService-> setPixelPipeline( m_pixelPipeline, m_pixelPipeline-> cacheId() );

    // register with connector as a view
    m_connector-> registerView( this );

    // register callback for zoom
    m_connector-> addCommandCallback(
        m_statePrefix + "/zoom",
        std::bind( & ImageViewController::zoomCB, this,
                   std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 )
        );

    // register callback for center
    m_connector-> addCommandCallback(
        m_statePrefix + "/center",
        std::bind( & ImageViewController::panCB, this,
                   std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 )
        );

    m_connector-> setState( m_statePrefix + "/frame", "n/a" );

    typedef const QString & CSR;

    // hook-up mini movie player slider
//    auto mmpSliderCB = [&] ( CSR, CSR par, CSR ) -> QString {
//        auto frame = Impl::s2vd( par );
//        if ( frame.size() > 0 && m_astroImage-> dims().size() > 2 ) {
//            qDebug() << "Slider" << frame[0];
//            loadFrame( frame[0] * m_astroImage-> dims()[2] );
//        }
//        return "";
//    };
//    m_connector-> addCommandCallback( m_statePrefix + "/setFrame", mmpSliderCB );

    auto ptrMoveCB = [&] ( CSR, CSR par ) -> void {
        auto coords = Impl::s2vd( par );
        if ( coords.size() >= 2 ) {
            qDebug() << "ptrmove" << coords[0] << coords[1];
        }
    };

    m_connector-> addStateCallback( m_statePrefix + "/pointer-move",  ptrMoveCB );

    // hook-up movie play button
    m_connector-> addStateCallback( m_statePrefix + "/playToggle", [&] ( CSR, CSR val ) {
                                        playMovie( val == "1" );
                                    }
                                    );

    // hook up grid toggle
    m_connector-> addStateCallback( m_statePrefix + "/gridToggle", [&] ( CSR, CSR val ) {
                                        m_gridToggle = ( val == "1" );
                                        m_wcsGridRenderer-> setEmptyGrid( ! m_gridToggle);

                                        // this is very inefficient, but it'll get the job done for now
                                        m_renderService-> render( 0 );
                                        updateGridAfterPanZoomResize();
                                    }
                                    );

    // connect movie timer to the frame advance slot
    connect( & m_movieTimer, & QTimer::timeout, this, & ImageViewController::loadNextFrame );

    // create a controller for grid options
    m_wcsGridOptionsController.reset(
        new WcsGridOptionsController(
            this,
            "/hacks/gridControls/c1",
            m_wcsGridRenderer
            ) );

#define SetupVar( vname, type, path, initValue) \
    vname.reset( new type( prefix.with(path))); \
    vname-> set( initValue); \
    connect( vname.get(), & type::valueChanged, this, & Me::stdVarCB);
#undef SetupVar

    namespace SS = Carta::Lib::SharedState;
    const SS::FullPath prefix = SS::FullPath::fromQString( m_statePrefix);

    m_frameVar.reset( new Carta::Lib::SharedState::DoubleVar( prefix.with( "frameSlider")));
    m_frameVar-> set( 0);
    connect( m_frameVar.get(), & SS::BoolVar::valueChanged, [&] () {

        if( m_astroImage-> dims().size() < 2) return;
        int frame = m_frameVar-> get() * m_astroImage-> dims()[2] / 1000000.0;
        frame = Carta::Lib::clamp<int>( frame, 0, m_astroImage-> dims()[2] - 1);
        loadFrame( frame);
    });
}

void
ImageViewController::playMovie( bool flag )
{
    if ( flag ) {
        m_movieTimer.start( 1000 / 60 );
    }
    else {
        m_movieTimer.stop();
    }
}

void
ImageViewController::updateGridAfterPanZoomResize()
{
    // erase current grid
    m_gridVG = Nullable < Carta::Lib::VectorGraphics::VGList > ();

    // if grid drawing is not enabled, we are done (are we?)
    if ( ! ( m_gridToggle && m_wcsGridRenderer ) ) {
        return;
    }

    auto renderSize = m_renderService-> outputSize();
    m_wcsGridRenderer-> setOutputSize( renderSize );

    int leftMargin = 50;
    int rightMargin = 10;
    int bottomMargin = 50;
    int topMargin = 10;

    QRectF outputRect( leftMargin, topMargin,
                       renderSize.width() - leftMargin - rightMargin,
                       renderSize.height() - topMargin - bottomMargin );
    QRectF inputRect(
        m_renderService-> screen2img( outputRect.topLeft() ),
        m_renderService-> screen2img( outputRect.bottomRight() ) );

    m_wcsGridRenderer-> setImageRect( inputRect );
    m_wcsGridRenderer-> setOutputRect( outputRect );

    m_wcsGridRenderer-> startRendering();
} // updateGridAfterPanZoom

QString
ImageViewController::zoomCB( const QString &, const QString & params, const QString & )
{
    auto vals = Impl::s2vd( params );

    // ignore misformatted messages
    if ( vals.size() < 3 ) {
        qWarning() << "zoomCB: bad message" << params;
        return "";
    }

    // remember where the user clicked
    QPointF clickPtScreen( vals[0], vals[1] );
    QPointF clickPtImageOld = m_renderService-> screen2img( clickPtScreen );

    // apply new zoom
    double z = vals[2];
    double newZoom;
    if ( z < 0 ) {
        newZoom = m_renderService-> zoom() / 0.9;
    }
    else {
        newZoom = m_renderService-> zoom() * 0.9;
    }
    m_renderService-> setZoom( newZoom );

    // what is the new image pixel under the mouse cursor?
    QPointF clickPtImageNew = m_renderService-> screen2img( clickPtScreen );

    // calculate the difference
    QPointF delta = clickPtImageOld - clickPtImageNew;

    // add the delta to the current center
    QPointF currCenter = m_renderService-> pan();
    m_renderService-> setPan( currCenter + delta );

    // tell the service to rerender
    m_renderedAstroImage = QImage();
    m_renderService-> render( 0 );

    // if grid is active, ask grid renderer to re-render grid as well
    updateGridAfterPanZoomResize();

    return "";
} // zoomCB

QString
ImageViewController::panCB( const QString &, const QString & params, const QString & )
{
    auto vals = Impl::s2vd( params );
    if ( vals.size() > 1 ) {
        auto newCenter = m_renderService-> screen2img( { vals[0], vals[1] }
                                                       );
        m_renderService-> setPan( newCenter );
        m_renderService-> render( 0 );
    }

    // update grid if appropriate
    updateGridAfterPanZoomResize();

    return "";
} // zoomCB

ImageViewController::~ImageViewController()
{ }

QSize
ImageViewController::size()
{
//    return m_renderService-> outputSize();
    return m_renderBuffer.size();
}

const QImage &
ImageViewController::getBuffer()
{
    return m_renderBuffer;
}

void
ImageViewController::setCmapInvert( bool flag )
{
    m_pixelPipeline-> setInvert( flag );
    m_renderService-> setPixelPipeline( m_pixelPipeline, m_pixelPipeline-> cacheId() );
    m_renderService-> render( 0 );
}

void
ImageViewController::setCmapReverse( bool flag )
{
    m_pixelPipeline-> setReverse( flag );
    m_renderService-> setPixelPipeline( m_pixelPipeline, m_pixelPipeline-> cacheId() );
    m_renderService-> render( 0 );
}

void
ImageViewController::setColormap( Carta::Lib::PixelPipeline::IColormapNamed::SharedPtr cmap )
{
    m_pixelPipeline-> setColormap( cmap );
    m_renderService-> setPixelPipeline( m_pixelPipeline, m_pixelPipeline-> cacheId() );
    m_renderService-> render( 0 );
}

void
ImageViewController::setPPCsettings( ImageViewController::PPCsettings settings )
{
    m_renderService-> setPixelPipelineCacheSettings( settings );
    m_renderService-> render( 0 );
}

ImageViewController::PPCsettings
ImageViewController::getPPCsettings()
{
    return m_renderService-> pixelPipelineCacheSettings();
}

void
ImageViewController::handleResizeRequest( const QSize & size )
{
    // redraw the image with the new size
    m_renderedAstroImage = QImage();
    m_renderService-> setOutputSize( size );
    m_renderService-> render( 0 );

    // request grid redraw with the new size as well
    updateGridAfterPanZoomResize();
}

void
ImageViewController::loadImage( QString fname )
{
    m_fileName = fname;

    qDebug() << "loadImage() Trying to load astroImage...";
    auto res =
        Globals::instance()-> pluginManager()
            -> prepare < Carta::Lib::Hooks::LoadAstroImage > ( fname ).first();
    if ( res.isNull() ) {
        qWarning( "Could not find any plugin to load astroImage" );
    }
    m_astroImage = res.val();

    // reset zoom/pan
    m_renderService-> setZoom( 1.0 );
    m_renderService-> setPan( { m_astroImage-> dims()[0] / 2.0 - 0.5,
                                m_astroImage-> dims()[1] / 2.0 - 0.5 }
                              );

    // clear quantile cache
    m_quantileCache.resize( 0 );
    int nf = 1;
    if ( m_astroImage-> dims().size() > 2 ) {
        nf = m_astroImage-> dims()[2];
    }
    m_quantileCache.resize( nf );

    // set the frame to first one
    loadFrame( 0 );
} // loadImage

void
ImageViewController::loadFrame( int frame )
{
    qDebug() << "loadFrame" << frame;

    // make sure the frame makes sense (i.e. clip it to allowed range)
    if ( frame < 0 ) {
        frame = 0;
    }
    if ( m_astroImage-> dims().size() <= 2 ) {
        frame = 0;
    }
    else {
        frame = Carta::Lib::clamp( frame, 0, m_astroImage-> dims()[2] - 1 );
    }
    m_currentFrame = frame;

    int oldFrameVar = m_frameVar-> get() * m_astroImage-> dims()[2] / 1e6;
    if( oldFrameVar != m_currentFrame) {
        m_frameVar-> set( m_currentFrame * 1e6 / m_astroImage-> dims()[2]);
    }

    // prepare slice description corresponding to the entire frame [:,:,frame,0,0,...0]
    auto frameSlice = SliceND().next();
    for ( size_t i = 2 ; i < m_astroImage->dims().size() ; i++ ) {
        frameSlice.next().index( i == 2 ? m_currentFrame : 0 );
    }

    // get a view of the data using the slice description and make a shared pointer out of it
    NdArray::RawViewInterface::SharedPtr view( m_astroImage-> getDataSlice( frameSlice ) );

    // compute 95% clip values, unless we already have them in the cache
    std::vector < double > clips = m_quantileCache[m_currentFrame];
    if ( clips.size() < 2 ) {
        NdArray::Double doubleView( view.get(), false );
        clips = Carta::Core::Algorithms::quantiles2pixels(
            doubleView, { 0.025, 0.975 }
            );
        qDebug() << "recomputed clips" << clips;
        m_quantileCache[m_currentFrame] = clips;
    }

    m_pixelPipeline-> setMinMax( clips[0], clips[1] );
    m_renderService-> setPixelPipeline( m_pixelPipeline, m_pixelPipeline-> cacheId() );

    // tell the render service to render this job
    m_renderService-> setInputView( view, QString( "%1//%2" ).arg( m_fileName ).arg( m_currentFrame ) );
    m_renderService-> render( 0 );

    // if grid is active, request a grid rendering as well
    if ( m_wcsGridRenderer ) {
        m_wcsGridRenderer-> setInputImage( m_astroImage );
    }
    updateGridAfterPanZoomResize();

    // update the GUI
    m_connector-> setState( m_statePrefix + "/frame", QString::number( m_currentFrame ) );

    qDebug() << "loadFrame done" << frame;
} // loadFrame

void
ImageViewController::loadNextFrame()
{
    int nf = 1;
    if ( m_astroImage-> dims().size() > 2 ) {
        nf = m_astroImage-> dims()[2];
    }
    loadFrame( ( m_currentFrame + 1 ) % nf );
} // loadFrame

void
ImageViewController::combineImageAndGrid()
{
    // first paint the rendered astro image (if we have it)
    if ( ! m_renderedAstroImage.isNull() ) {
        m_renderBuffer = m_renderedAstroImage;
    }

    // draw the grid over top
    if ( ! m_gridVG.isNull() && m_gridToggle ) {
        QTime t; t.restart();
        QPainter p( & m_renderBuffer );
        p.setRenderHint( QPainter::Antialiasing, true );
        Carta::Lib::VectorGraphics::VGListQPainterRenderer vgRenderer;
        if ( ! vgRenderer.render( m_gridVG.val(), p ) ) {
            qWarning() << "could not render grid vector graphics";
        }
        qDebug() << "Grid VG rendered in" << t.elapsed() / 1000.0 << "sec" << "xyz";
    }

    // schedule a repaint with the connector
    m_connector-> refreshView( this );
} // combineImageAndGrid

void
ImageViewController::irsDoneSlot( QImage img, Carta::Core::ImageRenderService::JobId jobId )
{
    Q_UNUSED( jobId );
    m_renderedAstroImage = img;

    // update colormap preview for the clients
    // this does not really belong here, but since we are just hacking...
    QStringList list;
    double clipMin, clipMax;
    m_pixelPipeline-> getClips( clipMin, clipMax );
    int n = 300;
    for ( int i = 0 ; i <= n ; i++ ) {
        double x = ( clipMax - clipMin ) / n * i + clipMin;
        QRgb col;
        m_pixelPipeline-> convertq( x, col );
        list << QColor( col ).name().mid( 1 );
    }
    m_connector-> setState( "/hacks/cm-preview", list.join( "," ) );

    // combine the result with grid and render
    combineImageAndGrid();
} // irsDoneSlot

void
ImageViewController::wcsGridSlot(Carta::Lib::VectorGraphics::VGList vglist , Carta::Lib::IWcsGridRenderService::JobId jobId)
{
    Q_UNUSED( jobId);
    qDebug() << "wcsgrid: wcsGridSlot" << vglist.entries().size() << "entries";
    m_gridVG = vglist;

    combineImageAndGrid();
}
}
