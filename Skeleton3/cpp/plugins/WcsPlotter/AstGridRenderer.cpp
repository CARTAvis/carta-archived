/**
 *
 **/

#include "AstGridRenderer.h"
#include <QPainter>

namespace WcsPlotterPluginNS
{
AstGridRenderer::AstGridRenderer() : IWcsGridRenderer()
{
    qDebug() << "creating ast grid renderer";

    QImage img( 10, 10, QImage::Format_ARGB32_Premultiplied);
    img.fill( QColor( 0, 0, 0, 0));
    m_vgList.setQImage( img);
}

AstGridRenderer::~AstGridRenderer() { }

void
AstGridRenderer::setInputImage( Image::ImageInterface::SharedPtr image )
{
    m_iimage = image;
}

void
AstGridRenderer::setOutputSize( const QSize & size )
{
    QImage img( size, QImage::Format_ARGB32_Premultiplied);
    img.fill( QColor( 0, 0, 0, 0));

    QPainter p( & img);
    p.setPen( QPen( QColor("yellow"), 20));
    p.setBrush( QColor( 255,0,0,128));
    p.drawEllipse( img.rect());
    m_vgList.setQImage( img);
}

void
AstGridRenderer::setImageRect( const QRectF & rect )
{
    Q_UNUSED( rect );
}

void
AstGridRenderer::setOutputRect( const QRectF & rect )
{
    Q_UNUSED( rect );
}

void
AstGridRenderer::setLineColor( QColor color )
{
    Q_UNUSED( color );
}

void
AstGridRenderer::startRendering()
{
    emit done( m_vgList );
}
}
