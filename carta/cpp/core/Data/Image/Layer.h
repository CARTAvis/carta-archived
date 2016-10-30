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
#include "Data/Image/Render/RenderRequest.h"
#include "Data/Image/Render/RenderResponse.h"
#include <QImage>
#include <QStack>
#include <set>


class CoordinateFormatterInterface;

namespace Carta {
namespace Lib {
namespace PixelPipeline {
class CustomizablePixelPipeline;
}
namespace NdArray {
class RawViewInterface;
}
}

namespace Data {

class ColorState;
class DataContours;
class DataSource;
class LayerCompositionModes;

class Layer : public QObject, public Carta::State::CartaObject {

    friend class Controller;
    friend class CurveData;
    friend class LayerGroup;
    friend class Profiler;
    friend class ProfileRenderService;
    friend class ProfileRenderRequest;
    friend class Stack;
    friend class DrawGroupSynchronizer;
    friend class DrawStackSynchronizer;
    friend class SaveService;

    Q_OBJECT

public:


    static const QString CLASS_NAME;


    virtual ~Layer();



signals:
    void contourSetRemoved( const QString& name );
    void contourSetAdded(Layer* data, const QString& name );
    void colorStateChanged();


    //Notification that a new image has been produced.
    void renderingDone( const std::shared_ptr<RenderResponse>& response );


protected:

    /**
     * Add a contour set.
     * @param contour - the contour set to add.
     */
    virtual void _addContourSet( std::shared_ptr<DataContours> contour ) = 0;

    /**
     * Add a layer to this one at the given index.
     * @param layer - the layer to add.
     * @param targetIndex - the index for the new layer.
     */
    virtual void _addLayer( std::shared_ptr<Layer> layer, int targetIndex = -1 );

    /**
     * Set the child count to zero without actually deleting them.
     */
    //Used when putting layers into groups.
    virtual void _clearChildren();


    /**
     * Remove the color map.
     */
    virtual void _clearColorMap();

    virtual bool _closeData( const QString& id );

    /**
     * Respond to a change in display axes.
     * @param displayAxisTypes - the x-, y-, and z- axes to display.
     * @param frames - list of image frames.
     */
    virtual void _displayAxesChanged(std::vector<Carta::Lib::AxisInfo::KnownType> displayAxisTypes,
            const std::vector<int>& frames ) = 0;

    virtual Carta::Lib::AxisInfo::KnownType _getAxisType( int index ) const = 0;
    virtual Carta::Lib::AxisInfo::KnownType _getAxisXType() const = 0;
    virtual Carta::Lib::AxisInfo::KnownType _getAxisYType() const = 0;
    virtual std::vector<Carta::Lib::AxisInfo::KnownType> _getAxisZTypes() const = 0;
    virtual std::vector<Carta::Lib::AxisInfo::KnownType> _getAxisTypes() const = 0;

    virtual QPointF _getCenterPixel() const = 0;

    /**
     * Return a list of child layers.
     * @return - a list of child layers.
     */
    virtual QList<std::shared_ptr<Layer> > _getChildren();

    /**
     * Return stored information about the color map.
     * @return - information about the color map.
     */
    virtual std::shared_ptr<ColorState> _getColorState();

    /**
     * Return the mode used to composed the layer.
     * @return - a string identifier for the composition mode.
     */
    virtual QString _getCompositionMode() const;

    /**
     * Return the point on the image corresponding to the pixel point in the context
     * view.
     * @param pixelPt - a pixel position in the context view.
     * @param outputSize - the size of the context view in pixels.
     * @param valid - whether or not the returned point is valid.
     * @return - the pixel position of the point in image coordinates.
     */
    virtual QPointF _getContextPt( const QPointF& pixelPt, const QSize& outputSize, bool* valid ) const = 0;

    /**
     * Return the contour set with the indicated name.
     * @return - the corresponding contour set with the designated name or a nullptr
     *  if no such set exists.
     */
    virtual std::shared_ptr<DataContours> _getContour( const QString& name ) = 0;

