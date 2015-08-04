/**
 *
 **/

#include "VGView.h"
#include "IConnector.h"

namespace Carta
{
namespace Core
{
VGView::VGView( QString viewName, IConnector * connector, QObject * parent ) : QObject( parent )
{
    m_connector = connector;
    m_viewName = viewName;
    m_rasterImage = QImage();

    m_connector->registerView( this );
}

void
VGView::setServerSideVGRendering( bool flag )
{
    CARTA_ASSERT( flag == true );

    m_serverSideVG = flag;
}

qint64
VGView::scheduleRepaint( qint64 id )
{
    if ( id >= 0 ) {
        m_id = id;
    }
    else {
        m_id++;
    }

    return m_id;
}

void
VGView::registration( IConnector * connector )
{
    CARTA_ASSERT( connector = m_connector );
}

const QString &
VGView::name() const
{
    return m_viewName;
}

QSize
VGView::size()
{
    /// @todo probably incorrect
    return m_rasterImage.size();
}

const QImage &
VGView::getBuffer()
{
    return m_rasterImage;
}

void
VGView::handleResizeRequest( const QSize & /*size*/ )
{ }

void
VGView::handleMouseEvent( const QMouseEvent & /*event*/ )
{ }

void
VGView::handleKeyEvent( const QKeyEvent & /*event*/ )
{ }
}
}
