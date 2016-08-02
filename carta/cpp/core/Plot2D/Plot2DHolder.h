/**
 * Generates an image of a 2D plot based on set configuration (display) parameters.
 */
#pragma once

#include "CartaLib/Hooks/Plot2DResult.h"
#include <QFont>
#include <QString>
#include <memory>
#include <qwt_plot.h>
#include <qwt_plot_grid.h>


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

class Plot;
class Plot2D;
class Plot2DLine;
class Plot2DLineHorizontal;
class Plot2DRangeMarker;
class Plot2DSelection;
class Plot2DTextMarker;

class Plot2DHolder{

public:

    enum class PlotType {
        HISTOGRAM,
        PROFILE,
        OTHER
    };

    /**
     * Constructor.
     */
    Plot2DHolder( /*PlotType plotType*/ );

    /**
     * Sets the data for the plot.
     * @param data the plot data (x,y) pairs and additional information for plotting.
     * @param id - an identifier for the new data set.
     * @param primary - true if this a primary curve for the plot; false if it is a secondary
     *      curve such as a 1-d fit.
     */
    void addData( std::vector< std::pair<double,double> > data, const QString& id, bool primary = true );

    /**
     * Add information labels to the plot.
     * @param labels - a list containing the location and text for each label.
     */
    void addLabels( const std::vector<std::tuple<double,double,QString> >& labels );

    /**
     * Clear any restrictions on the extent of the y-axis.
     */
    void clearAxisExtentY();

    /**
     * Remove all data from the plot.
     */
    void clearData();

    /**
     * Remove all fit data from the plot.
     */
    void clearDataFit();

    /**
     * Clear informational labels from the plot.
     */
    void clearLabels();

    /**
     * Clear the zoom selection.
     */
    void clearSelection();

    /**
     * Clear the secondary selection.
     */
    void clearSelectionColor();

    /**
     * Return the maximum width of the axis widget.
     * @return - the maximum width of the axis widget.
     */
    int getAxisExtentY() const;

    /**
     * Return the minimum and maximum values on the x-axis.
     * @return - the minimum and maximum values on the x-axis.
     */
    std::pair<double,double> getAxisXRange() const;

    /**
     * Return the y-axis label for the plot.
     * @return - the plot y-axis label.
     */
    QString getAxisUnitsY() const;

    /**
     * Returns the color used to draw the data identified by the id.
     * @param id - an identifier for a data set.
     * @param valid - set to true if a curve with the given id was found and
     *      its color correctly obtained; false otherwise.
     * @return - the color used to draw the identified data.
     */
    QColor getColor( const QString& id, bool* valid ) const;

    /**
     * Get the location of the legend (right,top,bottom,left) with respect
     * to the plot area.
     * @return - the location of the legend.
     */
    QString getLegendLocation() const;

    /**
     * Return the external legend.
     * @return - the external legend.
     */
    QSize getLegendSize() const;

    /**
     * Return the (min,max) y-values of the plot.
     * @param id - the identifier for the data set where the bounds are needed.
     * @param valid - true if the plot y-values are valid; false otherwise (for
     *      example, the plot contains no data.
     * @return - the range of plot y-values.
     */
    std::pair<double,double> getPlotBoundsY( const QString& id, bool* valid ) const;

    /**
     * Return the title of the plot.
     * @return - the plot title.
     */
    QString getPlotTitle() const;

    /**
     * Return the minimum and maximum value of the user's zoom selection.
     * @param valid true if there is a zoom selection with a minimum/maximum value; false otherwise.
     * @return the zoom selection range.
     */
    std::pair<double,double> getRange(bool* valid ) const;

    /**
     * Return the minimum and maximum value of the user's secondary selection.
     * @param valid true if there is a secondary selection with a minimum/maximum value; false otherwise.
     * @return the clip selection range.
     */
    std::pair<double,double> getRangeColor(bool* valid ) const;

    /**
     * Return the screen point corresponding to the given image
     * point.
     * @return - the screen point corresponding to the given image
     *      point.
     */
    QPointF getScreenPoint( const QPointF& dataPoint ) const;

    /**
     * Return the size of the plot including axes, legends, etc.
     * @retun - the entire size of the plotting area.
     */
    QSize getSize() const;

    /**
     * Return the title of the x-axis.
     * @return - the title of the x-axis.
     */
    QString getTitleAxisX() const;

