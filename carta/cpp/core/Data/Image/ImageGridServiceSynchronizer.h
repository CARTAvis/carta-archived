/***
 * Synchronizes the image rendering with the grid rendering.
 */

#pragma once
#include <CartaLib/VectorGraphics/VGList.h>

namespace Carta {
namespace Lib {
    class IWcsGridRenderService;
    namespace VectorGraphics {
        class VGList;
    }
}
namespace Core {
    namespace ImageRenderService {
        class Service;
    }
}

namespace Data {

class ImageGridServiceSynchronizer : public QObject {

    Q_OBJECT

public:
    ImageGridServiceSynchronizer( std::shared_ptr<Carta::Core::ImageRenderService::Service> imageRendererService,
            std::shared_ptr<Carta::Lib::IWcsGridRenderService> gridRendererService, QObject* parent = nullptr);

    /**
     * Start a synchronized rendering.
     * @param jobId - an identifier for the rendering task.
     */
    int64_t start( int64_t jobId = - 1 );

    virtual ~ImageGridServiceSynchronizer();

signals:
    void done( QImage img, Carta::Lib::VectorGraphics::VGList, int64_t jobId );

private slots:
    /// slot for receiving updates from the rendering service
    void irsDoneSlot( QImage img, int64_t jobId );
    /// slot for grid
    void wcsGridSlot( Carta::Lib::VectorGraphics::VGList vgList, int64_t jobId );

private:

    int64_t m_irsJobId = - 1;
    int64_t m_grsJobId = - 1;
    int64_t m_jobId = - 1;

    bool m_irsDone = false;
    bool m_grsDone = false;

    QImage m_irsImage;

    Carta::Lib::VectorGraphics::VGList m_grsVGList;

    std::shared_ptr<Carta::Core::ImageRenderService::Service> m_irs;
    std::shared_ptr<Carta::Lib::IWcsGridRenderService> m_grs;
};


}
}
