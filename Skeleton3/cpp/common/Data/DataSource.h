/***
 * Manages and loads a single source of data.
 */

#pragma once

#include "CartaLib/Nullable.h"
#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "Data/IColoredView.h"

#include <QImage>
#include <memory>

namespace NdArray {
    class RawViewInterface;
}

namespace Image {
    class ImageInterface;
}

class CoordinateFormatterInterface;

namespace Carta {
    namespace Lib {
        namespace PixelPipeline {
            class CustomizablePixelPipeline;
        }
    }
}

namespace Carta {
    namespace Core {
        namespace ImageRenderService {
            class Service;
        }
    }
}

namespace Carta {
    namespace Core {
        namespace ScriptedClient {
            class ScriptedRenderService;
        }
    }
}

namespace Carta {

namespace Data {

class DataSource : public QObject, public IColoredView {

Q_OBJECT

public:

    /**
     * Constructor.
     * @param the base path for state identification.
     * @param id the particular id for this object.
     */
    DataSource( );

    /**
     * Returns whether or not the data was successfully loaded.
     * @param fileName an identifier for the location of a data source.
     * @return true if the data souce was successfully loaded; false otherwise.
     */
    bool setFileName( const QString& fileName );

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


    /**
     * Set the data transform.
     * @param name QString a unique identifier for a data transform.
     */
    void setTransformData( const QString& name );

    /**
     * Loads the data source as a QImage.
     * @param frameIndex the channel to load.
     * @param true to force a recompute of the image clip.
     */
    Nullable<QImage> load(int frameIndex, bool forceReload);

    /**
     * Returns true if this data source manages the data corresponding
     * to the fileName; false, otherwise.
     * @param fileName a locator for data.
     */
    bool contains(const QString& fileName) const;

    /**
     * Return a QImage representation of this data.
     * @param frameIndex the index of the spectral coordinate to load.
     * @param autoClip true if clips should be automatically generated; false otherwise.
     * @param clipMinPercentile the minimum clip value.
     * @param clipMaxPercentile the maximum clip value.
     */
    void load(int frameIndex, bool autoClip, double clipMinPercentile, double clipMaxPercentile );

    /**
     * Return the number of channels in the image.
     * @return the number of channels in the image.
     */
    int getFrameCount() const;

    /**
     * Return the number of dimensions in the image.
     * @return the number of image dimensions.
     */
    int getDimensions() const;

    /**
     * Returns the location on the image corresponding to a screen point in
     * pixels.
     * @param screenPt an (x,y) pair of pixel coordinates.
     * @param valid set to true if an image is loaded that can do the translation; otherwise false;
     * @return the corresponding location on the image.
     */
    QPointF getImagePt( QPointF screenPt, bool* valid ) const;
    
    /**
     * Returns the location on the screen corresponding to a location in image coordinates.
     * @param imagePt an (x,y) pair of image coordinates.
     * @param valid set to true if an image is loaded that can do the translation; otherwise false;
     * @return the corresponding pixel coordinates.
     */
    QPointF getScreenPt( QPointF imagePt, bool* valid ) const;

    /**
     * Return the current pan center.
     * @return the centered image location.
     */
    QPointF getCenter() const;

    /**
     * Return the zoom factor for this image.
     * @return the zoom multiplier.
     */
    double getZoom() const;

    /**
     * Get the dimensions of the image viewer (window size).
     * @return the image viewer dimensions.
     */
    QSize getOutputSize() const;

    /**
     * Set the center for this image's display.
     * @param imgX the x-coordinate of the center.
     * @param imgY the y-coordinate of the center.
     */
    void setPan( double imgX, double imgY );

    /**
     * Set the zoom factor for this image.
     * @param zoomFactor the zoom multiplier.
     */
    void setZoom( double zoomFactor );

    /**
     * Return the image size for the given coordinate index.
     * @param coordIndex an index of a coordinate of the image.
     * @return the corresponding dimension for that coordinate or -1 if none exists.
     */
    int getDimension( int coordIndex ) const;

    /**
     * Returns the underlying image.
     */
    std::shared_ptr<Image::ImageInterface> getImage();

    /**
     * Returns the image's file name.
     * @return the path to the image.
     */
    QString getFileName() const;
    
    /**
     * Returns a name suitable for display.
     * @return a display name for the image.
     */
    QString getImageViewName() const;

