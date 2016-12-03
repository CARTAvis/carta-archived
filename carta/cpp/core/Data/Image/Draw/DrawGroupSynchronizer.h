/***
 * Draws the layers in a group using a combine mode.
 */

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/VectorGraphics/VGList.h"


#include <QString>
#include <QList>
#include <QObject>
#include <QImage>
#include <QMap>

#include <memory>

namespace Carta {
namespace Data {

class Layer;
class RenderRequest;
class RenderResponse;


class DrawGroupSynchronizer: public QObject {


    Q_OBJECT

public:

    /**
     *  Constructor.
     */
    DrawGroupSynchronizer();

    /**
     * Render a group of images using the given input parameters.
     * @param request - stored parameters for the render.
     */
    void render( const std::shared_ptr<RenderRequest>& request);

    /**
     * Set the group layers to combine.
     * @param layers - layers in the group to combine.
     */
    void setLayers( QList< std::shared_ptr<Layer> > layers);

    /**
     * Set the method of combining group layers.
     * @param combineMode - an identifier of a method for combining group layers.
     */
    void setCombineMode ( const QString& combineMode );

    /**
     * Set the size of the combined image that is produced.
     * @param newSize - the size of the produced image.
     */
    void viewResize( const QSize& newSize );

    virtual ~DrawGroupSynchronizer();

signals:

    /**
     * Signal that rendering has finished.
     */
    void done( QImage img, Carta::Lib::VectorGraphics::VGList graphics );


private slots:

    /**
    * Notification that a stack layer has finished rendering its image.
    */
    void _scheduleFrameRepaint( const std::shared_ptr<RenderResponse>& );

private:

    //Layers to combine
    QList< std::shared_ptr<Layer> > m_layers;
    bool m_repaintFrameQueued;
    QMap<QString, std::shared_ptr<RenderResponse> > m_images;
    QString m_combineMode;
    QSize m_imageSize;
    int m_renderCount;
    int m_redrawCount;

    DrawGroupSynchronizer(const DrawGroupSynchronizer& other);
    DrawGroupSynchronizer& operator=(const DrawGroupSynchronizer& other);

};

}
}
