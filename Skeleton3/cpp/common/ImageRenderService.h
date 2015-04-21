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
#include <QImage>
#include <QObject>
#include <QStringList>
#include <QCache>

namespace Carta
{
namespace Core
{
namespace ImageRenderService
{
/// shortcut for pixel pipeline type
typedef Lib::PixelPipeline::IPixelPipeline IPixelPipeline;
typedef Lib::PixelPipeline::IClippedPixelPipeline IClippedPixelPipeline;

/// job id
typedef int64_t JobId;

/*
///
/// \brief Describes rendering parameters for the image rendering service
///
/// any parameter left at default will be interpreted as 'use previous value'
///
struct Input
{
    /// pointer to raw view representing input
    NdArray::RawViewInterface::SharedPtr inputView = nullptr;

    /// string unique for the view, as it will be used by caching
    ///   it could be something like "/home/john/file.fits//0:20:10,::-1,1:20:2"
    QString inputViewCacheId = QString();

    /// pointer to the pixel pipeline to use
    PixelPipeline::SharedPtr pixelPipeline = nullptr;

    /// specifies coordinates of the data pixel to be centered in the generated
    ///   image, in zero-based image coordinates, e.g. (0,0) is bottom left corner of pixel
    ///   (0,0), while (1,1) is it's right-top corner
    Nullable < QPointF > panCenter;

    /// how big is the data pixel in the generated image, in pixels
    Nullable < double > pixelZoom;

    /// desired output size
    Nullable < QSize > outputSize;

    /// should pixel pipeline cache be used
    Nullable < bool > pixelPipelineCacheEnabled;

    /// how many entries to cache for pixel pipeline
    Nullable < int64_t > pixelPipelineCacheSize;

    /// whether pixel pipeline cache should use linear interpolation or not
    Nullable < bool > pixelPipelineCacheInterpolation;

    /// job id
    JobId jobId = 0;
};

*/

/// pixel pipeline cache settings
struct PixelPipelineCacheSettings {
    /// size of the cache (in entries, must be >= 2
    int size = 1000;

    /// whether caching is enabled or not
    bool enabled = true;

    /// whether interpolation is enabled or not
    bool interpolated = true;
};

/// Implementation of the rendering service
/// \warning this object could potentially live it a separate thread, so make all connections
/// to it as explicitly queued
/// \todo start refactoring the pure API of this to ready this for plugins...
class Service : public QObject
{
    CLASS_BOILERPLATE( Service );

    Q_OBJECT

public:

    ///
    /// \brief sets the input data (view) for rendering
    /// \param view pointer to the view
    /// \param cacheId unique id for this view, used for caching some information
    /// if not supplied, it will be assumed it's different from any views seen before (i.e.
    /// caching will not be used)
    ///
    void
    setInputView( NdArray::RawViewInterface::SharedPtr view, QString cacheId = QString() );

    ///
    /// \brief set the desired output size of the image
    /// \param size the size to output
    ///
    void
    setOutputSize( QSize size );

    ///
    /// \brief return the output size of the image
    ///
    QSize
    getOutputSize();

    /// set coordinates of the data pixel to be centered in the generated
    /// image, in zero-based image coordinates, e.g. (0,0) is bottom left corner of pixel
    /// (0,0), while (1,1) is it's right-top corner, and (1/2,1/2) is it's center
    void
    setPan( QPointF pt );

    /// getter for pan (see setPan())
    QPointF
    pan();

    /// specify zoom
    /// \param zoom how many screen pixels does a data pixel occupy on screen
    void
    setZoom( double zoom );

    /// return current zoom
    double
    zoom();

    /// \brief sets the pixel pipeline (non-cached) to be used to render the image
    /// \param pixelPipeline
    ///
    /// if pixel pipeline caching is enabled, the cache will be updated
    void
    setPixelPipeline( IClippedPixelPipeline::SharedPtr pixelPipeline, QString cacheId );

    /// set settings that control pixel pipeline cache
    void
    setPixelPipelineCacheSettings( const PixelPipelineCacheSettings & params );

    /// get the current settings for pixel pipeline cache
    const PixelPipelineCacheSettings &
    pixelPipelineCacheSettings() const;

    /// ask the service to render using the current settings and use the given
    /// jobId when notifying us of the results
    /// any previous rendering will most likely be canceled, unless the results
    /// have already been queued up for delivery
    void
    render( JobId jobId );

    /// get the last used render parameters
//    Input
//    lastJobParameters()
//    {
//        return m_currentInput;
//    }

    /// constructor
    explicit
    Service( QObject * parent = 0 );

    /// no copy constructor
    Service( const Service & ) = delete;

    /// no assignment
    Service &
    operator= ( const Service & ) = delete;

    /// destructor
    ~Service();

    /// convert image coordinates to screen coordinates
    /// \param p point to convert
    /// \return translated point
    ///
    QPointF
    img2screen( const QPointF & p );

    /// the inverse of img2screen()
    /// \param p point to convert
    /// \return translated point
    ///
    QPointF
    screen2img( const QPointF & p );

    /// save a copy of the full image at the desired scale (zoom level).
    bool
    saveFullImage( const QString& filename, double scale );

protected slots:

    /// internal helper, this will execute in our own thread
    void internalRenderSlot( JobId );

signals:

    /// emitted when job is done
    /// \warning connect to this using queued connection
    void done( QImage, JobId );

    /// emitted when job is still processing, but partial results are available
    /// \warning connect to this using queued connection
    void progress( QImage, JobId );

    /// emitted when job terminated due to some errors
    /// \warning connect to this using queued connection
    void error( QStringList, JobId );

    void internalRenderSignal( JobId );

private:

    // the following are rendering parameters
    NdArray::RawViewInterface::SharedPtr m_inputView = nullptr;
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
    Lib::PixelPipeline::CachedPipeline < true >::UniquePtr m_cachedPPinterp = nullptr;
    Lib::PixelPipeline::CachedPipeline < false >::UniquePtr m_cachedPP = nullptr;
    PixelPipelineCacheSettings m_pixelPipelineCacheSettings;

    /// here we store the whole frame rendered, it is essentially a cache to make
    /// pan/zoom to work faster
    QImage m_frameImage;

    /// render the frame if needed
    void _renderFrame();

    /// cache for individual frames (to make movie playing little bit faster)
    QCache < QString, QImage > m_frameCache;
};
}
}
}

//Q_DECLARE_METATYPE( Carta::Core::ImageRenderService::Input )

#endif // IMAGERENDERSERVICE_H