    /**
     * Return all contour sets for this particular layer.
     * @return - all contour sets in the layer.
     */
    virtual std::set< std::shared_ptr<DataContours> > _getContours();

    /**
     * Return the coordinates at pixel (x, y) in the given coordinate system.
     * @param x the x-coordinate of the desired pixel.
     * @param y the y-coordinate of the desired pixel.
     * @param system the desired coordinate system.
     * @param frames - list of image frames.
     * @param system - an enumerated coordinate system type.
     * @return the coordinates at pixel (x, y).
     */
    virtual QStringList _getCoordinates( double x, double y, Carta::Lib::KnownSkyCS system,
            const std::vector<int>& frames) const = 0;
    /**
     * Return the coordinate system in use.
     * @return - an enumerated coordinate system type.
     */
    virtual Carta::Lib::KnownSkyCS _getCoordinateSystem() const = 0;

    /**
     * Returns information about the image at the current location of the cursor.
     * @param mouseX the mouse x-position in screen coordinates.
     * @param mouseY the mouse y-position in screen coordinates.
     * @param frames - list of image frames.
     * @param outputSize - the size of the image in pixels.
     * @return a QString containing cursor text.
     */
    virtual QString _getCursorText( int mouseX, int mouseY,
            const std::vector<int>& frames, const QSize& outputSize ) = 0;


    /**
     * Return the data source of the image.
     * @return - the data source of the image.
     */
    virtual std::shared_ptr<DataSource> _getDataSource() = 0;


    /**
     * Return the image size for the given coordinate index.
     * @param coordIndex an index of a coordinate of the image.
     * @return the corresponding dimension for that coordinate or -1 if none exists.
     */
    virtual int _getDimension( int coordIndex ) const = 0;

    /**
     * Return the number of dimensions in the image.
     * @return the number of image dimensions.
     */
    virtual int _getDimension() const = 0;

    /**
     * Return the dimensions of the displayed image; normally, this will
     * be the number of frames in the RA x DEC directions.  However, if
     * the image is being display as a Frequency x DEC plot, this will be
     * the number of frames in the frequency & DEC axes.
     * @return - the displayed dimensions of the image.
     */
    virtual QSize _getDisplaySize() const = 0;

    /**
     * Return the number of frames for the given axis in the image.
     * @param type  - the axis for which a frame count is needed.
     * @return the number of frames for the given axis in the image.
     */
    virtual int _getFrameCount( Carta::Lib::AxisInfo::KnownType type ) const = 0;


    //Return grid state.
    virtual Carta::State::StateInterface _getGridState() const = 0;


    /**
     * Returns the underlying image.
     */
    virtual std::shared_ptr<Carta::Lib::Image::ImageInterface> _getImage() = 0;


    /**
     * Get the image dimensions.
     * @return - a list containing frame counts for each dimension of the image.
     */
    virtual std::vector<int> _getImageDimensions( ) const = 0;

    virtual std::vector< std::shared_ptr<Carta::Lib::Image::ImageInterface> > _getImages();

    /**
     * Returns the location on the image corresponding to a screen point in
     * pixels.
     * @param screenPt an (x,y) pair of pixel coordinates.
     * @param outputSize - the size in pixels of the output image.
     * @param valid set to true if an image is loaded that can do the translation; otherwise false;
     * @return the corresponding location on the image.
     */
    virtual QPointF _getImagePt( const QPointF& screenPt, const QSize& outputSize, bool* valid ) const = 0;


    /**
     * Return the portion of the image that is displayed given current zoom and
     * pan values.
     * @param size - the size of the displayed image.
     * @return - the portion of the image that is visible.
     */
    virtual QRectF _getInputRect( const QSize& size ) const = 0;

