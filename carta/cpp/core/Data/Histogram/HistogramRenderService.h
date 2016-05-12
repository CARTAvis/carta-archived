/**
 * Manages the production of histogram data from an image cube.
 **/

#pragma once

#include <QObject>
#include <memory>
#include "CartaLib/CartaLib.h"
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
     * Returns the appropriate data for generating a histogram.
     * @return - the data needed for plotting a histogram.
     */
    Carta::Lib::Hooks::HistogramResult getResult() const;

    /**
     * Initiates the process of rendering thee histogram.
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
    bool renderHistogram(std::shared_ptr<Carta::Lib::Image::ImageInterface> dataSource,
            int binCount, int minChannel, int maxChannel, double minFrequency, double maxFrequency,
            const QString& rangeUnits, double minIntensity, double maxIntensity,
            const QString& fileName);

    /**
     * Destructor.
     */
    ~HistogramRenderService();

signals:

    /**
     * Notification that new histogram data has been computed.
     */
    void histogramResult( );

private slots:

    void _postResult( );

private:
    void _scheduleRender( std::shared_ptr<Carta::Lib::Image::ImageInterface> dataSource,
                int binCount, int minChannel, int maxChannel, double minFrequency, double maxFrequency,
                const QString& rangeUnits, double minIntensity, double maxIntensity,
                const QString& fileName);
    Carta::Lib::Hooks::HistogramResult m_result;
    HistogramRenderWorker* m_worker;
    HistogramRenderThread* m_renderThread;
    bool m_renderQueued;

    HistogramRenderService( const HistogramRenderService& other);
    HistogramRenderService& operator=( const HistogramRenderService& other );
};
}
}



