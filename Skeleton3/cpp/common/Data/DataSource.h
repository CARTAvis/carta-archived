/***
 * Manages and loads a single source of data.
 */

#pragma once

#include "CartaLib/Nullable.h"
#include "State/ObjectManager.h"
#include "State/StateInterface.h"

#include <QImage>
#include <memory>

namespace Image {
class ImageInterface;
}
class RawView2QImageConverter;

class DataSource : public CartaObject {

public:
    /**
     * Returns whether or not the data was successfully loaded.
     * @param fileName an identifier for the location of a data source.
     * @return true if the data souce was successfully loaded; false otherwise.
     */
    bool setFileName( const QString& fileName );

    /**
     * Set a new color map to use.
     * @param index a color map index.
     */
    void setColorMap( int index );

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
    Nullable<QImage> load(int frameIndex, bool forceClipRecompute, bool autoClip, float clipValue );

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

    virtual ~DataSource();

    const static QString CLASS_NAME;

private:

    /**
     * Constructor.
     * @param the base path for state identification.
     * @param id the particular id for this object.
     */
    DataSource(const QString& path, const QString& id );

    class Factory : public CartaObjectFactory {

    public:

        CartaObject * create (const QString & path, const QString & id)
        {
            return new DataSource (path, id);
        }
    };

    void _initializeState();

    //Reset the amount of clip to perform on the image.
    void resetClipValue();

    //Path for loading data - todo-- do we need to store this?
    QString m_fileName;

    static bool m_registered;
    static const QString DATA_PATH;

    //Pointer to image interface.
    std::shared_ptr<Image::ImageInterface> m_image;

    /// pointer to the rendering algorithm
    std::shared_ptr<RawView2QImageConverter> m_rawView2QImageConverter;
};
