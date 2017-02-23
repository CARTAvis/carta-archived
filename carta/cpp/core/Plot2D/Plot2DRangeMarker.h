/**
 * Represents a shaded rectangle on a plot covering a specified interval on the y-axis
 * and stretching across the x-axis.
 */
#pragma once
#include <qwt_plot_item.h>
#include <qwt_interval.h>

class QPainter;

namespace Carta {
namespace Plot2D {


class Plot2DRangeMarker : public QwtPlotItem{
public:
    /**
     * Constructor.
     */
	Plot2DRangeMarker();

	/**
     * Draw the range.
     * @param painter - the painter.
     * @param xMap - map for x-coordinate values.
     * @param yMap - map for y-coordinate values.
     * @param canvasRect - size of the plot.
     */
    virtual void draw ( QPainter* painter, const QwtScaleMap& xMap,
        const QwtScaleMap& yMap, const QRectF& canvasRect) const;

    /**
     * Set the interval to shade on the y-axis.
     * @param interval - the shaded band range on the y-axis.
     */
    void setRange( const QwtInterval& interval );

	/**
	 * Destructor.
	 */
	virtual ~Plot2DRangeMarker();

private:
	Plot2DRangeMarker( const Plot2DRangeMarker& );
	Plot2DRangeMarker& operator=( const Plot2DRangeMarker& );
	QColor m_shadeColor;
	QwtInterval m_interval;
};

}
}


