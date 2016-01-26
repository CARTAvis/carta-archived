/**
 * Represents a vertical line on a plot.
 */
#pragma once
#include <qwt_plot_marker.h>

class QPainter;

namespace Carta {
namespace Plot2D {


class Plot2DLine : public QwtPlotMarker{
public:
    /**
     * Constructor.
     */
	Plot2DLine();

	/**
     * Draw the range.
     * @param painter
     * @param xMap
     * @param yMap
     * @param canvasRect
     */
    virtual void draw ( QPainter* painter, const QwtScaleMap& xMap,
        const QwtScaleMap& yMap, const QRectF& canvasRect) const;

	/**
	 * Set the color used to shade the clip region.
	 * @param color the shade color for the clip region.
	 */
	void setColor( QColor color );

	/**
     * Set the height of the range.
     * @param h - the pixel height of the line.
     */
    void setHeight( int h );


    /**
     * Set the x-location for the line in world units.
     * @param val - the x-coordinate of this line in world units.
     */
    void setPosition( double val );


	/**
	 * Destructor.
	 */
	virtual ~Plot2DLine();

private:
	Plot2DLine( const Plot2DLine& );
	Plot2DLine& operator=( const Plot2DLine& );
	int m_height;
	int m_position;
	QColor m_color;

};

}
}


