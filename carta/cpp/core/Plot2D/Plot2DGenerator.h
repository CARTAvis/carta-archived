/**
 * Manages one or more 2-dimensional plots.
 */
#pragma once

#include "CartaLib/Hooks/Plot2DResult.h"
#include "Plot2DHolder.h"
#include <QFont>
#include <QString>
#include <memory>

namespace Carta {
namespace Lib {
namespace PixelPipeline {
class CustomizablePixelPipeline;
}
}
}

class QImage;

namespace Carta {
namespace Plot2D {


class Plot2DGenerator{

public:

    /**
     * Constructor.
     */
    Plot2DGenerator( /*Plot2DHolder::PlotType plotType*/ );

    /**
     * Sets the data for the plot.
     * @param data the plot data (x,y) pairs and additional information for plotting.
     * @param id - an identifier for the new data set.
     * @param index - the plot index.
     * @param primary - true if this a primary curve for the plot; false if it is a secondary
     *      curve such as a 1-d fit.
     */
    void addData( std::vector< std::pair<double,double> > data, const QString& id, int index = 0, bool primary = true );

    /**
     * Add informational labels to the plot.
     * @param labels - the list of informational labels that should be shown on the plot.
     * @param index - the plot index.
     */
    void addLabels( const std::vector<std::tuple<double,double,QString> >& labels, int index = 0 );

    /**
     * Add a plot to the display.
     */
    int addPlot();

    /**
     * Remove all data from the plot.
     */
    void clearData( );

    /**
     * Remove all fit data from the plots.
     */
    void clearDataFit();

    /**
     * Clear informational labels from the plot.
     * @param index - the plot index.
     */
    void clearLabels( int index = 0 );

    /**
     * Clear the zoom selection.
     * @param index - the plot index.
     */
    void clearSelection( int index = 0 );

    /**
     * Clear the secondary selection.
     * @param index - the plot index.
     */
    void clearSelectionColor( int index = 0 );

    /**
     * Return the y-axis label for the plot.
     * @param index - the plot index.
     * @return - the plot y-axis label.
     */
    QString getAxisUnitsY( int index = 0) const;

    /**
     * Translate a pixel point (x,y) contained in a plot of the given size into a
     * world point.
     * @param screenPt - a point in pixel coordinates.
     * @param index - the plot index.
     * @param valid - set to true if the image point corresponds to an actual data point.
     * @return - the world coordinates of the pixel point.
     */
    QPointF getImagePoint(const QPointF& screenPt, bool* valid, int index = 0 ) const;

    /**
     * Return the (min,max) y-values of the plot.
     * @param id - the identifier for the data set where the bounds are needed.
     * @param valid - true if the plot y-values are valid; false otherwise (for
     *      example, the plot contains no data.
     * @param index - the plot index.
     * @return - the range of plot y-values.
     */
    std::pair<double,double> getPlotBoundsY( const QString& id, bool* valid, int index = 0 ) const;

    /**
     * Return the title of the plot.
     * @param index - the plot index.
     * @return - the plot title.
     */
    QString getPlotTitle( int index = 0 ) const;

    /**
     * Return the minimum and maximum value of the user's zoom selection.
     * @param valid true if there is a zoom selection with a minimum/maximum value; false otherwise.
     * @param index - the plot index.
     * @return the zoom selection range.
     */
    std::pair<double,double> getRange(bool* valid, int index = 0 ) const;

    /**
     * Return the minimum and maximum value of the user's secondary selection.
     * @param valid true if there is a secondary selection with a minimum/maximum value; false otherwise.
     * @param index - the plot index.
     * @return the clip selection range.
     */
    std::pair<double,double> getRangeColor(bool* valid, int index = 0 ) const;

    /**
     * Return the screen point corresponding to the given image
     * point.
     * @param index - the plot index.
     * @return - the screen point corresponding to the given image
     *      point.
     */
    QPointF getScreenPoint( const QPointF& dataPoint, int index = 0 ) const;



    /**
     * Return the size of the actual plotting area in pixels.
     * @param index - the plot index.
     * @return - the size of the actual plotting area in pixels.
     */
    QSize getPlotSize( int index = 0 ) const;

