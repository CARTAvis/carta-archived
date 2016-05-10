/**
 * Combines the layers of a stack into an image.
 */

#pragma once

#include "CartaLib/VectorGraphics/VGList.h"
#include <QSize>
#include <QImage>

namespace Carta {
namespace Lib {
    class IQImageCombiner;
}
namespace Data {

class SaveView;

class SaveViewLayered {

    public:

        /**
         * Constructor.
         */
        SaveViewLayered();

        /**
         * Combine the layers into an image.
         */
        void paintLayers();

        /**
         * Clear the layers.
         */
        void resetLayers();

        /**
         * Set a raster layer.
         * @param layer - the index of the layer in the stack.
         * @param img - the raster graphic image.
         */
        void setRasterLayer( int layer, const QImage & img );

        /**
         * Set a vector graphics layer.
         * @param layer - the index of the layer in the stack.
         * @param vglist - the vector graphics list.
         */
        void setVectorGraphicsLayer( int layer, const Carta::Lib::VectorGraphics::VGList & vglist );

        /**
         * Returns the image produced.
         */
        QImage getImage() const;

        ~SaveViewLayered();

    private:

        std::unique_ptr<SaveView> m_vgView;

        struct RasterLayerInfo {
            QImage qimg;

            std::shared_ptr<Carta::Lib::IQImageCombiner> combiner = nullptr;
        };

        struct VGLayerInfo {
            Carta::Lib::VectorGraphics::VGList vglist;
        };

        std::vector < RasterLayerInfo > m_rasterLayers;
        std::vector < VGLayerInfo > m_vgLayers;

};
}
}
