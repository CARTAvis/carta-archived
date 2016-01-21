/***
 * Ties together the data source, grid, and contours; manages rendering.
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "CartaLib/IImage.h"
#include "CartaLib/AxisInfo.h"
#include "CartaLib/AxisLabelInfo.h"
#include "CartaLib/VectorGraphics/VGList.h"
#include <QImage>
#include <memory>
#include <set>


class CoordinateFormatterInterface;

namespace Carta {
namespace Lib {
    class IWcsGridRenderService;
    class IContourGeneratorService;
    namespace PixelPipeline {
        class CustomizablePixelPipeline;
    }
    namespace NdArray {
        class RawViewInterface;
    }
}
namespace Core {
    namespace ImageRenderService {
        class Service;
    }
    namespace ImageSaveService {
        class ImageSaveService;
    }
}



namespace Data {

class ColorState;
class DrawSynchronizer;
class DataContours;
class DataGrid;
class DataSource;
class LayerCompositionModes;

class ControllerData : public QObject, public Carta::State::CartaObject {

friend class Controller;
friend class DrawStackSynchronizer;

Q_OBJECT

public:


    static const QString CLASS_NAME;


    virtual ~ControllerData();

signals:

    void contourSetRemoved( const QString& name );
    void contourSetAdded(ControllerData* data, const QString& name );


    //Notification that a new image has been produced.
    void renderingDone();

    /// Return the result of SaveFullImage() after the image has been rendered
    /// and a save attempt made.
    void saveImageResult( bool result );

    void colorStateChanged();

private slots:

    //Notification from the rendering service that a new image and assiciated vector
    //graphics have been produced.
    void _renderingDone(  QImage image,
                          Carta::Lib::VectorGraphics::VGList vgList,
                          Carta::Lib::VectorGraphics::VGList contourList,
                          int64_t jobId );


    // Asynchronous result from saveFullImage().
    void _saveImageResultCB( bool result );

    void _colorChanged();

private:

    /**
     * Add a contour set.
     * @param contour - the contour set to add.
     */
    void _addContourSet( std::shared_ptr<DataContours> contour );

    void _clearColorMap();
    void _clearData();

    Carta::Lib::AxisInfo::KnownType _getAxisXType() const;
    Carta::Lib::AxisInfo::KnownType _getAxisYType() const;
    std::vector<Carta::Lib::AxisInfo::KnownType> _getAxisZTypes() const;
    std::vector<Carta::Lib::AxisInfo::KnownType> _getAxisTypes() const;

    /**
     * Get label format information for the given axis and type.
     * @param axisIndex - the plotting index of the axis (0 or 1).
     * @param axisType - the type of the axis (declination, ra, etc )
     * @return - information about how the axis labels should be formatted.
     */
    Carta::Lib::AxisLabelInfo _getAxisLabelInfo( int axisIndex, Carta::Lib::AxisInfo::KnownType axisType ) const;

    /**
     * Return the current pan center.
     * @return the centered image location.
     */
    QPointF _getCenter() const;

    /**
     * Return stored information about the color map.
     * @return - information about the color map.
     */
    std::shared_ptr<ColorState> _getColorState();

    /**
     * Return the mode used to composed the layer.
     * @return - a string identifier for the composition mode.
     */
    QString _getCompositionMode() const;

    /**
     * Return the coordinates at pixel (x, y) in the given coordinate system.
     * @param x the x-coordinate of the desired pixel.
     * @param y the y-coordinate of the desired pixel.
     * @param system the desired coordinate system.
     * @param frames - list of image frames.
     * @param system - an enumerated coordinate system type.
     * @return the coordinates at pixel (x, y).
     */
    QStringList _getCoordinates( double x, double y, Carta::Lib::KnownSkyCS system,
            const std::vector<int>& frames) const;

    /**
     * Return the coordinate system in use.
     * @return - an enumerated coordinate system type.
     */
    Carta::Lib::KnownSkyCS _getCoordinateSystem() const;

    /**
     * Return the contour set with the indicated name.
     * @return - the corresponding contour set with the designated name or a nullptr
     *  if no such set exists.
     */
    std::shared_ptr<DataContours> _getContour( const QString& name );

    /**
     * Return all contour sets for this particular layer.
     * @return - all contour sets in the layer.
     */
    std::set< std::shared_ptr<DataContours> > _getContours();

    /**
     * Returns information about the image at the current location of the cursor.
     * @param mouseX the mouse x-position in screen coordinates.
     * @param mouseY the mouse y-position in screen coordinates.
     * @param frames - list of image frames.
     * @return a QString containing cursor text.
     */
    QString _getCursorText( int mouseX, int mouseY, const std::vector<int>& frames );


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
     * Return the number of frames for the given axis in the image.
     * @param type  - the axis for which a frame count is needed.
     * @return the number of frames for the given axis in the image.
     */
    int _getFrameCount( Carta::Lib::AxisInfo::KnownType type ) const;


    //Return data source state.
    Carta::State::StateInterface _getGridState() const;

    /**
     * Returns the image's file name.
     * @return the path to the image.
     */
    QString _getFileName() const;

    /**
     * Returns the underlying image.
     */
    std::shared_ptr<Carta::Lib::Image::ImageInterface> _getImage();

    QImage _getQImage() const;

    /**
     * Returns the location on the image corresponding to a screen point in
     * pixels.
     * @param screenPt an (x,y) pair of pixel coordinates.
     * @param valid set to true if an image is loaded that can do the translation; otherwise false;
     * @return the corresponding location on the image.
     */
    QPointF _getImagePt( QPointF screenPt, bool* valid ) const;

    /**
     * Returns the intensity corresponding to a given percentile.
     * @param frameLow a lower bound for the image frames or -1 if there is no lower bound.
     * @param frameHigh an upper bound for the image frames or -1 if there is no upper bound.
     * @param percentile a number [0,1] for which an intensity is desired.
     * @param intensity the computed intensity corresponding to the percentile.
     * @return true if the computed intensity is valid; otherwise false.
     */
    bool _getIntensity( int frameLow, int frameHigh, double percentile, double* intensity ) const;


    /**
     * Returns information about this layer in the stack.
     * @return - a string representation of layer specific information.
     */
    QString _getLayerString() const;

    /**
     * Get the transparency for the layer.
     * @return - a transparency amount for the layer.
     */
    float _getMaskAlpha() const;

    /**
     * Return the color filter for the layer.
     * @return - a color filter for the layer.
     */
    quint32 _getMaskColor() const;

    /**
     * Get the dimensions of the image viewer (window size).
     * @return the image viewer dimensions.
     */
    QSize _getOutputSize() const;

    /**
     * Return the percentile corresponding to the given intensity.
     * @param frameLow a lower bound for the frame index or -1 if there is no lower bound.
     * @param frameHigh an upper bound for the frame index or -1 if there is no upper bound.
     * @param intensity a value for which a percentile is needed.
     * @return the percentile corresponding to the intensity.
     */
    double _getPercentile( int frameLow, int frameHigh, double intensity ) const;

    /**
     * Returns the pipeline responsible for rendering the image.
     * @retun the pipeline responsible for rendering the image.
     */
    std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> _getPipeline() const;


    /**
     * Return the pixel coordinates corresponding to the given world coordinates.
     * @param ra the right ascension (in radians) of the world coordinates.
     * @param dec the declination (in radians) of the world coordinates.
     * @return a list consisting of the x- and y-coordinates of the pixel
     *  corresponding to the given world coordinates.
     */
    QStringList _getPixelCoordinates( double ra, double dec ) const;

    /**
     * Return the units of the pixels.
     * @return the units of the pixels, or blank if units could not be obtained.
     */
    QString _getPixelUnits() const;

    /**
     * Return the value of the pixel at (x, y).
     * @param x the x-coordinate of the desired pixel
     * @param y the y-coordinate of the desired pixel.
     * @param frames - list of image frames.
     * @return the value of the pixel at (x, y), or blank if it could not be obtained.
     *
     * Note the xy coordinates are expected to be in casa pixel coordinates, i.e.
     * the CENTER of the left-bottom-most pixel is 0.0,0.0.
     */
    QString _getPixelValue( double x, double y, const std::vector<int>& frames ) const;

    /**
     * Returns the location on the screen corresponding to a location in image coordinates.
     * @param imagePt an (x,y) pair of image coordinates.
     * @param valid set to true if an image is loaded that can do the translation; otherwise false;
     * @return the corresponding pixel coordinates.
     */
    QPointF _getScreenPt( QPointF imagePt, bool* valid ) const;

    /**
     * Return the state of this layer.
     * @return - a string representation of the layer state.
     */
    QString _getStateString() const;

    /**
     * Return the layer vector graphics.
     * @return - the layer vector graphics, which can include both the grid and contours.
     */
    Carta::Lib::VectorGraphics::VGList _getVectorGraphics();

    /**
     * Return the zoom factor for this image.
     * @return the zoom multiplier.
     */
    double _getZoom() const;


    void _gridChanged( const Carta::State::StateInterface& state);

    /**
     * Respond to a change in display axes.
     * @param displayAxisTypes - the x-, y-, and z- axes to display.
     * @param frames - list of image frames.
     */
    void _displayAxesChanged(std::vector<Carta::Lib::AxisInfo::KnownType> displayAxisTypes,
            const std::vector<int>& frames );
    void _initializeState();
    void _initializeSingletons( );

    /**
     * Returns true if at least one contour set should be drawn; false otherwise.
     * @return - true if there is at least one contour set to draw; false otherwise.
     */
    bool _isContourDraw() const;

    /**
     * Returns true if the name identifies this layer; false otherwise.
     * @return true if the name identifies this layer; false otherwise.
     */
    bool _isMatch( const QString& name ) const;

    /**
     * Returns true if this data is selected; false otherwise.
     * @return true if this data is selected; false otherwise.
     */
    bool _isSelected() const;


    /**
     * Returns true if this layer is not hidden; false otherwise.
     * @return true if the layer is visible; false otherwise.
     */
    bool _isVisible() const;

    /**
     * Loads the data source as a QImage.
     * @param frames - list of frames to load, one for each of the known axis types.
     * @param true to force a recompute of the image clip.
     */
    Nullable<QImage> _load(std::vector<int> frames, bool forceReload );

    /**
     * Return a QImage representation of this data.
     * @param frames - a list of frames to load, one for each of the known axis types.
     * @param autoClip true if clips should be automatically generated; false otherwise.
     * @param clipMinPercentile the minimum clip value.
     * @param clipMaxPercentile the maximum clip value.
     */
    void _load( vector<int> frames, bool autoClip, double clipMinPercentile,
            double clipMaxPercentile );

    /**
     * Generate a new QImage.
     * @param frames - list of image frames.
     * @param cs - an enumerated coordinate system type.
     */
    void _render( const std::vector<int>& frames, const Carta::Lib::KnownSkyCS& cs, bool topOfStack );

    /**
     * Center the image.
     */
    void _resetPan();

    /**
     * Reset the prefereence state of this layer.
     * @param restoreState - the new layer state.
     */
    void _resetState( const Carta::State::StateInterface& restoreState );

    /**
     * Reset the layer contours.
     * @param restoreeState - the new layer state.
     */
    void _resetStateContours(const Carta::State::StateInterface& restoreState );

    /**
     * Reset the zoom to the original value.
     */
    void _resetZoom();

    /**
     * Save a copy of the full image in the current image view.
     * @param saveName the full path where the file is to be saved.
     * @param scale the scale (zoom level) of the saved image.
     * @param frames - list of image frames.
     * @return an error message if there was a problem saving the image.
     */
    QString _saveImage( const QString& saveName,  double scale, const std::vector<int>& frames );

    /**
     * Reset the color map information for this data.
     * @param colorState - stored information about the color map.
     */
    void _setColorMapGlobal( std::shared_ptr<ColorState> colorState );

    /**
     * Set the mode used to compose this layer.
     * @param compositionMode - the mode used to compose this layer.
     * @param errorMsg - a error message if the composition mode was not successfully set.
     */
    bool _setCompositionMode( const QString& compositionMode,
            QString& errorMsg );

    /**
     * Remove the contour set from this layer.
     * @param contourSet - the contour set to remove from the layer.
     */
    void _removeContourSet( std::shared_ptr<DataContours> contourSet );

    /**
     * Restore the state of this layer.
     * @param stateStr - the new layer state.
     */
    void _resetState( const QString& stateStr );

    /**
     * Returns whether or not the data was successfully loaded.
     * @param fileName an identifier for the location of a data source.
     * @return true if the data souce was successfully loaded; false otherwise.
     */
    bool _setFileName( const QString& fileName );

    /**
     * Set the color to use for the mask.
     * @param redAmount - the amount of red in [0,255].
     * @param greenAmount - the amount of green in [0,255].
     * @param blueAmount - the amount of blue in [0,255].
     * @param result - a list of errors that might have occurred in setting the
     *      mask color; an empty string otherwise.
     * @return - true if the mask color was changed; false otherwise.
     */
    bool _setMaskColor( int redAmount,
            int greenAmount, int blueAmount, QStringList& result );

    /**
     * Set the opacity of the mask.
     * @param alphaAmount - the transparency level in [0,255] with 255 being opaque.
     * @param result - an error message if there was a problem setting the mask opacity or
     *      an empty string otherwise.
     * @return - true if the mask opacity was changed; false otherwise.
     */
    bool _setMaskAlpha( int alphaAmount, QString& result );

    /**
     * Set the center for this image's display.
     * @param imgX the x-coordinate of the center.
     * @param imgY the y-coordinate of the center.
     */
    void _setPan( double imgX, double imgY );

    /**
     * Set this data source selected.
     * @param selected - true if the data source is selected; false otherwise.
     * @return -true if the selected state changed; false otherwise.
     */
    bool _setSelected( bool selected );


    /**
     * Show/hide this layer.
     * @param visible - true to show the layer; false to hide it.
     */
    void _setVisible( bool visible );

    /**
     * Set the zoom factor for this image.
     * @param zoomFactor the zoom multiplier.
     */
    void _setZoom( double zoomFactor );

    /**
     * Resize the view of the image.
     */
    void _viewResize( const QSize& newSize );

    void _updateClips( std::shared_ptr<Carta::Lib::NdArray::RawViewInterface>& view,
            double minClipPercentile, double maxClipPercentile, const std::vector<int>& frames );

    /**
     *  Constructor.
     */
    ControllerData( const QString& path, const QString& id );

    class Factory;
    static bool m_registered;

    static const QString COMPOSITION_MODE;
    static const QString LAYER;
    static const QString LAYER_COLOR;
    static const QString LAYER_ALPHA;
    static const QString MASK;
    static const QString SELECTED;

    static LayerCompositionModes* m_compositionModes;

    std::shared_ptr<ColorState> m_stateColor;

    std::unique_ptr<DataGrid> m_dataGrid;

    std::set< std::shared_ptr<DataContours> > m_dataContours;

    //Pointer to image interface.
    std::unique_ptr<DataSource> m_dataSource;


     /// image-and-grid-service result synchronizer
    std::unique_ptr<DrawSynchronizer> m_drawSync;

    /// Saves images
    Carta::Core::ImageSaveService::ImageSaveService *m_saveService;
    QImage m_qimage;
    Carta::Lib::VectorGraphics::VGList m_vectorGraphics;
    ControllerData(const ControllerData& other);
    ControllerData& operator=(const ControllerData& other);
};
}
}
