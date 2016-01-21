/**
 * The ImageSaveService is responsible for saving images to a file.
 * It uses ImageRenderService internally
 **/

#ifndef IMAGESAVESERVICE_H
#define IMAGESAVESERVICE_H

#include "ImageRenderService.h"

#include <QObject>
#include <QSize>
#include <QImage>

#include <memory>

namespace Carta {
    namespace Lib {
        namespace PixelPipeline {
            class CustomizablePixelPipeline;
        }
        namespace Image {
            class ImageInterface;
        }
        namespace NdArray {
            class RawViewInterface;
        }
    }
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
    /// \param m_pixelPipeline pixel pipeline.
    explicit
    ImageSaveService( QString savename,
            std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> m_pixelPipeline,
            QObject * parent = 0 );

    ///
    /// \brief set the desired output size of the image
    /// \param size the size to output
    ///
    void
    setOutputSize( QSize size );

    /// set the scaling options if an output size is set.
    void setAspectRatioMode( Qt::AspectRatioMode mode );

    /// Set the number of frames in the horizontal and vertical display axes, respectively.
    /// \param dimAxis1 - the number of frames in the horizontal display axis.
    /// \param dimAxis2 - the number of frames in the vertical display axis.
    void setDisplayShape( int dimAxis1, int dimAxis2 );

    /// Set the data that represent a 2D view of the image along with an identifier for the
    /// data being displayed.
    /// \param view - the data
    /// \param viewId - an identifier for the data being displayed.
    void setInputView( std::shared_ptr<Carta::Lib::NdArray::RawViewInterface> view, const QString& viewId );

    /// specify zoom
    /// \param zoom how many screen pixels does a data pixel occupy on screen
    void
    setZoom( double zoom );

    /// destructor
    ~ImageSaveService();

    /// Prepares the image to be rendered using ImageRenderService. After the rendering
    /// has finished, the image is then saved to the location stored in m_outputFilename.
    /// The return value of the save attempt is passed asynchronously via
    /// _saveFullImageCB() once the image is saved, but this method returns true or false
    /// depending on whether an attempt is made to save the image.
    bool saveFullImage();

signals:

    /// Return the result of SaveFullImage() after the image has been rendered
    /// and a save attempt made.
    void saveImageResult( bool result );

private slots:

    /// Notification from the rendering service that a new image has been
    /// produced and is ready to be saved.
    void _saveFullImageCB( QImage img );

private:

    Carta::Core::ImageRenderService::Service::UniquePtr m_renderService = nullptr; 

    /**
     * Prepare the data for rendering.
     * It sets the input view, output size, pixel pipeline, and pan.
     * @return true if the input data was successfully set in the renderer; false otherwise.
     */
    bool _prepareData();

    /// Input data and id.
    std::shared_ptr<Carta::Lib::NdArray::RawViewInterface> m_inputView;
    QString m_inputViewId;

    /// Full path of the output image
    QString m_outputFilename;

    /// The size of the output image
    QSize m_outputSize;

    // Frame counts on the horizontal and vertical display axes.
    int m_inputXFrames;
    int m_inputYFrames;

    /// Determines how the output image will be scaled if an output size is set.
    Qt::AspectRatioMode m_aspectRatioMode;

    ///pixel pipeline
    std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> m_pixelPipelineCopy;
};
}
}
}

#endif // IMAGESAVESERVICE_H
