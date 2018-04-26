/**
 * Represents the data on a 2D plot in the format of (value, count) data.
 */
#pragma once
#include "Plot2D.h"
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
namespace Plot2D {


class Plot2DHistogram : public Plot2D, public QwtPlotHistogram {
public:

    /**
     * Constructor.
     */
    Plot2DHistogram();

    /**
     * Attach the data to the plot.
     * @param plot - the plot on which the data should be drawn.
     */
    virtual void attachToPlot( QwtPlot* plot ) Q_DECL_OVERRIDE;

    /**
     * Remove the data from the plot.
     * @param plot - the plot where the data should be removed.
     */
    virtual void detachFromPlot() Q_DECL_OVERRIDE;

    /**
      * Set the base y-vale for the plot.
      * @param val - the baseline for the plot.
      */
    virtual void setBaseLine( double val ) Q_DECL_OVERRIDE;

    /**
     * Store the data to be plotted.
     * @param data the plot data.
     */
    virtual void setData ( std::vector<std::pair<double,double> > data ) Q_DECL_OVERRIDE;

    /**
     * Destructor.
     */
    virtual ~Plot2DHistogram();

protected:
    virtual void drawColumn (QPainter *, const QwtColumnRect &, const QwtIntervalSample &) const Q_DECL_OVERRIDE;

    virtual void drawSeries ( QPainter* painter, const QwtScaleMap& xMap, const QwtScaleMap& yMap,
            const QRectF& canvasRect, int from, int to ) const Q_DECL_OVERRIDE;

private:

    QVector< QwtIntervalSample > m_data;
    mutable double m_lastY;
    mutable double m_lastX;
    Plot2DHistogram( const Plot2DHistogram& other);
    Plot2DHistogram& operator=( const Plot2DHistogram& other );
};

}
}
