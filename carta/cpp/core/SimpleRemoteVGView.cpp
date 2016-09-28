/**
 *
 **/

#include "SimpleRemoteVGView.h"
#include "IConnector.h"
#include <QDebug>
#include <functional>

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

void
SimpleRemoteVGView::setRasterAndVG( const QImage & image, const Lib::IRemoteVGView::VGList & vglist )
{
    setRaster( image );
    setVG( vglist );
}

void
SimpleRemoteVGView::setVGrenderedOnServer( bool flag )
{
    /// \todo client side rendering is not implemented yet
    CARTA_ASSERT( flag == true );
    Q_UNUSED( flag );
}

bool
SimpleRemoteVGView::isVGrenderedOnServer()
{
    /// \todo only server side is supported at the moment
    return true;
}

qint64
SimpleRemoteVGView::scheduleRepaint( qint64 id )
{
    // indicate m_buffer needs to be repainted
    m_buffer = QImage();

    // remember the ID of the job
    if ( id == - 1 ) {
        id = m_lastRepaintId + 1;
    }
    m_lastRepaintId = id;

    // ask the connector for a repaint
    m_connector-> refreshView( this );

    // return the id
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

    using namespace std::placeholders;

    m_connector->addCommandCallback(
        QString( "vgview/inputEvent/%1" ).arg( viewName ),
        std::bind( & Me::inputEventCB, this, _1, _2, _3 ) );
}

void
SimpleRemoteVGView::registration( IConnector * connector )
{
    CARTA_ASSERT( connector == m_connector );
    Q_UNUSED( connector ); // get rid of warning in release build
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
    if ( m_buffer.isNull() ) {
        m_buffer = m_raster;
        QPainter painter( & m_buffer );
        if ( painter.isActive() ){
			Carta::Lib::VectorGraphics::VGListQPainterRenderer renderer;
			renderer.render( m_vgList, painter );
			painter.end();
        }
    }
    return m_buffer.size();
}

const QImage &
SimpleRemoteVGView::getBuffer()
{
    (void) size();
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

void
SimpleRemoteVGView::viewRefreshed( qint64 id )
{
    Q_UNUSED( id );
}

QString
SimpleRemoteVGView::inputEventCB( const QString & cmd,
                                  const QString & params,
                                  const QString & sessionId )
{
    Q_UNUSED( cmd );
    Q_UNUSED( sessionId );
    qDebug() << "Input event[" << this->m_viewName << "]:" << params;

//    emit inputEvent( Carta::Lib::InputEvent( Carta::Lib::InputEvent::Type::Custom, "tap"));
    emit inputEvent( Carta::Lib::InputEvent( QJsonDocument::fromJson( params.toLatin1() ).object() ) );
    return QString();
}
}
}
