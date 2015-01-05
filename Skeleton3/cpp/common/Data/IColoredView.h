/*
 * Interface implemented by classes wanting to receive notification of
 * color map changes.
 */

#ifndef ICOLOREDVIEW_H_
#define ICOLOREDVIEW_H_

namespace Carta {

namespace Data {

class IColoredView {
public:
    /**
     * Notification that the selected color map has changed.
     * @param colorMapName a QString identifying the selected color map.
     */
    virtual void colorMapChanged( const QString& colorMapName )=0;

};
}
}


#endif /* ICOLOREDVIEW_H_ */
