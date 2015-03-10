/***
 * Entry point into the model/controller for Python scripting
 * commands.
 *
 */

#pragma once
#include <QString>

class ObjectManager;
namespace Carta {
    namespace Data {
        class ViewManager;
    }
}

class ScriptFacade {

public:

    /**
     * Return a list of the image views defined by the layout.
     * @return a QStringList containing the id values of image views.
     */
    QStringList getImageViews();

    /**
     * Return a list of the colormap views defined by the layout.
     * @return a QStringList containing the id values of colormap views.
     */
    QStringList getColorMapViews();

    /**
     * Return a list of the animator views defined by the layout.
     * @return a QStringList containing the id values of animator views.
     */
    QStringList getAnimatorViews();

    /**
     * Return a list of the histogram views defined by the layout.
     * @return a QStringList containing the id values of histogram views.
     */
    QStringList getHistogramViews();

    /**
     * Return a list of the statistics views defined by the layout.
     * @return a QStringList containing the id values of statistics views.
     */
    QStringList getStatisticsViews();

    /**
     * Return a list of the names of colormaps available on the server.
     * @return a QStringList containing the names of available color maps.
     */
    QStringList getColorMaps() const;

    /**
     * Return a list of filenames that can be loaded.
     * @return a QStringList containing the names of available files.
     */
    QString getFileList() const;

    /**
     * Establish a link between a source and destination.
     * @param sourceId the unique server side id of the link source.
     * @param destId the unique server side id of an object responsible for an image view.
     */
    QString addLink( const QString& sourceId, const QString& destId );

    /**
     * Remove a link from a source to a destination.
     * @param sourceId the unique server-side id for the plugin that is the source of the link.
     * @param destId the unique server side id for the plugin that is the destination of the link.
     * @return an error message if the link does not succeed.
     */
    QString removeLink( const QString& sourceId, const QString& destId );

    /**
     * Load a file from /scratch/Images into an image view.
     * @param objectId the unique server-side id of an object that displays an
     * image view.
     * @param fileName a path, relative to /scratch/Images, identifying the
     * file containing an image.
     */
    QString loadFile( const QString& objectId, const QString& fileName);

    /**
     * Load a file into an image view.
     * @param objectId the unique server-side id of an object that displays an image view.
     * @param fileName a path identifying the file containing an image.
     */
    QString loadLocalFile( const QString& objectId, const QString& fileName);

    /**
     * Set the layout to a predefined analysis layout.
     */
    QString setAnalysisLayout();

    /**
     * Set the layout to a custom layout.
     */
    QString setCustomLayout( int rows, int cols );

    /**
     * Set the layout to a predefined layout displaying a single image.
     */
    QString setImageLayout();

    /**
     * Set the color map that is being displayed.
     * @param colormapId the unique server-side id of an object managing a color map.
     * @param colormapName the name of the color map.
     */
    QString setColorMap( const QString& colormapId, const QString& colormapName );

    /**
     * Reverse the current colormap.
     * @param colormapId the unique server-side id of an object managing a color map.
     * @param trueOrFalse should be equal to either "true" or "false".
     */
    QString reverseColorMap( const QString& colormapId, const QString& trueOrFalse );

    /**
     * Set caching for the current colormap
     * @param colormapId the unique server-side id of an object managing a color map.
     * @param cacheStr should be equal to either "true" or "false"
     */
    QString setCacheColormap( const QString& colormapId, const QString& cacheStr );

    /**
     * Set the cache size of the colormap
     * @param colormapId the unique server-side id of an object managing a color map.
     * @param cacheSize the desired cache size.
     */
    QString setCacheSize( const QString& colormapId, const QString& cacheSize );

    /**
     * Interpolate the current colormap.
     * @param colormapId the unique server-side id of an object managing a color map.
     * @param trueOrFalse should be equal to either "true" or "false".
     */
    QString setInterpolatedColorMap( const QString& colormapId, const QString& trueOrFalse );

    /**
     * Invert the current colormap.
     * @param colormapId the unique server-side id of an object managing a color map.
     * @param trueOrFalse should be equal to either "true" or "false".
     */
    QString invertColorMap( const QString& colormapId, const QString& trueOrFalse );

    /**
     * Set a color mix.
     * @param colormapId the unique server-side id of an object managing a color map.
     * @param percentString a formatted string specifying the blue, green, and red percentanges.
     */
    QString setColorMix( const QString& colormapId, const QString& percentString );

