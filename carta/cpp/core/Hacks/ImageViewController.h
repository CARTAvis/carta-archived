/**
 * controleler for zoomable, pannable image view
 *
 * this is to replace most of HackViewer functionality, but hopefully in a more portable
 * manner. This class should only contain code related to the single view of an image.
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/PixelPipeline/CustomizablePixelPipeline.h"
#include "CartaLib/Algorithms/ContourConrec.h"
#include "CartaLib/Nullable.h"
#include "CartaLib/Hooks/GetWcsGridRenderer.h"
#include "WcsGridOptionsController.h"
#include "ContourEditorController.h"
#include "../ImageRenderService.h"
#include "../IView.h"
#include "SharedState.h"
#include "core/DummyGridRenderer.h"
#include <QTimer>
#include <QObject>

namespace Carta
{
namespace Hacks
{


/// this class is responsible for synchronizing results from:
/// image render service
/// grid render service
/// contour controller
class ServiceSync : public QObject
{
    Q_OBJECT
    CLASS_BOILERPLATE( ServiceSync );

public:

    typedef int64_t JobId;
    //    typedef Carta::Core::ImageRenderService::Service IRS;
    typedef Carta::Lib::IImageRenderService IRS;
    typedef Carta::Lib::IWcsGridRenderService GRS;
    typedef ContourEditorController CEC;

//    ServiceSync( IRS::Service::SharedPtr imageRendererService,
//                 GRS::SharedPtr gridRendererService,
//                 CEC::SharedPtr contourController,
//                 QObject * parent = nullptr )
    ServiceSync( IRS::SharedPtr imageRendererService,
                 GRS::SharedPtr gridRendererService,
                 CEC::SharedPtr contourController,
                 QObject * parent = nullptr )
        : QObject( parent )
    {
        if ( ! connect( imageRendererService.get(), & IRS::done, this, & Me::irsDoneSlot ) ) {
            qCritical() << "Could not connect imageRenderService done slot";
        }
        if ( ! connect( gridRendererService.get(), & GRS::done, this, & Me::wcsGridSlot ) ) {
            qCritical() << "Could not connect gridRenderService done slot";
        }
        if ( ! connect( contourController.get(), & CEC::done, this, & Me::contourSlot ) ) {
            qCritical() << "Could not connect contour editor controller done slot";
        }

        m_irs = imageRendererService;
        m_grs = gridRendererService;
        m_cec = contourController;
    }

    JobId
    startAll()
    {
        startImage();
        startGrid();
        startContour();
        return m_myJobId;
    }

    JobId
    startImage()
    {
        m_irsDone = false;
        m_irsJobId = m_irs-> render();
        m_myJobId++;
        return m_myJobId;
    }

    JobId
    startGrid()
    {
        m_grsDone = false;
        m_grsJobId = m_grs-> startRendering();
        m_myJobId++;
        return m_myJobId;
    }

    JobId
    startContour()
    {
        m_cecDone = false;
        m_cecJobId = m_cec-> startRendering();
        m_myJobId++;
        return m_myJobId;
    }

    void
    checkAndEmit()
    {
        // emit done only if all three are finished
        if ( m_grsDone && m_irsDone && m_cecDone ) {
            emit done( m_irsImage, m_grsVGList, m_cecVGList, m_myJobId );
        }
    }

    virtual
    ~ServiceSync() { }

signals:

    /// this will be emitted when all services report done with their respective job ids
    void
    done( QImage img, Carta::Lib::VectorGraphics::VGList, Carta::Lib::VectorGraphics::VGList, JobId );

private slots:

    /// slot for receiving updates from the rendering service
    void
    irsDoneSlot( QImage img, Carta::Core::ImageRenderService::JobId jobId )
    {
        qDebug() << "irsDone xyz" << jobId;

        // if this is not the expected job, do nothing
        if ( jobId != m_irsJobId ) { return; }
        m_irsDone = true;
        m_irsImage = img;
        checkAndEmit();
    }

    /// slot for grid
    void
    wcsGridSlot( Carta::Lib::VectorGraphics::VGList vgList,
                 Carta::Lib::IWcsGridRenderService::JobId jobId )
    {
        qDebug() << "grsDone xyz" << jobId;

        // if this is not the expected job, do nothing
        if ( jobId != m_grsJobId ) { return; }
        m_grsDone = true;
        m_grsVGList = vgList;
        checkAndEmit();
    }

    void
    contourSlot( Carta::Lib::VectorGraphics::VGList vgList,
                 ContourEditorController::JobId jobId )
    {
        qDebug() << "contourDone xyz" << jobId;

        // if this is not the expected job, do nothing
        if ( jobId != m_cecJobId ) { return; }
        m_cecDone = true;
        m_cecVGList = vgList;
        checkAndEmit();
    }

private:

    Carta::Core::ImageRenderService::JobId m_irsJobId = - 1;
    GRS::JobId m_grsJobId = - 1;
    ContourEditorController::JobId m_cecJobId = - 1;

    JobId m_myJobId = - 1;
    bool m_irsDone = false;
    bool m_grsDone = false;
    bool m_cecDone = false;
    QImage m_irsImage;
    Carta::Lib::VectorGraphics::VGList m_grsVGList;
    Carta::Lib::VectorGraphics::VGList m_cecVGList;

//    IRS::Service::SharedPtr m_irs = nullptr;
    IRS::SharedPtr m_irs = nullptr;
    GRS::SharedPtr m_grs = nullptr;
    ContourEditorController::SharedPtr m_cec = nullptr;
};

/// image controller with grid overlays and contour overlays
class ImageViewController : public QObject, public IView
{
    Q_OBJECT
    CLASS_BOILERPLATE( ImageViewController );

public:

    typedef Carta::Lib::IImageRenderService::PixelPipelineCacheSettings PPCsettings;

    explicit
    ImageViewController( QString statePrefix, QString viewName, QObject * parent = 0 );

    ~ImageViewController();

    /// IView interface
    virtual void
    registration( IConnector * ) override { }

    /// IView interface
    virtual const QString &
    name() const override { return m_rawViewName; }

    /// IView interface
    virtual QSize
    size() override;

    /// IView interface
    virtual const QImage &
    getBuffer() override;

    /// IView interface
    virtual void
    handleResizeRequest( const QSize & size ) override;

    /// IView interface
    virtual void
    handleMouseEvent( const QMouseEvent & ) override { }

    /// IView interface
    virtual void
    handleKeyEvent( const QKeyEvent & ) override { }

    /// IView interface
    virtual void
    viewRefreshed( qint64 id);

    void setColormap( Carta::Lib::PixelPipeline::IColormapNamed::SharedPtr );
    void
    setPPCsettings( PPCsettings settings );

    PPCsettings
    getPPCsettings();

public slots:

    /// load the requested image from a local file
    void
    loadImage( QString filePath );

    void
    setCmapInvert( bool flag );

    void
    setCmapReverse( bool flag );

    void
    loadFrame( int frame );

    void
    loadNextFrame();

signals:

private slots:

    void
    playMovieToggleCB( );

    /// combined slot for results from image & grid & contour
    void
    imageAndGridDoneSlot( QImage image,
                          Carta::Lib::VectorGraphics::VGList gridVG,
                          Carta::Lib::VectorGraphics::VGList contourVG,
                          ServiceSync::JobId jobId );

    /// schedule a grid update after pan/zoom were modified
    void
    requestImageAndGridUpdate();

    /// when client changes zoom, this gets called
    QString
    zoomCB( const QString &, const QString & params, const QString & );

    /// when client changes pan, this gets called
    QString
    panCB( const QString &, const QString & params, const QString & );

    /// callback for m_frameVar shared state
    void
    frameVarCB();

    /// callback for m_gridToggleVar
    void
    gridToggleCB();

private:

    /// this is the part of the state we are allowed to use
    QString m_statePrefix;

    /// the name of the view
    QString m_viewName;

    /// our copy of the colormap
    Carta::Lib::PixelPipeline::CustomizablePixelPipeline::SharedPtr m_pixelPipeline;

    /// the image rendering service
    Carta::Lib::IImageRenderService::SharedPtr m_renderService;

//    Carta::Core::ImageRenderService::Service::SharedPtr m_renderService;

    /// pointer to connector
    IConnector * m_connector = nullptr;

    /// image rendering + grid rendering
    QImage m_renderBuffer;

    /// raw view name for the connector
    QString m_rawViewName;

    /// the loaded astro image
    Carta::Lib::Image::ImageInterface::SharedPtr m_astroImage = nullptr;

    /// clip cache, hard-coded to single quantile
    std::vector < std::vector < double > > m_quantileCache;

    /// current filename
    QString m_fileName;

    /// movie playing timer
    QTimer m_movieTimer;

    /// wcs grid render service
    Carta::Lib::IWcsGridRenderService::SharedPtr m_wcsGridRenderer = nullptr;

    /// wcs grid options controller
    WcsGridOptionsController::UniquePtr m_wcsGridOptionsController = nullptr;

    /// contour editor controller
    ContourEditorController::SharedPtr m_contourEditorController = nullptr;

    /// state variable for current frame
    Carta::Lib::SharedState::DoubleVar::UniquePtr m_frameVar = nullptr;

    /// state var for grid toggle
    Carta::Lib::SharedState::BoolVar::UniquePtr m_gridToggleVar = nullptr;

    /// state var for play movie toggle
    Carta::Lib::SharedState::BoolVar::UniquePtr m_playToggle = nullptr;

    /// current frame
    int m_currentFrame = - 1;

    /// image-and-grid-service result synchronizer
    ServiceSync::UniquePtr m_syncSvc = nullptr;
};
}
}
