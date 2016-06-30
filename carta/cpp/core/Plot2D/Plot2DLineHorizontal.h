/**
 * Represents a horizontal line on a plot.
 */
#pragma once
#include <qwt_plot_marker.h>

class QPainter;

namespace Carta {
namespace Plot2D {


class Plot2DLineHorizontal : public QwtPlotMarker{

public:

    /**
     * Constructor.
     */
	Plot2DLineHorizontal();

	/**
     * Draw the line.
     * @param painter - the painter.
     * @param xMap - map between world and pixel coordinates for x-axis.
     * @param yMap - map between world and pixel coordinates for y-axis.
     * @param canvasRect - size of plotting area.
     */
    virtual void draw ( QPainter* painter, const QwtScaleMap& xMap,
        const QwtScaleMap& yMap, const QRectF& canvasRect) const;

	/**
	 * Set the color used to draw the line.
	 * @param color - the color used to draw the line.
	 */
	void setColor( QColor color );


	/**
	 * Set the y-location for the line in world units.
     * @param val - the y-coordinate of this line in world units.
     */
    void setPosition( double val);

	/**
	 * Destructor.
	 */
	virtual ~Plot2DLineHorizontal();

private:
	Plot2DLineHorizontal( const Plot2DLineHorizontal& );
	Plot2DLineHorizontal& operator=( const Plot2DLineHorizontal& );

	//Position of the line in world coordinates.
	double m_position;
	QColor m_color;
};

}
}


