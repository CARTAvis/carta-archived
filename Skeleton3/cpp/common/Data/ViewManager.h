/***
 * Main class that manages the data state for the views.
 *
 */

#pragma once

#include "State/StateInterface.h"
#include "State/ObjectManager.h"

namespace Carta {

namespace Data {

class Animator;
class Controller;
class DataLoader;
class Histogram;
class Colormap;
class Layout;
class Statistics;
class ViewPlugins;

class ViewManager : public CartaObject {

public:

    virtual ~ViewManager(){}

    /**
     * Return the unique server side id of the object with the given name and index in the
     * layout.
     * @param pluginName an identifier for the kind of object.
     * @param index an index in the case where there is more than one object of the given kind
     *      in the layout.
     */
    QString getObjectId( const QString& pluginName, int index );

    /**
     * Link a source plugin to a destination plugin.
     * @param sourceId the unique server-side id for the plugin that is the source of the link.
     * @param destId the unique server side id for the plugin that is the destination of the link.
     * @return an error message if the link does not succeed.
     */
    QString linkAdd( const QString& sourceId, const QString& destId );

    /**
     * Remove a link from a source to a destination.
     * @param sourceId the unique server-side id for the plugin that is the source of the link.
     * @param destId the unique server side id for the plugin that is the destination of the link.
     * @return an error message if the link does not succeed.
     */
    QString linkRemove( const QString& sourceId, const QString& destId );

    /**
     * Return a list of filenames that can be loaded into the image viewer.
     */
    QString getFileList();

    /**
     * Return the number of controllers (image views).
     */
    int getControllerCount() const;

    /**
     * Return the number of colormap views.
     */
    int getColorMapCount() const;

    /**
     * Return the number of animator views.
     */
    int getAnimatorCount() const;

    /**
     * Return the number of histogram views.
     */
    int getHistogramCount() const;

    /**
     * Return the number of statistics views.
     */
    int getStatisticsCount() const;

    /**
     * Load the file into the controller with the given id.
     * @param fileName a locater for the data to load.
     * @param objectId the unique server side id of the controller which is
     * responsible for displaying the file.
     */
    void loadFile( const QString& objectId, const QString& fileName);

    /**
     * Load a local file into the controller with the given id.
     * @param fileName a locater for the data to load.
     * @param objectId the unique server side id of the controller which is
     * responsible for displaying the file.
     */
    void loadLocalFile( const QString& objectId, const QString& fileName);

    /**
     * Reset the layout to a predefined analysis view.
     */
    void setAnalysisView();

    /**
     * Set the number of rows and columns in the layout grid.
     * @param rows the number of rows in the grid.
     * @param cols the number of columns in the grid.
     */
    void setCustomView( int rows, int cols );

    /**
     * Change the color map to the map with the given name.
     * @param colormapId the unique server-side id of a Colormap object.
     * @param colormapName a unique identifier for the color map to be displayed.
     */
    bool setColorMap( const QString& colormapId, const QString& colormapName );

    /**
     * Set the image frame to the specified value.
     * @param animatorId the unique server-side id of an object managing an animator.
     * @param index the frame number.
     */
    bool setFrame( const QString& animatorId, const QString& index );

    /**
     * Reset the layout to a predefined view displaying only a single image.
     */
    void setImageView();

    /**
     * Set plugins for each of the views in the layout
     * @param names a list of plugin names.
     */
    void setPlugins( const QStringList& names );

    /**
     * Set the histogram to show the specified percentage of the data.
     * @param controlId the unique server-side id of an object managing a controller.
     * @param clipValue the percentage of data to be shown.
     */
    void setClipValue( const QString& controlId, const QString& param );

    static const QString CLASS_NAME;

private:
    ViewManager( const QString& path, const QString& id);
    class Factory;

    void _clear();

    int _findAnimator( const QString& id ) const;
    int _findColorMap( const QString& id ) const;
    int _findController( const QString& id ) const;


    void _initCallbacks();

    //void _initializeExistingAnimationLinks( int index );
    //Sets up a default set of states for constructing the UI if the user
    //has not saved one.
    void _initializeDefaultState();

    QString _makeAnimator();
    QString _makeLayout();
    QString _makePluginList();
    QString _makeController();
    QString _makeHistogram();
    QString _makeColorMap();
    QString _makeStatistics();
    void _makeDataLoader();


    bool _readState( const QString& fileName );
    bool _saveState( const QString& fileName );

    //A list of Controllers requested by the client.
    QList <std::shared_ptr<Controller> > m_controllers;

    //A list of Animators requested by the client.
    QList < std::shared_ptr<Animator> > m_animators;

    //Colormap
    QList<std::shared_ptr<Colormap>  >m_colormaps;

    //Histogram
    QList<std::shared_ptr<Histogram> >m_histograms;

    //Statistics
    QList<std::shared_ptr<Statistics> > m_statistics;

    static bool m_registered;
    std::shared_ptr<Layout> m_layout;
    std::shared_ptr<DataLoader> m_dataLoader;
    std::shared_ptr<ViewPlugins> m_pluginsLoaded;

    const static QString SOURCE_ID;
    const static QString DEST_ID;

    ViewManager( const ViewManager& other);
    ViewManager operator=( const ViewManager& other );
};

}
}
