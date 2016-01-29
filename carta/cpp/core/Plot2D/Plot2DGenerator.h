/**
 * Generates an image of a 2D plot based on set configuration (display) parameters.
 */
#pragma once

#include "CartaLib/Hooks/Plot2DResult.h"
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

class QwtPlot;
class QImage;

namespace Carta {
namespace Plot2D {

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
     * @param valid - true if the plot y-values are valid; false otherwise (for
     *      example, the plot contains no data.
     * @return - the range of plot y-values.
     */
    std::pair<double,double> getPlotBoundsY( bool* valid ) const;

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
     * Set whether or not the plot should be colored.
     * @param colored true if the plot should be colored; false if it should be drawn in just a single color.
     */
    void setColored( bool colored );

    /**
     * Sets the data for the plot.
     * @param data the plot data (x,y) pairs and additional information for plotting.
     */
    void setData( Carta::Lib::Hooks::Plot2DResult data);

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
     */
    void setStyle( QString style );

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

    //Update the y-axis scales (where to plot from).
    void _updateScales();

    const static double EXTRA_RANGE_PERCENT;
    //Actual qwt plot
    QwtPlot *m_plot;
    //Data and styling for the plot
    Plot2D* m_plot2D;
    Plot2DSelection *m_range;
    Plot2DSelection * m_rangeColor;
    Plot2DLine* m_vLine;
    int m_height;
    int m_width;
    QString m_axisNameX;
    QString m_axisNameY;
    QString m_axisUnitX;
    QString m_axisUnitY;
    QFont m_font;
};
}
}
