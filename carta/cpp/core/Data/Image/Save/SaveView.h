/**
 * Combines a graphics layer with a raster layer to produce a QImage.
 **/

#pragma once

#include "CartaLib/IRemoteVGView.h"
#include "CartaLib/VectorGraphics/VGList.h"

#include <QSize>
#include <QImage>

namespace Carta{

namespace Data{


class SaveView {

public:

    /**
     * Constructor.
     */
    SaveView();

    /**
     * Returns the combined image.
     * @return - the produced image.
     */
    const QImage & getBuffer();

    /**
     * Set the raster layer.
     * @param image - the image with raster graphics.
     */
    void setRaster( const QImage & image );

    /**
     * Set the vector graphics layer.
     * @param vglist - the list of vector graphics.
     */
    void setVectorGraphics( const Carta::Lib::VectorGraphics::VGList & vglist );

    /**
     * Produce a new image.
     */
    qint64 scheduleRepaint( qint64 id = - 1 );

    virtual ~SaveView();

private:

    QImage m_raster;
    QImage m_buffer;

    Carta::Lib::VectorGraphics::VGList m_vgList;
    qint64 m_lastRepaintId = - 1;

    SaveView( const SaveView& other);
    SaveView& operator=( const SaveView& other );
};
}
}
