/*
 * Stores the results of rendering an image (QImage & vector graphics).
 */

#pragma once

#include <CartaLib/VectorGraphics/VGList.h>
#include <QString>
#include <QImage>

namespace Carta {

namespace Data {

class RenderResponse {

public:

    /**
     * Constructor.
     */
    RenderResponse( const QImage image,
            const Carta::Lib::VectorGraphics::VGList vgList,
            const QString& layerName );

    /**
     * Renders the rendered image.
     * @return - the rendered image.
     */
    QImage getImage() const;

    /**
     * Returns an identifier for the layer that produced the image.
     * @return - an identifier for the layer that produced the image.
     */
    QString getLayerName() const;

    /**
     * Returns the vector graphics associated with the image.
     * @return - the vector graphics associated with the image.
     */
    Carta::Lib::VectorGraphics::VGList getVectorGraphics() const;

    virtual ~RenderResponse();

private:

    QImage m_image;
    Carta::Lib::VectorGraphics::VGList m_graphics;
    QString m_layerId;

    RenderResponse( const RenderResponse& other);
    RenderResponse& operator=( const RenderResponse& other );
};

}
}

