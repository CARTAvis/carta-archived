/**
 * Initiates the work of computing the histogram in a separate process.
 **/

#pragma once

#include <memory>
#include "HistogramRenderRequest.h"
#include "CartaLib/Hooks/HistogramResult.h"

namespace Carta {
namespace Lib {

namespace Image {
class ImageInterface;
}
namespace Regions {
class RegionBase;
}
}

namespace Data{

class Region;

class HistogramRenderWorker{

public:

    /**
     * Constructor.
     */
    HistogramRenderWorker();

    /**
     * Store the parameters needed for computing the histogram.
     * @param request - a collection of parameters specifying how the histogram
     * 	should be computed.
     */
    void setParameters( const HistogramRenderRequest& request );

    /**
     * Performs the work of computing the histogram data in a separate process.
     * @return - the file descriptor for the pipe that can be used to read the
     *      histogram data.
     */
    int computeHist();

    /**
     * Destructor.
     */
    ~HistogramRenderWorker();

private:
    std::shared_ptr<Carta::Lib::Image::ImageInterface> m_dataSource;
    int m_binCount;
    int m_minChannel;
    int m_maxChannel;
    double m_minFrequency;
    double m_maxFrequency;
    QString m_rangeUnits;
    double m_minIntensity;
    double m_maxIntensity;
    QString m_fileName;
    QString m_regionId;
    Carta::Lib::Hooks::HistogramResult m_result;
    std::shared_ptr<Carta::Lib::Regions::RegionBase> m_region;

    HistogramRenderWorker( const HistogramRenderWorker& other);
    HistogramRenderWorker& operator=( const HistogramRenderWorker& other );
};
}
}



