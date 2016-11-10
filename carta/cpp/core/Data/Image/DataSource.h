/***
 * Manages and loads a single source of data.
 */

#pragma once

#include "CartaLib/Nullable.h"
#include "CartaLib/AxisDisplayInfo.h"
#include "CartaLib/CartaLib.h"
#include "CartaLib/AxisInfo.h"
#include "LeastRecentlyUsedCache.h"

#include <memory>

class CoordinateFormatterInterface;
class SliceND;

namespace Carta {
namespace Lib {
namespace PixelPipeline {
class CustomizablePixelPipeline;
}
namespace Image {
class ImageInterface;
}
namespace NdArray {
class RawViewInterface;
}
}



namespace Core {
namespace ImageRenderService {
class Service;
}
}

namespace Data {

class CoordinateSystems;

class DataSource : public QObject {

    friend class LayerData;
    friend class DataFactory;
    friend class Histogram;
    friend class Profiler;
    friend class Colormap;

    Q_OBJECT

public:


    static const QString CLASS_NAME;
    static const double ZOOM_DEFAULT;
    static const QString DATA_PATH;

    virtual ~DataSource();


private:

    /**
     * Resizes the frame indices to fit the current image.
     * @param sourceFrames - a list of current image frames.
     * @return a list of frames that will fit the current image.
     */
    std::vector<int> _fitFramesToImage( const std::vector<int>& sourceFrames ) const;

    /**
     * Return a list of information about the axes to display.
     * @return a list showing information about which axes should be displayed and how
     *  they should be displayed.
     */
    std::vector<Carta::Lib::AxisDisplayInfo> _getAxisDisplayInfo() const;

    /**
     * Return a list of axes in the image.
     * @return - a list of axes that are present in the image.
     */
    std::vector<Carta::Lib::AxisInfo::KnownType> _getAxisTypes() const;

    /**
     * Returns the type of the axis with the given index in the image.
     * @param index - the index of the axis in the coordinate system.
     * @return the type of the corresponding axis or AxisInfo::KnownType::OTHER
     *      if the index does not correspond to a coordinate axis in the image.
     */
    Carta::Lib::AxisInfo::KnownType _getAxisType( int index ) const;

    /**
     * Return the x display axis.
     * @return the x display axis.
     */
    Carta::Lib::AxisInfo::KnownType _getAxisXType() const;

    /**
     * Return the y display axis.
     * @return the y display axis.
     */
    Carta::Lib::AxisInfo::KnownType _getAxisYType() const;

    /**
     * Return the hidden axes.
     * @return the hidden axes.
     */
    std::vector<Carta::Lib::AxisInfo::KnownType> _getAxisZTypes() const;

    /**
     * Return the current pan center.
     * @return the centered image location.
     */
    QPointF _getCenter() const;

    /**
     * Return the coordinates at pixel (x, y) in the given coordinate system.
     * @param x the x-coordinate of the desired pixel.
     * @param y the y-coordinate of the desired pixel.
     * @param system the desired coordinate system.
     * @param frames - a list of current image frames.
     * @return a list formatted coordinates.
     */
    QStringList _getCoordinates( double x, double y, Carta::Lib::KnownSkyCS system,
            const std::vector<int>& frames) const;

    /**
     * Returns information about the image at the current location of the cursor.
     * @param mouseX the mouse x-position in screen coordinates.
     * @param mouseY the mouse y-position in screen coordinates.
     * @param frames - a list of current image frames.
     * @return a QString containing cursor text.
     */
    QString _getCursorText( int mouseX, int mouseY, Carta::Lib::KnownSkyCS cs, const std::vector<int>& frames,
            double zoom, const QPointF& pan, const QSize& outputSize );


    /**
     * Return the image size for the given coordinate index.
     * @param coordIndex an index of a coordinate of the image.
     * @return the corresponding dimension for that coordinate or -1 if none exists.
     */
    int _getDimension( int coordIndex ) const;

    /**
     * Return the number of dimensions in the image.
     * @return the number of image dimensions.
     */
    int _getDimensions() const;

    /**
     * Returns the number of frames in the horizontal and vertical display directions,
     * respectively.
     * @return - a pair consisting of frame counts on the horizontal and vertical axis.
     */
    std::pair<int,int> _getDisplayDims() const;

