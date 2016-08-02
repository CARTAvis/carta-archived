/**
 * Represents textual information about a particular location on a plot.
 */
#pragma once
#include <qwt_plot_marker.h>

namespace Carta {
namespace Plot2D {


class Plot2DTextMarker : public QwtPlotMarker{

public:

    /**
     * Constructor.
     */
	Plot2DTextMarker();

	/**
	 * Set the text that the marker will display.
	 * @param labelText - the text the marker will display.
	 */
	void setContent( const QString& labelText );

	/**
	 * Destructor.
	 */
	virtual ~Plot2DTextMarker();

private:
	Plot2DTextMarker( const Plot2DTextMarker& );
	Plot2DTextMarker& operator=( const Plot2DTextMarker& );
};

}
}


