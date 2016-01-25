/**
 * Represents the data on a 2D plot as a set of (x,y) values connected
 * by a line.
 */
#pragma once
#include "Plot2D.h"
#include <qwt_plot_curve.h>
#include <QString>
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


class Plot2DProfile : public Plot2D, public QwtPlotCurve {
public:

    /**
     * Constructor.
     */
    Plot2DProfile();

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
    virtual ~Plot2DProfile();

protected:

    virtual void drawLines (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap,
            const QRectF &canvasRect, int from, int to) const;
private:

    std::vector<double> m_datasX;
    std::vector<double> m_datasY;

};

}
}
