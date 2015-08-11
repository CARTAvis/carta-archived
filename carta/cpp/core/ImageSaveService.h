/**
 * The ImageSaveService is responsible for saving images to a file.
 * It uses ImageRenderService internally
 **/

#ifndef IMAGESAVESERVICE_H
#define IMAGESAVESERVICE_H

#include <QObject>
#include <QSize>
#include <QImage>

#include <memory>

namespace Carta {
    namespace Core {
        namespace ImageRenderService {
            class Service;
        }
    }
    namespace Lib {
        namespace PixelPipeline {
            class CustomizablePixelPipeline;
        }
    }
}
namespace Image {
    class ImageInterface;
}

namespace Carta{
namespace Core{
namespace ImageSaveService{

class ImageSaveService : public QObject
{
    Q_OBJECT

public:

    /// constructor
    /// \param savename the full path of the desired output image.
    /// \param m_image pointer to image interface.
    /// \param m_pixelPipeline pixel pipeline.
    /// \param filename the input FITS file
    explicit
    ImageSaveService( QString savename, std::shared_ptr<Image::ImageInterface> &m_image, std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> &m_pixelPipeline, QString filename, QObject * parent = 0 );

    ///
    /// \brief set the desired output size of the image
    /// \param size the size to output
    ///
    void
    setOutputSize( QSize size );

    ///
    /// \brief set the desired frame index/channel of the image
    /// \param index the frame index/channel
    ///
    void
    setFrameIndex( int index );

    /// set the scaling options if an output size is set.
    void setAspectRatioMode( Qt::AspectRatioMode mode );

    /// specify zoom
    /// \param zoom how many screen pixels does a data pixel occupy on screen
    void
    setZoom( double zoom );

    /// destructor
    ~ImageSaveService();

    /// Prepares the image to be rendered using ImageRenderService. After the rendering
    /// has finished, the image is then saved to the location stored in m_outputFilename.
    /// The return value of the save attempt is passed asynchronously via
    /// _saveFullImageCB().
    void saveFullImage();

signals:

    /// Return the result of SaveFullImage() after the image has been rendered
    /// and a save attempt made.
    void saveImageResult( bool result );

private slots:

    /// Notification from the rendering service that a new image has been
    /// produced and is ready to be saved.
    void _saveFullImageCB( QImage img );

private:

    Carta::Core::ImageRenderService::Service *m_renderService; 

    /**
     * Prepare the data for rendering.
     * Most of this code is borrowed from void DataSource::load().
     * It sets the input view, output size, pixel pipeline, and pan.
     * @param frameIndex the index of the spectral coordinate to load.
     * @param clipMinPercentile the minimum clip value.
     * @param clipMaxPercentile the maximum clip value.
     */
    void _prepareData( int frameIndex, double minClipPercentile, double maxClipPercentile );

    /// The input FITS file
    QString m_inputFilename;

    /// Full path of the output image
    QString m_outputFilename;

    /// The size of the output image
    QSize m_outputSize;

    /// The frame of the output image
    int m_frameIndex;

    /// Determines how the output image will be scaled if an output size is set.
    Qt::AspectRatioMode m_aspectRatioMode;

    //Pointer to image interface.
    std::shared_ptr<Image::ImageInterface> m_imageCopy;

    ///pixel pipeline
    std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> m_pixelPipelineCopy;
};
}
}
}

#endif // IMAGESAVESERVICE_H
