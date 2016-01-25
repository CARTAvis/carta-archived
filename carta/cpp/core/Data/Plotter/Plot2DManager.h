/***
 * Provides the glue between an ImageView, displaying a plot, and
 * an application class which provides data to the plot and sets
 * plot properties.
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "CartaLib/Hooks/Histogram.h"
#include <QObject>

namespace Carta {

namespace Lib {
namespace PixelPipeline {
class CustomizablePixelPipeline;
}
}
}

class ImageView;

namespace Carta {
namespace Plot2D {
class Plot2DGenerator;
}

namespace Data {

class Plot2DManager : public QObject, public Carta::State::CartaObject {

    Q_OBJECT

public:

    /**
     * Constructor.
     * @param path - a base path identifier.
     * @param id - an object specific identifier.
     */
    //Note:  This class is a CartaObject to make it easy to add callbacks & receive
    //events.  However, it does not have state that needs to be persisted, so it has
    //a public constructor.  The path and id passed in are that of the application
    //class.
    Plot2DManager( const QString& path, const QString& id );


    /**
     * Clear the zoom selection.
     */
    void clearSelection();

    /**
     * Clear the secondary selection.
     */
    void clearSelectionColor();

    /**
     * End the zoom selection.
     * @param params - the x-coordinate where the selection ended.
     */
    void endSelection(const QString& params );

    /**
     * End the secondary selection.
     * @param params - the x-coordinate where the selection ended.
     */
    void endSelectionColor(const QString& params );

    /**
     * Get the min and max of the zoom selection.
     * @param valid - set to true if there is a valid zoom selection;
     *      false otherwise.
     * @return - the range of the zoom selections.
     */
    std::pair<double,double> getRange( bool* valid ) const;

    /**
     * Get the min and max of the secondary selection.
     * @param valid - set to true if there is a valid zoom selection;
     *      false otherwise.
     * @return - the range of the secondary selections.
     */
    std::pair<double,double> getRangeColor( bool* valid ) const;

    /**
     * Save a copy of the plot as an image.
     * @param filename the full path where the file is to be saved.
     * @return an error message if there was a problem saving the Plot2DManager;
     *      an empty string otherwise.
     */
    QString savePlot( const QString& filename );

    /**
     * Set a range of values for the x-axis.
     * @param min - the minimum x-value.
     * @param max - the maximum x-value.
     */
    void setAxisXRange( double min, double max );

    /**
     * Set whether or not the graph should be colored.
     * @param colored - true if the graph should be colored; false otherwise.
     */
    void setColored( bool colored );

    /**
     * Set the plot data.
     * @param data - a list of (x,y)-values for the plot.
     */
    void setData( Carta::Lib::Hooks::Plot2DResult data);

    /**
     * Set whether or not the y-axis of the plot should use a log scale.
     * @param logScale - true if a log scale should be used; false otherwise.
     */
    void setLogScale( bool logScale );

    /**
     * Set information for coloring the plot.
     * @param pipeline - information about how the data should be colored.
     */
    void setPipeline( std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> pipeline);

    /**
     * Set the type of data that will be generated for the plot.
     * @param gen - the generator for the plot data.
     */
    void setPlotGenerator( Carta::Plot2D::Plot2DGenerator* gen );

    /**
     * Set the zoom range for the plot.
     * @param min - the minimum zoom value.
     * @param max - the maximum zoom value.
     */
    void setRange( double min, double max );

    /**
     * Set the secondary selection range for the plot.
     * @param min - the lower boundary of the secondary selection.
     * @param max - the upper boundary of the secondary selection.
     */
    void setRangeColor( double min, double max );

    /**
     * Set the line/fill style for the plot.
     * @param styleName - a plot style identifier.
     */
    void setStyle( const QString& styleName );

    /**
     * Set the label for the x-axis.
     * @param title - the label for the x-axis.
     */
    void setTitleAxisX( const QString& title );

    /**
     * Set the label for the y-axis.
     * @param title - the label for the y-axis.
     */
    void setTitleAxisY( const QString& title );

    /**
     * Start a zoom selection.
     * @param params - the x-value where the selection should start.
     */
    void startSelection(const QString& params );

    /**
     * Start a secondary plot selection.
     * @param params - the x-value where the selection should start.
     */
    void startSelectionColor( const QString& params );

    /**
     * Update the plot graph.
     */
    void updatePlot( );

    /**
     * Update a user selection.
     * @param x - the current end value of the selection.
     */
    void updateSelection(int x );

    virtual ~Plot2DManager();

    const static QString CLASS_NAME;
    const static QString GRAPH_STYLE_LINE;
    const static QString GRAPH_STYLE_OUTLINE;
    const static QString GRAPH_STYLE_FILL;

signals:

    /**
     * Notification to the application class that the user has made
     * a selection through the GUI.
     */
    void userSelection();

    /**
     * Notification to the application class that the user has made
     * a secondary selection through the GUI.
     */
    void userSelectionColor();

private slots:

    /**
     * Update the size of the plot.
     * @param size - the new size of the plot in pixels.
     */
    void _updateSize( const QSize& size );

private:

    const static QString DATA_PATH;
    const static QString X_COORDINATE;
    const static QString POINTER_MOVE;

    void _initializeDefaultState();
    void _initializeCallbacks();

    void _refreshView();

    void  _updateColorSelection();

    bool m_selectionEnabled;
    double m_selectionStart;
    double m_selectionEnd;
    bool m_selectionEnabledColor;

    //View of plot
    std::shared_ptr<ImageView> m_view = nullptr;


    Carta::Plot2D::Plot2DGenerator* m_plotGenerator;

    //Separate state for mouse events since they get updated rapidly and not
    //everyone wants to listen to them.
    Carta::State::StateInterface m_stateMouse;
	Plot2DManager( const Plot2DManager& other);
	Plot2DManager operator=( const Plot2DManager& other );
};
}
}
