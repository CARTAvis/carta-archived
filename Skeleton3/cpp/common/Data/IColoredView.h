/*
 * Interface implemented by classes wanting to receive notification of
 * color map changes.
 */

#ifndef ICOLOREDVIEW_H_
#define ICOLOREDVIEW_H_

namespace Image {
class ImageInterface;
}

namespace Carta {

namespace Data {

class IColoredView {
public:
    /**
     * Notification that the selected color map has changed.
     * @param colorMapName a QString identifying the selected color map.
     */
    virtual void colorMapChanged( const QString& colorMapName )=0;

    /**
     * Sets whether or not the colors in the map have been inverted.
     * @param inverted true if the colors are inverted; false otherwise.
     */
    virtual void setColorInverted( bool inverted ) = 0;

    /**
     * Sets whether or not the colors in the map have been reversed.
     * @param reversed true if the colors have been reversed; false otherwise.
     */
    virtual void setColorReversed( bool reversed ) = 0;

    virtual std::vector<std::shared_ptr<Image::ImageInterface>> getDataSources() = 0;

};
}
}


#endif /* ICOLOREDVIEW_H_ */
