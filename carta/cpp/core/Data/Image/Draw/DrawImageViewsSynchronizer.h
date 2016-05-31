/**
 * Synchronizes the main, context, and zoom views of the image so that
 * render parameters do not conflict with each other.
 */

#pragma once
#include <QObject>
#include <memory>


namespace Carta {

namespace Data {

class DrawStackSynchronizer;
class Layer;
class RenderRequest;

class DrawImageViewsSynchronizer : public QObject {

    Q_OBJECT

public:

    /**
     * Constructor.
     */
    DrawImageViewsSynchronizer( QObject* parent = nullptr);

    /**
     * Returns true if the infrastructure is in place to support a zoom view;
     * false otherwise.
     * @return - true if a zoom view can be supported; false otherwise.
     */
    bool isZoomView() const;

    /**
     * Set the draw object for the main view of an image.
     * @param drawStack - object responsible for drawing the main view of an
     *      image.
     */
    void setViewDraw( std::shared_ptr<DrawStackSynchronizer> drawStack );

    /**
     * Set the draw object for the context view of an image.
     * @param stackDraw - object responsible for drawing the context view of an image.
     */
    void setViewDrawContext( std::shared_ptr<DrawStackSynchronizer> stackDraw );

    /**
     * Set the draw object for the zoom view of an image.
     * @param stackDraw - object responsible for drawing the zoom view of an image.
     */
    void setViewDrawZoom( std::shared_ptr<DrawStackSynchronizer> stackDraw );

    /**
     * Start a synchronized rendering.
     * @param datas - the layers to draw.
     * @param request - information about draw parameters that should be used.
     */
    void render( QList<std::shared_ptr<Layer> >& datas,
               const std::shared_ptr<RenderRequest>& request );

    virtual ~DrawImageViewsSynchronizer();

signals:

    /**
     * Signal that all rendered views of the images are done.
     */
    void done();

private slots:

    //The main image view is completed.
    void _doneMain( bool success );
    //The context view of the image is completed.
    void _doneContext( bool success );
    //The zoom view of the image is completed.
    void _doneZoom( bool success );

private:

    /**
     * Start the next rendered view of the image.
     */
    void _startNextDraw();

    bool m_doneMain = true;
    bool m_doneContext = true;
    bool m_doneZoom = true;

    QList<std::shared_ptr<Layer> > m_datas;
    std::shared_ptr<RenderRequest> m_request;

    std::shared_ptr<DrawStackSynchronizer> m_drawMain;
    std::shared_ptr<DrawStackSynchronizer> m_drawContext;
    std::shared_ptr<DrawStackSynchronizer> m_drawZoom;

    DrawImageViewsSynchronizer( const DrawImageViewsSynchronizer& other);
    DrawImageViewsSynchronizer& operator=( const DrawImageViewsSynchronizer& other );

};


}
}
