/**
 * controleler for zoomable, pannable image view
 *
 * this is to replace most of HackViewer functionality, but hopefully in a more portable
 * manner. This class should only contain code related to the single view of an image.
 **/

#pragma once

#include "WcsGridOptionsController.h"
#include "CartaLib/CartaLib.h"
#include "CartaLib/PixelPipeline/CustomizablePixelPipeline.h"
#include "../ImageRenderService.h"
#include "../IView.h"
#include "CartaLib/Nullable.h"
#include "CartaLib/Hooks/GetWcsGridRenderer.h"
#include "common/Hacks/SharedState.h"
#include <QTimer>
#include <QObject>

namespace Hacks
{
/// grid render service that does nothing
class DummyGridRenderer
    : public Carta::Lib::IWcsGridRenderService
{
    Q_OBJECT
    CLASS_BOILERPLATE( DummyGridRenderer );

public:

    // IWcsGridRenderService interface

public:

    DummyGridRenderer()
        : Carta::Lib::IWcsGridRenderService()
    {
        m_timer.setSingleShot( true );
        connect( & m_timer, & QTimer::timeout,
                 this, & Me::reportResult );
    }

    virtual void
    setInputImage( Image::ImageInterface::SharedPtr ) override
    { }

    virtual void
    setImageRect( const QRectF & ) override
    { }

    virtual void
    setOutputSize( const QSize & ) override
    { }

    virtual void
    setOutputRect( const QRectF & ) override
    { }

    virtual void
    setPen( Element, const QPen & ) override
    { }

    virtual void
    setFont( Element, int, double ) override
    { }

    virtual void
    setGridDensityModifier( double ) override
    { }

    virtual void
    setGridLinesVisible( bool ) override
    {}

    virtual void
    setAxesVisible( bool ) override
    {}

    virtual void
    setInternalLabels( bool ) override
    { }

    virtual void
    setSkyCS( Carta::Lib::KnownSkyCS ) override
    { }

    virtual void
    setTicksVisible( bool ) override
    {}


    virtual JobId
    startRendering( JobId jobId = - 1 ) override
    {
        if ( jobId < 0 ) {
            m_jobId++;
        }
        else {
            m_jobId = jobId;
        }
        if ( ! m_timer.isActive() ) {
            m_timer.start( 1 );
        }
        return m_jobId;
    }

    virtual void
    setEmptyGrid( bool ) override
    { }

private slots:

    void
    reportResult()
    {
        emit done( Carta::Lib::VectorGraphics::VGList(), m_jobId );
    }

private:

    QTimer m_timer;
    JobId m_jobId = - 1;
};

/// an example of a class that is capable of synchronizing results coming from
/// ImageRenderService & IWcsGridRenderService
class ImageGridServiceSynchronizer : public QObject
{
    Q_OBJECT
    CLASS_BOILERPLATE( ImageGridServiceSynchronizer );

public:

    typedef int64_t JobId;
    typedef Carta::Core::ImageRenderService::Service IRS;
    typedef Carta::Lib::IWcsGridRenderService GRS;

    ImageGridServiceSynchronizer( IRS::Service::SharedPtr imageRendererService,
                                  GRS::SharedPtr gridRendererService,
                                  QObject * parent = nullptr )
        : QObject( parent )
    {
        if ( ! connect( imageRendererService.get(), & IRS::done, this, & Me::irsDoneSlot ) ) {
            qCritical() << "Could not connect imageRenderService done slot";
        }
        if ( ! connect( gridRendererService.get(), & GRS::done, this, & Me::wcsGridSlot ) ) {
            qCritical() << "Could not connect gridRenderService done slot";
        }

        m_irs = imageRendererService;
        m_grs = gridRendererService;
    }

//    JobId
//    listenFor( Carta::Core::ImageRenderService::JobId id1,
//               Carta::Lib::IWcsGridRenderService::JobId id2,
//               JobId jobId = - 1 )
//    {
//        qDebug() << "expect xyz" << id1 << id2;
//        m_irsJobId = id1;
//        m_grsJobId = id2;
//        m_irsDone = false;
//        m_grsDone = false;
//        if ( jobId < 0 ) {
//            m_myJobId++;
//        }
//        else {
//            m_myJobId = jobId;
//        }
//        return m_myJobId;
//    }

