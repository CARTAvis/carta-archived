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
     * Returns the data point closest to the target point.
     * @param targetX - the x-coordinate of the target point.
     * @param targetY - the y-coordinate of the target point.
     * @param xError - set to the error in the x-direction.
     * @param yError - set to the error in the y-direction.
     * @return - the data point closest to the target point.
     */
    virtual std::pair<double,double> getClosestPoint( double targetX, double targetY,
            double* xError, double* yError ) const;

    /**
     * Returns the color used to draw the data.
     * @return - the color used to draw the data.
     */
    QColor getColor() const;

    /**
     * Return the minimum/maximum x-values in the data set.
     * @return - the minimum & maxium x-values in the data set.
     */
    std::pair<double,double> getBoundsX() const;

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
     * Set a single color to use in plotting all of the data.
     * @param color - the color to use in plotting the data.
     */
    void setColor( QColor color );

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
    virtual void setDrawStyle( const QString& style );

    /**
     * Set whether or not a sample legend line should be drawn with legend items.
     * @param showLegendLine - true if legend lines should be drawn; false, otherwise.
     */
    virtual void setLegendLine( bool showLegendLine );

    /**
     * Turn on/off the legend for this data item.
     * @param visible - true to see a corresponding legend for this data; false, to
     *  not display a legend for this data.
     */
    virtual void setLegendVisible( bool visible );

    /**
     * Set the line style to use in plotting data sets.
     * @param lineStyle - an identifier for a line style.
     */
    void setLineStyle( const QString& lineStyle );

    /**
     * Set an identifier for the data set.
     * @param id - an identifier for the data set.
     */
    virtual void setId( const QString& id );

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
    Qt::PenStyle m_penStyle;
    QColor m_defaultColor;
    QBrush m_brush;
    bool m_colored;
    double m_minValueX;
    double m_maxValueX;
    double m_maxValueY;
    double m_minValueY;
    QString m_id;

    Plot2D( const Plot2D& other);
    Plot2D& operator=( const Plot2D& other );

};

}
}
