/***
 * Entry point into the model/controller for Python scripting
 * commands.
 *
 */

#pragma once
#include <QString>
#include <QObject>
#include "CartaLib/CartaLib.h"

namespace Carta {
    namespace Data {
        class ViewManager;
    }
}

class ScriptFacade: public QObject {

    Q_OBJECT

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
     * Establish a link between a source and destination.
     * @param sourceId the unique server side id of the link source.
     * @param destId the unique server side id of an object responsible for an image view.
     * @return an error message if the link does not succeed.
     */
    QStringList addLink( const QString& sourceId, const QString& destId );

    /**
     * Remove a link from a source to a destination.
     * @param sourceId the unique server-side id for the plugin that is the source of the link.
     * @param destId the unique server side id for the plugin that is the destination of the link.
     * @return an error message if the link does not succeed.
     */
    QStringList removeLink( const QString& sourceId, const QString& destId );

    /**
     * Set the layout to a predefined analysis layout.
     * @return currently just an empty QStringList, as the methods that it
     *  calls have void return values
     */
    QStringList setAnalysisLayout();

    /**
     * Set the layout to a custom layout.
     * @return a possible error message or an empty QStringList if there is no error.
     */
    QStringList setCustomLayout( int rows, int cols );

    /**
     * Set the layout to a predefined layout displaying a single image.
     * @return currently just an empty QStringList, as the methods that it
     *  calls have void return values
     */
    QStringList setImageLayout();

    /**
     * Load a file from /scratch/Images into an image view.
     * @param objectId the unique server-side id of an object that displays an
     * image view.
     * @param fileName a path, relative to /scratch/Images, identifying the
     * file containing an image.
     * @return an error message if there was a problem loading the file;
     *      an empty string otherwise.
     */
    QStringList loadFile( const QString& objectId, const QString& fileName);

    /**
     * Load a file into an image view.
     * @param objectId the unique server-side id of an object that displays an image view.
     * @param fileName a path identifying the file containing an image.
     * @return an error message if there was a problem loading the file;
     *      an empty string otherwise.
     */
    QStringList loadLocalFile( const QString& objectId, const QString& fileName);

    /**
     * Show the image animator.
     * @param animatorId the unique server-side id of an object managing an animator.
     * @return an error message if there was a problem showing the image animator;
     *      an empty string otherwise.
     */
    QStringList showImageAnimator( const QString& animatorId );

    /**
     * Set the color map that is being displayed.
     * @param colormapId the unique server-side id of an object managing a color map.
     * @param colormapName the name of the color map.
     * @return an error message if there was a problem setting the colormap;
     *      an empty string otherwise.
     */
    QStringList setColorMap( const QString& colormapId, const QString& colormapName );

    /**
     * Reverse the current colormap.
     * @param colormapId the unique server-side id of an object managing a color map.
     * @param reverseStr should be equal to either "true" or "false".
     * @return error information if the color map was not successfully reversed.
     */
    QStringList reverseColorMap( const QString& colormapId, const QString& reverseStr );

    /**
     * Set caching for the current colormap
     * @param colormapId the unique server-side id of an object managing a color map.
     * @param cacheStr should be equal to either "true" or "false"
     * @return error information if the cache size was not successfully set.
     */
    //QStringList setCacheColormap( const QString& colormapId, const QString& cacheStr );

    /**
     * Set the cache size of the colormap
     * @param colormapId the unique server-side id of an object managing a color map.
     * @param cacheSize the desired cache size.
     * @return error information if the cache size was not successfully set.
     */
    //QStringList setCacheSize( const QString& colormapId, const QString& cacheSize );

    /**
     * Interpolate the current colormap.
     * @param colormapId the unique server-side id of an object managing a color map.
     * @param interpolateStr should be equal to either "true" or "false".
     * @return error information if the color map was not successfully interpolated.
     */
    //QStringList setInterpolatedColorMap( const QString& colormapId, const QString& interpolateStr );

