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
    setServerSideVGRendering( bool flag);

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

    // IView interface

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
}
}
