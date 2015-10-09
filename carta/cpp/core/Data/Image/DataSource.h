/***
 * Manages and loads a single source of data.
 */

#pragma once

#include "CartaLib/Nullable.h"
#include "Data/IColoredView.h"
#include "CartaLib/CartaLib.h"
#include "CartaLib/AxisInfo.h"

#include <QImage>
#include <memory>

namespace NdArray {
    class RawViewInterface;
}

namespace Image {
    class ImageInterface;
}

class CoordinateFormatterInterface;
class SliceND;

namespace Carta {
namespace Lib {
    namespace PixelPipeline {
        class CustomizablePixelPipeline;
    }
}
namespace Core {
    namespace ImageRenderService {
        class Service;
    }
}

namespace Data {

class CoordinateSystems;

class DataSource : public QObject, public IColoredView {

friend class ControllerData;

Q_OBJECT

public:

       /**
        * Sets a new color map.
        * @param name the identifier for the color map.
        */
       virtual void setColorMap( const QString& name ) Q_DECL_OVERRIDE;

       /**
        * Sets whether the colors in the map are inverted.
        * @param inverted true if the colors in the map are inverted; false
        *        otherwise.
        */
       virtual void setColorInverted( bool inverted )  Q_DECL_OVERRIDE;

       /**
        * Sets whether the colors in the map are reversed.
        * @param reversed true if the colors in the map are reversed; false
        *        otherwise.
        */
       virtual void setColorReversed( bool reversed ) Q_DECL_OVERRIDE;

       /**
        * Set the amount of red, green, and blue in the color scale.
        * @param newRed the amount of red; should be in the range [0,1].
        * @param newGreen the amount of green; should be in the range [0,1].
        * @param newBlue the amount of blue; should be in the range[0,1].
        */
       virtual void setColorAmounts( double newRed, double newGreen, double newBlue ) Q_DECL_OVERRIDE;

       /**
        * Set the gamma color map parameter.
        * @param gamma a color map parameter.
        */
       virtual void setGamma( double gamma )  Q_DECL_OVERRIDE;

       static const QString CLASS_NAME;
       static const double ZOOM_DEFAULT;
       static const QString DATA_PATH;

    virtual ~DataSource();


private:

    /**
     * Returns true if this data source manages the data corresponding
     * to the fileName; false, otherwise.
     * @param fileName a locator for data.
     */
    bool _contains(const QString& fileName) const;

    /**
     * Resizes the frame indices to fit the current image.
     * @param sourceFrames - a list of current image frames.
     * @return a list of frames that will fit the current image.
     */
    std::vector<int> _fitFramesToImage( const std::vector<int>& sourceFrames ) const;

    /**
     * Returns the index of the axis of the given type in the image or -1 if there is
     * no such axis
     * @param axisType - the type of axis.
     * @return the index of the axis in the image or -1 if there is no such axis.
     */
    int _getAxisIndex( Carta::Lib::AxisInfo::KnownType axisType ) const;

    /**
     * Return a list of the current axis permutations.
     * @return a list showing the current axis permutations.
     */
    std::vector<int> _getAxisPerms() const;

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
     * Return the number of frames for a particular axis in the image.
     * @param type - the axis for which a frame count is needed.
     * @return the number of frames for the given axis in the image.
     */
    int _getFrameCount( Carta::Lib::AxisInfo::KnownType type ) const;

    /**
     * Return the number of dimensions in the image.
     * @return the number of image dimensions.
     */
    int _getDimensions() const;

    /**
     * Returns the location on the image corresponding to a screen point in
     * pixels.
     * @param screenPt an (x,y) pair of pixel coordinates.
     * @param valid set to true if an image is loaded that can do the translation; otherwise false;
     * @return the corresponding location on the image.
     */
    QPointF _getImagePt( QPointF screenPt, bool* valid ) const;
    
    /**
     * Returns the location on the screen corresponding to a location in image coordinates.
     * @param imagePt an (x,y) pair of image coordinates.
     * @param valid set to true if an image is loaded that can do the translation; otherwise false;
     * @return the corresponding pixel coordinates.
     */
    QPointF _getScreenPt( QPointF imagePt, bool* valid ) const;

    /**
     * Return the current pan center.
     * @return the centered image location.
     */
    QPointF _getCenter() const;
    

    /**
     * Return the image size for the given coordinate index.
     * @param coordIndex an index of a coordinate of the image.
     * @return the corresponding dimension for that coordinate or -1 if none exists.
     */
    int _getDimension( int coordIndex ) const;

    /**
     * Returns the underlying image.
     */
    std::shared_ptr<Image::ImageInterface> _getImage();

    /**
     * Returns the image's file name.
     * @return the path to the image.
     */
    QString _getFileName() const;
    
    /**
     * Returns information about the image at the current location of the cursor.
     * @param mouseX the mouse x-position in screen coordinates.
     * @param mouseY the mouse y-position in screen coordinates.
     * @param frames - a list of current image frames.
     * @return a QString containing cursor text.
     */
    QString _getCursorText( int mouseX, int mouseY, Carta::Lib::KnownSkyCS cs, const std::vector<int>& frames);

