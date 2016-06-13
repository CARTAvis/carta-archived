/**
 * The ImageRenderService::Service is responsible for rendering astro images.
 * Esentially it converts raw astro data -> RGB image, using an asynchronous API.
 *
 * Inputs:
 *   - raw data (view)
 *   - instance of pixel pipeline (can be slow, service will cache it)
 *   - pan/zoom
 *   - output size
 *
 * Output:
 *   - RGB image, via signal
 *
 *
 * caching considerations (internal notes)
 *   eg. when zooming/panning there is no need to re-apply colormap
 *   or when switching between frames, maybe we can cache some frames to make this faster
 *   or when looking at really large 2d data, we could use mipmaps (future optimization)
 *
 * asynchronous result reporting
 *   the render service might possibly live in a separate thread
 *
 * Note that the rendering service does not have any convenience APIs for manipulating
 * colormaps/pixel pipelines. It is up to the caller to set this up. The reason is to keep
 * the responisibilities to a minimum. Also, this class would not benefit from knowing the
 * internals of the pixel pipeline very much, as it's most likely going to cache it anyways.
 **/

#ifndef IMAGERENDERSERVICE_H
#define IMAGERENDERSERVICE_H

#include "CartaLib/IImage.h"
#include "CartaLib/PixelPipeline/IPixelPipeline.h"
#include "CartaLib/Nullable.h"
#include "CartaLib/IImageRenderService.h"
#include <QImage>
#include <QObject>
#include <QColor>
#include <QStringList>
#include <QCache>
#include <QTimer>

namespace Carta
{
namespace Core
{
/// \todo ImageRenderService namespace is probably not necessary after last cleanup
namespace ImageRenderService
{
/// shortcut for pixel pipeline type
typedef Lib::PixelPipeline::IPixelPipeline IPixelPipeline;
typedef Lib::PixelPipeline::IClippedPixelPipeline IClippedPixelPipeline;

/// job id
typedef int64_t JobId;

/// Implementation of the rendering service
/// \warning this object could potentially live it a separate thread, so make all connections
/// to it as explicitly queued
/// \note All pixel coordinates are in "casa-pixel" coordinates, which happens to be
/// the same as FITS standard, but numbering starts from 0 instead of 1.
///
/// For example:
/// (0,0) is the _CENTER_ of the first pixel (bottom-left pixel!!!)
/// (-1/2,-1/2) is the bottom left corner of the bottom left pixel
/// (1/2,1/2) is the top right corner of the bottom left pixel
class Service : public Carta::Lib::IImageRenderService
{
    CLASS_BOILERPLATE( Service );
    Q_OBJECT

public:

    typedef Carta::Lib::IImageRenderService::PixelPipelineCacheSettings PixelPipelineCacheSettings;

    /// constructor
    explicit
    Service( QObject * parent = 0 );

    /// disable copy constructor
    Service( const Service & ) = delete;

    /// disable assignment
    Service &
    operator= ( const Service & ) = delete;

    /// destructor
    virtual
    ~Service() override;

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

    //Set the color to use for nan values.
    //Note: this color will be ignored if we are using a default nan value from
    //the bottom of the color map.
    void
    setNanColor( QColor color );

    //Return the color that will be used to draw nan values.
    QColor
    getNanColor() const;

    //Set whether or not to use the default nan value (bottom of the color map).
    void
    setDefaultNan( bool useDefaultNan );

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
    screen2image( const QPointF & p, const QPointF& pan, double zoom, const QSize& size ) const override;

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
    IClippedPixelPipeline::SharedPtr m_pixelPipelineRaw = nullptr;

    /// current zoom
    double m_zoom = 1.0;

    /// current pan (coordinates of the image pixel that is to be centered on the screen)
    QPointF m_pan = QPointF( 0, 0 );

    // cached pipelines
    Lib::PixelPipeline::CachedPipeline < true >::UniquePtr m_cachedPPinterp = nullptr;
    Lib::PixelPipeline::CachedPipeline < false >::UniquePtr m_cachedPP = nullptr;
    PixelPipelineCacheSettings m_pixelPipelineCacheSettings;

    /// here we store the whole frame rendered, it is essentially a cache to make
    /// pan/zoom to work faster
    QImage m_frameImage;

    /// cache for individual frames (to make movie playing little bit faster)
    QCache < QString, QImage > m_frameCache;

    /// last requested job id
    JobId m_lastSubmittedJobId = - 1;

    /// Whether or not to use the default nan value (bottom of color map).
    bool m_defaultNan;
    /// User-settable color for nan values when the default nan value is not used.
    QColor m_nanColor;

    /// timer to make sure we only fire one render signal even if multiple requests
    /// are submitted
    QTimer m_renderTimer;

};
}
}
}

//Q_DECLARE_METATYPE( Carta::Core::ImageRenderService::Input )

#endif // IMAGERENDERSERVICE_H