    /**
     * Invert the current colormap.
     * @param colormapId the unique server-side id of an object managing a color map.
     * @param invertStr should be equal to either "true" or "false".
     * @return error information if the color map was not successfully inverted.
     */
    QStringList invertColorMap( const QString& colormapId, const QString& invertStr );

    /**
     * Set a color mix.
     * @param colormapId the unique server-side id of an object managing a color map.
     * @param red the amount of red in the mix [0,1].
     * @param green the amount of green in the mix [0,1].
     * @param blue the amount of blue in the mix [0,1].
     * @return error information if the color mix was not successfully set.
     */
    QStringList setColorMix( const QString& colormapId, double red, double green, double blue );

    /**
     * Set the gamma color map parameter.
     * @param colormapId the unique server-side id of an object managing a color map.
     * @param gamma a parameter for color mapping.
     * @return error information if gamma could not be set.
     */
    QStringList setGamma( const QString& colormapId, double gamma );

    /**
     * Set the name of the data transform.
     * @param colormapId the unique server-side id of an object managing a color map.
     * @param transformString a unique identifier for a data transform.
     * @return error information if the data transfrom was not set.
     */
    QStringList setDataTransform( const QString& colormapId, const QString& transformString );

    /**
     * Set plugins for each of the views in the layout.
     * @param names a list of plugin names.
     * @return error information if plugins could not be set.
     */
    QStringList setPlugins( const QStringList& names );

    /**
     * Set the image channel to the specified value.
     * @param animatorId the unique server-side id of an object managing an animator.
     * @param index the channel number.
     * @return error information if the channel could not be set.
     */
    QStringList setChannel( const QString& animatorId, int index );

    /**
     * Set the image to the specified value.
     * @param animatorId the unique server-side id of an object managing an animator.
     * @param index the image number.
     * @return error information if the image could not be set.
     */
    QStringList setImage( const QString& animatorId, int index );

    /**
     * Set the histogram to show the specified percentage of the data.
     * @param controlId the unique server-side id of an object managing a controller.
     * @param clipValue the percentage of data to be shown.
     * @return error information if the clip value could not be set.
     */
    QStringList setClipValue( const QString& controlId, double clipValue );

    /**
     * Save a screenshot of the current image view.
     * @param controlId the unique server-side id of an object managing a controller.
     * @param fileName the full path where the file is to be saved.
     * @return an error message if there was a problem saving the image;
     *      an empty string otherwise.
     */
    QStringList saveImage( const QString& controlId, const QString& fileName );

    /**
     * Save a copy of the full image in the current image view.
     * @param controlId the unique server-side id of an object managing a controller.
     * @param filename the full path where the file is to be saved.
     * @param width the width of the saved image.
     * @param height the height of the saved image.
     * @param scale the scale (zoom level) of the saved image.
     * @param aspectRatioMode can be either "ignore", "keep", or "expand".
            See http://doc.qt.io/qt-5/qt.html#AspectRatioMode-enum for further information.
     */
    void saveFullImage( const QString& controlId, const QString& filename, int width, int height, double scale, Qt::AspectRatioMode aspectRatioMode );

    /**
     * Save the current state.
     * @param fileName - an identifier for the state to be saved.
     * @param layoutSave - true if the layout should be saved; false otherwise.
     * @param preferencesSave -true if the preferences should be saved; false otherwise.
     * @param dataSave - true if the data should be saved; false otherwise.
     * @param saveDescription - notes about the state being saved.
     * @return an error message if there was a problem saving state; an empty list otherwise.
     */
    QStringList saveSnapshot( const QString& sessionId, const QString& saveName, bool saveLayout,
            bool savePreferences, bool saveData, const QString& description );

    /**
     * Returns a list of the names of available snapshots
     * @param sessionId - an identifier for a user session.
     * @return a list of the names of supported snapshots.
     */
    QStringList getSnapshots(const QString& sessionId );

    /**
     * Returns a list of the available snapshots
     * @param sessionId - an identifier for a user session.
     * @return a list of supported snapshots.
     */
    QStringList getSnapshotObjects(const QString& sessionId );