    /**
     * Return the title of the y-axis.
     * @return - the title of the y-axis.
     */
    QString getTitleAxisY() const;

    /**
     * Return the position of the vertical plot line in world coordinates.
     * @param valid - set to true if the plot has data and a vertical line; otherwise,
     *  false.
     * @return - the position in world coordinates of the plot vertical line.
     */
    double getVLinePosition( bool* valid ) const;

    /**
     * Translate a pixel point (x,y) contained in a plot of the given size into a
     * world point.
     * @param screenPt - a point in pixel coordinates.
     * @param valid - set to true if the point found is an actual data point.
     * @return - the world coordinates of the pixel point.
     */
    QPointF getImagePoint(const QPointF& screenPt, bool* valid ) const;

    /**
     * Return the x-location of the vertical marker line.
     * @return - the x-coordinate of the vertical marker line.
     */
    double getMarkerLine() const;

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
     * Returns whether or not the plot has a visible external legend.
     * @return - true if the plot has a visible external legend; false, otherwise.
     */
    bool isExternalLegend() const;

    /**
     * Returns whether or not the legend is visible.
     * @return - true if the legend is visible; false, otherwise.
     */
    bool isLegendVisible() const;

    /**
     * Return true if the parameter is on the canvas itself rather than in the
     * margin of the canvas.
     * @param xPos - a pixel position in the horizontal direction.
     * @return - true if the position is actually on the plot canvas itself; false,
     *    otherwise.
     */
    bool isSelectionOnCanvas( int xPos ) const;

    /**
     * Returns whether or not the vertical marker line is visible on the plot.
     * @return - true if the vertical marker line is visible on the plot; false, otherwise.
     */
    bool isMarkerLineVisible() const;

    /**
     * Paint the legend to an image.
     * @param painter- the image painter.
     * @param geom - where in the image the legend should be painted.
     */
    void legendToImage( QPainter* paint, const QRectF& geom ) const;

    /**
     * Remove a set of data form the plot.
     * @param dataName - an identifier for the data set to remove.
     */
    void removeData( const QString& dataName );

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
     * Set the range of values for the x-axis.
     * @param min - the smallest desired x-axis value.
     * @param max - the largest desired x-axis value.
     */
    void setAxisXRange( double min, double max );

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
     */
    void setColored( bool colored, const QString& id = QString() );

    /**
     * Rename a curve.
     * @param oldName - the original identifier for the curve.
     * @param newName - the new identifier for the curve.
     */
    void setCurveName( const QString& oldName, const QString& newName );

    /**
     * Set whether or not to show/hide grid lines.
     * @param showGrid - true to show grid lines; false to hide them.
     */
    void setGridLines( bool showGrid );

    /**
     * Set the location of the horizontal line on the y-axis in world coordinates.
     * @param position - a world y-coordinate value that is the position of the horizontal line.
     */
    void setHLinePosition( double position );

    /**
     * Set whether or not the horizontal line marker should be shown.
     * @param visible - true if the horizontal line marker should be shown; false, otherwise.
     */
    void setHLineVisible( bool visible );

    /**
     * Set whether or not a sample line should be drawn with legend items.
     * @param showLegendLine - true if a sample line should be shown; false, otherwise.
     */
    void setLegendLine( bool showLegendLine );

    /**
     * Set the location of the legend on the plot.
     * @param location - an identifier for a plot location (right, bottom, etc).
     */
    void setLegendLocation( const QString& location );

    /**
     * Set whether the legend should be external or internal to the plot.
     * @param external - true if the legend should be external to the plot; false, otherwise.
     */
    void setLegendExternal( bool external );

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
     * @param primary - true if this a primary curve for the plot; false if it is a secondary
     *      curve such as a 1-d fit.
     */
    void setLineStyle( const QString& style, const QString& id = QString(), bool primary = true );

    /**
     * Set whether or not the plot should use a log scale.
     * @param logScale true if the y-axis should use a log scale; false otherwise.
     */
    void setLogScale(bool logScale);

    /**
     * Set the interval for the horizontal rectangular shaded region.
     * @param minY - the minimum y-value to be shaded in world coordinates.
     * @param maxY - the maximum y-value to be shaded in world coordinates.
     */
    void setMarkedRange( double minY, double maxY );

    /**
     * Set the position of the vertical marker line.
     * @param xPos - the x-coordinate of the line in world units.
     */
    void setMarkerLine( double xPos );