    /**
     * Set the gamma color map parameter.
     * @param colormapId the unique server-side id of an object managing a color map.
     * @param gamma a parameter for color mapping.
     * @return error information if gamma could not be set.
     */
    QString setGamma( const QString& colormapId, double gamma );

    /**
     * Set the name of the data transform.
     * @param colormapId the unique server-side id of an object managing a color map.
     * @param transformString a unique identifier for a data transform.
     * @return error information if the data transfrom was not set.
     */
    QString setDataTransform( const QString& colormapId, const QString& transformString );

    /**
     * Set plugins for each of the views in the layout
     * @param names a list of plugin names.
     */
    QString setPlugins( const QStringList& names );

    /**
     * Set the image frame to the specified value.
     * @param animatorId the unique server-side id of an object managing an animator.
     * @param index the frame number.
     */
    QString setFrame( const QString& animatorId, const QString& index );

    /**
     * Set the image to the specified value.
     * @param animatorId the unique server-side id of an object managing an animator.
     * @param index the image number.
     */
    QString setImage( const QString& animatorId, const QString& index );

    /**
     * Set the histogram to show the specified percentage of the data.
     * @param controlId the unique server-side id of an object managing a controller.
     * @param clipValue the percentage of data to be shown.
     */
    QString setClipValue( const QString& controlId, const QString& clipValue );

    /**
     * Save the current layout to a .json file in the /tmp directory.
     * @param fileName the base name of the file. The layout will be saved to
     * /tmp/fileName.json.
     * @return whether the operation was a success or not.
     */
    QString saveState( const QString& saveName );

    /**
     * Get the animators that are linked to the given image view.
     * @param controlId the unique server-side id of an object managing a controller.
     */
    QStringList getLinkedAnimators( const QString& controlId );

    /**
     * Get the colormaps that are linked to the given image view.
     * @param controlId the unique server-side id of an object managing a controller.
     */
    QStringList getLinkedColorMaps( const QString& controlId );

    /**
     * Get the histograms that are linked to the given image view.
     * @param controlId the unique server-side id of an object managing a controller.
     */
    QStringList getLinkedHistograms( const QString& controlId );

    /**
     * Get the statistics views that are linked to the given image view.
     * @param controlId the unique server-side id of an object managing a controller.
     */
    QStringList getLinkedStatistics( const QString& controlId );

    /*
     * Singleton accessor.
     * @return the unique instance of this object.
     */
    static ScriptFacade * getInstance ();
    virtual ~ScriptFacade(){}

private:
    Carta::Data::ViewManager* m_viewManager; //Used
    ScriptFacade();
    ScriptFacade( const ScriptFacade& other);
    ScriptFacade operator=( const ScriptFacade& other );

    /**
     * Return the unique server-side id of the object responsible for displaying an
     * image in the view.
     * @param index an index identifying the view for the case where there is more than
     *      one view displaying an image; the index is zero based for existing views whereas
     *      the default index will create a view.
     * @return the unique server side id of the object displaying the image.
     */
    QString getImageViewId( int index = -1 ) const;

    /**
     * Return the unique server side id of the object responsible for managing a color map.
     * @param index an index identifying the color map in the case that more than one
     *      color map is being displayed; an index of -1 indicates a new color map should
     *      be created whereas nonnegative indices refer to existing color maps.
     * @return the unique server side id of the object managing the color map.
     */
    QString getColorMapId( int index = -1 ) const;

    /**
     * Return the unique server side id of the object responsible for managing an animator.
     * @param index an index identifying the animator in the case that more than one
     *      animator is being displayed; an index of -1 indicates a new animator should
     *      be created whereas nonnegative indices refer to existing animators.
     * @return the unique server side id of the object managing the animator.
     */
    QString getAnimatorViewId( int index = -1 ) const;

    /**
     * Return the unique server side id of the object responsible for managing a histogram.
     * @param index an index identifying the histogram in the case that more than one
     *      histogram is being displayed; an index of -1 indicates a new histogram should
     *      be created whereas nonnegative indices refer to existing histograms.
     * @return the unique server side id of the object managing the histogram.
     */
    QString getHistogramViewId( int index = -1 ) const;

    /**
     * Return the unique server side id of the object responsible for managing a statistics view.
     * @param index an index identifying the statistics view in the case that more than one
     *      statistics view is being displayed; an index of -1 indicates a new statistics view should
     *      be created whereas nonnegative indices refer to existing statistics views.
     * @return the unique server side id of the object managing the statistics view.
     */
    QString getStatisticsViewId( int index = -1 ) const;
};

