/***
 * Manages and loads a single source of data.
 */

#pragma once

#include "CartaLib/Nullable.h"
#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "CartaLib/ICoordinateFormatter.h"
#include <QImage>
#include <memory>

namespace NdArray {
    class RawViewInterface;
}

namespace Image {
    class ImageInterface;
}
namespace Carta {
    namespace Core {
        class RawView2QImageConverter3;
    }
}
class ICoordinateFormatter;

namespace Carta {

namespace Data {

class DataSource : public CartaObject {

public:
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
    void setColorMap( const QString& name );

    /**
     * Sets whether the colors in the map are inverted.
     * @param inverted true if the colors in the map are inverted; false
     *        otherwise.
     */
    void setColorInverted( bool inverted );

    /**
     * Sets whether the colors in the map are reversed.
     * @param reversed true if the colors in the map are reversed; false
     *        otherwise.
     */
    void setColorReversed( bool reversed );

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
     * Saves the state.
     * @param winId an identifier for the DataController displaying the data.
     * @param index of the data in the DataController.
     */
    void saveState(/*QString winId, int dataIndex*/);

    /**
     * Return a QImage representation of this data.
     * @param frameIndex the index of the spectral coordinate to load.
     * @param forceClipRecompute true if the clip should be recomputed; false if
     *      a cached value can be used.
     */
    QImage load(int frameIndex, bool forceClipRecompute, bool autoClip, float clipValue );

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
     * Returns the underlying image.
     */
    std::shared_ptr<Image::ImageInterface> getImage();

    /**
     * Returns the image's file name.
     * @return the path to the image.
     */
    QString getFileName() const;

    /**
     * Returns the raw data as an array.
     * @param channel the index of the channel needed.
     * @return the raw data or nullptr if there is none.
     */
    NdArray::RawViewInterface *  getRawData( int channel ) const;


    QStringList formatCoordinates( int mouseX, int mouseY, int frameIndex);

    virtual ~DataSource();

    const static QString CLASS_NAME;

private:

    /**
     * Constructor.
     * @param the base path for state identification.
     * @param id the particular id for this object.
     */
    DataSource(const QString& path, const QString& id );

    class Factory;

    void _initializeState();

    //Reset the amount of clip to perform on the image.
    void resetClipValue();



    //Path for loading data - todo-- do we need to store this?
    QString m_fileName;
    bool m_cmapUseCaching;
    bool m_cmapUseInterpolatedCaching;
    int m_cmapCacheSize;

    static bool m_registered;
    static const QString DATA_PATH;

    //Pointer to image interface.
    std::shared_ptr<Image::ImageInterface> m_image;

    /// pointer to the rendering algorithm
    //std::shared_ptr<RawView2QImageConverter3> m_rawView2QImageConverter;
    std::unique_ptr<Carta::Core::RawView2QImageConverter3> m_rawView2QImageConverter;

    /// coordinate formatter
    CoordinateFormatterInterface::SharedPtr m_coordinateFormatter;

};
}
}
