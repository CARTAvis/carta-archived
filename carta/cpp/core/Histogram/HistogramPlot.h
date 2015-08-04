/**
 * Represents the histogram data on a plot.
 */
#pragma once

#include <qwt_interval.h>
#include <qwt_plot_histogram.h>
#include <QRectF>
#include <QString>
#include <qwt_scale_map.h>
#include <memory>

namespace Carta {
    namespace Lib {
        namespace PixelPipeline {
            class CustomizablePixelPipeline;
        }
    }
}
class QPainter;

namespace Carta {
namespace Histogram {


class HistogramPlot : public QwtPlotHistogram {
	public:

        /**
         * Constructor.
         */
        HistogramPlot();
        /**
         * Store the data to be plotted.
         * @param data the histogram data.
         */
        void setData (const QVector< QwtIntervalSample > & data );

        /**
         * Set the draw style for the data (outline, filled, line).
         * @param style an identifier for a data drawing style.
         */
        void setDrawStyle( const QString& style );

        /**
         * Store the color map capable of mapping intensities to colors.
         * @param CMap an object capable of intensity to color mapping.
         */
        void setPipeline( std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> pipeline);

        /**
         * Set whether the histogram should be drawn in a single color or whether it should be
         * multi-colored based on intensity.
         * @param colored true for a histogram colored based on intensity; false otherwise.
         */
        void setColored( bool colored );

        /**
         * Destructor.
         */
        virtual ~HistogramPlot();

	protected:
        virtual void drawColumn (QPainter *, const QwtColumnRect &, const QwtIntervalSample &) const;

        virtual void drawSeries ( QPainter* painter, const QwtScaleMap& xMap, const QwtScaleMap& yMap,
                    const QRectF& canvasRect, int from, int to ) const;

     private:
       std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> m_pipeline;
	   QString m_drawStyle;
	   QColor m_defaultColor;
	   QBrush m_brush;
	   bool m_colored;
	   QVector< QwtIntervalSample > m_data;
	   mutable double m_lastY;
	   mutable double m_lastX;
};

}
}
