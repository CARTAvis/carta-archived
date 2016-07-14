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
     * Clear any constraints on the size of the y-axis widget.
     */
    void clearAxisExtentY();

    /**
     * Return the maximum width of the axis widget.
     * @return - the maximum width of the axis widget.
     */
    int getAxisExtentY() const;

    /**
     * Return the location of the x-axis (top or bottom).
     * @return - the location of the x-axis.
     */
    QwtPlot::Axis getAxisLocationX() const;

    /**
     * Return the min and max value of the x-axis.
     * @return - the (min,max) range of the x-axis.
     */
    std::pair<double,double> getAxisScaleX() const;

    /**
     * Return the title of the x-axis.
     * @return - the title of the x-axis.
     */
    QString getAxisTitleX() const;

    /**
     * Return the title of the y-axis.
     * @return - the title of the y-axis.
     */
    QString getAxisTitleY() const;

    /**
     * Return the y-axis units.
     * @return - the y-axis units.
     */
    QString getAxisUnitsY() const;

    /**
     * Return the external legend.
     * @return - the external legend.
     */
    QSize getLegendSize() const;

    /**
     * Return the screen point corresponding to the given image
     * point.
     * @return - the screen point corresponding to the given image
     *      point.
     */
    QPointF getScreenPoint( const QPointF& dataPoint ) const;

    /**
     * Return the image point corresponding to the given screen
     * point.
     * @return - the image point corresponding to the given screen point.
     */
    QPointF getImagePoint(const QPointF& screenPt ) const;

    /**
     * Return the size of the actual plotting area in pixels.
     * @return - the size of the actual plotting area in pixels.
     */
    QSize getPlotSize() const;

    /**
     * Get the upper left corner of the plotting area in pixels.
     * @return - the upper left corner of the plotting area in pixels.
     */
    QPointF getPlotUpperLeft() const;

    /**
     * Returns whether or not the x-coordinate is a position on the plot canvas.
     * @param xPos - a value in pixel coordinates.
     * @return - true if the value lies in the canvas area of the plot.
     */
    bool isSelectionOnCanvas( int xPos ) const;

    /**
     * Paint the legend to an image.
     * @param painter- the image painter.
     * @param geom - where in the image the legend should be painted.
     */
    void legendToImage(QPainter* painter, const QRectF& geom) const;

    /**
     * Set the maximum width of the y-axis widget.
     * @param extent - the maximum width of the y-axis.
     */
    void setAxisExtentY( int extent );

    /**
     * Set whether the x-axis should be located on the top or bottom of the graph.
     * @param axis - the location for the x-axis.
     */
    void setAxisLocationX( QwtPlot::Axis axis );

    /**
     * Set how the y-axis should be scaled (logarithmic or linear).
     * @param engine - how the y-axis should be scaled.
     */
    void setAxisScaleEngineY(QwtScaleEngine* engine );

    /**
     * Set the min and max value for the x-axis.
     * @param min - the minimum value for the x-axis.
     * @param max - the maximum value for the x-axis.
     */
    void setAxisScaleX( double min, double max );

    /**
     * Set the min and max value for thee y-axis.
     * @param min - the minimum value for the y-axis.
     * @param max - the maximum value for the y-axis.
     */
    void setAxisScaleY( double min, double max );

    /**
     * Set the title of the x-axis.
     * @param title - the title of the x-axis.
     */
    void setAxisTitleX( const QwtText& title );

    /**
     * Set the title of the y-axis.
     * @param title - the title of the y-axis.
     */
    void setAxisTitleY( const QwtText& title );

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
     * Set the size of the plot based on the screen size.
     * @param width - the width of the plot in pixels.
     * @param height - the height of the plot in pixels.
     */
    void setPlotSize( int width, int height );

    /**
     * Destructor.
     */
    virtual ~Plot();

private:
    const static int ZERO_OFFSET;
    int _calculateAlignment( const QString& pos ) const;
    QwtPlot::LegendPosition _calculatePosition( const QString& pos ) const;
    void _removeLegendExternal();
    void _removeLegendInternal();

    QwtLegend* m_externalLegend;
    QwtPlotLegendItem* m_legendItem;
    QwtPlot::Axis m_axisLocationY;
    QwtPlot::Axis m_axisLocationX;

    bool m_external;
    bool m_visible;
    QwtPlot::LegendPosition m_legendLocation;
    Plot( const Plot& other);
    Plot& operator=( const Plot& other );
};

}
}
