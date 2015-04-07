/**
 * Pavol's experiments
 */

#pragma once

#include "../Algorithms/RawView2QImageConverter.h"
#include "../IConnector.h"
#include "../PluginManager.h"
#include "../IPlatform.h"
#include "CartaLib/IImage.h"
#include "ImageViewController.h"

#include <QPixmap>
#include <QObject>

namespace Hacks
{

class TestView2 : public QObject, public IView
{
    Q_OBJECT

public:

    TestView2( QString prefix,
               QString viewName,
               QColor bgColor,
               Image::ImageInterface::SharedPtr astroImage );

    // image to be rendered:
    QImage m_qImageToRender;

    void
    scheduleRedraw();

    virtual void
    registration( IConnector * connector ) override;

    virtual const QString &
    name() const override;

    virtual QSize
    size() override;

    virtual const QImage &
    getBuffer() override;

    QImage &
    getBufferRW() { return m_imageBuffer; }

    virtual void
    handleResizeRequest( const QSize & pSize ) override;

    virtual void
    handleMouseEvent( const QMouseEvent & ev ) override; // handleMouseEvent

    virtual void
    handleKeyEvent( const QKeyEvent & /*event*/ ) override
    { }

signals:

    void
    resized();

    void
    mouseX( double );

protected:

    void
    redrawBuffer(); // redrawBuffer

    QColor m_bgColor;
    Image::ImageInterface::SharedPtr m_astroImage;
    IConnector * m_connector = nullptr;
    QImage m_imageBuffer;
    QString m_viewName, m_prefix;
    QPointF m_lastMouse;
};

///
/// \brief The HackView class contains Pavol's experiments.
///
/// The purpose of this class is to keep the Viewer class code as clean as possible.
///
class HackViewer : public QObject
{
    Q_OBJECT

    CLASS_BOILERPLATE( HackViewer);

public:

    /// constructor
    /// should be called when platform is initialized, but connector isn't
    explicit
    HackViewer( QString prefix = "/hacks" );

    /// this should be called when connector is already initialized (i.e. it's
    /// safe to start setting/getting state)
    void
    start();

protected slots:

    /// this does the actual frame reload
    void
    _reloadFrameNow();

    void
    _repaintFrameNow();

protected:

    // prefixed version of set state
    void
    setState( const QString & path,  const QString & value );

    // prefixed version of getState
    QString
    getState( const QString & path );

    // prefixed version of addStateCallback
    IConnector::CallbackID
    addStateCallback( QString path, IConnector::StateChangedCallback cb );

    /// pointer to the loaded image
    Image::ImageInterface::SharedPtr m_astroImage = nullptr;

    /// are we recomputing clip on frame change?
    bool m_clipRecompute = true;

    /// pointer to the rendering algorithm
    Carta::Core::RawView2QImageConverter3::UniquePtr m_rawView2QImageConverter;

    /// current frame
    int m_currentFrame = - 1;

    /// pointer to connector
    IConnector * m_connector;

    /// coordinate formatter
    CoordinateFormatterInterface::SharedPtr m_coordinateFormatter;

    /// our own part of the state
    QString m_statePrefix;

    /// options for controlling the pixel pipeline (cmap)
    bool m_cmapUseCaching = true;
    bool m_cmapUseInterpolatedCaching = true;
    int m_cmapCacheSize = 1000;

    /// reload frame timer
    bool m_reloadFrameQueued = false;
    bool m_repaintFrameQueued = false;

    /// schedules a reload of the current frame
    void
    scheduleFrameReload();

    void
    scheduleFrameRepaint();

    /// how big (in screen pixels) is a single data pixel
    double m_pixelZoom = 1.0;
    /// coordinates of data pixel that is centered on the screen
    QPointF m_centeredImagePoint { 1, 1 };
    /// current frame rendering (entire frame)
    QImage m_wholeImage;

    Hacks::TestView2 * m_testView2 = nullptr;

    QPointF img2screen(QPointF p);
    QPointF screen2img(QPointF p);

    // view controller with the new render service
    Hacks::ImageViewController::UniquePtr m_imageViewController;
    std::vector < Carta::Lib::PixelPipeline::IColormapNamed::SharedPtr > m_allColormaps;

};
}
