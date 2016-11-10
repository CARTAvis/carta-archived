/***
 * Draws the layers in a stack.
 */

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/InputEvents.h"
#include "CartaLib/VectorGraphics/VGList.h"
#include "Data/Image/Render/RenderRequest.h"
#include "Data/Image/Render/RenderResponse.h"

#include <QString>
#include <QList>
#include <QObject>

#include <memory>

namespace Carta {
    namespace Lib {
        class LayeredViewArbitrary;
    }
}

namespace Carta {
namespace Data {

class Layer;

typedef Carta::Lib::InputEvents::JsonEvent InputEvent;
class DrawStackSynchronizer: public QObject {

    friend class DrawImageViewsSynchronizer;
    friend class ImageContext;
    friend class ImageZoom;

    Q_OBJECT

public:

    /**
     *  Constructor.
     *  @param view- the stack view.
     */
    DrawStackSynchronizer( Carta::Lib::LayeredViewArbitrary* view);

    /**
     * Return the client view size.
     * @return - the size of the stack view on the client.
     */
    QSize getClientSize() const;

    virtual ~DrawStackSynchronizer();

signals:

	/**
	 * Notification that an input event such as a mouse event has occurred.
	 * @param ev - the input event.
	 */
	void inputEvent( InputEvent ev );

    /**
     * The view has been resized.
     */
    void viewResize();

    /**
     * The view has been redrawn.
     * @param drawn - true if an image was drawn; false otherwise (for example if the
     *      view has not been resized and is too small.
     */
    void done( bool drawn );

private slots:

    /**
     * Repaint the image.
     */
    void _repaintFrameNow();

    /**
    * Notification that a stack layer has changed its image or vector graphics.
    */
    void _scheduleFrameRepaint( const std::shared_ptr<RenderResponse>& response );

private:

    void _clear();

    //Return the list of data that can actually load the indicated frames.
    QList<std::shared_ptr<Layer> >  _getLoadableData( const std::shared_ptr<RenderRequest>& request );

    void _render( const std::shared_ptr<RenderRequest>& request );

    //Data View
    std::unique_ptr<Carta::Lib::LayeredViewArbitrary> m_view;

    QList< std::shared_ptr<Layer> > m_layers;
    QMap<QString, std::shared_ptr<RenderResponse> > m_images;

    bool m_repaintFrameQueued;

    int m_renderCount;
    int m_redrawCount;
    int m_selectIndex;

    DrawStackSynchronizer(const DrawStackSynchronizer& other);
    DrawStackSynchronizer& operator=(const DrawStackSynchronizer& other);

};

}
}
