/***
 * Represents a single image layer in the stack.
 */

#pragma once
#include "Layer.h"
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
}


namespace Data {

class ColorState;
class DrawSynchronizer;
class DataContours;
class DataGrid;
class DataSource;

class LayerData : public Layer {

friend class Controller;
friend class LayerGroup;
friend class DrawStackSynchronizer;

Q_OBJECT

public:


    static const QString CLASS_NAME;


    virtual ~LayerData();



protected:

    /**
         * Add a contour set.
         * @param contour - the contour set to add.
         */
    virtual void _addContourSet( std::shared_ptr<DataContours> contour );

    /**
        * Remove the color map.
        */
       virtual void _clearColorMap();

    /**
         * Return a list of information about the axes to display.
         * @return a list showing information about which axes should be displayed and how
         *  they should be displayed.
         */
    //std::vector<Carta::Lib::AxisDisplayInfo> _getAxisDisplayInfo() const;

    //virtual std::shared_ptr<Carta::Core::ImageRenderService::Service> _getRenderer() Q_DECL_OVERRIDE;
    /**
     * Respond to a change in display axes.
     * @param displayAxisTypes - the x-, y-, and z- axes to display.
     * @param frames - list of image frames.
     */
    virtual void _displayAxesChanged(std::vector<Carta::Lib::AxisInfo::KnownType> displayAxisTypes,
                const std::vector<int>& frames ) Q_DECL_OVERRIDE;

    virtual Carta::Lib::AxisInfo::KnownType _getAxisType( int index ) const Q_DECL_OVERRIDE;
    virtual Carta::Lib::AxisInfo::KnownType _getAxisXType() const Q_DECL_OVERRIDE;
    virtual Carta::Lib::AxisInfo::KnownType _getAxisYType() const Q_DECL_OVERRIDE;
    virtual std::vector<Carta::Lib::AxisInfo::KnownType> _getAxisZTypes() const Q_DECL_OVERRIDE;
    virtual std::vector<Carta::Lib::AxisInfo::KnownType> _getAxisTypes() const Q_DECL_OVERRIDE;

    virtual QPointF _getCenterPixel() const Q_DECL_OVERRIDE;

    virtual std::shared_ptr<ColorState> _getColorState() Q_DECL_OVERRIDE;


    /**
         * Return the contour set with the indicated name.
         * @return - the corresponding contour set with the designated name or a nullptr
         *  if no such set exists.
         */
    virtual std::shared_ptr<DataContours> _getContour( const QString& name ) Q_DECL_OVERRIDE;


    /**
     * Return all contour sets for this particular layer.
     * @return - all contour sets in the layer.
     */
    virtual std::set< std::shared_ptr<DataContours> > _getContours() Q_DECL_OVERRIDE;

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
            const std::vector<int>& frames) const Q_DECL_OVERRIDE;

    /**
         * Return the coordinate system in use.
         * @return - an enumerated coordinate system type.
         */
    virtual Carta::Lib::KnownSkyCS _getCoordinateSystem() const Q_DECL_OVERRIDE;

    /**
     * Returns information about the image at the current location of the cursor.
     * @param mouseX the mouse x-position in screen coordinates.
     * @param mouseY the mouse y-position in screen coordinates.
     * @param frames - list of image frames.
     * @return a QString containing cursor text.
     */
    virtual QString _getCursorText( int mouseX, int mouseY,
            const std::vector<int>& frames ) Q_DECL_OVERRIDE;

    /**
     * Return the data source of the image.
     * @return - the data source of the image.
     */
    virtual std::shared_ptr<DataSource> _getDataSource() Q_DECL_OVERRIDE;


    /**
     * Return the image size for the given coordinate index.
     * @param coordIndex an index of a coordinate of the image.
     * @return the corresponding dimension for that coordinate or -1 if none exists.
     */
    virtual int _getDimension( int coordIndex ) const Q_DECL_OVERRIDE;


    /**
     * Return the number of dimensions in the image.
     * @return the number of image dimensions.
     */
    virtual int _getDimension() const Q_DECL_OVERRIDE;


    /**
     * Return the number of frames for the given axis in the image.
     * @param type  - the axis for which a frame count is needed.
     * @return the number of frames for the given axis in the image.
     */
    virtual int _getFrameCount( Carta::Lib::AxisInfo::KnownType type ) const Q_DECL_OVERRIDE;



    //Return data source state.
     virtual Carta::State::StateInterface _getGridState() const Q_DECL_OVERRIDE;

     /**
      * Returns the underlying image.
      */
     virtual std::shared_ptr<Carta::Lib::Image::ImageInterface> _getImage() Q_DECL_OVERRIDE;

     /**
      * Get the image dimensions.
      * @return - a list containing frame counts for each dimension of the image.
      */
     virtual std::vector<int> _getImageDimensions( ) const Q_DECL_OVERRIDE;