    /**
     * Returns the image's file name.
     * @return the path to the image.
     */
    QString _getFileName() const;


    /**
     * Return the number of frames for a particular axis in the image.
     * @param type - the axis for which a frame count is needed.
     * @return the number of frames for the given axis in the image.
     */
    int _getFrameCount( Carta::Lib::AxisInfo::KnownType type ) const;

    /**
     * Get the index of the current frame of the axis specified by the sourceFrameIndex.
     * @param sourceFrameIndex - an index referring to a specific element in sourceFrames.
     * @param sourceFrames - a list for each axis type, indicating the current frame of the axis.
     * @return - the current frame for the axis.
     */
    int _getFrameIndex( int sourceFrameIndex, const std::vector<int>& sourceFrames ) const;


    /**
     * Returns the underlying image.
     */
    std::shared_ptr<Carta::Lib::Image::ImageInterface> _getImage();

    /**
     * Returns the location on the image corresponding to a screen point in
     * pixels.
     * @param screenPt an (x,y) pair of pixel coordinates.
     * @param zoom - the zoom factor.
     * @param pan - the point where the image should be centered.
     * @param outputSize - the size in pixels of the produced image.
     * @param valid set to true if an image is loaded that can do the translation; otherwise false;
     * @return the corresponding location on the image.
     */
    QPointF _getImagePt( const QPointF& screenPt, double zoom, const QPointF& pan,
            const QSize& outputSize, bool* valid ) const;

    /**
     * Returns the intensities corresponding to a given percentiles.
     * @param frameLow - a lower bound for the image channels or -1 if there is no lower bound.
     * @param frameHigh - an upper bound for the image channels or -1 if there is no upper bound.
     * @param percentiles - a list of numbers in [0,1] for which an intensity is desired.
     * @return - a list of corresponding (location,intensity) pairs.
     */
    std::vector<std::pair<int,double> > _getIntensity( int frameLow, int frameHigh,
            const std::vector<double>& percentiles);

    /**
     * Returns the intensities corresponding to a given percentiles.
     * @param frameLow - a lower bound for the image channels or -1 if there is no lower bound.
     * @param frameHigh - an upper bound for the image channels or -1 if there is no upper bound.
     * @param percentiles - a list of numbers in [0,1] for which an intensity is desired.
     * @return - a list of corresponding (location,intensity) pairs.
     */
    std::vector<std::pair<int,double> > _getIntensityCache( int frameLow, int frameHigh,
        const std::vector<double>& percentiles );

    /**
     * Returns the color used to draw nan pixels.
     * @return - the color used to draw nan pixels.
     */
    QColor _getNanColor() const;


    /**
     * Return the percentile corresponding to the given intensity.
     * @param frameLow a lower bound for the frames or -1 if there is no lower bound.
     * @param frameHigh an upper bound for the frames or -1 if there is no upper bound.
     * @param intensity a value for which a percentile is needed.
     * @return the percentile corresponding to the intensity.
     */
    double _getPercentile( int frameLow, int frameHigh, double intensity ) const;


    std::shared_ptr<Carta::Lib::Image::ImageInterface> _getPermutedImage() const;

    /**
     * Returns the pipeline responsible for rendering the image.
     * @retun the pipeline responsible for rendering the image.
     */
    std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> _getPipeline() const;

    /**
     * Return the pixel coordinates corresponding to the given world coordinates.
     * @param ra - the right ascension (in radians) of the world coordinates.
     * @param dec - the declination (in radians) of the world coordinates.
     * @param valid - true if the pixel coordinates are valid; false otherwise.
     * @return - a point containing the pixel coordinates.
     */
    QPointF _getPixelCoordinates( double ra, double dec, bool* valid ) const;

    /**
     * Return the rest frequency and units for the image.
     * @return - the image rest frequency and units; a blank string and a negative
     * 		value are returned with the rest frequency can not be found.
     */
    std::pair<double,QString> _getRestFrequency() const;

