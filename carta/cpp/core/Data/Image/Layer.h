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
namespace Core {
namespace ImageSaveService {
class ImageSaveService;
}
}


namespace Data {

class ColorState;
class DataContours;
class DataSource;
class LayerCompositionModes;

class Layer : public QObject, public Carta::State::CartaObject {

    friend class Controller;
    friend class LayerGroup;
    friend class Stack;
    friend class DrawGroupSynchronizer;
    friend class DrawStackSynchronizer;

    Q_OBJECT

public:


    static const QString CLASS_NAME;


    virtual ~Layer();

signals:

    void contourSetRemoved( const QString& name );
    void contourSetAdded(Layer* data, const QString& name );


    //Notification that a new image has been produced.
    void renderingDone();

    /// Return the result of SaveFullImage() after the image has been rendered
    /// and a save attempt made.
    void saveImageResult( bool result );

    void colorStateChanged();

protected:
    /**
     * Add a contour set.
     * @param contour - the contour set to add.
     */
    virtual void _addContourSet( std::shared_ptr<DataContours> contour ) = 0;

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
     * @return a QString containing cursor text.
     */
    virtual QString _getCursorText( int mouseX, int mouseY,
            const std::vector<int>& frames ) = 0;


    /**
     * Return the data source of the image.
     * @return - the data source of the image.
     */
    virtual std::shared_ptr<DataSource> _getDataSource() = 0;

    virtual std::vector< std::shared_ptr<DataSource> > _getDataSources() = 0;


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
     * Return the number of frames for the given axis in the image.
     * @param type  - the axis for which a frame count is needed.
     * @return the number of frames for the given axis in the image.
     */
    virtual int _getFrameCount( Carta::Lib::AxisInfo::KnownType type ) const = 0;


    //Return grid state.
    virtual Carta::State::StateInterface _getGridState() const = 0;

    /**
     * Return an identifier for the layer.
     * @return - a layer identifier.
     */
    virtual QString _getId() const;

    /**
     * Returns the underlying image.
     */
    virtual std::shared_ptr<Carta::Lib::Image::ImageInterface> _getImage() = 0;
    virtual std::vector< std::shared_ptr<Carta::Lib::Image::ImageInterface> > _getImages();

    /**
     * Returns the location on the image corresponding to a screen point in
     * pixels.
     * @param screenPt an (x,y) pair of pixel coordinates.
     * @param valid set to true if an image is loaded that can do the translation; otherwise false;
     * @return the corresponding location on the image.
     */
    virtual QPointF _getImagePt( QPointF screenPt, bool* valid ) const = 0;


    /**
     * Returns the intensity corresponding to a given percentile.
     * @param frameLow a lower bound for the image frames or -1 if there is no lower bound.
     * @param frameHigh an upper bound for the image frames or -1 if there is no upper bound.
     * @param percentile a number [0,1] for which an intensity is desired.
     * @param intensity the computed intensity corresponding to the percentile.
     * @return true if the computed intensity is valid; otherwise false.
     */
    virtual bool _getIntensity( int frameLow, int frameHigh, double percentile, double* intensity ) const = 0;

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
     * Get the dimensions of the image viewer (window size).
     * @return the image viewer dimensions.
     */
    virtual QSize _getOutputSize() const = 0;

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
     * @return a list consisting of the x- and y-coordinates of the pixel
     *  corresponding to the given world coordinates.
     */
    virtual QStringList _getPixelCoordinates( double ra, double dec ) const = 0;

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
     * Returns the location on the screen corresponding to a location in image coordinates.
     * @param imagePt an (x,y) pair of image coordinates.
     * @param valid set to true if an image is loaded that can do the translation; otherwise false;
     * @return the corresponding pixel coordinates.
     */
    virtual QPointF _getScreenPt( QPointF imagePt, bool* valid ) const = 0;

    virtual std::vector< std::shared_ptr<ColorState> >  _getSelectedColorStates() = 0;


    /**
     * Return the state of this layer.
     * @param truncatePaths - true if full paths to files should not be given.
     * @return - a string representation of the layer state.
     */
    virtual QString _getStateString( bool truncatePaths ) const = 0;

    /**
     * Return the layer vector graphics.
     * @return - the layer vector graphics, which can include both the grid and contours.
     */
    virtual Carta::Lib::VectorGraphics::VGList _getVectorGraphics();

