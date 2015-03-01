/***
 * Meshes together data, selection(s), and view(s).
 */

#pragma once

#include "CartaLib/ICoordinateFormatter.h"
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

namespace Data {

class DataSource;
class Region;
class RegionRectangle;
class Selection;

class Controller: public QObject, public CartaObject, public IColoredView {

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
     * Close the given image.
     * @param name an identifier for the image to close.
     */
    QString closeImage( const QString& name );

    /**
     * Returns the raw data.
     * @param fileName a full path to the data.
     * @param channel a channel frame specifying a subset of the data.
     * @return the raw data if it exists; otherwise, a nullptr.
     */
    NdArray::RawViewInterface *  getRawData( const QString& fileName, int channel ) const;

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
     * Returns an identifier for the data source at the given index.
     * @param index the index of a data source.
     * @return an identifier for the image.
     */
    QString getImageName(int index) const;

    /**
     * Make a channel selection.
     * @param val a String representing a channel selection.
     */

    void setFrameChannel(const QString& val);

    /**
     *  Make a data selection.
     *  @param val a String representing the index of a specific data selection.
     */
    void setFrameImage(const QString& val);


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
     * Returns a json string representing the state of this controller.
     * @return a string representing the state of this controller.
     */
    virtual QString getStateString() const;

    void setClipValue( const QString& params );

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
     *  Notification that the data/selection managed by this controller has
     *  changed.
     *  @param controller this Controller.
     */
    void dataChanged( Controller* controller );

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
    QString _makeRegion( const QString& regionType );
    void _removeData( int index );
    void _render();
    void _saveRegions();
    void _scheduleFrameRepaint( const QImage& img );
    void _updateCursor( int mouseX, int mouseY );

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

    //Separate state for mouse events since they get updated rapidly and not
    //everyone wants to listen to them.
    StateInterface m_stateMouse;

    QSize m_viewSize;

    bool m_reloadFrameQueued;
    bool m_repaintFrameQueued;

    Controller(const Controller& other);
    Controller operator=(const Controller& other);

};

}
}
