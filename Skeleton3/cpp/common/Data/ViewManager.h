/***
 * Main class that manages the data state for the views.
 *
 */

#pragma once

#include "State/StateInterface.h"
#include "State/ObjectManager.h"
#include <QVector>
#include <QObject>

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

class ViewManager : public QObject, public CartaObject {

    Q_OBJECT

public:
    /**
     * Return the unique server side id of the object with the given name and index in the
     * layout.
     * @param pluginName an identifier for the kind of object.
     * @param index an index in the case where there is more than one object of the given kind
     *      in the layout.
     */
    QString getObjectId( const QString& pluginName, int index, bool forceCreate = false );

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
     * Reset the layout to show objects under active development.
     */
    void setDeveloperView();

    /**
     * Change the color map to the map with the given name.
     * @param colormapId the unique server-side id of a Colormap object.
     * @param colormapName a unique identifier for the color map to be displayed.
     */
    bool setColorMap( const QString& colormapId, const QString& colormapName );

    /**
     * Reverse the current colormap.
     * @param colormapId the unique server-side id of an object managing a color map.
     * @param trueOrFalse should be equal to either "true" or "false".
     */
    bool reverseColorMap( const QString& colormapId, const QString& trueOrFalse );

    /**
     * Set caching for the current colormap.
     * @param colormapId the unique server-side id of an object managing a color map.
     * @param cacheStr should be equal to "true" or "false".
     */
    QString setCacheColormap( const QString& colormapId, const QString& cacheStr );

    /**
     * Set the cache size of the color map.
     * @param colormapId the unique server-side id of an object managing a color map.
     * @param percentString the desired size of the cache
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
    bool invertColorMap( const QString& colormapId, const QString& trueOrFalse );

    /**
     * Set a color mix.
     * @param colormapId the unique server-side id of an object managing a color map.
     * @param percentString a formatted string specifying the blue, green, and red percentanges.
     */
    bool setColorMix( const QString& colormapId, const QString& percentString );

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
     * Set the image frame to the specified value.
     * @param animatorId the unique server-side id of an object managing an animator.
     * @param index the frame number.
     */
    bool setChannel( const QString& animatorId, int index );

    /**
     * Set the image to the specified value.
     * @param animatorId the unique server-side id of an object managing an animator.
     * @param index the image number.
     */
    bool setImage( const QString& animatorId, int index );

    /**
     * Reset the layout to a predefined view displaying only a single image.
     */
    void setImageView();

    /**
     * Set the histogram to show the specified percentage of the data.
     * @param controlId the unique server-side id of an object managing a controller.
     * @param clipValue the percentage of data to be shown.
     */
    void setClipValue( const QString& controlId, const QString& param );

    /**
     * Save a screenshot of the current image view.
     * @param controlId the unique server-side id of an object managing a controller.
     * @param filename the full path where the file is to be saved.
     * @return an error message if there was a problem saving the image;
     *      an empty string otherwise.
     */
    bool saveImage( const QString& controlId, const QString& filename );

    /**
     * Save a copy of the full image in the current image view at its native resolution.
     * @param controlId the unique server-side id of an object managing a controller.
     * @param fileName the full path where the file is to be saved.
     * @param scale the scale (zoom level) of the saved image.
     * @return an error message if there was a problem saving the image;
     *      an empty string otherwise.
     */
    bool saveFullImage( const QString& controlId, const QString& filename, double scale );

    /**
     * Get the colormaps that are linked to the given image view.
     * @param controlId the unique server-side id of an object managing a controller.
     */
    QStringList getLinkedColorMaps( const QString& controlId );

    /**
     * Get the animators that are linked to the given image view.
     * @param controlId the unique server-side id of an object managing a controller.
     */
    QStringList getLinkedAnimators( const QString& controlId );

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

    /**
     * Center the image on the pixel with coordinates (x, y).
     * @param controlId the unique server-side id of an object managing a controller.
     * @param x the x-coordinate for the center of the pan.
     * @param y the y-coordinate for the center of the pan.
     */
    QString centerOnPixel( const QString& controlId, double x, double y );

    /**
     * Set the zoom level.
     * @param controlId the unique server-side id of an object managing a controller.
     * @param zoomLevel either positive or negative depending on the desired zoom direction.
     */
    QString setZoomLevel( const QString& controlId, double zoomLevel );

    /**
     * Get the current zoom level.
     * @param controlId the unique server-side id of an object managing a controller.
     */
    double getZoomLevel( const QString& controlId );

    /**
     * Get the image dimensions.
     * @param controlId the unique server-side id of an object managing a controller.
     */
    QStringList getImageDimensions( const QString& controlId );

