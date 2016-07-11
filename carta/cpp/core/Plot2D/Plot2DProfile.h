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
     * Returns the data point closest to the target point.
     * @param targetX - the x-coordinate of the target point.
     * @param targetY - the y-coordinate of the target point.
     * @param xError - set to the error in the x-direction.
     * @param yError - set to the error in the y-direction.
     * @return - the data point closest to the target point.
     */
    virtual std::pair<double,double> getClosestPoint( double targetX, double targetY,
            double* xError, double* yError ) const Q_DECL_OVERRIDE;

    /**
     * Return a custom icon to use for the legend item.
     * @param index - unused.
     * @param size - the size of the icon.
     * @return - a custom icon for the legend item.
     */
    virtual QwtGraphic legendIcon( int index, const QSizeF& size ) const;

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
     * Set the draw style for the data (continuous, step, etc).
     * @param style - an identifier for a draw style.
     */
    virtual void setDrawStyle( const QString& style );

    /**
     * Set an identifier for this data set.
     * @param id - an identifier for this data set.
     */
    virtual void setId( const QString& id );

    /**
     * Set whether or not to display a sample line with the legend item.
     * @param showLegendLine - true to display a sample line; false, otherwise.
     */
    virtual void setLegendLine( bool showLegendLine ) Q_DECL_OVERRIDE;

    /**
     * Turn on/off the legend for this data item.
     * @param visible - true to see a corresponding legend for this data; false, to
     *  not display a legend for this data.
     */
    virtual void setLegendVisible( bool visible ) Q_DECL_OVERRIDE;

    /**
     * Destructor.
     */
    virtual ~Plot2DProfile();

protected:

    virtual void drawLines (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap,
            const QRectF &canvasRect, int from, int to) const;
    virtual void drawSteps (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap,
            const QRectF &canvasRect, int from, int to) const;
    //This method was put in so that profiles consisting of a single point could be drawn.
    void drawSymbol( QPainter* painter, const QwtScaleMap & xMap,
            const QwtScaleMap & yMap, const QRectF & canvasRect, int from, int to ) const;

private:

    std::vector<double> m_datasX;
    std::vector<double> m_datasY;
    Plot2DProfile( const Plot2DProfile& other);
    Plot2DProfile& operator=( const Plot2DProfile& other );

};

}
}
