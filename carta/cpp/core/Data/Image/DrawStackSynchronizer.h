/***
 * Draws the layers in a stack.
 */

#pragma once

#include "CartaLib/CartaLib.h"

#include <QString>
#include <QList>
#include <QObject>
#include <QImage>

#include <memory>

namespace Carta {
    namespace Lib {
        class LayeredRemoteVGView;
    }
}

namespace Carta {
namespace Data {

class ControllerData;


class DrawStackSynchronizer: public QObject {

    friend class Controller;

    Q_OBJECT

public:

    /**
     *  Constructor.
     *  @param view- the stack view.
     */
    DrawStackSynchronizer( Carta::Lib::LayeredRemoteVGView* view);

    /**
     * Return the client view size.
     * @return - the size of the stack view on the client.
     */
    QSize getClientSize() const;

    /**
     * Set the potential data that are layers in the stack if they are visible.
     * @param layers- a list of potential stack layers.
     */
    void setLayers( QList< std::shared_ptr<ControllerData> > layers);

    /**
     * Set the top index in the stack.
     * @param index - the top stack index.
     */
    void setSelectIndex( int index );


    virtual ~DrawStackSynchronizer();

signals:
    /**
     * The view has been resized.
     */
    void viewResize();

private slots:

    /**
     * Repaint the image.
     */
    void _repaintFrameNow();

    /**
    * Notification that a stack layer has changed its image or vector graphics.
    */
    void _scheduleFrameRepaint();

private:

    void _render( QList<std::shared_ptr<ControllerData> >& datas,
            std::vector<int> frames, const Carta::Lib::KnownSkyCS& cs, int gridIndex );

    //Data View
    std::unique_ptr<Carta::Lib::LayeredRemoteVGView> m_view;
    QList< std::shared_ptr<ControllerData> > m_layers;
    bool m_repaintFrameQueued;

    int m_renderCount;
    int m_redrawCount;
    int m_selectIndex;

    DrawStackSynchronizer(const DrawStackSynchronizer& other);
    DrawStackSynchronizer& operator=(const DrawStackSynchronizer& other);

};

}
}
