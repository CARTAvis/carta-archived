/***
 * Meshes together data, selection(s), and view(s).
 */

#pragma once

#include <State/StateInterface.h>
#include <State/ObjectManager.h>
#include <Data/IColoredView.h>

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

class DataSource;
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
     */
    void addData(const QString& fileName);

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
    /**
     * Set the pixel cache size.
     * @param size the new pixel cache size.
     */
    virtual void setCacheSize( int size ) Q_DECL_OVERRIDE;

    /**
     * Set whether or not to use pixel cache interpolation.
     * @param enabled true if pixel cache interpolation should be used; false otherwise.
     */
    virtual void setCacheInterpolation( bool enabled ) Q_DECL_OVERRIDE;

    /**
     * Set whether or not to use pixel caching.
     * @param enabled true if pixel caching should be used; false otherwise.
     */
    virtual void setPixelCaching( bool enabled ) Q_DECL_OVERRIDE;


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
     *  Make a data selection.
     *  @param val a String representing the index of a specific data selection.
     */
    void setFrameImage(int imageIndex);


    /**
     * Set the data transform.
     * @param name QString a unique identifier for a data transform.
     */
    void setTransformData( const QString& name );

    /**
     * Returns the state associated with the key.
     * @param key a look up for a specific state.
     */
    int getState( const QString& type, const QString& key );

    /**
     * Save the state of this controller.
     */
    void saveState();

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
     * Change the pan of the current image.
     * @param imgX the x-coordinate for the center of the pan.
     * @param imgY the y-coordinate for the center of the pan.
     */
    void updatePan( double imgX , double imgY);

    /**
     * Update the zoom settings.
     * @param centerX the screen x-coordinate where the zoom was centered.
     * @param centerY the screen y-coordinate where the zoom was centered.
     * @param z either positive or negative depending on the desired zoom direction.
     */
    void updateZoom( double centerX, double centerY, double z );

    /**
     * Return a count of the number of images in the stack.
     * @return the number of images in the stack.
     */
    int getStackedImageCount() const;

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

protected:
    virtual QString getType(CartaObject::SnapshotType snapType) const Q_DECL_OVERRIDE;

private slots:

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
    static const QString DATA_COUNT;
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

    //Data available to and managed by this controller.
    QList<DataSource* > m_datas;



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
    Controller operator=(const Controller& other);

};

}
}