    /**
     * Return the world coordinates corresponding to the given pixel coordinates.
     * @param pixelX - the first pixel coordinate.
     * @param pixelY - the second pixel coordinate.
     * @param valid - true if the pixel coordinates are valid; false otherwise.
     * @return - a point containing the pixel coordinates.
     */
    QPointF _getWorldCoordinates( double pixelX, double pixelY,
            Carta::Lib::KnownSkyCS coordSys, bool* valid ) const;

    /**
     * Return the units of the pixels.
     * @return the units of the pixels, or blank if units could not be obtained.
     */
    QString _getPixelUnits() const;

    /**
     * Return the value of the pixel at (x, y).
     * @param x the x-coordinate of the desired pixel
     * @param y the y-coordinate of the desired pixel.
     * @param frames - a list of current image frames.
     * @return the value of the pixel at (x, y), or blank if it could not be obtained.
     *
     * Note the xy coordinates are expected to be in casa pixel coordinates, i.e.
     * the CENTER of the left-bottom-most pixel is 0.0,0.0.
     */
    QString _getPixelValue( double x, double y, const std::vector<int>& frames ) const;

    int _getQuantileCacheIndex( const std::vector<int>& frames ) const;

    /**
     * Returns the raw data as an array.
     * @param axisIndex - an index of an image axis.
     * @param frameLow the lower bound for the frames or -1 for the whole image.
     * @param frameHigh the upper bound for the frames or -1 for the whole image.
     * @param axisIndex - the axis for the frames or -1 for all axes.
     * @return the raw data or nullptr if there is none.
     */
    Carta::Lib::NdArray::RawViewInterface *  _getRawData( int frameLow, int frameHigh, int axisIndex ) const;

    /**
     * Returns the raw data for the current view.
     * @param frames - a list of current image frames.
     * @return the raw data for the current view or nullptr if there is none.
     */
    Carta::Lib::NdArray::RawViewInterface* _getRawData( const std::vector<int> frames ) const;

    std::shared_ptr<Carta::Core::ImageRenderService::Service> _getRenderer() const;

    /**
     * Returns the location on the screen corresponding to a location in image coordinates.
     * @param imagePt an (x,y) pair of image coordinates.
     * @param valid set to true if an image is loaded that can do the translation; otherwise false;
     * @return the corresponding pixel coordinates.
     */
    QPointF _getScreenPt( const QPointF& imagePt, const QPointF& pan, double zoom,
            const QSize& outputSize, bool* valid ) const;


    //Returns an identifier for the current image slice being rendered.
    QString _getViewIdCurrent( const std::vector<int>& frames ) const;


    //Initialize static objects.
    void _initializeSingletons( );

    ///Returns whether the frames actually exist in the image.
    bool _isLoadable( std::vector<int> frames ) const;

    //Returns whether or not there is a spectral axis in the image.
    bool _isSpectralAxis() const;

    /**
     * Generate a QImage representation of this data.
     * @param -frames a list of frames to load, one for each axis.
     * @param - recomputeClipsOnNewFrame - true if the clips should be recalculated when the frame
     *      is changed; false otherwise.
     * @param clipMinPercentile the minimum clip value.
     * @param clipMaxPercentile the maximum clip value.
     */
    void _load( std::vector<int> frames, bool recomputeClipsOnNewFrame,
            double clipMinPercentile, double clipMaxPercentile );

    /**
     * Center the image.
     */
    void _resetPan();

    /**
     * Reset the zoom to the original value.
     */
    void _resetZoom();

    void _resizeQuantileCache();

    /**
     * Sets a new color map.
     * @param name the identifier for the color map.
     */
    void _setColorMap( const QString& name );

    /**
     * Sets whether the colors in the map are inverted.
     * @param inverted true if the colors in the map are inverted; false
     *        otherwise.
     */
    void _setColorInverted( bool inverted );

    /**
     * Sets whether the colors in the map are reversed.
     * @param reversed true if the colors in the map are reversed; false
     *        otherwise.
     */
    void _setColorReversed( bool reversed );

    /**
     * Set the amount of red, green, and blue in the color scale.
     * @param newRed the amount of red; should be in the range [0,1].
     * @param newGreen the amount of green; should be in the range [0,1].
     * @param newBlue the amount of blue; should be in the range[0,1].
     */
    void _setColorAmounts( double newRed, double newGreen, double newBlue );