    /**
     * Set whether or not the vertical marker line should be shown.
     * @param visible - true if the vertical marker line should be shown;
     *      false, otherwise.
     */
    void setMarkerLineVisible( bool visible );

    /**
     * Set the pipeline used to determine colors of points.
     * @param pipeline the mapping from point to color.
     */
    void setPipeline( std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> pipeline);

    /**
     * Set the type of plot.
     * @param type - the type of plot.
     */
    void setPlotType( PlotType type );

    /**
     * Sets the zoom selection range in world coordinates.
     * @param min the minimum zoom range in world units.
     * @param max the maximum zoom range in world units.
     */
    void setRange(double min, double max);

    /**
     * Sets the secondary selection range in world coordinates.
     * @param min the minimum clip range value in world units.
     * @param max the maximum clip range value in world units.
     */
    void setRangeColor(double min, double max);

    /**
     * Sets whether or not the horizontal shaded rectangle should be visible.
     * @param visible - true if the horizontal shaded rectangle should be visible;
     *      false otherwise.
     */
    void setRangeMarkerVisible( bool visible );

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
     * @param selecting true if a zoom range is currently being selected; false otherwsie.
     */
    void setSelectionMode(bool selecting);

    /**
     * Set whether or not the user is selecting a secondary range on the histogram.
     * @param selection true if a secondary selection is being selected; false otherwise.
     */
    void setSelectionModeColor( bool selection );

    /**
     * Set the size of the image that will be generated.
     * @param width the width of the generated image.
     * @param height the height of the generated image.
     * @return true if there is a resize; false if the size remains the same.
     */
    bool setSize( int width, int height );

    /**
     * Set the drawing style for the plot.
     * @param style {QString} the plot draw style.
     * @param id - an identifier for the data set that should be styled or an empty string
     *      to apply the style to all data sets.
     */
    void setStyle( const QString& style, const QString& id = QString() );

    /**
     * Set a label for the x-axis.
     * @param title - a label for the x-axis.
     */
    void setTitleAxisY( const QString& title );

    /**
     * Set a label for the y-axis.
     * @param title - a label for the y-axis.
     */
    void setTitleAxisX( const QString& title );

    /**
     * Paints the plot to an image.
     * @param painter - the painter.
     * @param geom - rectangle in the image where the plot should be painted.
     */
    void toImage( QPainter* painter, const QRect& geom ) const;

    virtual ~Plot2DHolder();

private:
    void _clearItem( QwtPlotItem* item );
    void _clearMarkers();
    void _detachData(QList<std::shared_ptr<Plot2D> >& datas);
    double _getRelativeError( double minValue, double maxValue) const;
    std::shared_ptr<Plot2D> _findData( const QString& id, bool primary ) const;
    std::shared_ptr<Plot2D> _findDataPrimary( const QString& id  ) const;
    std::shared_ptr<Plot2D> _findDataSecondary( const QString& id  ) const;
    void _getDataBounds( double* dataMinX, double* dataMaxX,
            double* dataMinY, double* dataMaxY ) const;
    std::shared_ptr<Plot2D> _makeData() const;
    void _setColorData( QColor color, std::shared_ptr<Plot2D> plotData );
    void _setLineStyle( const QString& style, std::shared_ptr<Plot2D> data );

    void _updateLegend();

    //Update the y-axis scales (where to plot from).
    void _updateScales();

    const static double EXTRA_RANGE_PERCENT;
    //Actual qwt plot
    Plot *m_plot;
    //Data and styling for the plot
    QList< std::shared_ptr<Plot2D> > m_datas;
    QList< std::shared_ptr<Plot2D> > m_dataSecondary;
    Plot2DSelection *m_range;
    Plot2DSelection * m_rangeColor;
    Plot2DLineHorizontal* m_hLine;
    Plot2DLine* m_vLine;
    Plot2DRangeMarker* m_rangeMarker;
    QwtPlotGrid* m_gridLines;
    QList< std::shared_ptr<Plot2DTextMarker> > m_textMarkers;
    int m_height;
    int m_width;

    bool m_legendVisible;
    bool m_legendExternal;
    bool m_legendLineShow;
    QString m_legendPosition;
    bool m_logScale;
    QFont m_font;
    PlotType m_plotType;


    Plot2DHolder( const Plot2DHolder& other);
    Plot2DHolder& operator=( const Plot2DHolder& other );
};
}
}