    /**
     * Delete the snapshot with the given identifier.
     * @param sessionId an identifier for a user session.
     * @param saveName an identifier for the snapshot to delete.
     * @return an empty list if the snapshot was deleted; an error message if
     *      there was a problem deleting the snapshot.
     */
    QStringList deleteSnapshot( const QString& sessionId, const QString& saveName );

    /**
     * Read and restore state for a particular sessionId from a string.
     * @param sessionId an identifier for a user session.
     * @param saveName an identifier for the snapshot to restore.
     * @return an empty list if the snapshot was restored; an error message if
     *      there was a problem restoring the snapshot.
     */
    QStringList restoreSnapshot( const QString& sessionId, const QString& saveName );

    /**
     * Get the animators that are linked to the given image view.
     * @param controlId the unique server-side id of an object managing a controller.
     * @return a list of ID values for the linked animators, or a list of a single
     *      empty string if no linked animators were found.
     */
    QStringList getLinkedAnimators( const QString& controlId );

    /**
     * Get the colormaps that are linked to the given image view.
     * @param controlId the unique server-side id of an object managing a controller.
     * @return a list of ID values for the linked colormaps, or a list of a single
     *      empty string if no linked colormaps were found.
     */
    QStringList getLinkedColorMaps( const QString& controlId );

    /**
     * Get the histograms that are linked to the given image view.
     * @param controlId the unique server-side id of an object managing a controller.
     * @return a list of ID values for the linked histograms, or a list of a single
     *      empty string if no linked histograms were found.
     */
    QStringList getLinkedHistograms( const QString& controlId );

    /**
     * Get the statistics views that are linked to the given image view.
     * @param controlId the unique server-side id of an object managing a controller.
     * @return a list of ID values for the linked statistics, or a list of a single
     *      empty string if no linked statistics were found.
     */
    QStringList getLinkedStatistics( const QString& controlId );

    /**
     * Center the image on the pixel with coordinates (x, y).
     * @param controlId the unique server-side id of an object managing a controller.
     * @param x the x-coordinate for the center of the pan.
     * @param y the y-coordinate for the center of the pan.
     * @return error information if the image could not be centered on the given pixel.
     */
    QStringList centerOnPixel( const QString& controlId, double x, double y );

    /**
     * Set the zoom level.
     * @param controlId the unique server-side id of an object managing a controller.
     * @param zoomLevel either positive or negative depending on the desired zoom direction.
     * @return error information if the zoom level could not be set.
     */
    QStringList setZoomLevel( const QString& controlId, double zoomLevel );

    /**
     * Get the current zoom level.
     * @param controlId the unique server-side id of an object managing a controller.
     * @return the zoom level, or error information if it could not be obtained.
     */
    QStringList getZoomLevel( const QString& controlId );

    /**
     * Get the image dimensions.
     * @param controlId the unique server-side id of an object managing a controller.
     * @return a list of the image dimensions, or error information if the
     *      dimensions could not be obtained.
     */
    QStringList getImageDimensions( const QString& controlId );

    /**
     * Get the dimensions of the image viewer (window size).
     * @param controlId the unique server-side id of an object managing a controller.
     * @return the width and height of the image viewer as a list, or error
     *      information if the output size could not be obtained.
     */
    QStringList getOutputSize( const QString& controlId );

    /**
     * Return the pixel coordinates corresponding to the given world coordinates.
     * @param controlId the unique server-side id of an object managing a controller.
     * @param ra the right ascension (in radians) of the world coordinates.
     * @param dec the declination (in radians) of the world coordinates.
     * @return a list consisting of the x- and y-coordinates of the pixel
     *  corresponding to the given world coordinates.
     */
    QStringList getPixelCoordinates( const QString& controlId, double ra, double dec );

    /**
     * Return the value of the pixel at (x, y).
     * @param controlId the unique server-side id of an object managing a controller.
     * @param x the x-coordinate of the desired pixel.
     * @param y the y-coordinate of the desired pixel.
     * @return the value of the pixel at (x, y), or blank if it could not be obtained.
     */
    QStringList getPixelValue( const QString& controlId, double x, double y );