     /**
      * Get the transparency for the layer.
      * @return - a transparency amount for the layer.
      */
     virtual float _getMaskAlpha() const Q_DECL_OVERRIDE;

     /**
      * Return the color filter for the layer.
      * @return - a color filter for the layer.
      */
     virtual quint32 _getMaskColor() const Q_DECL_OVERRIDE;

     /**
      * Return the percentile corresponding to the given intensity.
      * @param frameLow a lower bound for the frame index or -1 if there is no lower bound.
      * @param frameHigh an upper bound for the frame index or -1 if there is no upper bound.
      * @param intensity a value for which a percentile is needed.
      * @return the percentile corresponding to the intensity.
      */
     virtual double _getPercentile( int frameLow, int frameHigh, double intensity ) const Q_DECL_OVERRIDE;

     /**
      * Return the pixel coordinates corresponding to the given world coordinates.
      * @param ra the right ascension (in radians) of the world coordinates.
      * @param dec the declination (in radians) of the world coordinates.
      * @return a list consisting of the x- and y-coordinates of the pixel
      *  corresponding to the given world coordinates.
      */
     virtual QStringList _getPixelCoordinates( double ra, double dec ) const Q_DECL_OVERRIDE;


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
    virtual QString _getPixelValue( double x, double y, const std::vector<int>& frames ) const Q_DECL_OVERRIDE;



    /**
     * Returns the location on the screen corresponding to a location in image coordinates.
     * @param imagePt an (x,y) pair of image coordinates.
     * @param valid set to true if an image is loaded that can do the translation; otherwise false;
     * @return the corresponding pixel coordinates.
     */
    virtual QPointF _getScreenPt( QPointF imagePt, bool* valid ) const Q_DECL_OVERRIDE;

    virtual std::vector< std::shared_ptr<ColorState> >  _getSelectedColorStates() Q_DECL_OVERRIDE;

    /**
     * Return the state of this layer.
     * @param truncatePaths - true if full paths to files should not be given.
     * @return - a string representation of the layer state.
     */
    virtual QString _getStateString( bool truncatePaths ) const Q_DECL_OVERRIDE;

    /**
     * Return the zoom factor for this image.
     * @return the zoom multiplier.
     */
    virtual double _getZoom() const;


    virtual void _gridChanged( const Carta::State::StateInterface& state) Q_DECL_OVERRIDE;

    /**
         * Return a QImage representation of this data.
         * @param frames - a list of frames to load, one for each of the known axis types.
         * @param autoClip true if clips should be automatically generated; false otherwise.
         * @param clipMinPercentile the minimum clip value.
         * @param clipMaxPercentile the maximum clip value.
         */
    virtual void _load( vector<int> frames, bool autoClip, double clipMinPercentile,
                double clipMaxPercentile ) Q_DECL_OVERRIDE;


    /**
     * Center the image.
     */
    virtual void _resetPan() Q_DECL_OVERRIDE;

    /**
     * Reset the zoom to the original value.
     */
    virtual void _resetZoom() Q_DECL_OVERRIDE;

    /**
     * Attempts to load an image file.
     * @param fileName - an identifier for the location of the image file.
     * @param success - set to true if the file is successfully loaded.
     * @return - an error message if the file could not be loaded or the id of
     *      the layer if it is successfully loaded.
     */
    virtual QString _setFileName( const QString& fileName, bool* success ) Q_DECL_OVERRIDE;

    /**
     * Returns the location on the image corresponding to a screen point in
     * pixels.
     * @param screenPt an (x,y) pair of pixel coordinates.
     * @param valid set to true if an image is loaded that can do the translation; otherwise false;
     * @return the corresponding location on the image.
     */
    virtual QPointF _getImagePt( QPointF screenPt, bool* valid ) const Q_DECL_OVERRIDE;



    /**
     * Returns the intensity corresponding to a given percentile.
     * @param frameLow a lower bound for the image frames or -1 if there is no lower bound.
     * @param frameHigh an upper bound for the image frames or -1 if there is no upper bound.
     * @param percentile a number [0,1] for which an intensity is desired.
     * @param intensity the computed intensity corresponding to the percentile.
     * @return true if the computed intensity is valid; otherwise false.
     */
    virtual bool _getIntensity( int frameLow, int frameHigh, double percentile, double* intensity ) const Q_DECL_OVERRIDE;


    /**
     * Return the units of the pixels.
     * @return the units of the pixels, or blank if units could not be obtained.
     */
    virtual QString _getPixelUnits() const Q_DECL_OVERRIDE;

    /**
     * Get the dimensions of the image viewer (window size).
     * @return the image viewer dimensions.
     */
    virtual QSize _getOutputSize() const Q_DECL_OVERRIDE;


