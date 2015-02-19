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

    // create a render service
    m_renderService.reset( new Carta::Core::ImageRenderService::Service() );
    connect( m_renderService.get(), & Carta::Core::ImageRenderService::Service::done,
             this, & ImageViewController::irsDoneSlot );

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

    // hook-up mini movie player slider
    typedef const QString & CSR;
    auto mmpSliderCB = [&] ( CSR, CSR par, CSR ) -> QString {
        auto frame = Impl::s2vd( par );
        if ( frame.size() > 0 && m_astroImage-> dims().size() > 2 ) {
            qDebug() << "Slider" << frame[0];
            loadFrame( frame[0] * m_astroImage-> dims()[2] );
        }
        return "";
    };
    m_connector-> addCommandCallback( m_statePrefix + "/setFrame", mmpSliderCB );

    // hook-up movie play button
    m_connector-> addStateCallback( m_statePrefix + "/playToggle", [&] ( CSR, CSR val ) {
                                        playMovie( val == "1" );
                                    }
                                    );

    /// connect movie timer to the frame advance slot
    connect( & m_movieTimer, & QTimer::timeout, this, & ImageViewController::loadNextFrame );
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

/// \todo implement zoom to center on supplied x,y
QString
ImageViewController::zoomCB( const QString &, const QString & params, const QString & )
{
    auto vals = Impl::s2vd( params );
    if ( vals.size() > 2 ) {
        // remember where the user clicked
        QPointF clickPtScreen( vals[0], vals[1]);
        QPointF clickPtImageOld = m_renderService-> screen2img (clickPtScreen);

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
        QPointF clickPtImageNew = m_renderService-> screen2img (clickPtScreen);

        // calculate the difference
        QPointF delta = clickPtImageOld - clickPtImageNew;

        // add the delta to the current center
        QPointF currCenter = m_renderService-> pan();
        m_renderService-> setPan( currCenter + delta);

        // tell the service to rerender
        m_renderService-> render( 0 );
    }

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

    return "";
} // zoomCB

ImageViewController::~ImageViewController()
{ }

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

    m_renderService-> render( 0 );
}

void
ImageViewController::handleResizeRequest( const QSize & size )
{
    m_renderService-> setOutputSize( size );
    m_renderService-> render( 0 );
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
    m_renderService-> setPan( { m_astroImage-> dims()[0] / 2.0, m_astroImage-> dims()[1] / 2.0 }
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
    if ( frame < 0 ) {
        frame = 0;
    }
    if ( m_astroImage-> dims().size() <= 2 ) {
        frame = 0;
    }
    else {
        frame = clamp( frame, 0, m_astroImage-> dims()[2] - 1 );
    }
    m_currentFrame = frame;

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
ImageViewController::irsDoneSlot( QImage img, Carta::Core::ImageRenderService::JobId jobId )
{
    Q_UNUSED( jobId );
    m_imageBuffer = img;
    m_connector-> refreshView( this );
}
}
