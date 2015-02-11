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
     * Return the unique server-side id of the object responsible for displaying an
     * image in the view.
     * @param index an index identifying the view for the case where there is more than
     *      one view displaying an image; the index is zero based for existing views whereas
     *      the default index will create a view.
     * @return the unique server side id of the object displaying the image.
     */
    QString getImageViewId( int index = -1 ) const;

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
    QString linkAdd( const QString& sourceId, const QString& destId );

    /**
     * Load a file into an image view.
     * @param objectId the unique server-side id of an object that displays an image view.
     * @param fileName a path identifying the file containing an image.
     */
    void loadFile( const QString& objectId, const QString& fileName);

    /**
     * Set the layout to a predefined analysis layout.
     */
    void setAnalysisLayout();

    /**
     * Set the layout to a custom layout.
     */
    void setCustomLayout( int rows, int cols );

    /**
     * Set the layout to a predefined layout displaying a single image.
     */
    void setImageLayout();

    /**
     * Set the color map that is being displayed.
     * @param colormapId the unique server-side id of an object managing a color map.
     * @param colormapName the name of the color map.
     */
    void setColorMap( const QString& colormapId, const QString& colormapName );

    /**
     * Set plugins for each of the views in the layout
     * @param names a list of plugin names.
     */
    void setPlugins( const QStringList& names );

    /**
     * Set the image frame to the specified value.
     * @param animatorId the unique server-side id of an object managing an animator.
     * @param index the frame number.
     */
    void setFrame( const QString& animatorId, const QString& index );

    /**
     * Set the histogram to show the specified percentage of the data.
     * @param controlId the unique server-side id of an object managing a controller.
     * @param clipValue the percentage of data to be shown.
     */
    void setClipValue( const QString& controlId, const QString& clipValue );

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
};