    /**
     * Set the color used to draw nan pixels.
     * @param red - the amount of red in [0,255].
     * @param green - the amount of green in [0,255].
     * @param blue - the amount of blue in [0,255].
     */
    void _setColorNan( double red, double green, double blue );

    /**
     * Set the x-, y-, and z- axes that are to be displayed.
     * @param displayAxisTypes - the list of display axes.
     * @param frames - a list of current image frames.
     */
    void _setDisplayAxes(std::vector<Carta::Lib::AxisInfo::KnownType> displayAxisTypes,
            const std::vector<int>& frames );

    /**
     * Set a particular axis type to be one of the display axes.
     * @param axisType - the type of axis.
     * @param axisIndex - a pointer to the display axis index.
     * @return true if the specified display axis changes; false otherwise.
     */
    bool _setDisplayAxis( Carta::Lib::AxisInfo::KnownType axisType, int* axisIndex );

    /**
     * Set the gamma color map parameter.
     * @param gamma a color map parameter.
     */
    void _setGamma( double gamma );

    /**
     * Set whether or not to use the default nan color (bottom of the color map).
     * @param nanDefault - true if the default nan color should be used; false if
     *      a user set nan color should be used.
     */
    void _setNanDefault( bool nanDefault );


    /**
     * Set the center for this image's display.
     * @param imgX the x-coordinate of the center.
     * @param imgY the y-coordinate of the center.
     */
    void _setPan( double imgX, double imgY );

    /**
     * Set the zoom factor for this image.
     * @param zoomFactor the zoom multiplier.
     */
    void _setZoom( double zoomFactor );


    /**
     * Attempts to load an image file.
     * @param fileName an identifier for the location of a data source.
     * @param success - set to true if the image file is successfully loaded; otherwise,
     *      set to false.
     * @return - an error message if the image was not successfully loaded; otherwise,
     *      an empty string.
     */
    QString _setFileName( const QString& fileName, bool* success );


    /**
     * Set the data transform.
     * @param name QString a unique identifier for a data transform.
     */
    void _setTransformData( const QString& name );

    /**
     * Update the data when parameters that govern data selection have changed
     * such as when different display axes have been selected.
     * @param frames - a list of current image frames.
     */
    std::shared_ptr<Carta::Lib::NdArray::RawViewInterface> _updateRenderedView( const std::vector<int>& frames );

    /**
     * Resize the view of the image.
     */
    void _viewResize( const QSize& newSize );


    void _updateClips( std::shared_ptr<Carta::Lib::NdArray::RawViewInterface>& view,
            double minClipPercentile, double maxClipPercentile, const std::vector<int>& frames );

    /**
     *  Constructor.
     */
    DataSource();

    QString m_fileName;
    int m_cmapCacheSize;

    //Used pointer to coordinate systems.
    static CoordinateSystems* m_coords;

    //Pointer to image interface.
    std::shared_ptr<Carta::Lib::Image::ImageInterface> m_image;
    std::shared_ptr<Carta::Lib::Image::ImageInterface> m_permuteImage;

    /// coordinate formatter
    std::shared_ptr<CoordinateFormatterInterface> m_coordinateFormatter;

    struct QuantileCacheEntry {
    	double m_minPercentile;
    	double m_maxPercentile;
    	std::vector<double> m_clips;
    };

    /// clip cache to avoid time-consuming operation or recomputing
    /// unnecessary clips.
    std::vector<QuantileCacheEntry> m_quantileCache;

    ///Percentile/Intensity cache
    LeastRecentlyUsedCache m_cachedPercentiles;

    /// the rendering service
    std::shared_ptr<Carta::Core::ImageRenderService::Service> m_renderService;

    ///pixel pipeline
    std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> m_pixelPipeline;

    //Indices of the display axes.
    int m_axisIndexX;
    int m_axisIndexY;

    const static int INDEX_LOCATION;
    const static int INDEX_INTENSITY;
    const static int INDEX_PERCENTILE;
    const static int INDEX_FRAME_LOW;
    const static int INDEX_FRAME_HIGH;

    DataSource(const DataSource& other);
    DataSource& operator=(const DataSource& other);
};
}
}
