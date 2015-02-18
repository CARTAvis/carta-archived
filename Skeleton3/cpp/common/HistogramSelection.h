#include <qwt_plot_marker.h>

class QPainter;

/**
 * Handles drawing the range selection rectangle for
 * the Histogram Generator
 */
class HistogramSelection : public QwtPlotMarker {
public:
	HistogramSelection();
	virtual void draw ( QPainter* painter, const QwtScaleMap& xMap,
		const QwtScaleMap& yMap, const QRectF& canvasRect) const;
	void setHeight( int h );
	void boundaryLineMoved( const QPoint& pos );
	int getLowerBound() const;
	int getUpperBound() const;
	void setBoundaryValues( double minX, double maxX );
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


