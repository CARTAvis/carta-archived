/***
 * Meshes together data, selection(s), and view(s).
 */

#pragma once

#include <State/StateInterface.h>
#include <State/ObjectManager.h>
#include <Data/IColoredView.h>
#include "CartaLib/CartaLib.h"

#include <QString>
#include <QList>
#include <QObject>
#include <QImage>
#include <memory>

class ImageView;
class CoordinateFormatterInterface;

namespace NdArray {
    class RawViewInterface;
}

namespace Carta {
    namespace Lib {
        namespace PixelPipeline {
            class CustomizablePixelPipeline;
        }
    }
}

namespace Carta {
namespace Data {
class ControllerData;
class GridControls;
class Settings;
class Region;
class RegionRectangle;
class Selection;

class Controller: public QObject, public Carta::State::CartaObject, public IColoredView {

    Q_OBJECT

public:
    
    /**
     * Clear the view.
     */
    void clear();

    /**
     * Add data to this controller.
     * @param fileName the location of the data;
     *        this could represent a url or an absolute path on a local filesystem.
     * @return true upon success, false otherwise.
     */
    bool addData(const QString& fileName);

    /**
     * Apply the indicated clips to managed images.
     * @param minIntensityPercentile the minimum clip percentile [0,1].
     * @param maxIntensityPercentile the maximum clip percentile [0,1].
     * @return a QString indicating if there was an error applying the clips or an empty
     *      string if there was not an error.
     */
    QString applyClips( double minIntensityPercentile, double maxIntensityPercentile );

    /**
     * Close the given image.
     * @param name an identifier for the image to close.
     */
    QString closeImage( const QString& name );

    /**
     * Return the percentile corresponding to the given intensity.
     * @param frameLow a lower bound for the channel range or -1 if there is no lower bound.
     * @param frameHigh an upper bound for the channel range or -1 if there is no upper bound.
     * @param intensity a value for which a percentile is needed.
     * @return the percentile corresponding to the intensity.
     */
    double getPercentile( int frameLow, int frameHigh, double intensity ) const;

    /**
     * Return the pipeline being used to draw the image.
     * @return a Carta::Lib::PixelPipeline::CustomizablePixelPipeline being used to draw the
     *      image.
     */
    std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> getPipeline() const;

    /**
     * Returns the intensity corresponding to a given percentile.
     * @param frameLow a lower bound for the image channels or -1 if there is no lower bound.
     * @param frameHigh an upper bound for the image channels or -1 if there is no upper bound.
     * @param percentile a number [0,1] for which an intensity is desired.
     * @param intensity the computed intensity corresponding to the percentile.
     * @return true if the computed intensity is valid; otherwise false.
     */
    bool getIntensity( int frameLow, int frameHigh, double percentile, double* intensity ) const;

    //IColoredView interface.
    virtual void setColorMap( const QString& colorMapName ) Q_DECL_OVERRIDE;
    virtual void setColorInverted( bool inverted ) Q_DECL_OVERRIDE;
    virtual void setColorReversed( bool reversed ) Q_DECL_OVERRIDE;
    virtual void setColorAmounts( double newRed, double newGreen, double newBlue ) Q_DECL_OVERRIDE;
    virtual void setGamma( double gamma ) Q_DECL_OVERRIDE;



    std::vector<std::shared_ptr<Image::ImageInterface>> getDataSources();


    /**
     * Return the index of the image that is currently at the top of the stack.
     * @return the index of the current image.
     */
    int getSelectImageIndex() const ;

    /**
     * Return the channel upper bound.
     * @return the largest channel in the image.
     */
    int getChannelUpperBound() const;

    /**
     * Returns an identifier for the data source at the given index.
     * @param index the index of a data source.
     * @return an identifier for the image.
     */
    QString getImageName(int index) const;

    /**
     * Make a channel selection.
     * @param val  a channel selection.
     */
    void setFrameChannel(int val);

    /**
     * Return the current channel selection.
     * @return the current channel selection.
     */
    int getFrameChannel() const;

    /**
     * Center the image.
     */
    void resetPan();

    /**
     * Reset the zoom to its original value.
     */
    void resetZoom();

    /**
     *  Make a data selection.
     *  @param imageIndex - a String representing the index of a specific data selection.
     */
    void setFrameImage(int imageIndex);


    /**
     * Set the data transform.
     * @param name QString a unique identifier for a data transform.
     */
    void setTransformData( const QString& name );


    /**
     * Save the state of this controller.
     */
    void saveState();

    /**
     * Save a copy of the full image in the current image view.
     * @param filename the full path where the file is to be saved.
     * @param scale the scale (zoom level) of the saved image.
     * @return an error message if there is an initial problem with saving;
     *      an empty string if the save operation has been initiated.
     */
    QString saveImage( const QString& filename,  double scale );

    /**
     * Save a copy of the full image in the current image view using the current scale.
     * @param filename the full path where the file is to be saved.
     * @return an error message if there is an initial problem with saving;
     *      an empty string if the save operation has been initiated.
     */
    QString saveImage( const QString& filename );

    /**
     * Reset the images that are loaded and other data associated state.
     * @param state - the data state.
     */
    virtual void resetStateData( const QString& state ) Q_DECL_OVERRIDE;

    /**
     * Returns a json string representing the state of this controller.
     * @param type - the type of snapshot to return.
     * @param sessionId - an identifier for the user's session.
     * @return a string representing the state of this controller.
     */
    virtual QString getStateString( const QString& sessionId, SnapshotType type ) const Q_DECL_OVERRIDE;

    /**
     * Set the overall clip amount for the data.
     * @param clipValue a number between 0 and 1.
     * @return an error message if the clip value cannot be set; otherwise and empty string.
     */
    QString setClipValue( double clipValue );

