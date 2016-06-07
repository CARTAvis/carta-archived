/**
 * Initiates the work of computing the histogram in a separate process.
 **/

#pragma once

#include <memory>
#include "CartaLib/Hooks/HistogramResult.h"

namespace Carta {
namespace Lib {


namespace Image {
class ImageInterface;
}
}
}

namespace Carta{
namespace Data{

class HistogramRenderWorker{

public:

    /**
     * Constructor.
     */
    HistogramRenderWorker();

    /**
     * Store the parameters needed for computing the histogram.
     * @param dataSource - the image that will bee the source of the histogram.
     * @param binCount - the number of bins the histogram should have.
     * @param minChannel - the minimum channel or -1 if there is no minimum.
     * @param maxChannel - the maximum channel or -1 if there is no maximum.
     * @param minFrequency - the minimum frequency.
     * @param maxFrequency - the maximum frequency.
     * @param rangeUnits - intensity units for the histogram.
     * @param minIntensity - minimum histogram intensity.
     * @param maxIntensity - maximum histogram intensity.
     * @param fileName - the file name.
     */
    bool setParameters(std::shared_ptr<Carta::Lib::Image::ImageInterface> dataSource,
            int binCount, int minChannel, int maxChannel, double minFrequency, double maxFrequency,
            const QString& rangeUnits, double minIntensity, double maxIntensity,
            const QString& fileName);

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
    Carta::Lib::Hooks::HistogramResult m_result;

    HistogramRenderWorker( const HistogramRenderWorker& other);
    HistogramRenderWorker& operator=( const HistogramRenderWorker& other );
};
}
}