    /**
     * Get the upper left corner of the plotting area in pixels.
     * @param index - the plot index.
     * @return - the upper left corner of the plotting area in pixels.
     */
    QPointF getPlotUpperLeft( int index = 0) const;

    /**
     * Return the position of the vertical plot line in world coordinates.
     * @param valid - set to true if the plot has data and a vertical line; otherwise,
     *  false.
     * @param index - the plot index.
     * @return - the position in world coordinates of the plot vertical line.
     */
    double getVLinePosition( bool* valid, int index = 0 ) const;

    /**
     * Return true if the parameter is on the canvas itself rather than in the
     * margin of the canvas.
     * @param xPos - a pixel position in the horizontal direction.
     * @param index - the plot index.
     * @return - true if the position is actually on the plot canvas itself; false,
     *    otherwise.
     */
    bool isSelectionOnCanvas( int xPos, int index = 0 ) const;

    /**
     * Remove a set of data form the plot.
     * @param dataName - an identifier for the data set to remove.
     * @param index - the plot index.
     */
    void removeData( const QString& dataName, int index = 0 );

    /**
     * Remove the specified plot from the display.
     * @param index - the plot index.
     */
    void removePlot( int index );

    /**
     * Set the range of values for the x-axis.
     * @param min - the smallest desired x-axis value.
     * @param max - the largest desired x-axis value.
     */
    void setAxisXRange( double min, double max);

    /**
     * Set a color to use in graphing the data from a particular set.
     * @param color - a color to use in graphing a data set.
     * @param id - an identifier for a data set or an empty string if the color
     *      should be applied to all data sets.
     */
    void setColor( QColor color, const QString& id = QString() );

    /**
     * Set whether or not the plot should be colored.
     * @param colored true if the plot should be colored; false if it should be drawn in just a single color.
     * @param id - the data set that should be colored or an empty string to apply
     *      the colored attribute to all data sets.
     * @param index - the plot index.
     */
    void setColored( bool colored, const QString& id = QString(), int index = 0 );

    /**
     * Rename a curve.
     * @param oldName - the original identifier for the curve.
     * @param newName - the new identifier for the curve.
     * @param index - the plot index.
     */
    void setCurveName( const QString& oldName, const QString& newName, int index = 0 );

    /**
     * Set whether or not to show/hide grid lines.
     * @param showGrid - true to show grid lines; false to hide them.
     */
    void setGridLines( bool showGrid);

    /**
     * Set whether the managed plot is a histogram.
     * @param histogram - true if the plot is a histogram; false otherwise.
     * @param index - the plot index.
     */
    void setHistogram( bool histogram, int index = 0  );

    /**
     * Set the location of the horizontal line on the y-axis in world coordinates.
     * @param position - a world y-coordinate value that is the position of the horizontal line.
     * @param index - the plot index.
     */
    void setHLinePosition( double position, int index = 0 );

    /**
     * Set whether or not the horizontal line marker should be shown.
     * @param visible - true if the horizontal line marker should be shown; false, otherwise.
     * @param index - the plot index.
     */
    void setHLineVisible( bool visible, int index = 0 );

    /**
     * Set whether or not a sample line should be drawn with legend items.
     * @param showLegendLine - true if a sample line should be shown; false, otherwise.
     */
    void setLegendLine( bool showLegendLine );

    /**
     * Set the location of the legend on the plot.
     * @param location - an identifier for a plot location (right, bottom, etc).
     */
    void setLegendLocation( const QString& location);

    /**
     * Set whether the legend should be external or internal to the plot.
     * @param external - true if the legend should be external to the plot; false, otherwise.
     */
    void setLegendExternal( bool external);

    /**
     * Set whether or not to draw a legend on the plot.
     * @param visible - true if the legend should be visible; false, otherwise.
     */
    void setLegendVisible( bool visible );

    /**
     * Set the style to use in connecting points of a data set (dotted,solid,etc).
     * @param style - an identifier for a line style.
     * @param id - an identifier for the data set where the style should be applied
     *  or an empty string to apply the style to all data sets.
     * @param index - the plot index.
     * @param primary - true if this a primary curve for the plot; false if it is a secondary
     *      curve such as a 1-d fit.
     */
    void setLineStyle( const QString& style, const QString& id = QString(), int index = 0, bool primary = true );

