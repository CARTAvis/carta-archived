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
    virtual void setColorMap( const QString& colorMapName )=0;

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

    /**
    * Set the amount of red, green, and blue in the color scale.
    * @param newRed the amount of red; should be in the range [0,1].
    * @param newGreen the amount of green; should be in the range [0,1].
    * @param newBlue the amount of blue; should be in the range[0,1].
    */
    virtual void setColorAmounts( double newRed, double newGreen, double newBlue ) = 0;

    /**
     * Set the gamma color map parameter.
     * @param gamma a color map parameter.
     */
    virtual void setGamma( double gamma ) = 0;

};
}
}


#endif /* ICOLOREDVIEW_H_ */
