/**
 *
 **/

#include "ImageViewController.h"
#include "../Globals.h"
#include "../IConnector.h"
#include "../PluginManager.h"
#include "CartaLib/Hooks/LoadAstroImage.h"

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

    // initialize colormap
    m_pixelPipeline = std::make_shared < Carta::Lib::PixelPipeline::CustomizablePixelPipeline > ();

    // create a render service
    m_renderService.reset( new Carta::Core::ImageRenderService::Service() );
    connect( m_renderService.get(), & Carta::Core::ImageRenderService::Service::done,
             this, & ImageViewController::irsDoneSlot );

    // register with connector as a view
    m_connector-> registerView( this );
}

ImageViewController::~ImageViewController()
{ }

void
ImageViewController::setCmapInvert( bool flag )
{
    m_pixelPipeline-> setInvert( flag );
    m_renderService-> setPixelPipeline( m_pixelPipeline );
    m_renderService-> render( 0 );
}

void
ImageViewController::setCmapReverse( bool flag )
{
    m_pixelPipeline-> setReverse( flag );
    m_renderService-> setPixelPipeline( m_pixelPipeline );
    m_renderService-> render( 0 );
}

void
ImageViewController::setColormap( Carta::Lib::PixelPipeline::IColormap::SharedPtr cmap )
{
    m_pixelPipeline-> setColormap( cmap );
    m_renderService-> setPixelPipeline( m_pixelPipeline );
    m_renderService-> render( 0 );
}

void
ImageViewController::handleResizeRequest( const QSize & size )
{
    qDebug() << "IVC resize" << size;
    m_renderService-> setOutputSize( size );
    m_renderService-> render( 0 );
}

void
ImageViewController::loadImage( QString fname )
{
//    Carta::Core::ImageRenderService::Input jobParams;

    qDebug() << "loadImage() Trying to load astroImage...";
    auto res =
        Globals::instance()-> pluginManager()
            -> prepare < Carta::Lib::Hooks::LoadAstroImage > ( fname ).first();
    if ( res.isNull() ) {
        qWarning( "Could not find any plugin to load astroImage" );
    }
    m_astroImage = res.val();

    // prepare slice description corresponding to the entire frame [:,:,frame,0,0,...0]
    int m_currentFrame = 0;
    auto frameSlice = SliceND().next();
    for ( size_t i = 2 ; i < m_astroImage->dims().size() ; i++ ) {
        frameSlice.next().index( i == 2 ? m_currentFrame : 0 );
    }

    // get a view of the data using the slice description and make a shared pointer out of it
    NdArray::RawViewInterface::SharedPtr view( m_astroImage-> getDataSlice( frameSlice ) );

    // tell the render service to render this job
    m_renderService-> setInputView( view );
    m_renderService-> render( 0 );
} // loadImage

void
ImageViewController::irsDoneSlot( QImage img, Carta::Core::ImageRenderService::JobId jobId )
{
    qDebug() << "ivc receive" << img.size();
    Q_UNUSED( jobId );
    m_imageBuffer = img;
    m_connector-> refreshView( this );
}
}
