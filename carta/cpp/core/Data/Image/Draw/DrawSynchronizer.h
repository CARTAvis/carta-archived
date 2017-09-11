/***
 * Synchronizes the image, contour, and grid drawing.
 */

#pragma once
#include <CartaLib/VectorGraphics/VGList.h>
#include <set>


namespace Carta {
namespace Lib {
    class IWcsGridRenderService;
    class IContourGeneratorService;
    class ContourSet;
    namespace VectorGraphics {
        class VGList;
    }
    namespace NdArray {
        class RawViewInterface;
    }
}

namespace Core {
    namespace ImageRenderService {
        class Service;
    }
}

namespace Data {

class DataContours;

class DrawSynchronizer : public QObject {

    Q_OBJECT

public:
    DrawSynchronizer( std::shared_ptr<Carta::Core::ImageRenderService::Service> imageRendererService,
            std::shared_ptr<Carta::Lib::IWcsGridRenderService> gridRendererService,
            QObject* parent = nullptr);

    /**
     * Sets the data to be used in calculating contours.
     * @param rawView - the data for calculating contours.
     */
    void setInput( std::shared_ptr<Carta::Lib::NdArray::RawViewInterface> rawView );

    /**
     * Sets the contour set(s) to be drawn.
     * @param contours - a set of contours to be drawn.
     */
    void setContours( const std::set<std::shared_ptr<DataContours> > & contours );

    /**
     * Store graphics for drawing regions in the image.
     * @param regionVGList - graphics for drawing regions.
     */
    void setRegionGraphics( const Carta::Lib::VectorGraphics::VGList& regionVGList );

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
            Carta::Lib::VectorGraphics::VGList, Carta::Lib::VectorGraphics::VGList, int64_t jobId );

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
    Carta::Lib::VectorGraphics::VGList m_regionVGList;

    std::shared_ptr<Carta::Core::ImageRenderService::Service> m_irs;
    std::shared_ptr<Carta::Lib::IWcsGridRenderService> m_grs;
    std::shared_ptr<Carta::Lib::IContourGeneratorService> m_cec;
    std::vector<QPen> m_pens;

    DrawSynchronizer( const DrawSynchronizer& other);
    DrawSynchronizer& operator=( const DrawSynchronizer& other );

};


}
}
