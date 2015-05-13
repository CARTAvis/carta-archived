/**
 * The ScriptedRenderService is responsible for rendering images for the scripted client.
 * It uses ImageRenderService internally
 **/

#ifndef SCRIPTEDRENDERSERVICE_H
#define SCRIPTEDRENDERSERVICE_H

#include "ImageRenderService.h"

namespace Carta {
    namespace Core {
        namespace ImageRenderService {
            class Service;
        }
    }
}

namespace Carta {
    namespace Lib {
        namespace PixelPipeline {
            class CustomizablePixelPipeline;
        }
    }
}

namespace Carta
{
namespace Core
{
namespace ScriptedClient
{

class ScriptedRenderService : public QObject
{
public:

    /// constructor
    /// \param savename the full path of the desired output image.
    /// \param m_image pointer to image interface.
    /// \param m_pixelPipeline pixel pipeline.
    /// \param filename the input FITS file
    explicit
    ScriptedRenderService( QString savename, std::shared_ptr<Image::ImageInterface> &m_image, std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> &m_pixelPipeline, QString filename, QObject * parent = 0 );

    /// specify zoom
    /// \param zoom how many screen pixels does a data pixel occupy on screen
    void
    setZoom( double zoom );

    /// destructor
    ~ScriptedRenderService();

    /// Prepares the image to be rendered using ImageRenderService. After the rendering
    /// has finished, the image is then saved to the location stored in m_outputFilename.
    bool saveFullImage();

private slots:

    /// Notification from the rendering service that a new image has been
    /// produced and is ready to be saved.
    /// [NOTE: the QImage save() method returns a boolean value, but at this
    /// point I am not sure how to return that value to DataSource or anywhere
    /// else.]
    void _readyToSave( QImage img );

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

    //Pointer to image interface.
    std::shared_ptr<Image::ImageInterface> m_imageCopy;

    ///pixel pipeline
    std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> m_pixelPipelineCopy;

};

}
}
}
#endif // SCRIPTEDRENDERSERVICE_H
