#pragma once

#include "CartaLib/IImageRenderService.h"
#include "CartaLib/PixelPipeline/IPixelPipeline.h"

#include <QCache>
#include <QTimer>


class MyImageRenderService : public Carta::Lib::IImageRenderService
{
    CLASS_BOILERPLATE( MyImageRenderService );
    Q_OBJECT

public:

    typedef Carta::Lib::IImageRenderService::PixelPipelineCacheSettings PixelPipelineCacheSettings;
    typedef Carta::Lib::PixelPipeline::IClippedPixelPipeline IClippedPixelPipeline;

    /// constructor
    explicit
    MyImageRenderService( QObject * parent = 0 );

    /// disable copy constructor
    MyImageRenderService( const MyImageRenderService & ) = delete;

    /// disable assignment
    MyImageRenderService &
    operator= ( const MyImageRenderService & ) = delete;

    /// destructor
    virtual
    ~MyImageRenderService() override;

    ///
    /// \brief sets the input data (view) for rendering
    /// \param view pointer to the view
    /// \param cacheId unique id for this view, used for caching some information
    /// if not supplied, it will be assumed it's different from any views seen before (i.e.
    /// caching will not be used)
    ///
    virtual void
    setInputView( Carta::Lib::NdArray::RawViewInterface::SharedPtr view,
                  QString cacheId = QString() ) override;

    ///
    /// \brief set the desired output size of the image
    /// \param size the size to output
    ///
    virtual void
    setOutputSize( QSize size ) override;

    ///
    /// \brief return the last output size requested
    /// \return
    ///
    virtual QSize
    outputSize() const override;

    /// set coordinates of the data pixel to be centered in the generated
    /// image, in zero-based image coordinates, e.g. (0,0) is bottom left corner of pixel
    /// (0,0), while (1,1) is it's right-top corner, and (1/2,1/2) is it's center
    virtual void
    setPan( QPointF pt ) override;

    /// getter for pan (see setPan())
    virtual QPointF
    pan() override;

    /// specify zoom
    /// \param zoom how many screen pixels does a data pixel occupy on screen
    virtual void
    setZoom( double zoom ) override;

    /// return current zoom
    virtual double
    zoom() override;

    /// \brief sets the pixel pipeline (non-cached) to be used to render the image
    /// \param pixelPipeline
    ///
    /// if pixel pipeline caching is enabled, the cache will be updated
    virtual void
    setPixelPipeline( IClippedPixelPipeline::SharedPtr pixelPipeline,
                      QString cacheId ) override;

    /// set settings that control pixel pipeline cache
    virtual void
    setPixelPipelineCacheSettings( const PixelPipelineCacheSettings & params ) override;

    /// get the current settings for pixel pipeline cache
    virtual const PixelPipelineCacheSettings &
    pixelPipelineCacheSettings() const override;

    /// convert image coordinates to screen coordinates
    /// \param p coordinates to convert
    /// \return converted coordinates
    ///
    virtual QPointF
    img2screen( const QPointF & p ) override;

    /**
     * Return the screen point corresponding to the image point.
     * @param p - a point in the image.
     * @param pan - the central point of the image.
     * @param zoom - the amount of zoom.
     * @param outputSize - the pixel size of the image.
     * @return - the corresponding point in pixel coordinates.
     */
    virtual QPointF
    image2screen( const QPointF& p, const QPointF& pan,
            double zoom, const QSize& outputSize ) const override;

    /// the inverse of img2screen()
    /// \param p coordinates to convert
    /// \return converted coordinates
    ///
    virtual QPointF
    screen2img( const QPointF & p ) override;

    /**
     * Return the image point corresponding to the screen point.
     * @param p - a point in the image.
     * @param pan - the central point of the image.
     * @param zoom - the amount of zoom.
     * @param outputSize - the pixel size of the image.
     * @return - the corresponding point in image coordinates.
     */
    virtual QPointF
    screen2image( const QPointF & p, const QPointF& pan,
               double zoom, const QSize& outputSize ) const override;

public slots:

    /// ask the service to render using the current settings and use the given
    /// jobId when notifying us of the results
    /// any previous rendering will most likely be canceled, unless the results
    /// have already been queued up for delivery
    ///
    /// \param jobId id assigned to the rendering request, which will be reported back
    /// witht the done() signal, which can be used to make sure the arrived done() signal
    /// corresponds to the latest request. It should be a positive number. If unspecified
    /// (or negative number is supplied, a new id will be generated, which will
    /// the previous one + 1)
    ///
    /// \return the jobId to expect when the rendering is done (useful for unspecified
    /// jobId)
    virtual JobId
    render( JobId jobId = - 1 ) override;

protected slots:

    /// internal helper, this will execute in our own thread
    void
    internalRenderSlot();

private:

    // the following are rendering parameters
    Carta::Lib::NdArray::RawViewInterface::SharedPtr m_inputView = nullptr;
    QString m_inputViewCacheId;
    QString m_pixelPipelineCacheId;
    QSize m_outputSize = QSize( 10, 10 );

    /// instance of the pixel pipeline (very likely slow)
    IClippedPixelPipeline::SharedPtr m_pixelPipelineRaw;

    /// current zoom
    double m_zoom = 1.0;

    /// current pan (coordinates of the image pixel that is to be centered on the screen)
    QPointF m_pan = QPointF( 0, 0 );

    // cached pipelines
    Carta::Lib::PixelPipeline::CachedPipeline < true >::UniquePtr m_cachedPPinterp = nullptr;
    Carta::Lib::PixelPipeline::CachedPipeline < false >::UniquePtr m_cachedPP = nullptr;
    PixelPipelineCacheSettings m_pixelPipelineCacheSettings;

    /// here we store the whole frame rendered, it is essentially a cache to make
    /// pan/zoom to work faster
    QImage m_frameImage;

    /// cache for individual frames (to make movie playing little bit faster)
    QCache < QString, QImage > m_frameCache;

    /// last requested job id
    JobId m_lastSubmittedJobId = - 1;

    /// timer to make sure we only fire one render signal even if multiple requests
    /// are submitted
    QTimer m_renderTimer;
};