    /**
     * Change the pan of the current image.
     * @param imgX the x-coordinate for the center of the pan.
     * @param imgY the y-coordinate for the center of the pan.
     */
    void updatePan( double imgX , double imgY);

    /**
     * Center the image on the pixel with coordinates (x, y).
     * @param imgX the x-coordinate for the center of the pan.
     * @param imgY the y-coordinate for the center of the pan.
     */
    void centerOnPixel( double imgX , double imgY);

    /**
     * Update the zoom settings.
     * @param centerX the screen x-coordinate where the zoom was centered.
     * @param centerY the screen y-coordinate where the zoom was centered.
     * @param z either positive or negative depending on the desired zoom direction.
     */
    void updateZoom( double centerX, double centerY, double z );

    /**
     * Set the zoom level
     * @param zoomLevel either positive or negative depending on the desired zoom direction.
     */
    void setZoomLevel( double zoomLevel );

    /**
     * Get the current zoom level
     */
    double getZoomLevel( );

    /**
     * Get the image dimensions.
     */
    QStringList getImageDimensions( );

    /**
     * Get the dimensions of the image viewer (window size).
     */
    QStringList getOutputSize( );


    QString getPreferencesId() const;


    /**
     * Return a count of the number of images in the stack.
     * @return the number of images in the stack.
     */
    int getStackedImageCount() const;

    /**
     * Return the pixel coordinates corresponding to the given world coordinates.
     * @param ra the right ascension (in radians) of the world coordinates.
     * @param dec the declination (in radians) of the world coordinates.
     * @return a list consisting of the x- and y-coordinates of the pixel
     *  corresponding to the given world coordinates.
     */
    QStringList getPixelCoordinates( double ra, double dec ) const;

    /**
     * Return the value of the pixel at (x, y).
     * @param x the x-coordinate of the desired pixel.
     * @param y the y-coordinate of the desired pixel.
     * @return the value of the pixel at (x, y), or blank if it could not be obtained.
     */
    QString getPixelValue( double x, double y ) const;

    /**
     * Return the units of the pixels.
     * @return the units of the pixels, or blank if units could not be obtained.
     */
    QString getPixelUnits() const;

    /**
     * Return the coordinates at pixel (x, y) in the given coordinate system.
     * @param x the x-coordinate of the desired pixel.
     * @param y the y-coordinate of the desired pixel.
     * @param system the desired coordinate system.
     * @return the coordinates at pixel (x, y).
     */
    QStringList getCoordinates( double x, double y, Carta::Lib::KnownSkyCS system ) const;

    /**
     * Restore the state from a string representation.
     * @param state- a json representation of state.
     */
    void resetState( const QString& state );

    virtual ~Controller();

    static const QString CLASS_NAME;
    static const QString CURSOR;
    static const QString PLUGIN_NAME;

signals:
    /**
     *  Notification that the image/selection managed by this controller has
     *  changed.
     *  @param controller this Controller.
     */
    void dataChanged( Controller* controller );

    /**
     * Notification that the channel/selection managed by this controller has
     * changed.
     * @param controller this Controller.
     */
    void channelChanged( Controller* controller );

    /// Return the result of SaveFullImage() after the image has been rendered
    /// and a save attempt made.
    void saveImageResult( bool result );

protected:
    virtual QString getSnapType(CartaObject::SnapshotType snapType) const Q_DECL_OVERRIDE;

private slots:

    void _gridChanged( const Carta::State::StateInterface& state, bool applyAll );

    //Refresh the view based on the latest data selection information.
    void _loadView();

    /**
     * The rendering service has finished and produced a new QImage for display.
     */
    void _renderingDone( QImage img );

    /**
     * The view has been resized.
     */
    void _viewResize( const QSize& newSize );

    /**
     * Schedule a frame reload event.
     */
    void _scheduleFrameReload();

    /**
     * Repaint the image.
     */
    void _repaintFrameNow();

    // Asynchronous result from saveFullImage().
    void saveImageResultCB( bool result );

private:

    /**
     *  Constructor.
     */
    Controller( const QString& id, const QString& path );

    class Factory;



    //Provide default values for state.
    void _initializeState();
    void _initializeCallbacks();
    void _initializeSelections();
    void _initializeSelection( Selection* & selection );

    void _clearData();
    QString _makeRegion( const QString& regionType );
    void _removeData( int index );
    void _render();
    void _saveRegions();
    void _scheduleFrameRepaint( const QImage& img );
    void _updateCursor( int mouseX, int mouseY );
    void _updateCursorText(bool notifyClients );

    static bool m_registered;

    static const QString CLIP_VALUE_MIN;
    static const QString CLIP_VALUE_MAX;
    static const QString CLOSE_IMAGE;
    static const QString AUTO_CLIP;
    static const QString DATA;
    static const QString DATA_PATH;
    static const QString REGIONS;
    static const QString CENTER;
    static const QString POINTER_MOVE;
    static const QString ZOOM;

    //Data Selections
    Selection* m_selectChannel;
    Selection* m_selectImage;

    //Data View
    std::shared_ptr<ImageView> m_view;

    std::unique_ptr<GridControls> m_gridControls;


    std::unique_ptr<Settings> m_settings;


    //Data available to and managed by this controller.
    QList<ControllerData* > m_datas;


    QList<Region* > m_regions;

    //Holds image that are loaded and selections on the data.
    Carta::State::StateInterface m_stateData;

    //Separate state for mouse events since they get updated rapidly and not
    //everyone wants to listen to them.
    Carta::State::StateInterface m_stateMouse;

    QSize m_viewSize;

    bool m_reloadFrameQueued;
    bool m_repaintFrameQueued;

    Controller(const Controller& other);
    Controller& operator=(const Controller& other);

};

}
}