    /**
     * Set whether or not the plot should use a log scale.
     * @param logScale true if the y-axis should use a log scale; false otherwise.
     * @param index - the plot index.
     */
    void setLogScale(bool logScale, int index = 0);

    /**
     * Set the interval for the horizontal rectangular shaded region.
     * @param minY - the minimum y-value to be shaded in world coordinates.
     * @param maxY - the maximum y-value to be shaded in world coordinates.
     * @param index - the plot index.
     */
    void setMarkedRange( double minY, double maxY, int index = 0 );

    /**
     * Set the position of the vertical marker line.
     * @param xPos - the x-coordinate of the line in world units.
     */
    void setMarkerLine( double xPos);

    /**
     * Set whether or not the vertical marker line should be shown.
     * @param visible - true if the vertical marker line should be shown;
     *      false otherwise.
     */
    void setMarkerLineVisible( bool visible );

    /**
     * Set the pipeline used to determine colors of points.
     * @param pipeline the mapping from point to color.
     * @param index - the plot index.
     */
    void setPipeline( std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> pipeline,
            int index = 0 );

    /**
     * Sets the zoom selection range in world coordinates.
     * @param min the minimum zoom range in world units.
     * @param max the maximum zoom range in world units.
     * @param index - the plot index.
     */
    void setRange(double min, double max, int index = 0);

    /**
     * Sets the secondary selection range in world coordinates.
     * @param min the minimum clip range value in world units.
     * @param max the maximum clip range value in world units.
     * @param index - the plot index.
     */
    void setRangeColor(double min, double max, int index = 0 );

    /**
     * Sets whether or not the horizontal shaded rectangle should be visible.
     * @param visible - true if the horizontal shaded rectangle should be visible;
     *      false otherwise.
     * @param index - the plot index.
     */
    void setRangeMarkerVisible( bool visible, int index = 0 );

    /**
     * Set the min and max zoom range for the plot.
     * @param min the minimum zoom value.
     * @param max the maximum zoom value.
     */
    void setRangePixels(double min, double max);

    /**
     * Set the min and max secondary selection range.
     * @param min the minimum secondary selection value.
     * @param max the maximum secondary selection value.
     */
    void setRangePixelsColor( double min, double max );

    /**
     * Set whether or not the user is selecting a zoom range on the plot.
     * @param selecting true if a zoom range is currently being selected; false otherwise.
     */
    void setSelectionMode(bool selecting );

    /**
     * Set whether or not the user is selecting a secondary range on the histogram.
     * @param selection true if a secondary selection is being selected; false otherwise.
     */
    void setSelectionModeColor( bool selection);

    /**
     * Set the size of the image that will be generated.
     * @param width the width of the generated image.
     * @param height the height of the generated image.
     * @param index - the plot index.
     * @return true if there is a resize; false if the size remains the same.
     */
    bool setSize( int width, int height, int index  = 0);

    /**
     * Set the drawing style for the plot.
     * @param style {QString} the plot draw style.
     * @param id - an identifier for the data set that should be styled or an empty string
     *      to apply the style to all data sets.
     * @param index - the plot index.
     */
    void setStyle( const QString& style, const QString& id = QString(), int index = 0 );


    /**
     * Set a label for the x-axis.
     * @param title - a label for the x-axis.
     */
    void setTitleAxisY( const QString& title );

    /**
     * Set a label for the y-axis.
     * @param title - a label for the y-axis.
     */
    void setTitleAxisX( const QString& title);

    /**
     * Returns the QImage reflection the current state of the plot.
     * @return QImage the plot image.
     * @param width - the width of the desired image.
     * @param height - the height of the desired image.
     */
    QImage toImage( int width = 0, int height = 0) const;

    virtual ~Plot2DGenerator();

private:
    bool _checkIndex( int index ) const;
    void _paintLegend( int x, int y, int width, int height, QPainter* painter ) const;
    void _resetExtents();
    //Actual qwt plot
    std::vector<Plot2DHolder*> m_plots;

    int m_height;
    int m_width;

    Plot2DGenerator( const Plot2DGenerator& other);
    Plot2DGenerator& operator=( const Plot2DGenerator& other );
};
}
}
