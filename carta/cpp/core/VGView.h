/**
 *
 **/

#pragma once

class IConnector;
class QString;

#include "IView.h"
#include "CartaLib/VectorGraphics/VGList.h"
#include <QObject>

namespace Carta
{
namespace Core
{
/// a view implementation that does not need subclassing, and is capable
/// of rendering vector graphics
class VGView : public QObject, protected IView
{
    Q_OBJECT

public:

    VGView( QString viewName,
            IConnector * connector,
            QObject * parent = nullptr );

    /// return current size
    QSize
    getSize();

    /// get the name of the view
    QString
    getName();

    /// what image to render
    void
    setImage( const QImage & );

    /// what vg overlay to render
    void
    setVG( const Carta::Lib::VectorGraphics::VGList & );

    /// whether to render VG on server
    /// by default it's set to server side
    void
    setServerSideVGRendering( bool flag );

    virtual
    ~VGView() { }

signals:

    /// emitted when the client resizes the view
    void
    resized( const QSize & );

    /// emitted when client refreshed the view
    void
    refreshed( qint64 id );

public slots:

    /// schedule a refresh with a given ID
    qint64
    scheduleRepaint( qint64 id = - 1 );

private slots:

private:

    IConnector * m_connector = nullptr;
    qint64 m_id = - 1;
    QString m_viewName;
    bool m_serverSideVG = true;
    QImage m_rasterImage;

private:

    virtual void
    registration( IConnector * connector ) override;

    virtual const QString &
    name() const override;

    virtual QSize
    size() override;

    virtual const QImage &
    getBuffer() override;

    virtual void
    handleResizeRequest( const QSize & size ) override;

    virtual void
    handleMouseEvent( const QMouseEvent & event ) override;

    virtual void
    handleKeyEvent( const QKeyEvent & event ) override;
};


///
/// \brief The RemoteView class is an API specification for interacting with
/// graphical views displayed on client(s)
///
/// It offers the basic building block of rendering raster and/or vector graphics
/// on the client. The vector graphics is always layed over the raster graphics.
///
class IRemoteView
    : public QObject
{
    Q_OBJECT


public:

    typedef Carta::Lib::VectorGraphics::VGList VGList;

    /// returns size of the view in the master's UI
    virtual const QSize &
    size() = 0;

    /// returns the unique name of this view
    virtual QString
    name() = 0;

    /// sets the raster to be rendered in the view
    virtual void
    setRaster( const QColor & color ) = 0;

    virtual void
    setRaster( const QImage & image ) = 0;

    /// sets the VG to be rendered on top of the raster
    virtual void
    setVG( const VGList & vglist ) = 0;

public slots:

    /// schedule a refresh with a given ID
    /// this allows the caller to be notified when the latest painting has reached the client
    virtual qint64
    scheduleRepaint( qint64 id = - 1 ) = 0;

signals:

    /// this signal is emitted when the master UI changes the size of the view
    void
    sizeChanged();

    /// emitted when client repainted the view
    void
    repainted( qint64 id );
};

std::shared_ptr<IRemoteView> createRemoteView ( QString viewName);

class IQImageCombiner {
public:
    virtual void combine( QImage & src1dst, const QImage & src2) = 0;
};

class AlphaCombiner : public IQImageCombiner {

public:
    void setAlpha( double alpha) {
        CARTA_ASSERT( alpha >= 0.0 && alpha <= 1.0);
        m_alpha = alpha;
    }

    virtual void combine(QImage & src1dst, const QImage & src2) override
    {
        QPainter p( & src1dst);
        p.setOpacity( m_alpha);
        p.drawImage( 0, 0, src2);
    }
private:
    double m_alpha = 1.0;
};

class PixelMaskCombiner : public IQImageCombiner {
public:
    void setMask( quint32 mask = DefaultMask) {
        m_mask = mask;
    }
    void setAlpha( double alpha = 1.0) {
        CARTA_ASSERT( alpha >= 0.0 && alpha <= 1.0);
        m_alpha = alpha;
    }
    virtual void combine(QImage & src1dst, const QImage & src2) override
    {
        // if the mask is default, we do simple alpha composition
        if( m_mask == DefaultMask) {
            QPainter p( & src1dst);
            p.setOpacity( m_alpha);
            p.drawImage( 0, 0, src2);
            return;
        }
        // otherwise we need to
        QImage src22 = src2;
        if( src22.format() != QImage::Format_ARGB32) {
            src22 = src22.convertToFormat( QImage::Format_ARGB32);
        }
        for( int y = 0 ; y < src2.height() ; y ++) {
            unsigned char * chr = src22.scanLine( y);
            QRgb * ptr = (QRgb *) (chr);
            for( int x = 0 ; x < src22.width() ; x ++) {
                * ptr = ( * ptr) & m_mask;
                ptr ++;
            }
        }
        QPainter p( & src1dst);
        p.setOpacity( m_alpha);
        p.drawImage( 0, 0, src22);
    }

    static constexpr quint32 DefaultMask = 0xffffffff;

private:
    quint32 m_mask = DefaultMask;
    double m_alpha = 1.0;
};

}
}