    /**
     * Returns the intensity corresponding to a given percentile.
     * @param frameLow - a lower bound for the image frames or -1 if there is no lower bound.
     * @param frameHigh - an upper bound for the image frames or -1 if there is no upper bound.
     * @param percentiles - a list of numbers in [0,1] for which an intensity is desired.
     * @return - a list of (location,intensity) pairs.
     */
    virtual std::vector<std::pair<int,double> > _getIntensity( int frameLow, int frameHigh,
            const std::vector<double>& percentiles ) const = 0;

    /**
     * Returns whether or not the layer can be loaded with the indicated frames.
     * @param frames - list of frame indices to load.
     * @return - whether or not the layer can be loaded with the indicated frames.
     */
    virtual bool _isLoadable( const std::vector<int>& frames ) const;


    /**
     * Return the layer with the given name, if a name is specified; otherwise, return the current
     * layer.
     * @name - the name of a layer or an empty string to specify the current layer.
     * @return - the current layer.
     */
    virtual std::shared_ptr<Layer> _getLayer( const QString& name );

    /**
     * Return all layers containing images.
     * @return - all layers containing images.
     */
    virtual std::vector<std::shared_ptr<Layer> > _getLayers();

    /**
     * Returns an identifier for the layer.
     * @return - an identifier for the layer.
     */
    virtual QString _getLayerId() const;

    /**
     * Returns the layer identifiers.
     * @return - a list containing identifiers for this layer and its children.
     */
    virtual QStringList _getLayerIds( ) const;

    /**
     * Get the transparency for the layer.
     * @return - a transparency amount for the layer.
     */
    virtual float _getMaskAlpha() const;

    /**
     * Return the color filter for the layer.
     * @return - a color filter for the layer.
     */
    virtual quint32 _getMaskColor() const;

    /**
     * Return the percentile corresponding to the given intensity.
     * @param frameLow a lower bound for the frame index or -1 if there is no lower bound.
     * @param frameHigh an upper bound for the frame index or -1 if there is no upper bound.
     * @param intensity a value for which a percentile is needed.
     * @return the percentile corresponding to the intensity.
     */
    virtual double _getPercentile( int frameLow, int frameHigh, double intensity ) const = 0;


    /**
     * Return the pixel coordinates corresponding to the given world coordinates.
     * @param ra the right ascension (in radians) of the world coordinates.
     * @param dec the declination (in radians) of the world coordinates.
     * @param valid - true if the coordinates are valid; false, otherwise.
     * @return - a point containing the pixel coordinates.
     */
    virtual QPointF _getPixelCoordinates( double ra, double dec, bool* valid ) const = 0;

    /**
     * Return the world coordinates corresponding to the given pixel coordinates.
     * @param pixelX - the first pixel coordinate.
     * @param pixelY - the second pixel coordinate.
     * @param coordSys - the coordinate system.
     * @param valid - true if the pixel coordinates are valid; false otherwise.
     * @return - a point containing the pixel coordinates.
     */
    virtual QPointF _getWorldCoordinates( double ra, double dec,
            Carta::Lib::KnownSkyCS coordSys, bool* valid ) const = 0;

    /**
     * Return the units of the pixels.
     * @return the units of the pixels, or blank if units could not be obtained.
     */
    virtual QString _getPixelUnits() const = 0;

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
    virtual QString _getPixelValue( double x, double y, const std::vector<int>& frames ) const = 0;

    /**
     * Return the graphics for drawing regions.
     * @return - a list of graphics for drawing regions.
     */
    virtual Carta::Lib::VectorGraphics::VGList _getRegionGraphics() const = 0;

    /**
     * Return the rest frequency and units for the image.
     * @return - the image rest frequency and units; a blank string and a negative
     * 		value are returned with the rest frequency can not be found.
     */
    virtual std::pair<double,QString> _getRestFrequency() const = 0;

