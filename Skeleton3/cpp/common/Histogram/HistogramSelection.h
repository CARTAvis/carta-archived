#include <qwt_plot_marker.h>

class QPainter;

/**
 * Handles drawing the range selection rectangle for
 * the Histogram Generator
 */
class HistogramSelection : public QwtPlotMarker {
public:
    /**
     * Constructor.
     */
	HistogramSelection();

	/**
	 * Draw the range.
	 */
	virtual void draw ( QPainter* painter, const QwtScaleMap& xMap,
		const QwtScaleMap& yMap, const QRectF& canvasRect) const;

	/**
	 * Set the height of the range.
	 */
	void setHeight( int h );

	/**
	 * Set the new position of the boundary line.
	 */
	void boundaryLineMoved( const QPoint& pos );

	/**
	 * Returns the lower bound of the range.
	 */
	int getLowerBound() const;

	/**
	 * Returns the upper bound of the range.
	 */
	int getUpperBound() const;

	/**
	 * Set the min and max values of the range.
	 */
	void setBoundaryValues( double minX, double maxX );


	/**
	 * Set the range back to a single line.
	 */

	void reset();
	virtual ~HistogramSelection();
private:
	HistogramSelection( const HistogramSelection& );
	HistogramSelection& operator=( const HistogramSelection& );
	int m_height;
	double lowerBound;
	double upperBound;
	bool rangeSet;

};


