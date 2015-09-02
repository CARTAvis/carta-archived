/**
 *
 **/

#include "DummyGridRenderer.h"

namespace Carta
{
namespace Core
{
DummyGridRenderer::DummyGridRenderer()
    : Carta::Lib::IWcsGridRenderService()
{
    m_timer.setSingleShot( true );
    connect( & m_timer, & QTimer::timeout,
             this, & Me::reportResult );
}

void
DummyGridRenderer::setInputImage( Image::ImageInterface::SharedPtr )
{ }

void
DummyGridRenderer::setImageRect( const QRectF & )
{ }

void
DummyGridRenderer::setOutputSize( const QSize & )
{ }

void
DummyGridRenderer::setOutputRect( const QRectF & )
{ }

void
DummyGridRenderer::setPen( Lib::IWcsGridRenderService::Element, const QPen & )
{ }

void
DummyGridRenderer::setFont( Lib::IWcsGridRenderService::Element, int, double )
{ }

void
DummyGridRenderer::setGridDensityModifier( double )
{ }

void
DummyGridRenderer::setGridLinesVisible( bool )
{ }

void
DummyGridRenderer::setAxesVisible( bool )
{ }

void
DummyGridRenderer::setInternalLabels( bool )
{ }

void
DummyGridRenderer::setSkyCS( Lib::KnownSkyCS )
{ }

void
DummyGridRenderer::setTicksVisible( bool )
{ }

Lib::IWcsGridRenderService::JobId
DummyGridRenderer::startRendering( Lib::IWcsGridRenderService::JobId jobId )
{
    if ( jobId < 0 ) {
        m_jobId++;
    }
    else {
        m_jobId = jobId;
    }
    if ( ! m_timer.isActive() ) {
        m_timer.start( 1 );
    }
    return m_jobId;
}

void
DummyGridRenderer::setEmptyGrid( bool )
{ }

void
DummyGridRenderer::reportResult()
{
    emit done( Carta::Lib::VectorGraphics::VGList(), m_jobId );
}
}
}
