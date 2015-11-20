/**
 * Basic implementation of IRemoteVGView api. This class will be probably removed in the future,
 * once we decide to specialize it for desktop/server environments.
 **/

#pragma once

#include "CartaLib/IRemoteVGView.h"
#include "IView.h"
#include <QSize>
#include <QString>
#include <QColor>
#include <QImage>

class ServerConnector;
class DesktopConnector;
class IConnector;

namespace Carta
{
namespace Core
{

/// Basic implementation of IRemoteVGView api. We'll most likely replace this with
/// specialized desktop/server versions.

class SimpleRemoteVGView
    : public Carta::Lib::IRemoteVGView
      , public IView
{
    Q_OBJECT
    CLASS_BOILERPLATE( SimpleRemoteVGView );

public:

    virtual const QSize &
    getClientSize() override;

    virtual const QString &
    getRVGViewName() override;

    virtual void
    setRaster( const QImage & image ) override;

    virtual void
    setVG( const VGList & vglist ) override;

    virtual void
    setRasterAndVG( const QImage & image, const VGList & vglist ) override;

    virtual void
    setVGrenderedOnServer( bool flag) override;

    virtual bool
    isVGrenderedOnServer() override;

//    virtual void
//    enableInputEvent( Carta::Lib::InputEvent::Type type, QString name = QString()) override;

public slots:

    virtual qint64
    scheduleRepaint( qint64 id = - 1 ) override;

private:

    friend class ::DesktopConnector;
    friend class ::ServerConnector;

    SimpleRemoteVGView( QObject * parent, QString viewName, IConnector * connector );

    QSize m_remoteSize = QSize( 1, 1 );
    QString m_viewName;
    IConnector * m_connector = nullptr;
    QImage m_raster, m_buffer;

    VGList m_vgList;
    qint64 m_lastRepaintId = - 1;

    // IView interface

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

    virtual void
    viewRefreshed( qint64 id) override ;

    // for now this is how we handle input events... same for desktop and server
    QString inputEventCB( const QString & cmd, const QString & params, const QString & sessionId);

};
}
}
