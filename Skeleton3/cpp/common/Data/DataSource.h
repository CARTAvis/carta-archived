/***
 * Manages and loads a single source of data.
 */

#pragma once

#include "Region.h"
#include "CartaLib/Nullable.h"
#include <QImage>
#include <memory>

namespace Image {
class ImageInterface;
}
class RawView2QImageConverter;

class DataSource {

public:

    /**
     * Constructor.
     * @param fileName a locator for finding the data.
     */
    DataSource(const QString& fileName);

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
    void saveState(QString winId, int dataIndex);

    /**
     * Return a QImage representation of this data.
     * @param frameIndex the index of the spectral coordinate to load.
     * @param forceClipRecompute true if the clip should be recomputed; false if
     *      a cached value can be used.
     */
    Nullable<QImage> load(int frameIndex, bool forceClipRecompute) const;

    /**
     * Return the number of channels in the image.
     * @return the number of channels in the image.
     */
    int getFrameCount() const;
    virtual ~DataSource();

private:
    //Reset the amount of clip to perform on the image.
    void resetClipValue();

    //Path for loading data - todo-- do we need to store this?
    QString m_fileName;
    Region m_region;

    //Pointer to image interface.
    std::shared_ptr<Image::ImageInterface> m_image;

    /// pointer to the rendering algorithm
    std::shared_ptr<RawView2QImageConverter> m_rawView2QImageConverter;
};