    /**
         * Returns true if at least one contour set should be drawn; false otherwise.
         * @return - true if there is at least one contour set to draw; false otherwise.
         */
    virtual bool _isContourDraw() const Q_DECL_OVERRIDE;

    /**
         * Remove the contour set from this layer.
         * @param contourSet - the contour set to remove from the layer.
         */
    virtual void _removeContourSet( std::shared_ptr<DataContours> contourSet ) Q_DECL_OVERRIDE;

    /**
     * Generate a new QImage.
     * @param frames - list of image frames.
     * @param cs - an enumerated coordinate system type.
     */
    virtual void _render( const std::vector<int>& frames,
            const Carta::Lib::KnownSkyCS& cs, bool topOfStack ) Q_DECL_OVERRIDE;


    /**
     * Reset the prefereence state of this layer.
     * @param restoreState - the new layer state.
     */
    virtual void _resetState( const Carta::State::StateInterface& restoreState ) Q_DECL_OVERRIDE;

    /**
         * Reset the layer contours.
         * @param restoreeState - the new layer state.
         */
    virtual void _resetStateContours(const Carta::State::StateInterface& restoreState );

    /**
     * Reset the color map information for this data.
     * @param colorState - stored information about the color map.
     */
    virtual void _setColorMapGlobal( std::shared_ptr<ColorState> colorState ) Q_DECL_OVERRIDE;

    virtual bool _setLayersGrouped( bool grouped ) Q_DECL_OVERRIDE;

    /**
     * Set the opacity of the mask.
     * @param alphaAmount - the transparency level in [0,255] with 255 being opaque.
     * @param result - an error message if there was a problem setting the mask opacity or
     *      an empty string otherwise.
     * @return - true if the mask opacity was changed; false otherwise.
     */
    virtual bool _setMaskAlpha( const QString& id, int alphaAmount );

    virtual void _setMaskAlphaDefault();

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
            int greenAmount, int blueAmount );

    virtual void _setMaskColorDefault();


    /**
     * Set the center for this image's display.
     * @param imgX the x-coordinate of the center.
     * @param imgY the y-coordinate of the center.
     */
    virtual void _setPan( double imgX, double imgY ) Q_DECL_OVERRIDE;

    virtual void _setSupportAlpha( bool supportAlpha );
    virtual void _setSupportColor( bool supportColor );

    /**
     * Set the zoom factor for this image.
     * @param zoomFactor the zoom multiplier.
     */
    virtual void _setZoom( double zoomFactor ) Q_DECL_OVERRIDE;
    virtual void _updateClips( std::shared_ptr<Carta::Lib::NdArray::RawViewInterface>& view,
                double minClipPercentile, double maxClipPercentile, const std::vector<int>& frames ) Q_DECL_OVERRIDE;


    virtual void _updateColor() Q_DECL_OVERRIDE;

    /**
     * Reset the view to its previous state after a save.
     */
    virtual void _viewReset() Q_DECL_OVERRIDE;

    /**
     * Resize the view of the image.
     */
    virtual void _viewResize( const QSize& newSize ) Q_DECL_OVERRIDE;

protected slots:
    virtual void _colorChanged() Q_DECL_OVERRIDE;

private slots:

    //Notification from the rendering service that a new image and assiciated vector
    //graphics have been produced.
    void _renderingDone(  QImage image,
                          Carta::Lib::VectorGraphics::VGList vgList,
                          Carta::Lib::VectorGraphics::VGList contourList,
                          int64_t jobId );

private:

    /**
     * Get label format information for the given axis and type.
     * @param axisIndex - the plotting index of the axis (0 or 1).
     * @param axisType - the type of the axis (declination, ra, etc )
     * @return - information about how the axis labels should be formatted.
     */
    Carta::Lib::AxisLabelInfo _getAxisLabelInfo( int axisIndex, Carta::Lib::AxisInfo::KnownType axisType ) const;


    void _initializeState();


    /**
     *  Constructor.
     */
    LayerData( const QString& path, const QString& id );

    class Factory;
    static bool m_registered;

    static const QString LAYER_COLOR;
    static const QString LAYER_ALPHA;
    static const QString MASK;


    std::unique_ptr<DataGrid> m_dataGrid;

    std::set< std::shared_ptr<DataContours> > m_dataContours;

    //Pointer to image interface.
    std::shared_ptr<DataSource> m_dataSource;

    QSize m_viewSize;
    QPointF m_viewPan;


     /// image-and-grid-service result synchronizer
    std::unique_ptr<DrawSynchronizer> m_drawSync;
    std::shared_ptr<ColorState> m_stateColor;

    LayerData(const LayerData& other);
    LayerData& operator=(const LayerData& other);
};
}
}
