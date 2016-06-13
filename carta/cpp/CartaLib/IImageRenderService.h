#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/IImage.h"
#include "CartaLib/PixelPipeline/IPixelPipeline.h"
#include <QObject>
#include <QImage>

namespace Carta
{
namespace Lib
{
/// API for image render service
///
/// This is what plugins would need to implement to offer image rendering service to the core.
class IImageRenderService : public QObject
{
    Q_OBJECT
    CLASS_BOILERPLATE( IImageRenderService );

public:

    typedef int64_t JobId;

    /// pixel pipeline cache settings
    struct PixelPipelineCacheSettings {
        /// size of the cache (in entries, must be >= 2
        int size = 1000;

        /// whether caching is enabled or not
        bool enabled = true;

        /// whether interpolation is enabled or not
        bool interpolated = true;
    };

    /// constructor
    IImageRenderService( QObject * parent = nullptr );

    /// virtual destructor
    virtual
    ~IImageRenderService();

    ///
    /// \brief sets the input data (view) for rendering
    /// \param view pointer to the view
    /// \param cacheId unique id for this view, used for caching some information
    /// if not supplied, it will be assumed it's different from any views seen before (i.e.
    /// caching will not be used)
    ///
    virtual void
    setInputView( NdArray::RawViewInterface::SharedPtr view,
                  QString cacheId = QString() ) = 0;

    ///
    /// \brief set the desired output size of the image
    /// \param size the size to output
    ///
    virtual void
    setOutputSize( QSize size ) = 0;

    ///
    /// \brief return the last output size requested
    /// \return
    ///
    virtual
    QSize
    outputSize() const = 0;

    /// set coordinates of the data pixel to be centered in the generated
    /// image, in zero-based image coordinates, e.g. (0,0) is bottom left corner of pixel
    /// (0,0), while (1,1) is it's right-top corner, and (1/2,1/2) is it's center
    virtual void
    setPan( QPointF pt ) = 0;

    /// getter for pan (see setPan())
    virtual QPointF
    pan() = 0;

    /// specify zoom
    /// \param zoom how many screen pixels does a data pixel occupy on screen
    virtual void
    setZoom( double zoom ) = 0;

    /// return current zoom
    virtual double
    zoom() = 0;

    /// \brief sets the pixel pipeline (non-cached) to be used to render the image
    /// \param pixelPipeline the pixel pipeline to use (this can be non-cached version)
    /// \param cacheId unique id for the pixel pipeline, in case pixel pipeline cache is
    /// enabled
    virtual void
    setPixelPipeline( PixelPipeline::IClippedPixelPipeline::SharedPtr pixelPipeline,
                      QString cacheId ) = 0;

    /// \brief set settings that control pixel pipeline cache
    /// \param params cache settings for pixel pipeline
    virtual void
    setPixelPipelineCacheSettings( const PixelPipelineCacheSettings & params ) = 0;

    /// get the current settings for pixel pipeline cache
    virtual const PixelPipelineCacheSettings &
    pixelPipelineCacheSettings() const = 0;

    /// convert image coordinates to screen coordinates
    /// \param p coordinates to convert
    /// \return converted coordinates
    virtual QPointF
    img2screen( const QPointF & p ) = 0;

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
            double zoom, const QSize& outputSize ) const = 0;


    /// the inverse of img2screen()
    /// \param p coordinates to convert
    /// \return converted coordinates
    virtual QPointF
    screen2img( const QPointF & p ) = 0;

    /**
     * Return the image point corresponding to the screen point.
     * @param p - a point in the image.
     * @param pan - the central point of the image.
     * @param zoom - the amount of zoom.
     * @param outputSize - the pixel size of the image.
     * @return - the corresponding point in image coordinates.
     */
    virtual QPointF
    screen2image( const QPointF & p, const QPointF& pan, double zoom, const QSize& size ) const = 0;

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
    render( JobId jobId = - 1 ) = 0;

signals:

    // note: signals are not virtual!

    /// emitted when job is done
    /// \warning connect to this using queued connection
    void done( QImage, JobId );

    /// emitted when job is still processing, but partial results are available
    /// \warning connect to this using queued connection
    void progress( QImage, JobId );

    /// emitted when job terminated due to some errors
    /// \warning connect to this using queued connection
    void error( QStringList, JobId );
};
}
}
