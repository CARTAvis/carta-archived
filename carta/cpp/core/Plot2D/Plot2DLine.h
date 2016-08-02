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
     * Return the position of the this line in world coordinates.
     * @return the x-coordinate of this line.
     */
    double getPosition( ) const;

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
	 * Set whether or not the user is currently selecting a range.
	 * @param drawing true if the user is selecting a range; false otherwise.
	 */
	void setSelectionMode(bool drawing);


	/**
	 * Set the x-location for the line in world units.
     * @param val - the x-coordinate of this line in world units.
     */
    void setPosition( double min);
    void setPositionPixel( double valMin, double valMax );


	/**
	 * Destructor.
	 */
	virtual ~Plot2DLine();

private:
	Plot2DLine( const Plot2DLine& );
	Plot2DLine& operator=( const Plot2DLine& );
	bool m_selection;
	int m_height;

	//Position of the line in world coordinates.
	double m_position;

	//Position of the line in pixel coordinates.
	int m_positionPixel;
	int m_positionPixelMin;

	//Used to keep track of the current draw position, in world coordinates,
	//which may be different from the m_position if the users is dragging the
	//line to start a movie.
	mutable int m_drawPosition;
	QColor m_color;

};

}
}


