/***
 * Draws the layers in a group using a combine mode.
 */

#pragma once

#include "CartaLib/CartaLib.h"

#include <QString>
#include <QList>
#include <QObject>
#include <QImage>

#include <memory>

namespace Carta {
namespace Data {

class Layer;


class DrawGroupSynchronizer: public QObject {


    Q_OBJECT

public:

    /**
     *  Constructor.
     */
    DrawGroupSynchronizer();

    /**
     * Render a group of images using the given input parameters.
     * @param frames - list of frame indices for each of the axes.
     * @param cs - the coordinate system.
     * @param gridIndex - the index of the image providing the grid.
     */
    void render( std::vector<int> frames, const Carta::Lib::KnownSkyCS& cs,
               int gridIndex );

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
    void done( QImage img );


private slots:

    /**
    * Notification that a stack layer has changed finished rendering its image.
    */
    void _scheduleFrameRepaint();

private:

    //Layers to combine
    QList< std::shared_ptr<Layer> > m_layers;
    bool m_repaintFrameQueued;
    QImage m_qImage;
    QString m_combineMode;
    QSize m_imageSize;
    int m_renderCount;
    int m_redrawCount;

    DrawGroupSynchronizer(const DrawGroupSynchronizer& other);
    DrawGroupSynchronizer& operator=(const DrawGroupSynchronizer& other);

};

}
}
