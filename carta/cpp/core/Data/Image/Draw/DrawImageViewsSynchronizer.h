/**
 * Synchronizes the main, context, and zoom views of the image so that
 * render parameters do not conflict with each other.
 */

#pragma once
#include <QObject>
#include <QQueue>
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
     * Returns true if there is support for a context view; false otherwise.
     * @return - true if a context view is supported; false otherwise.
     */
    bool isContextView() const;



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
     * @param request - information about draw parameters that should be used.
     */
    void render( const std::shared_ptr<RenderRequest>& request );

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

    //Minimizes the size of the queue by indicating if the request is already on the
    //stack - could be made more sophisticated.
    bool _isRequested( const std::shared_ptr<RenderRequest>& request ) const;

    /**
     * Start the next rendered view of the image.
     */
    void _startNextDraw();

    bool m_busy;

    std::shared_ptr<DrawStackSynchronizer> m_drawMain;
    std::shared_ptr<DrawStackSynchronizer> m_drawContext;
    std::shared_ptr<DrawStackSynchronizer> m_drawZoom;

    QQueue<std::shared_ptr<RenderRequest> > m_requests;

    DrawImageViewsSynchronizer( const DrawImageViewsSynchronizer& other);
    DrawImageViewsSynchronizer& operator=( const DrawImageViewsSynchronizer& other );

};


}
}
