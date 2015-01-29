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

namespace Hacks
{
class ImageViewController : public QObject, public IView
{
    Q_OBJECT

    CLASS_BOILERPLATE( ImageViewController );

public:

    explicit
    ImageViewController( QString statePrefix, QString viewName, QObject * parent = 0 );

    ~ImageViewController();

    /// IView interface
    virtual void registration(IConnector * ) override {}
    /// IView interface
    virtual const QString & name() const override { return m_rawViewName; }
    /// IView interface
    virtual QSize size() override { return m_imageBuffer.size(); }
    /// IView interface
    virtual const QImage & getBuffer() override { return m_imageBuffer; }
    /// IView interface
    virtual void handleResizeRequest(const QSize & size) override;
    /// IView interface
    virtual void handleMouseEvent(const QMouseEvent & ) override {}
    /// IView interface
    virtual void handleKeyEvent(const QKeyEvent & ) override {}

    void setColormap(Carta::Lib::PixelPipeline::IColormap::SharedPtr);

signals:

public slots:

    /// load the requested image from a local file
    void
    loadImage( QString filePath );

    void setCmapInvert(bool flag);
    void setCmapReverse(bool flag);

private slots:

    /// slot for receiving updates from the rendering service
    void irsDoneSlot( QImage img, Carta::Core::ImageRenderService::JobId jobId);

private:

    /// this is the part of the state we use
    QString m_statePrefix;

    /// the name of the view
    QString m_viewName;

    /// our copy of the colormap
    Carta::Lib::PixelPipeline::CustomizablePixelPipeline::SharedPtr m_pixelPipeline;

    /// the rendering service
    Carta::Core::ImageRenderService::Service::UniquePtr m_renderService;

    /// pointer to connector
    IConnector * m_connector = nullptr;

    /// buffered image store
    QImage m_imageBuffer;

    /// raw view name for the connector
    QString m_rawViewName;

    /// the loaded astro image
    Image::ImageInterface::SharedPtr m_astroImage = nullptr;


};
}