    /**
     * Return the zoom factor for this layer.
     * @return the zoom multiplier.
     */
    virtual double _getZoom() const = 0;

    virtual void _gridChanged( const Carta::State::StateInterface& state) = 0;


    /**
         * Returns true if at least one contour set should be drawn; false otherwise.
         * @return - true if there is at least one contour set to draw; false otherwise.
         */
    virtual bool _isContourDraw() const;

    /**
     * Returns true if this data is selected; false otherwise.
     * @return true if this data is selected; false otherwise.
     */
    bool _isSelected() const;

    /**
     * Return a QImage representation of this data.
     * @param frames - a list of frames to load, one for each of the known axis types.
     * @param autoClip true if clips should be automatically generated; false otherwise.
     * @param clipMinPercentile the minimum clip value.
     * @param clipMaxPercentile the maximum clip value.
     */
    virtual void _load( vector<int> frames, bool autoClip, double clipMinPercentile,
            double clipMaxPercentile ) = 0;

    /**
     * Remove the contour set from this layer.
     * @param contourSet - the contour set to remove from the layer.
     */
    virtual void _removeContourSet( std::shared_ptr<DataContours> contourSet ) = 0;


    /**
     * Generate a new QImage.
     * @param frames - list of image frames.
     * @param cs - an enumerated coordinate system type.
     */
    virtual void _render( const std::vector<int>& frames,
            const Carta::Lib::KnownSkyCS& cs, bool topOfStack ) = 0;

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
     * Reset the color map information for this data.
     * @param colorState - stored information about the color map.
     */
    virtual void _setColorMapGlobal( std::shared_ptr<ColorState> colorState ) = 0;

    /**
     * Set the mode used to compose this layer.
     * @param id - the identifier for the layer group where the composition mode will change.
     * @param compositionMode - the mode used to compose this layer.
     * @param errorMsg - a error message if the composition mode was not successfully set.
     */
    virtual bool _setCompositionMode( const QString& id, const QString& compositionMode,
            QString& errorMsg );

    /**
     * Returns whether or not the data was successfully loaded.
     * @param fileName an identifier for the location of a data source.
     * @return true if the data souce was successfully loaded; false otherwise.
     */
    virtual bool _setFileName( const QString& fileName );

    /**
     * Set the layer identifier.
     * @param id - the layer identifier.
     */
    virtual void _setId( const QString& id );
    virtual QString _setImageOrder( const QString& groupId, const std::vector<int>& indices );

    virtual bool _setLayersGrouped( bool grouped ) = 0;

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
     * Set this data source selected.
     * @param selected - true if the data source is selected; false otherwise.
     * @return -true if the selected state changed; false otherwise.
     */
    virtual bool _setSelected( const QStringList& selectNames );

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
     * Resize the view of the image.
     */
    virtual void _viewResize( const QSize& newSize ) = 0;

    /**
     *  Constructor.
     */
    Layer( const QString& className, const QString& path, const QString& id );

    static const QString LAYER_NAME;
    static const QString GROUP;
    static const QString LAYER;
    static const QString SELECTED;

    QImage m_qimage;
    Carta::Lib::VectorGraphics::VGList m_vectorGraphics;
    static LayerCompositionModes* m_compositionModes;

protected slots:
    virtual void _colorChanged();

private slots:
    // Asynchronous result from saveFullImage().
    void _saveImageResultCB( bool result );


private:

    QString _getLayerName() const;

    /**
     * Return the current image.
     * @return - the current image.
     */
    QImage _getQImage() const;




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

    /**
     * Restore the state of this layer.
     * @param stateStr - the new layer state.
     */
    virtual void _resetState( const QString& stateStr );

    /**
     * Save a copy of the full image in the current image view.
     * @param saveName the full path where the file is to be saved.
     * @param scale the scale (zoom level) of the saved image.
     * @param frames - list of image frames.
     * @return an error message if there was a problem saving the image.
     */
    virtual QString _saveImage( const QString& saveName,  double scale, const std::vector<int>& frames ) = 0;

    /// Saves images
    Carta::Core::ImageSaveService::ImageSaveService *m_saveService;

    Layer(const Layer& other);
    Layer& operator=(const Layer& other);
};
}
}
