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
     * Return an identifier for the data set.
     * @return - an identifier for the data set.
     */
    QString getId() const;

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
     * Set an identifier for the data set.
     * @param id - an identifier for the data set.
     */
    void setId( const QString& id );

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
    QString m_id;

};

}
}
