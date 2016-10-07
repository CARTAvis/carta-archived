/**
 * Manages the production of histogram data from an image cube.
 **/

#pragma once
#include "HistogramRenderRequest.h"
#include "CartaLib/CartaLib.h"
#include "CartaLib/Hooks/HistogramResult.h"

#include <QQueue>
#include <memory>

namespace Carta {
namespace Lib {

namespace Image {
class ImageInterface;
}
namespace Regions {
	class RegionBase;
}
}
}

namespace Carta{
namespace Data{

class HistogramRenderWorker;
class HistogramRenderThread;

class HistogramRenderService : public QObject {
    Q_OBJECT

public:

    /**
     * Constructor.
     */
    explicit HistogramRenderService( QObject * parent = 0 );

    /**
     * Initiates the process of rendering the histogram.
     * @param request - a collection of parameters that specify how to render the histogram.
     */
    bool renderHistogram(const HistogramRenderRequest& request );

    /**
     * Destructor.
     */
    ~HistogramRenderService();

signals:

    /**
     * Notification that new histogram data has been computed.
     */
    void histogramResult( const Carta::Lib::Hooks::HistogramResult& result );

private slots:

    void _postResult( );

private:
    void _scheduleRender( const HistogramRenderRequest& request );
    HistogramRenderWorker* m_worker;
    HistogramRenderThread* m_renderThread;
    bool m_renderQueued;
    QQueue<HistogramRenderRequest> m_requests;

    HistogramRenderService( const HistogramRenderService& other);
    HistogramRenderService& operator=( const HistogramRenderService& other );
};
}
}