    /**
     * Return the size of the saved image based on the user defined output size and the aspect
     * ratio mode.
     * @param outputSize - the output image size specified by the user.
     * @param aspectMode - whether the aspect ratio of the image should be preserved (etc).
     * @return - the size of the saved image.
     */
    virtual QSize _getSaveSize( const QSize& outputSize,  Qt::AspectRatioMode aspectMode) const = 0;

    /**
     * Return the color states that are eligible for state changes.
     * @param global - whether color state changes apply to all color maps or only to those that
     *      correspond to selected images.
     * @return - a list of color states whose states may be changed.
     */
    virtual std::vector< std::shared_ptr<ColorState> >  _getSelectedColorStates( bool global ) = 0;


    /**
     * Return the state of this layer.
     * @param truncatePaths - true if full paths to files should not be given.
     * @return - a string representation of the layer state.
     */
    virtual QString _getStateString( bool truncatePaths ) const = 0;

    /**
     * Return the zoom factor for this layer.
     * @return the zoom multiplier.
     */
    virtual double _getZoom() const = 0;

    virtual void _gridChanged( const Carta::State::StateInterface& state) = 0;

    /**
     * Returns whether or not the layer can contain other layers.
     * @return - true if the layer is composite; false otherwise.
     */
    virtual bool _isComposite() const;

    /**
     * Returns true if at least one contour set should be drawn; false otherwise.
     * @return - true if there is at least one contour set to draw; false otherwise.
     */
    virtual bool _isContourDraw() const;

    /**
     * Returns true if the identifier passed in matches the id of this layer or one
     * of its children.
     * @param id - an identifier for a layer.
     * @return - true if the passed in identifier matches this layer or one of its
     *      children; false otherwise.
     */
    virtual bool _isDescendant( const QString& id ) const;

    /**
     * Returns true if the layer contains nothing visible to the user; false
     * otherwise.
     * @return - true if the layer is empty; false otherwise.
     */
    virtual bool _isEmpty() const;


    /**
     * Returns true if this data is selected; false otherwise.
     * @return true if this data is selected; false otherwise.
     */
    bool _isSelected() const;

    /**
     * Returns whether or not the layered images have spectral axes.
     * @return - true if the layered images all have spectral axes; false, otherwise.
     */
    virtual bool _isSpectralAxis() const;

    /**
     * Remove the contour set from this layer.
     * @param contourSet - the contour set to remove from the layer.
     */
    virtual void _removeContourSet( std::shared_ptr<DataContours> contourSet ) = 0;

    /**
     * Generate a new rendered image.
     * @param request - parameters to use in rendering the image.
     */
    void _render( const std::shared_ptr<RenderRequest>& request );

    /**
     * Finish the render.
     */
    void _renderDone();

    /**
     * Start the render.
     */
    virtual void _renderStart() = 0;

    /**
     * Center the image.
     */
    virtual void _resetPan( ) = 0;

    /**
     * Reset the prefereence state of this layer.
     * @param restoreState - the new layer state.
     */
    virtual void _resetState( const Carta::State::StateInterface& restoreState );

    /**
         * Reset the layer contours.
         * @param restoreeState - the new layer state.
         */
    virtual void _resetStateContours(const Carta::State::StateInterface& restoreState );

    /**
     * Reset the zoom to the original value.
     */
    virtual void _resetZoom( ) = 0;

    /**
     * Set the mode used to compose this layer.
     * @param id - the identifier for the layer group where the composition mode will change.
     * @param compositionMode - the mode used to compose this layer.
     * @param errorMsg - a error message if the composition mode was not successfully set.
     */
    virtual bool _setCompositionMode( const QString& id, const QString& compositionMode,
            QString& errorMsg );

    /**
     * Attempts to load an image file.
     * @param fileName - an identifier for the location of the image file.
     * @param success - set to true if the file is successfully loaded.
     * @return - an error message if the file could not be loaded or the id of
     *      the layer if it is successfully loaded.
     */
    virtual QString _setFileName( const QString& fileName, bool* success );


