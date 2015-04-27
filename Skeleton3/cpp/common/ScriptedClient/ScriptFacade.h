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
        class Animator;
    }
}

class ScriptFacade {

public:
    /**
     * Return a container containing animators.
     * @param index the ID of the desired animator.
     * @return the first animator container at the given index.
     */
    Carta::Data::Animator* getAnimator( const QString& index );

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
    QStringList getFileList() const;

    /**
     * Establish a link between a source and destination.
     * @param sourceId the unique server side id of the link source.
     * @param destId the unique server side id of an object responsible for an image view.
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
     * Load a file from /scratch/Images into an image view.
     * @param objectId the unique server-side id of an object that displays an
     * image view.
     * @param fileName a path, relative to /scratch/Images, identifying the
     * file containing an image.
     */
    QStringList loadFile( const QString& objectId, const QString& fileName);

    /**
     * Load a file into an image view.
     * @param objectId the unique server-side id of an object that displays an image view.
     * @param fileName a path identifying the file containing an image.
     */
    QStringList loadLocalFile( const QString& objectId, const QString& fileName);

    /**
     * Show the image animator.
     * @param animatorId the unique server-side id of an object managing an animator.
     */
    QStringList showImageAnimator( const QString& animatorId );

    /**
     * Set the layout to a predefined analysis layout.
     */
    QStringList setAnalysisLayout();

    /**
     * Set the layout to a custom layout.
     */
    QStringList setCustomLayout( int rows, int cols );

    /**
     * Set the layout to a predefined layout displaying a single image.
     */
    QStringList setImageLayout();

//    /**
//     * Set an animator channel.
//     * @param channel the channel to set.
//     */
//    void setChannel( int channel );

    /**
     * Set the color map that is being displayed.
     * @param colormapId the unique server-side id of an object managing a color map.
     * @param colormapName the name of the color map.
     */
    QStringList setColorMap( const QString& colormapId, const QString& colormapName );

    /**
     * Reverse the current colormap.
     * @param colormapId the unique server-side id of an object managing a color map.
     * @param reverseStr should be equal to either "true" or "false".
     */
    QStringList reverseColorMap( const QString& colormapId, const QString& reverseStr );

    /**
     * Set caching for the current colormap
     * @param colormapId the unique server-side id of an object managing a color map.
     * @param cacheStr should be equal to either "true" or "false"
     */
    QStringList setCacheColormap( const QString& colormapId, const QString& cacheStr );

    /**
     * Set the cache size of the colormap
     * @param colormapId the unique server-side id of an object managing a color map.
     * @param cacheSize the desired cache size.
     */
    QStringList setCacheSize( const QString& colormapId, const QString& cacheSize );

    /**
     * Interpolate the current colormap.
     * @param colormapId the unique server-side id of an object managing a color map.
     * @param interpolateStr should be equal to either "true" or "false".
     */
    QStringList setInterpolatedColorMap( const QString& colormapId, const QString& interpolateStr );

    /**
     * Invert the current colormap.
     * @param colormapId the unique server-side id of an object managing a color map.
     * @param invertStr should be equal to either "true" or "false".
     */
    QStringList invertColorMap( const QString& colormapId, const QString& invertStr );

    /**
     * Set a color mix.
     * @param colormapId the unique server-side id of an object managing a color map.
     * @param percentString a formatted string specifying the blue, green, and red percentanges.
     */
    QStringList setColorMix( const QString& colormapId, const QString& percentString );

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
     * Set plugins for each of the views in the layout
     * @param names a list of plugin names.
     */
    QStringList setPlugins( const QStringList& names );

    /**
     * Set the image channel to the specified value.
     * @param animatorId the unique server-side id of an object managing an animator.
     * @param index the channel number.
     */
    QStringList setChannel( const QString& animatorId, int index );

    /**
     * Set the image to the specified value.
     * @param animatorId the unique server-side id of an object managing an animator.
     * @param index the image number.
     */
    QStringList setImage( const QString& animatorId, int index );

    /**
     * Set the histogram to show the specified percentage of the data.
     * @param controlId the unique server-side id of an object managing a controller.
     * @param clipValue the percentage of data to be shown.
     */
    QStringList setClipValue( const QString& controlId, const QString& clipValue );

    /**
     * Save a screenshot of the current image view.
     * @param controlId the unique server-side id of an object managing a controller.
     * @param fileName the full path where the file is to be saved.
     * @return an error message if there was a problem saving the image;
     *      an empty string otherwise.
     */
    QStringList saveImage( const QString& controlId, const QString& fileName );

    /**
     * Save a copy of the full image in the current image view at its native resolution.
     * @param controlId the unique server-side id of an object managing a controller.
     * @param fileName the full path where the file is to be saved.
     * @param scale the scale (zoom level) of the saved image.
     * @return an error message if there was a problem saving the image;
     *      an empty string otherwise.
     */
    QStringList saveFullImage( const QString& controlId, const QString& fileName, double scale );

//    /**
//     * Save the current layout to a .json file in the /tmp directory.
//     * @param fileName the base name of the file. The layout will be saved to
//     * /tmp/fileName.json.
//     * @return whether the operation was a success or not.
//     */
//    QStringList saveState( const QString& saveName );

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

    /**
     * Center the image on the pixel with coordinates (x, y).
     * @param x the x-coordinate for the center of the pan.
     * @param y the y-coordinate for the center of the pan.
     */
    QStringList centerOnPixel( const QString& controlId, double x, double y );

    /**
     * Set the zoom level.
     * @param controlId the unique server-side id of an object managing a controller.
     * @param zoomLevel either positive or negative depending on the desired zoom direction.
     */
    QStringList setZoomLevel( const QString& controlId, double zoomLevel );

    /**
     * Get the current zoom level.
     * @param controlId the unique server-side id of an object managing a controller.
     */
    QStringList getZoomLevel( const QString& controlId );

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
     * @param logCount true if the vertical axis should be logarithmic; false otherwise.
     *  Can also be equal to "toggle" to turn the log scale on or off depending on its
     *  current value.
     * @return an error message if there was a problem setting the flag; an empty string otherwise.
     */
    QStringList setLogCount( const QString& histogramId, const QString& logCount );

    /**
     * Set where or not the histogram should be colored by intensity.
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