    /**
     * Get the dimensions of the image viewer (window size).
     * @param controlId the unique server-side id of an object managing a controller.
     */
    QStringList getOutputSize( const QString& controlId );

    /**
     * Set the list of plugins to be displayed.
     * @param names a list of identifiers for the plugins.
     */
    bool setPlugins( const QStringList& names );

    /**
     * Set the amount of extra space on each side of the clip bounds.
     * @param histogramId the unique server-side id of an object managing a histogram.
     * @param bufferAmount a percentage in [0,100) representing the amount of extra space.
     * @return an error message if the clip buffer was not successfully set; an empty string otherwise.
     */
    QString setClipBuffer( const QString& histogramId, int bufferAmount );

    /**
     * Set whether or not to show extra space on each side of the clip bounds.
     * @param histogramId the unique server-side id of an object managing a histogram.
     * @param useBuffer true if extra space should be shown; false otherwise.
     * @return an error message if there was a problem; an empty string if the flag was set successfully.
     */
    QString setUseClipBuffer( const QString& histogramId, const QString& useBufferStr );

    /**
     * Set the lower and upper bounds for the histogram horizontal axis.
     * @param histogramId the unique server-side id of an object managing a histogram.
     * @param minRange a lower bound for the histogram horizontal axis.
     * @param maxRange an upper bound for the histogram horizontal axis.
     * @return an error message if there was a problem setting the range; an empty string otherwise.
     */
    QString setClipRange( const QString& histogramId, double minRange, double maxRange );

    /**
     * Applies clips to image.
     * @param histogramId the unique server-side id of an object managing a histogram.
     * @param clipMinValue the minimum of data to be shown.
     * @param clipMaxValue the maximum of data to be shown.
     * @param mode can be either "percent" or "intensity"
     */
    QString applyClips( const QString& histogramId, double clipMinValue, double clipMaxValue, QString mode );

    /**
     * Returns the intensity corresponding to a given percentile.
     * @param controlId the unique server-side id of an object managing a controller.
     * @param frameLow a lower bound for the image channels or -1 if there is no lower bound.
     * @param frameHigh an upper bound for the image channels or -1 if there is no upper bound.
     * @param percentile a number [0,1] for which an intensity is desired.
     */
    QString getIntensity( const QString& controlId, int frameLow, int frameHigh, double percentile ); 

    /**
     * Set the number of bins in the histogram.
     * @param histogramId the unique server-side id of an object managing a histogram.
     * @param binCount the number of histogram bins.
     * @return an error message if there was a problem setting the bin count; an empty string otherwise.
     */
    QString setBinCount( const QString& histogramId, int binCount );

    /**
     * Set the width of the histogram bins.
     * @param histogramId the unique server-side id of an object managing a histogram.
     * @param binWidth the histogram bin width.
     * @return an error message if there was a problem setting the bin width; an empty string otherwise.
     */
    QString setBinWidth( const QString& histogramId, double binWidth );

    /**
     * Set whether the histogram should be based on a single plane, a range of planes, or the entire cube.
     * @param histogramId the unique server-side id of an object managing a histogram.
     * @param planeMode a unique identifier for the 3D data range.
     * @return an error message if there was a problem setting the 3D data range; an empty string otherwise.
     */
    QString setPlaneMode( const QString& histogramId, const QString& planeMode );

    /**
     * Set the range of channels to include as data in generating the histogram.
     * @param histogramId the unique server-side id of an object managing a histogram.
     * @param minPlane the minimum frequency (GHz) to include.
     * @param maxPlane the maximum frequency (GHz) to include.
     * @return an error message if there was a problem setting the frame range; an empty string otherwise.
     */
    QString setPlaneRange( const QString& histogramId, double minPlane, double maxPlane );

    /**
     * Set the unit used to specify a channel range, for example, "GHz".
     * @param histogramId the unique server-side id of an object managing a histogram.
     * @param units the channel units used to specify a range.
     * @return an error message if there was a problem setting the channel units;
     *      otherwise and empty string.
     */
    QString setChannelUnit( const QString& histogramId, const QString& units );

    /**
     * Set the drawing style for the histogram (outline, filled, etc).
     * @param style a unique identifier for a histogram drawing style.
     * @return an error message if there was a problem setting the draw style; an empty string otherwise.
     */
    QString setGraphStyle( const QString& histogramId, const QString& graphStyle );