    /**
     * Give the layer (a more user-friendly) name.
     * @param id - an identifier for the layer to rename.
     * @param name - the new name for the layer.
     * @return - true if this layers identifier matches that of the one passed
     *      in and the name was successfully reset; false otherwise.
     */
    virtual bool _setLayerName( const QString& id, const QString& name );

    /**
     * Group or ungroup any child layers.
     * @param grouped - true if child layers should be grouped; false, otherwise.
     * @param viewSize - the view size.
     * @return - true if the operation was performed; false otherwise.
     */
    virtual bool _setLayersGrouped( bool grouped, const QSize& viewSize ) = 0;

    /**
     * Set the color to use for the mask.
     * @param redAmount - the amount of red in [0,255].
     * @param greenAmount - the amount of green in [0,255].
     * @param blueAmount - the amount of blue in [0,255].
     * @param result - a list of errors that might have occurred in setting the
     *      mask color; an empty string otherwise.
     * @return - true if the mask color was changed; false otherwise.
     */
    virtual bool _setMaskColor( const QString& id, int redAmount,
            int greenAmount, int blueAmount ) = 0;

    /**
     * Set the mask color back to its default value.
     */
    virtual void _setMaskColorDefault() = 0;

    /**
     * Set the opacity of the mask.
     * @param alphaAmount - the transparency level in [0,255] with 255 being opaque.
     * @param result - an error message if there was a problem setting the mask opacity or
     *      an empty string otherwise.
     * @return - true if the mask opacity was changed; false otherwise.
     */
    virtual bool _setMaskAlpha( const QString& id, int alphaAmount ) = 0;

    /**
     * Set the mask transparency back to its default value.
     */
    virtual void _setMaskAlphaDefault() = 0;


    /**
     * Set the center for this image's display.
     * @param imgX the x-coordinate of the center.
     * @param imgY the y-coordinate of the center.
     */
    virtual void _setPan( double imgX, double imgY ) = 0;

    /**
     * Set a list of graphics for drawing the current regions.
     * @param regionVGList - graphics for drawing the current regions.
     */
    virtual void _setRegionGraphics( const Carta::Lib::VectorGraphics::VGList& regionVGList ) = 0;

    /**
     * Set this data source selected.
     * @param selected - true if the data source is selected; false otherwise.
     * @return -true if the selected state changed; false otherwise.
     */
    virtual bool _setSelected( QStringList& selectNames );

    virtual void _setSupportAlpha( bool supportAlpha );
    virtual void _setSupportColor( bool supportColor );

    /**
     * Show/hide this layer.
     * @param visible - true to show the layer; false to hide it.
     */
    virtual bool _setVisible( const QString& id, bool visible );

    /**
     * Set the zoom factor for this image.
     * @param zoomFactor the zoom multiplier.
     */
    virtual void _setZoom( double zoomFactor ) = 0;


    virtual void _updateClips( std::shared_ptr<Carta::Lib::NdArray::RawViewInterface>& view,
            double minClipPercentile, double maxClipPercentile, const std::vector<int>& frames ) = 0;

    virtual void _updateColor();



    /**
     *  Constructor.
     */
    Layer( const QString& className, const QString& path, const QString& id );

    static const QString GROUP;
    static const QString LAYER;

    bool m_renderQueued;

    QStack<std::shared_ptr<RenderRequest>> m_renderRequests;

    static LayerCompositionModes* m_compositionModes;

protected slots:
    virtual void _colorChanged();



private:

    QString _getLayerName() const;

    void _initializeSingletons( );
    void _initializeState();


    /**
     * Returns true if the name identifies this layer; false otherwise.
     * @return true if the name identifies this layer; false otherwise.
     */
    bool _isMatch( const QString& name ) const;

    /**
     * Returns true if this layer is not hidden; false otherwise.
     * @return true if the layer is visible; false otherwise.
     */
    bool _isVisible() const;

    Layer(const Layer& other);
    Layer& operator=(const Layer& other);
};
}
}