    /**
     * Returns the number of frames in the horizontal and vertical display directions,
     * respectively.
     * @return - a pair consisting of frame counts on the horizontal and vertical axis.
     */
    std::pair<int,int> _getDisplayDims() const;

    /**
     * Return the percentile corresponding to the given intensity.
     * @param frameLow a lower bound for the frames or -1 if there is no lower bound.
     * @param frameHigh an upper bound for the frames or -1 if there is no upper bound.
     * @param intensity a value for which a percentile is needed.
     * @return the percentile corresponding to the intensity.
     */
    double _getPercentile( int frameLow, int frameHigh, double intensity ) const;
    
    /**
     * Returns the intensity corresponding to a given percentile.
     * @param frameLow a lower bound for the image channels or -1 if there is no lower bound.
     * @param frameHigh an upper bound for the image channels or -1 if there is no upper bound.
     * @param percentile a number [0,1] for which an intensity is desired.
     * @param intensity the computed intensity corresponding to the percentile.
     * @return true if the computed intensity is valid; otherwise false.
     */
    bool _getIntensity( int frameLow, int frameHigh, double percentile, double* intensity ) const;
    
    /**
     * Returns the pipeline responsible for rendering the image.
     * @retun the pipeline responsible for rendering the image.
     */
    std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> _getPipeline() const;

    std::shared_ptr<Carta::Core::ImageRenderService::Service> _getRenderer() const;

    /**
     * Return the zoom factor for this image.
     * @return the zoom multiplier.
     */
    double _getZoom() const;

    /**
     * Get the dimensions of the image viewer (window size).
     * @return the image viewer dimensions.
     */
    QSize _getOutputSize() const;

    /**
     * Return the pixel coordinates corresponding to the given world coordinates.
     * @param ra the right ascension (in radians) of the world coordinates.
     * @param dec the declination (in radians) of the world coordinates.
     * @return a list consisting of the x- and y-coordinates of the pixel
     *  corresponding to the given world coordinates.
     */
    QStringList _getPixelCoordinates( double ra, double dec ) const;

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

    /**
     * Return the units of the pixels.
     * @return the units of the pixels, or blank if units could not be obtained.
     */
    QString _getPixelUnits() const;

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
     * Returns the raw data as an array.
     * @param axisIndex - an index of an image axis.
     * @param frameLow the lower bound for the frames or -1 for the whole image.
     * @param frameHigh the upper bound for the frames or -1 for the whole image.
     * @param axisIndex - the axis for the frames or -1 for all axes.
     * @return the raw data or nullptr if there is none.
     */
    NdArray::RawViewInterface *  _getRawData( int frameLow, int frameHigh, int axisIndex ) const;

    /**
     * Returns the raw data for the current view.
     * @param frames - a list of current image frames.
     * @return the raw data for the current view or nullptr if there is none.
     */
    NdArray::RawViewInterface* _getRawData( const std::vector<int> frames ) const;

    //Returns an identifier for the current image slice being rendered.
    QString _getViewIdCurrent( const std::vector<int>& frames ) const;
    int _getQuantileCacheIndex( const std::vector<int>& frames ) const;

    //Initialize static objects.
    void _initializeSingletons( );

    /**
     * Return a QImage representation of this data.
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
     * Returns whether or not the data was successfully loaded.
     * @param fileName an identifier for the location of a data source.
     * @return true if the data souce was successfully loaded; false otherwise.
     */
    bool _setFileName( const QString& fileName );


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
    std::shared_ptr<NdArray::RawViewInterface> _updateRenderedView( const std::vector<int>& frames );

    /**
     * Resize the view of the image.
     */
    void _viewResize( const QSize& newSize );


    void _updateClips( std::shared_ptr<NdArray::RawViewInterface>& view,
            double minClipPercentile, double maxClipPercentile, const std::vector<int>& frames );

    /**
     *  Constructor.
     */
    DataSource();

    QString m_fileName;
    bool m_cmapUseCaching;
    bool m_cmapUseInterpolatedCaching;
    int m_cmapCacheSize;

    //Used pointer to coordinate systems.
    static CoordinateSystems* m_coords;

    //Pointer to image interface.
    std::shared_ptr<Image::ImageInterface> m_image;

    /// coordinate formatter
    std::shared_ptr<CoordinateFormatterInterface> m_coordinateFormatter;

    /// clip cache, hard-coded to single quantile
    std::vector< std::vector<double> > m_quantileCache;

    /// the rendering service
    std::shared_ptr<Carta::Core::ImageRenderService::Service> m_renderService;

    ///pixel pipeline
    std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> m_pixelPipeline;

    //Indices of the display axes.
    int m_axisIndexX;
    int m_axisIndexY;

    DataSource(const DataSource& other);
    DataSource& operator=(const DataSource& other);
};
}
}
