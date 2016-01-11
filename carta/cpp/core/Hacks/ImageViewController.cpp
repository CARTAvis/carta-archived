/**
 *
 **/

#include "ImageViewController.h"
#include "../Globals.h"
#include "../IConnector.h"
#include "../PluginManager.h"
#include "Algorithms/quantileAlgorithms.h"
#include "CartaLib/Hooks/LoadAstroImage.h"
#include "CartaLib/Hooks/GetImageRenderService.h"
#include "GrayColormap.h"
#include "ProfileExtractor.h"
#include <QPainter>
#include <QTime>
#include <functional>

namespace Impl
{
/// convert string to array of doubles
static
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

namespace Carta
{
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
//    m_renderService.reset( new Carta::Core::ImageRenderService::Service() );

    {
        qDebug() << "Looking for image render service in plugins";
        auto res = Globals::instance()-> pluginManager()
                       -> prepare < Carta::Lib::Hooks::GetImageRenderService > ().first();
        if ( res.isNull() || ! res.val() ) {
            qWarning( "Could not find image render service in plugins" );
            m_renderService = nullptr;
        }
        else {
            qWarning( "Obtained image render service from a plugin" );
            m_renderService = res.val();
        }
        if ( ! m_renderService ) {
            qWarning( "Creating default image render service" );
            m_renderService.reset( new Carta::Core::ImageRenderService::Service() );
        }
    }

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
        }
        if ( ! m_wcsGridRenderer ) {
            qWarning( "wcsgrid: Creating dummy grid renderer" );
            m_wcsGridRenderer.reset( new Carta::Core::DummyGridRenderer() );
        }
    }

    // contour editor controller
    m_contourEditorController.reset( new Hacks::ContourEditorController( this,
                                                                         "/hacks/contourEditor/ce1" ) );
    connect( m_contourEditorController.get(),
             & ContourEditorController::updated,
             [&] () {
                 qDebug() << "contourEditorController updated";

//                 m_contourEditorController-> startRendering();
                 m_syncSvc-> startContour();

//                 requestImageAndGridUpdate();
             }
             );

//    connect( m_contourEditorController.get(),
//             & ContourEditorController::done,
//             [&] ( Carta::Lib::VectorGraphics::VGList vg, int64_t ) {
//                 qDebug() << "contour vg:" << vg.entries().size() << "entries";
//             }
//             );

    // create the synchronizer
//    m_igSync.reset( new ImageGridServiceSynchronizer(
//                        m_renderService, m_wcsGridRenderer, m_contourEditorController, this ) );
    m_syncSvc.reset( new ServiceSync(
                         m_renderService, m_wcsGridRenderer, m_contourEditorController, this ) );

    // connect its done() slot to our imageAndGridDoneSlot()
//    connect( m_igSync.get(), & ImageGridServiceSynchronizer::done,
//             this, & Me::imageAndGridDoneSlot );
    connect( m_syncSvc.get(), & ServiceSync::done,
             this, & Me::imageAndGridDoneSlot );

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

    auto ptrMoveCB = [&] ( CSR, CSR par ) -> void {
        auto coords = Impl::s2vd( par );
        if ( coords.size() >= 2 ) {
//            qDebug() << "ptrmove" << coords[0] << coords[1];
        }
    };

    m_connector-> addStateCallback( m_statePrefix + "/pointer-move",  ptrMoveCB );

    // connect movie timer to the frame advance slot
    connect( & m_movieTimer, & QTimer::timeout, this, & ImageViewController::loadNextFrame );

    // create a controller for grid options
    m_wcsGridOptionsController.reset(
        new WcsGridOptionsController(
            this,
            "/hacks/gridControls/c1",
            m_wcsGridRenderer
            ) );
    connect( m_wcsGridOptionsController.get(), & WcsGridOptionsController::updated,
             this, & Me::requestImageAndGridUpdate );

    // shared state stuff
    // ------------------
    namespace SS = Carta::Lib::SharedState;
    const SS::FullPath prefix = SS::FullPath::fromQString( m_statePrefix );

    // create a shared state var for the frame slider
    m_frameVar.reset( new SS::DoubleVar( prefix.with( "frameSlider" ) ) );
    connect( m_frameVar.get(), & SS::DoubleVar::valueChanged, this, & Me::frameVarCB );

    // create a shared state var for the grid toggle
    m_gridToggleVar.reset( new SS::BoolVar( prefix.with( "gridToggle" ) ) );
    connect( m_gridToggleVar.get(), & SS::BoolVar::valueChanged, this, & Me::gridToggleCB );

    // create a shared state var for the grid toggle
    m_playToggle.reset( new SS::BoolVar( prefix.with( "playToggle" ) ) );
    connect( m_playToggle.get(), & SS::BoolVar::valueChanged, this, & Me::playMovieToggleCB );
}

void
ImageViewController::playMovieToggleCB()
{
    if ( m_playToggle-> get() ) {
        loadNextFrame();
    }
    return;

    if ( m_playToggle-> get() ) {
        m_movieTimer.start( 1000 / 60 );
    }
    else {
        m_movieTimer.stop();
    }
}