    /**
     * Return the units of the pixels.
     * @param controlId the unique server-side id of an object managing a controller.
     * @return the units of the pixels, or blank if units could not be obtained.
     */
    QStringList getPixelUnits( const QString& controlId );

    /**
     * Return the coordinates at pixel (x, y) in the given coordinate system.
     * @param controlId the unique server-side id of an object managing a controller.
     * @param x the x-coordinate of the desired pixel.
     * @param y the y-coordinate of the desired pixel.
     * @param system the desired coordinate system.
     * @return the coordinates at pixel (x, y).
     */
    QStringList getCoordinates( const QString& controlId, double x, double y, Carta::Lib::KnownSkyCS system );

    /**
     * Return a list of the images open in the specified image view.
     * @param controlId the unique server-side id of an object managing a controller.
     * @return the names of the images that are currently open in the specified image view.
     */
    QStringList getImageNames( const QString& controlId );

    /**
     * Close the specified image.
     * @param controlId the unique server-side id of an object managing a controller.
     * @param imageName an identifier for the image to close.
     * @return error information if the image could not be closed.
     */
    QStringList closeImage( const QString& controlId, const QString& imageName );

    /**
     * Set the amount of extra space on each side of the clip bounds.
     * @param histogramId the unique server-side id of an object managing a histogram.
     * @param bufferAmount a percentage in [0,100) representing the amount of extra space.
     * @return an error message if the clip buffer was not successfully set; an empty string otherwise.
     */
    QStringList setClipBuffer( const QString& histogramId, int bufferAmount );

    /**
     * Set whether or not to show extra space on each side of the clip bounds.
     * @param histogramId the unique server-side id of an object managing a histogram.
     * @param useBuffer true if extra space should be shown; false otherwise.
     * @return an error message if there was a problem; an empty string if the flag was set successfully.
     */
    QStringList setUseClipBuffer( const QString& histogramId, const QString& useBuffer );

    /**
     * Set the lower and upper bounds for the histogram horizontal axis.
     * @param histogramId the unique server-side id of an object managing a histogram.
     * @param minRange a lower bound for the histogram horizontal axis.
     * @param maxRange an upper bound for the histogram horizontal axis.
     * @return an error message if there was a problem setting the range; an empty string otherwise.
     */
    QStringList setClipRange( const QString& histogramId, double minRange, double maxRange );

    /**
     * Applies clips to image.
     * @param histogramId the unique server-side id of an object managing a histogram.
     * @param clipMinValue the minimum of data to be shown.
     * @param clipMaxValue the maximum of data to be shown.
     * @param mode can be either "percent" or "intensity"
     * @return an error message if there was a problem applying the clips; an empty string otherwise.
     */
    QStringList applyClips( const QString& histogramId, double clipMinPercent, double clipMaxPercent, QString mode );

    /**
     * Returns the intensity corresponding to a given percentile.
     * @param controlId the unique server-side id of an object managing a controller.
     * @param frameLow a lower bound for the image channels or -1 if there is no lower bound.
     * @param frameHigh an upper bound for the image channels or -1 if there is no upper bound.
     * @param percentile a number [0,1] for which an intensity is desired.
     */
    QStringList getIntensity( const QString& controlId, int frameLow, int frameHigh, double percentile ); 

    /**
     * Set the number of bins in the histogram.
     * @param histogramId the unique server-side id of an object managing a histogram.
     * @param binCount the number of histogram bins.
     * @return an error message if there was a problem setting the bin count; an empty string otherwise.
     */
    QStringList setBinCount( const QString& histogramId, int binCount );

    /**
     * Set the width of the histogram bins.
     * @param histogramId the unique server-side id of an object managing a histogram.
     * @param binWidth the histogram bin width.
     * @return an error message if there was a problem setting the bin width; an empty string otherwise.
     */
    QStringList setBinWidth( const QString& histogramId, double binWidth );

