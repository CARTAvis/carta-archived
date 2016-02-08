/**
 * Generates an image of a 2D plot based on set configuration (display) parameters.
 */
#pragma once

#include "CartaLib/Hooks/Plot2DResult.h"
#include <QFont>
#include <QString>
#include <memory>
#include <qwt_plot.h>


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
class Plot2DSelection;

class Plot2DGenerator{

public:

    enum class PlotType {
        HISTOGRAM,
        PROFILE,
        OTHER
    };

    /**
     * Constructor.
     */
    Plot2DGenerator( PlotType plotType );

    /**
     * Sets the data for the plot.
     * @param data the plot data (x,y) pairs and additional information for plotting.
     * @param id - an identifier for the new data set.
     */
    void addData( std::vector< std::pair<double,double> > data, const QString& id );

    /**
     * Remove all data from the plot.
     */
    void clearData();

    /**
     * Clear the zoom selection.
     */
    void clearSelection();

    /**
     * Clear the secondary selection.
     */
    void clearSelectionColor();


    /**
     * Return the y-axis label for the plot.
     * @return - the plot y-axis label.
     */
    QString getAxisUnitsY() const;

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
     * Return the position of the vertical plot line in world coordinates.
     * @param valid - set to true if the plot has data and a vertical line; otherwise,
     *  false.
     * @return - the position in world coordinates of the plot vertical line.
     */
    double getVLinePosition( bool* valid ) const;


    /**
     * Return true if the parameter is on the canvas itself rather than in the
     * margin of the canvas.
     * @param xPos - a pixel position in the horizontal direction.
     * @return - true if the position is actually on the plot canvas itself; false,
     *    otherwise.
     */
    bool isSelectionOnCanvas( int xPos ) const;

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
     */
    void setLineStyle( const QString& style, const QString& id = QString() );

    /**
     * Set whether or not the plot should use a log scale.
     * @param logScale true if the y-axis should use a log scale; false otherwise.
     */
    void setLogScale(bool logScale);

    /**
     * Set the position of the vertical marker line.
     * @param xPos - the x-coordinate of the line in world units.
     */
    void setMarkerLine( double xPos );

    /**
     * Set the pipeline used to determine colors of points.
     * @param pipeline the mapping from point to color.
     */
    void setPipeline( std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> pipeline);

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
     * Returns the QImage reflection the current state of the plot.
     * @return QImage the plot image.
     * @param width - the width of the desired image.
     * @param height - the height of the desired image.
     */
    QImage * toImage( int width = 0, int height = 0) const;

    virtual ~Plot2DGenerator();

private:
    std::shared_ptr<Plot2D> _findData( const QString& id ) const;

    void _updateLegend();

    //Update the y-axis scales (where to plot from).
    void _updateScales();

    const static double EXTRA_RANGE_PERCENT;
    //Actual qwt plot
    Plot *m_plot;
    //Data and styling for the plot
    QList< std::shared_ptr<Plot2D> > m_datas;
    Plot2DSelection *m_range;
    Plot2DSelection * m_rangeColor;
    Plot2DLine* m_vLine;
    int m_height;
    int m_width;
    QString m_axisNameX;
    QString m_axisNameY;
    QString m_axisUnitX;
    QString m_axisUnitY;
    bool m_legendVisible;
    bool m_legendExternal;
    bool m_legendLineShow;
    QString m_legendPosition;
    bool m_logScale;
    QFont m_font;
    PlotType m_plotType;
};
}
}
