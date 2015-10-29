/**
 *
 **/

#include "SimpleRemoteVGView.h"
#include "IConnector.h"
#include <QDebug>

namespace Carta
{
namespace Core
{
const QSize &
SimpleRemoteVGView::getClientSize()
{
    return m_remoteSize;
}

const QString &
SimpleRemoteVGView::getRVGViewName()
{
    return m_viewName;
}

//void
//SimpleRemoteVGView::setRaster( const QColor & color )
//{
//    m_raster = QImage();
//    m_bgColor = color;
//}

void
SimpleRemoteVGView::setRaster( const QImage & image )
{
    m_raster = image;
}

void
SimpleRemoteVGView::setVG( const Lib::IRemoteVGView::VGList & vglist )
{
    m_vgList = vglist;
}

qint64
SimpleRemoteVGView::scheduleRepaint( qint64 id )
{
    m_buffer = m_raster;
    QPainter painter( & m_buffer);
    Carta::Lib::VectorGraphics::VGListQPainterRenderer renderer;
    renderer.render( m_vgList, painter);
    painter.end();

    if ( id == - 1 ) {
        id = m_lastRepaintId++;
    }
    m_lastRepaintId = id;
    m_connector-> refreshView( this);
    return id;
}

SimpleRemoteVGView::SimpleRemoteVGView( QObject * parent,
                                        QString viewName,
                                        IConnector * connector )
    : IRemoteVGView( parent )
{
    m_viewName = viewName;
    m_connector = connector;

    m_raster = QImage( 100, 100, QImage::Format_ARGB32_Premultiplied );
    m_raster.fill( 0xff000000 );

    m_connector-> registerView( this );
}

void
SimpleRemoteVGView::registration( IConnector * connector )
{
    CARTA_ASSERT( connector == m_connector );
    Q_UNUSED(connector); // get rid of warning in release build
    qDebug() << "VGView " << m_viewName << "registered";
}

const QString &
SimpleRemoteVGView::name() const
{
    return m_viewName;
}

QSize
SimpleRemoteVGView::size()
{
    return m_buffer.size();
}

const QImage &
SimpleRemoteVGView::getBuffer()
{
    return m_buffer;
}

void
SimpleRemoteVGView::handleResizeRequest( const QSize & size )
{
    m_remoteSize = size;
    emit sizeChanged();
}

void
SimpleRemoteVGView::handleMouseEvent( const QMouseEvent & /*event*/ )
{ }

void
SimpleRemoteVGView::handleKeyEvent( const QKeyEvent & /*event*/ )
{ }

void SimpleRemoteVGView::viewRefreshed(qint64 id)
{
    Q_UNUSED( id);
}
}
}
