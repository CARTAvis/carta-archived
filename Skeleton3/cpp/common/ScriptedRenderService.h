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
    explicit
    ScriptedRenderService( QString savename, std::shared_ptr<Image::ImageInterface> &m_image, std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> &m_pixelPipeline, QString filename, QObject * parent = 0 );

    /// destructor
    ~ScriptedRenderService();

    bool saveFullImage();

private slots:

    //Notification from the rendering service that a new image has been produced.
    void _renderingDone( QImage img );

private:

    Carta::Core::ImageRenderService::Service *m_renderService; 

    void setInputView( int frameIndex, double minClipPercentile, double maxClipPercentile );

    QString m_fileName;
    QString m_savename;

    std::shared_ptr<Image::ImageInterface> m_imageCopy;

    ///pixel pipeline
    std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> m_pixelPipelineCopy;

};

}
}
}
#endif // SCRIPTEDRENDERSERVICE_H