void
ImageViewController::requestImageAndGridUpdate()
{
    // erase current grid
//    m_gridVG = Nullable < Carta::Lib::VectorGraphics::VGList > ();

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

//    m_igSync-> startAll();
    m_syncSvc-> startImage();
    m_syncSvc-> startGrid();
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

    // request repaint
    requestImageAndGridUpdate();

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

        // request repaint
//        m_renderService-> render( 0 );
        requestImageAndGridUpdate();
    }

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

//    m_renderService-> render( 0 );
//    requestImageAndGridUpdate();
    m_syncSvc-> startImage();
}

void
ImageViewController::setCmapReverse( bool flag )
{
    m_pixelPipeline-> setReverse( flag );
    m_renderService-> setPixelPipeline( m_pixelPipeline, m_pixelPipeline-> cacheId() );

//    m_renderService-> render( 0 );
//    requestImageAndGridUpdate();
    m_syncSvc-> startImage();
}

void
ImageViewController::setColormap( Carta::Lib::PixelPipeline::IColormapNamed::SharedPtr cmap )
{
    m_pixelPipeline-> setColormap( cmap );
    m_renderService-> setPixelPipeline( m_pixelPipeline, m_pixelPipeline-> cacheId() );

//    m_renderService-> render( 0 );
//    requestImageAndGridUpdate();
    m_syncSvc-> startImage();
}

void
ImageViewController::setPPCsettings( ImageViewController::PPCsettings settings )
{
    m_renderService-> setPixelPipelineCacheSettings( settings );

//    m_renderService-> render( 0 );
//    requestImageAndGridUpdate();
    m_syncSvc-> startImage();
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
    m_renderService-> setOutputSize( size );
    requestImageAndGridUpdate();
}

void
ImageViewController::viewRefreshed( qint64 id )
{
    qDebug() << "ImageViewController view" << name() << "refreshed" << id;
    if ( m_playToggle-> get() ) {
        loadNextFrame();
    }
}

void
ImageViewController::loadImage( QString fname )
{
    //    qDebug() << "xyz ImageViewController::loadImage" << fname;
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
    m_frameVar-> set( 0 );

    // hack for profile extraction, printing a profile to stderr
    std::vector < int > pos( m_astroImage-> dims().size(), 0 );
    Carta::Lib::Profiles::PrincipalAxisProfilePath path( m_astroImage-> dims().size()-1, pos );
    Carta::Lib::NdArray::RawViewInterface * rawView = m_astroImage-> getDataSlice( SliceND() );
    Carta::Lib::Profiles::ProfileExtractor * extractor = new Carta::Lib::Profiles::ProfileExtractor( rawView );
    auto profilecb = [ = ] () {
        auto data = extractor->getDataD();
        qDebug() << "profilecb"
                 << data.size()
                 << extractor-> getRawDataLength()
                 << extractor-> getTotalProfileLength()
        ;
        for(size_t i = 0 ; i < std::min(size_t(10),data.size()) ; i ++ ){
            qDebug() << "  :" << data[i];
        }
    };
    connect( extractor, & Carta::Lib::Profiles::ProfileExtractor::progress, profilecb );
    extractor-> start( path );
} // loadImage

void
ImageViewController::frameVarCB()
{
    qDebug() << "frameVar" << m_frameVar-> get();
    if ( m_astroImage-> dims().size() < 2 ) {
        return;
    }

    int nf = 1;
    if ( m_astroImage-> dims().size() > 2 ) {
        nf = m_astroImage-> dims()[2];
    }

    // convert from 0..999999 to integer frame number
    int frame = m_frameVar-> get() * nf / 1000000.0;

    // make sure frame integer is valid
    frame = Carta::Lib::clamp < int > ( frame, 0, nf - 1 );

    qDebug() << "current frame" << m_currentFrame << " frame=" << frame;

    // load the actual frame
    if ( frame != m_currentFrame ) {
        loadFrame( frame );
    }
} // frameVarCB

void
ImageViewController::gridToggleCB()
{
    m_wcsGridRenderer-> setEmptyGrid( ! m_gridToggleVar-> get() );
    m_syncSvc-> startGrid();
}