    /**
     * Set whether or not the histogram's vertical axis should use a log scale.
     * @param histogramId the unique server-side id of an object managing a histogram.
     * @param logCount true if the vertical axis should be logarithmic; false otherwise.
     *  Can also be equal to "toggle" to turn the log scale on or off depending on its
     *  current value.
     * @return an error message if there was a problem setting the flag; an empty string otherwise.
     */
    QString setLogCount( const QString& histogramId, const QString& logCountStr );

    /**
     * Set where or not the histogram should be colored by intensity.
     * @param histogramId the unique server-side id of an object managing a histogram.
     * @param colored true if the histogram should be colored by intensity; false otherwise.
     *  Can also be equal to "toggle" to turn the coloring on or off depending on its
     *  current value.
     * @return an error message if there was a problem setting the flag; an empty string otherwise.
     */
    QString setColored( const QString& histogramId, const QString& coloredStr );

    /**
     * Save a copy of the histogram as an image.
     * @param histogramId the unique server-side id of an object managing a histogram.
     * @param filename the full path where the file is to be saved.
     * @param width the width, in pixels, of the saved image.
     * @param height the height, in pixels, of the saved image.
     * @return an error message if there was a problem saving the histogram;
     *      an empty string otherwise.
     */
    QString saveHistogram( const QString& histogramId, const QString& filename, int width, int height );

    static const QString CLASS_NAME;

    /**
     * Destructor.
     */
    virtual ~ViewManager();
private slots:
    void _pluginsChanged( const QStringList& names, const QStringList& oldNames );
private:
    ViewManager( const QString& path, const QString& id);
    class Factory;
    void _adjustSize( int count, const QString& name, const QVector<int>& insertionIndices);
    void _clear();
    void _clearAnimators( int startIndex );
    void _clearColormaps( int startIndex );
    void _clearControllers( int startIndex );
    void _clearHistograms( int startIndex );
    void _clearStatistics( int startIndex );

    int _findColorMap( const QString& id ) const;
    int _findAnimator( const QString& id ) const;

    void _initCallbacks();

    //Sets up a default set of states for constructing the UI if the user
    //has not saved one.
    void _initializeDefaultState();

    QString _makeAnimator( int index );
    QString _makeLayout();
    QString _makePluginList();
    QString _makeController( int index );
    QString _makeHistogram( int index );
    QString _makeColorMap( int index );
    QString _makeStatistics( int index );
    void _makeDataLoader();


    void _removeView( const QString& plugin, int index );
    /**
     * Written because there is no guarantee what order the javascript side will use
     * to create view objects.  When there are linked views, the links may not get
     * recorded if one object is to be linked with one not yet created.  This flushes
     * the state and gives the object a second chance to establish their links.
     */
    void _refreshState();

    /**
     * Read and restore state for a particular sessionId from a file.
     * @param sessionId - an identifier for a user session.
     * @param fileName - the name of a saved session state.
     * @return true if the state was read and restored; false otherwise.
     */
    bool _readState( const QString& sessionId, const QString& fileName );

    /**
     * Read and restore the layout state for a particular sessionId from a file.
     * @param sessionId - an identifier for a user session.
     * @param saveName - the name of a file containing the layout state.
     * @return true if the layout state was read and restored; false otherwise.
     */
    bool _readStateLayout( const QString& sessionId, const QString& saveName );

    /**
     * Read and restore state for a particular sessionId from a string.
     * @param stateStr - a string representation of the state.
     * @param type - the type of state.
     * @return true if the state was read and restored; false otherwise.
     */
    bool _readState( const QString& stateStr, SnapshotType type );

    /**
     * Save the current state.
     * @param fileName - an identifier for the state to be saved.
     * @param layoutSave - true if the layout should be saved; false otherwise.
     * @param preferencesSave -true if the preferences should be saved; false otherwise.
     * @param dataSave - true if the data should be saved; false otherwise.
     * @return an error message if there was a problem saving state; an empty string otherwise.
     */
    QString saveState( const QString& sessionId, const QString& fileName, bool layoutSave, bool preferencesSave, bool dataSave );

    //A list of Controllers requested by the client.
    QList <Controller* > m_controllers;

    //A list of Animators requested by the client.
    QList < Animator* > m_animators;

    //Colormap
    QList<Colormap* >m_colormaps;

    //Histogram
    QList<Histogram* >m_histograms;

    //Statistics
    QList<Statistics* > m_statistics;

    static bool m_registered;
    Layout* m_layout;
    DataLoader* m_dataLoader;
    ViewPlugins* m_pluginsLoaded;

    const static QString SOURCE_ID;
    const static QString DEST_ID;

    ViewManager( const ViewManager& other);
    ViewManager operator=( const ViewManager& other );
};

}
}
