/**
 * Represents a plot.
 */
#pragma once

#include <qwt_plot.h>

class QwtLegend;
class QwtPlotLegendItem;

namespace Carta {
namespace Plot2D {


class Plot : public QwtPlot {
    Q_OBJECT
public:

    enum class LegendLocation {
       LEFT,
       RIGHT,
       BOTTOM,
       TOP,
       TOP_LEFT,
       TOP_RIGHT,
       BOTTOM_LEFT,
       BOTTOM_RIGHT,
       OTHER
    };

    /**
     * Constructor.
     */
    Plot( QWidget* parent = nullptr);

    /**
     * Return the screen point corresponding to the given image
     * point.
     * @return - the screen point corresponding to the given image
     *      point.
     */
    QPointF getScreenPoint( const QPointF& dataPoint ) const;

    /**
     * Set the font used to draw axis labels.
     * @param font - the font for axis labels.
     */
    void setFont( const QFont& font );

    /**
     * Set parameters that determine the display of the legend on the plot.
     * @param visible - true if the legend should be visible; false, otherwise.
     * @param legendLocation - location of the legend on the plot (left,right, etc).
     * @param external - true if the legend is external to the plot; false if the legend
     *      should appear on the plot.
     */
    void setLegendPosition( bool visible,
            const QString& legendLocation, bool external );

    /**
     * Destructor.
     */
    virtual ~Plot();


private:
    int _calculateAlignment( const QString& pos ) const;
    QwtPlot::LegendPosition _calculatePosition( const QString& pos ) const;
    QwtLegend* m_externalLegend;
    QwtPlotLegendItem* m_legendItem;
    Plot( const Plot& other);
    Plot& operator=( const Plot& other );
};

}
}
