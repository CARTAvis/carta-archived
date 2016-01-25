/**
 * Represents the data on a 2D plot.
 */
#pragma once

#include <QString>
#include <QBrush>
#include <qwt_plot.h>
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


class Plot2D {
public:

    /**
     * Constructor.
     */
    Plot2D();

    /**
     * Attach the data to the plot.
     * @param plot - the plot on which the data should be drawn.
     */
    virtual void attachToPlot( QwtPlot* plot ) = 0;

    /**
     * Remove the data from the plot.
     * @param plot - the plot where the data should be removed.
     */
    virtual void detachFromPlot() = 0;

    /**
     * Return the minimum/maximum y-values in the data set.
     * @return - the minimum & maxium y-values in the data set.
     */
    std::pair<double,double> getBoundsY() const;

    /**
     * Return whether or not the y-axis of the plot is using a logarithmic scale.
     * @return - true if the plot y-axis is using a logarithmic scale; false otherwise.
     */
    bool isLogScale() const;

    /**
     * Set the base y-vale for the plot.
     * @param val - the baseline for the plot.
     */
    virtual void setBaseLine( double val ) = 0;

    /**
     * Set whether the data should be drawn in a single color or whether it should be
     * multi-colored based.
     * @param colored true for multi-colored data; false otherwise.
     */
    void setColored( bool colored );

    /**
     * Store the data to be plotted.
     * @param data the plot data.
     */
    virtual void setData ( std::vector<std::pair<double,double> > data ) = 0;

    /**
     * Set the draw style for the data (outline, filled, line).
     * @param style an identifier for a data drawing style.
     */
    void setDrawStyle( const QString& style );

    /**
     * Set whether or not the plot should use a logarithmic scale on the y-axis.
     * @param logScale - true if the y-axis should use a log scale; false otherwise.
     */
    void setLogScale( bool logScale );

    /**
     * Store the color map.
     * @param pipeline -  an object capable of determining a color mapping.
     */
    void setPipeline( std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> pipeline);

    /**
     * Destructor.
     */
    virtual ~Plot2D();


protected:
    std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> m_pipeline;
    QString m_drawStyle;
    QColor m_defaultColor;
    QBrush m_brush;
    bool m_colored;
    double m_maxValueY;
    double m_minValueY;
    bool m_logScale;

};

}
}
