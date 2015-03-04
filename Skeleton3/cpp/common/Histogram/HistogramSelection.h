/**
 * Represents a user selection on a histogram (shaded region).
 */
#pragma once
#include <qwt_plot_marker.h>
//#include <QObject>

class QPainter;

namespace Carta {
namespace Histogram {


class HistogramSelection : public QwtPlotMarker{
public:
    /**
     * Constructor.
     */
	HistogramSelection();

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
	 * Returns the lower clip of the histogram.
	 */
	double getClipMin() const;

	/**
	 * Returns the upper clip of the histogram.
	 */
	double getClipMax() const;

	/**
	 * Set the min and max values of the range.
	 * @param minX the minimum value of the range.
	 * @param maxX the maximum value of the range.
	 */
	void setBoundaryValues( double minX, double maxX );

	/**
	 * Set whether or not the user is currently selecting a range.
	 * @param drawing true if the user is selecting a range; false otherwise.
	 */
	void setSelectionMode(bool drawing);

	/**
	 * Set the range back to a single line.
	 */
	void reset();

	/**
	 * Destructor.
	 */
	virtual ~HistogramSelection();

private:
	HistogramSelection( const HistogramSelection& );
	HistogramSelection& operator=( const HistogramSelection& );
	int m_height;
	double m_lowerBound;
	double m_upperBound;
	bool rangeSet;
	bool m_selection;
	mutable double m_clipMin;
	mutable double m_clipMax;

};

}
}