void
ImageViewController::loadFrame( int frame )
{
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

    // prepare slice description corresponding to the entire frame [:,:,frame,0,0,...0]
    auto frameSlice = SliceND().next();
    for ( size_t i = 2 ; i < m_astroImage->dims().size() ; i++ ) {
        frameSlice.next().index( i == 2 ? m_currentFrame : 0 );
    }

    // get a view of the data using the slice description and make a shared pointer out of it
    Carta::Lib::NdArray::RawViewInterface::SharedPtr view( m_astroImage-> getDataSlice( frameSlice ) );

    // compute 95% clip values, unless we already have them in the cache
    std::vector < double > clips = m_quantileCache[m_currentFrame];
    if ( clips.size() < 2 ) {
        Carta::Lib::NdArray::Double doubleView( view.get(), false );
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

    // tell the contour module about the changed input
    m_contourEditorController-> setInput( view );

    // if grid is active, request a grid rendering as well
    if ( m_wcsGridRenderer ) {
        m_wcsGridRenderer-> setInputImage( m_astroImage );
    }

    // update the GUI
    m_connector-> setState( m_statePrefix + "/frame", QString::number( m_currentFrame ) );

    // request repaint
    requestImageAndGridUpdate();
    m_syncSvc-> startContour();
} // loadFrame

void
ImageViewController::loadNextFrame()
{
    int nf = 1;
    if ( m_astroImage-> dims().size() > 2 ) {
        nf = m_astroImage-> dims()[2];
    }
    int currFrame = std::round( m_frameVar-> get() * nf / 1e6 );
    int nextFrame = ( currFrame + 1 ) % nf;

    qDebug() << "Setting next frame to" << nextFrame;
    m_frameVar-> set( ( nextFrame + 0.1 ) * 1e6 / nf );
} // loadFrame

void
ImageViewController::imageAndGridDoneSlot(
    QImage image,
    Carta::Lib::VectorGraphics::VGList gridVG,
    Carta::Lib::VectorGraphics::VGList contourVG,
    ServiceSync::JobId /*jobId*/ )
{
    /// \todo we should make sure the jobId matches the last submitted job, otherwise
    /// we are wasting CPU rendering old job...

//    qDebug() << "imageAndGridDoneSlot" << jobId << "xyz";
    m_renderBuffer = image;

    // draw the grid over top
    QTime t;
    t.restart();
    QPainter painter( & m_renderBuffer );
    painter.setRenderHint( QPainter::Antialiasing, true );
    Carta::Lib::VectorGraphics::VGListQPainterRenderer vgRenderer;
    if ( ! vgRenderer.render( gridVG, painter ) ) {
        qWarning() << "could not render grid vector graphics";
    }
    qDebug() << "Grid VG rendered in" << t.elapsed() / 1000.0 << "sec" << "xyz";

    // insert a matrix transform at the beginning
    Carta::Lib::VectorGraphics::VGComposer composer;
    t.restart();
    {
//        QPen lineColor( QColor( "red" ), 1 );
//        lineColor.setCosmetic( true );
//        painter.setPen( lineColor );

        // where does 0.5, 0.5 map to?
        QPointF p1 = m_renderService-> img2screen( { 0.5, 0.5 }
                                                   );

        // where does 1.5, 1.5 map to?
        QPointF p2 = m_renderService-> img2screen( { 1.5, 1.5 }
                                                   );
        QTransform tf;
        double m11 = p2.x() - p1.x();
        double m22 = p2.y() - p1.y();
        double m33 = 1; // no projection
        double m13 = 0; // no projection
        double m23 = 0; // no projection
        double m12 = 0; // no shearing
        double m21 = 0; // no shearing
        double m31 = p1.x() - m11 * 0.5;
        double m32 = p1.y() - m22 * 0.5;
        tf.setMatrix( m11, m12, m13, m21, m22, m23, m31, m32, m33 );

//        painter.setTransform( tf );
        composer.append < Carta::Lib::VectorGraphics::Entries::SetTransform > ( tf );
        composer.appendList( contourVG );
        contourVG = composer.vgList();
    }
    if ( ! vgRenderer.render( contourVG, painter ) ) {
        qWarning() << "could not render contour vector graphics";
    }
    qDebug() << "Contour VG rendered in" << t.elapsed() / 1000.0 << "sec" << "xyz";

//    // paint contours
//    QPen lineColor( QColor( "red" ), 1 );
//    lineColor.setCosmetic( true );
//    painter.setPen( lineColor );

//    // where does 0.5, 0.5 map to?
//    QPointF p1 = m_renderService-> img2screen( { 0.5, 0.5 }
//                                               );

//    // where does 1.5, 1.5 map to?
//    QPointF p2 = m_renderService-> img2screen( { 1.5, 1.5 }
//                                               );
//    QTransform tf;
//    double m11 = p2.x() - p1.x();
//    double m22 = p2.y() - p1.y();
//    double m33 = 1; // no projection
//    double m13 = 0; // no projection
//    double m23 = 0; // no projection
//    double m12 = 0; // no shearing
//    double m21 = 0; // no shearing
//    double m31 = p1.x() - m11 * 0.5;
//    double m32 = p1.y() - m22 * 0.5;
//    tf.setMatrix( m11, m12, m13, m21, m22, m23, m31, m32, m33 );
//    painter.setTransform( tf );

//    for ( size_t k = 0 ; k < m_contours.size() ; ++k ) {
//        std::vector < QPolygonF > con = m_contours[k];
//        for ( size_t i = 0 ; i < con.size() ; ++i ) {
//            QPolygonF & poly = con[i];
//            painter.drawPolyline( poly );
//        }
//    }

    // schedule a repaint with the connector
    m_connector-> refreshView( this );
} // imageAndGridDoneSlot
}
}
