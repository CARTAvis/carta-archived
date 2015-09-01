/***
 * Synchronizes the image, contour, and grid drawing.
 */

#pragma once
#include <CartaLib/VectorGraphics/VGList.h>

namespace NdArray {
    class RawViewInterface;
}

namespace Carta {
namespace Lib {
    class IWcsGridRenderService;
    class IContourGeneratorService;
    class ContourSet;
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

class DrawSynchronizer : public QObject {

    Q_OBJECT

public:
    DrawSynchronizer( std::shared_ptr<Carta::Core::ImageRenderService::Service> imageRendererService,
            std::shared_ptr<Carta::Lib::IWcsGridRenderService> gridRendererService,
            std::shared_ptr<Carta::Lib::IContourGeneratorService> contourController,
            QObject* parent = nullptr);

    /**
     * Sets the data to be used in calculating contours.
     * @param rawView - the data for calculating contours.
     */
    void setInput( std::shared_ptr<NdArray::RawViewInterface> rawView );

    /**
     * Sets the pens to be used for drawing contours.
     * @param pens - the contour pens.
     */
    void setPens( std::vector<QPen> pens );

    /**
     * Start a synchronized rendering.
     * @param contourDraw - true if contours should be rendered; false otherwise.
     * @param gridDraw - true if the grid should be rendered; false otherwise.
     * @param jobId - an identifier for the rendering task.
     */
    int64_t start( bool contourDraw, bool gridDraw, int64_t jobId = - 1 );
    typedef Carta::Lib::ContourSet Result;
    virtual ~DrawSynchronizer();

signals:
    /**
     * Signal that all the renderers have finished.
     */
    void done( QImage img, Carta::Lib::VectorGraphics::VGList,
            Carta::Lib::VectorGraphics::VGList, int64_t jobId );

private slots:
    //Callback for the image rendering service.
    void _irsDone( QImage img, int64_t jobId );
    //Callback for grids
    void _wcsGridDone( Carta::Lib::VectorGraphics::VGList vgList, int64_t jobId );
    //Callback for contours
    void _contourDone( const Result & result, int64_t jobId);

private:

    void _checkAndEmit();

    int64_t m_irsJobId = - 1;
    int64_t m_grsJobId = - 1;
    int64_t m_cecJobId = -1;
    int64_t m_jobId = - 1;

    bool m_irsDone = false;
    bool m_grsDone = false;
    bool m_cecDone = false;

    QImage m_irsImage;

    Carta::Lib::VectorGraphics::VGList m_grsVGList;
    Carta::Lib::VectorGraphics::VGList m_cecVGList;

    std::shared_ptr<Carta::Core::ImageRenderService::Service> m_irs;
    std::shared_ptr<Carta::Lib::IWcsGridRenderService> m_grs;
    std::shared_ptr<Carta::Lib::IContourGeneratorService> m_cec;
    std::vector<QPen> m_pens;
};


}
}