    JobId
    start( JobId jobId = - 1 )
    {
        qDebug() << "imageGridCombiner start() xyz";
        m_irsDone = false;
        m_grsDone = false;
        if ( jobId < 0 ) {
            m_myJobId++;
        }
        else {
            m_myJobId = jobId;
        }
        m_irsJobId = m_irs-> render();
        m_grsJobId = m_grs-> startRendering();
        qDebug() << "  waiting for xyz" << m_irsJobId << m_grsJobId << "as" << m_myJobId;
        return m_myJobId;
    }

    virtual
    ~ImageGridServiceSynchronizer() { }

signals:

    void
    done( QImage img, Carta::Lib::VectorGraphics::VGList, JobId );

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
        if ( m_grsDone ) {
            emit done( m_irsImage, m_grsVGList, m_myJobId );
        }
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
        if ( m_irsDone ) {
            emit done( m_irsImage, m_grsVGList, m_myJobId );
        }
    }

private:

    Carta::Core::ImageRenderService::JobId m_irsJobId = - 1;
    GRS::JobId m_grsJobId = - 1;

    JobId m_myJobId = - 1;
    bool m_irsDone = false;
    bool m_grsDone = false;
    QImage m_irsImage;
    Carta::Lib::VectorGraphics::VGList m_grsVGList;

    IRS::Service::SharedPtr m_irs = nullptr;
    GRS::SharedPtr m_grs = nullptr;
};

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

    void setColormap( Carta::Lib::PixelPipeline::IColormapNamed::SharedPtr );
    void
    setPPCsettings( PPCsettings settings );

    PPCsettings
    getPPCsettings();

signals:

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

    void
    playMovie( bool flag );

private slots:

//    /// slot for receiving updates from the rendering service
//    void
//    irsDoneSlot( QImage img, Carta::Core::ImageRenderService::JobId jobId );

//    /// slot for grid
//    void wcsGridSlot( Carta::Lib::VectorGraphics::VGList,
//                      Carta::Lib::IWcsGridRenderService::JobId jobId );

    /// combined image & grid slot
    void
    imageAndGridDoneSlot( QImage image,
                          Carta::Lib::VectorGraphics::VGList vgList,
                          ImageGridServiceSynchronizer::JobId jobId );

    /// schedule a grid update after pan/zoom were modified
    void
    requestImageAndGridUpdate();

private:

    /// when client changes zoom, this gets called
    QString
    zoomCB( const QString &, const QString & params, const QString & );

    /// when client changes pan, this gets called
    QString
    panCB( const QString &, const QString & params, const QString & );

    /// callback for frame state variable
    void
    frameVarCB();

    /// this is the part of the state we are allowed to use
    QString m_statePrefix;

    /// the name of the view
    QString m_viewName;

    /// our copy of the colormap
    Carta::Lib::PixelPipeline::CustomizablePixelPipeline::SharedPtr m_pixelPipeline;

    /// parts of pixel pipeline id
    struct {
        bool invert = false, reverse = false;
        QString cmap = "Gray";
        double clipMin = 0;
        double clipMax = 1;
    }
    m_ppCacheId;

    /// the image rendering service
    Carta::Core::ImageRenderService::Service::SharedPtr m_renderService;

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
    std::vector < std::vector < double > > m_quantileCache;

    /// current filename
    QString m_fileName;

    /// movie playing timer
    QTimer m_movieTimer;

    /// grid plotting
    bool m_gridToggle = false;

    /// results of grid drawing
    Nullable < Carta::Lib::VectorGraphics::VGList > m_gridVG;

    /// wcs grid render service
    Carta::Lib::IWcsGridRenderService::SharedPtr m_wcsGridRenderer = nullptr;

    /// wcs grid options controller
    WcsGridOptionsController::UniquePtr m_wcsGridOptionsController;

    /// state variable for current frame
    Carta::Lib::SharedState::DoubleVar::UniquePtr m_frameVar;

    /// current frame
    int m_currentFrame = - 1;

    /// image-and-grid-service result synchronizer
    ImageGridServiceSynchronizer::UniquePtr m_igSync = nullptr;
};
}