    /**
     * Set whether the histogram should be based on a single plane, a range of planes, or the entire cube.
     * @param histogramId the unique server-side id of an object managing a histogram.
     * @param planeMode a unique identifier for the 3D data range.
     * @return an error message if there was a problem setting the 3D data range; an empty string otherwise.
     */
    QStringList setPlaneMode( const QString& histogramId, const QString& planeMode );

    /**
     * Set the range of channels to include as data in generating the histogram.
     * @param histogramId the unique server-side id of an object managing a histogram.
     * @param minPlane the minimum frequency (GHz) to include.
     * @param maxPlane the maximum frequency (GHz) to include.
     * @return an error message if there was a problem setting the frame range; an empty string otherwise.
     */
    QStringList setPlaneRange( const QString& histogramId, double minPlane, double maxPlane );

    /**
     * Set the unit used to specify a channel range, for example, "GHz".
     * @param histogramId the unique server-side id of an object managing a histogram.
     * @param units the channel units used to specify a range.
     * @return an error message if there was a problem setting the channel units;
     *      otherwise and empty string.
     */
    QStringList setChannelUnit( const QString& histogramId, const QString& units );

    /**
     * Set the drawing style for the histogram (outline, filled, etc).
     * @param histogramId the unique server-side id of an object managing a histogram.
     * @param style a unique identifier for a histogram drawing style.
     * @return an error message if there was a problem setting the draw style; an empty string otherwise.
     */
    QStringList setGraphStyle( const QString& histogramId, const QString& style );

    /**
     * Set whether or not the histogram's vertical axis should use a log scale.
     * @param histogramId the unique server-side id of an object managing a histogram.
     * @param logCountStr true if the vertical axis should be logarithmic; false otherwise.
     *  Can also be equal to "toggle" to turn the log scale on or off depending on its
     *  current value.
     * @return an error message if there was a problem setting the flag; an empty string otherwise.
     */
    QStringList setLogCount( const QString& histogramId, const QString& logCountStr );

    /**
     * Set whether or not the histogram should be colored by intensity.
     * @param histogramId the unique server-side id of an object managing a histogram.
     * @param colored true if the histogram should be colored by intensity; false otherwise.
     *  Can also be equal to "toggle" to turn the coloring on or off depending on its
     *  current value.
     * @return an error message if there was a problem setting the flag; an empty string otherwise.
     */
    QStringList setColored( const QString& histogramId, const QString& colored );

    /**
     * Save a copy of the histogram as an image.
     * @param histogramId the unique server-side id of an object managing a histogram.
     * @param filename the full path where the file is to be saved.
     * @param width the width, in pixels, of the saved image.
     * @param height the height, in pixels, of the saved image.
     * @return an error message if there was a problem saving the histogram;
     *      an empty string otherwise.
     */
    QStringList saveHistogram( const QString& histogramId, const QString& filename, int width, int height );

    /**
     * Set the grid axes color.
     * @param red- an integer in [0, 255] indicating the amount of red.
     * @param green - an integer in [0,255] indicating the amount of green.
     * @param blue - an integer in [0,255] indicating the amount of blue.
     * @return a list of errors or an empty list if the color was successfully set.
     */
    QStringList setGridAxesColor( const QString& controlId, int red, int green, int blue );

    /**
     * Set whether or not the grid lines should be shown.
     * @param controlId the unique server-side id of an object managing a controller.
     * @param showGridLines - true if the grid lines should be shown; false otherwise.
     * @return an error message if there was a problem changing the visibility of the
     *     grid; an empty string otherwise.
     */
    QStringList setShowGridLines( const QString& controlId, bool showGridLines );

    /*
     * Singleton accessor.
     * @return the unique instance of this object.
     */
    static ScriptFacade * getInstance ();
    virtual ~ScriptFacade(){}

signals:

    /// Return the result of SaveFullImage() after the image has been rendered
    /// and a save attempt made.
    void saveImageResult( bool result );

private slots:

    // Asynchronous result from saveFullImage().
    void saveImageResultCB( bool result );

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

private:
    static QString TOGGLE;
};