    /**
     * Returns information about the image at the current location of the cursor.
     * @param mouseX the mouse x-position in screen coordinates.
     * @param mouseY the mouse y-position in screen coordinates.
     * @param frameIndex the current channel index.
     * @return a QString containing cursor text.
     */
    QString getCursorText( int mouseX, int mouseY, int frameIndex);

    /**
     * Return the percentile corresponding to the given intensity.
     * @param frameLow a lower bound for the channel range or -1 if there is no lower bound.
     * @param frameHigh an upper bound for the channel range or -1 if there is no upper bound.
     * @param intensity a value for which a percentile is needed.
     * @return the percentile corresponding to the intensity.
     */
    double getPercentile( int frameLow, int frameHigh, double intensity ) const;
    
    /**
     * Returns the intensity corresponding to a given percentile.
     * @param frameLow a lower bound for the image channels or -1 if there is no lower bound.
     * @param frameHigh an upper bound for the image channels or -1 if there is no upper bound.
     * @param percentile a number [0,1] for which an intensity is desired.
     * @param intensity the computed intensity corresponding to the percentile.
     * @return true if the computed intensity is valid; otherwise false.
     */
    bool getIntensity( int frameLow, int frameHigh, double percentile, double* intensity ) const;
    
    /**
     * Returns the pipeline responsible for rendering the image.
     * @retun the pipeline responsible for rendering the image.
     */
    std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> getPipeline() const;
    
    /**
     * Resize the view of the image.
     */
    void viewResize( const QSize& newSize );

    /**
     * Generate a new QImage.
     */
    void render();

    /**
     * Save a copy of the full image in the current image view.
     * @param filename the full path where the file is to be saved.
     * @param width the width of the saved image.
     * @param height the height of the saved image.
     * @param scale the scale (zoom level) of the saved image.
     * @param aspectRatioMode can be either "ignore", "keep", or "expand".
            See http://doc.qt.io/qt-5/qt.html#AspectRatioMode-enum for further information.
     */
    void saveFullImage( const QString& savename, int width, int height, double scale, Qt::AspectRatioMode aspectRatioMode );

    /**
     * Return the pixel coordinates corresponding to the given world coordinates.
     * @param ra the right ascension (in radians) of the world coordinates.
     * @param dec the declination (in radians) of the world coordinates.
     * @return a list consisting of the x- and y-coordinates of the pixel
     *  corresponding to the given world coordinates.
     */
    QStringList getPixelCoordinates( double ra, double dec );

    /**
     * Return the value of the pixel at (x, y).
     * @param x the x-coordinate of the desired pixel.
     * @param y the y-coordinate of the desired pixel.
     * @return the value of the pixel at (x, y), or blank if it could not be obtained.
     */
    QString getPixelValue( int x, int y );

    virtual ~DataSource();

signals:

    //Notification that a new image has been produced.
    void renderingDone( QImage img);

    /// Return the result of SaveFullImage() after the image has been rendered
    /// and a save attempt made.
    void saveImageResult( bool result );

private slots:

    //Notification from the rendering service that a new image has been produced.
    void _renderingDone( QImage img, int64_t jobId );

    // Asynchronous result from saveFullImage().
    void saveImageResultCB( bool result );

private:
    
    /**
     * Returns the raw data as an array.
     * @param frameLow the lower bound for the channel range or -1 for the whole image.
     * @param frameHigh the upper bound for the channel range or -1 for the whole image.
     * @return the raw data or nullptr if there is none.
     */
    NdArray::RawViewInterface *  _getRawData( int frameLow, int frameHigh ) const;

    void _initializeState();

    void _updateClips( std::shared_ptr<NdArray::RawViewInterface>& view, int frameIndex,
            double minClipPercentile, double maxClipPercentile );

    //Path for loading data - todo-- do we need to store this?
    QString m_fileName;
    bool m_cmapUseCaching;
    bool m_cmapUseInterpolatedCaching;
    int m_cmapCacheSize;

    static const QString DATA_PATH;

    //Pointer to image interface.
    std::shared_ptr<Image::ImageInterface> m_image;

    /// coordinate formatter
    std::shared_ptr<CoordinateFormatterInterface> m_coordinateFormatter;

    /// clip cache, hard-coded to single quantile
    std::vector< std::vector<double> > m_quantileCache;

    /// the rendering service
    std::unique_ptr<Carta::Core::ImageRenderService::Service> m_renderService;

    ///pixel pipeline
    std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> m_pixelPipeline;

    Carta::Core::ScriptedClient::ScriptedRenderService *m_scriptedRenderService;
    
    DataSource(const DataSource& other);
    DataSource operator=(const DataSource& other);
};
}
}
