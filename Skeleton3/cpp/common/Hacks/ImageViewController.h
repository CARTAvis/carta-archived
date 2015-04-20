/**
 * controleler for zoomable, pannable image view
 *
 * this is to replace most of HackViewer functionality, but hopefully in a more portable
 * manner. This class should only contain code related to the single view of an image.
 **/

#pragma once

#include <QObject>
#include "CartaLib/CartaLib.h"
#include "CartaLib/PixelPipeline/CustomizablePixelPipeline.h"
#include "../ImageRenderService.h"
#include "../IView.h"
#include "CartaLib/Nullable.h"
#include "CartaLib/Hooks/DrawWcsGrid.h"
#include <QTimer>

namespace Hacks
{
class ImageViewController : public QObject, public IView
{
    Q_OBJECT
    CLASS_BOILERPLATE( ImageViewController );

public:

    typedef Carta::Core::ImageRenderService::PixelPipelineCacheSettings PPCsettings;

    explicit
    ImageViewController( QString statePrefix, QString viewName, QObject * parent = 0 );

    ~ImageViewController();

    /// IView interface
    virtual void registration(IConnector * ) override {}
    /// IView interface
    virtual const QString & name() const override { return m_rawViewName; }
    /// IView interface
    virtual QSize size() override;
    /// IView interface
    virtual const QImage & getBuffer() override;
    /// IView interface
    virtual void handleResizeRequest(const QSize & size) override;
    /// IView interface
    virtual void handleMouseEvent(const QMouseEvent & ) override {}
    /// IView interface
    virtual void handleKeyEvent(const QKeyEvent & ) override {}

    void setColormap(Carta::Lib::PixelPipeline::IColormapNamed::SharedPtr);
    void setPPCsettings( PPCsettings settings);
    PPCsettings getPPCsettings();

signals:

public slots:

    /// load the requested image from a local file
    void
    loadImage( QString filePath );

    void setCmapInvert(bool flag);
    void setCmapReverse(bool flag);

    void loadFrame(int frame);
    void loadNextFrame();

    void playMovie(bool flag);

private slots:

    /// slot for receiving updates from the rendering service
    void irsDoneSlot( QImage img, Carta::Core::ImageRenderService::JobId jobId);

    /// slot for grid
    void wcsGridSlot(Carta::Lib::VectorGraphics::VGList);

private:
    /// when client changes zoom, this gets called
    QString zoomCB(const QString &, const QString & params, const QString &);

    /// when client changes pan, this gets called
    QString panCB(const QString &, const QString & params, const QString &);

    /// combine image rendering and grid rendering, and schedule repaint
    void combineImageAndGrid();

    /// schedule a grid update after pan/zoom were modified
    void updateGridAfterPanZoom();

    /// this is the part of the state we use
    QString m_statePrefix;

    /// the name of the view
    QString m_viewName;

    /// our copy of the colormap
    Carta::Lib::PixelPipeline::CustomizablePixelPipeline::SharedPtr m_pixelPipeline;

    /// parts of pixel pipeline id
    struct {
        bool invert = false, reverse = false;
        QString cmap = "Gray";
        double clipMin = 0; double clipMax = 1;
    } m_ppCacheId;

    /// the rendering service
    Carta::Core::ImageRenderService::Service::UniquePtr m_renderService;

    /// pointer to connector
    IConnector * m_connector = nullptr;

    /// copy of the image rendering service result
    QImage m_renderedAstroImage;

    /// image rendering + grid rendering
    QImage m_renderBuffer;

    /// raw view name for the connector
    QString m_rawViewName;

    /// the loaded astro image
    Image::ImageInterface::SharedPtr m_astroImage = nullptr;

    /// clip cache, hard-coded to single quantile
    std::vector< std::vector<double> > m_quantileCache;

    /// current filename
    QString m_fileName;

    /// movie playing timer
    QTimer m_movieTimer;

    /// current 'frame'... for movie playing
    int m_currentFrame = 0;

    /// grid plotting
    bool m_gridToggle = false;

    /// results of grid drawing
    Nullable< Carta::Lib::VectorGraphics::VGList > m_gridVG;

    /// wcs grid renderer
    Carta::Lib::IWcsGridRenderer::SharedPtr m_wcsGridRenderer = nullptr;

};
}
