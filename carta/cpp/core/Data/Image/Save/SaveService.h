/**
 * The SaveService is responsible for saving images to a file.
 * It uses ImageRenderService internally
 **/

#pragma once

#include <QObject>
#include <QSize>
#include <QImage>
#include <QList>
#include <memory>
#include "CartaLib/CartaLib.h"

namespace Carta{
namespace Data{

class SaveViewLayered;
class Layer;
class RenderRequest;
class RenderResponse;

class SaveService : public QObject {
    Q_OBJECT

public:

    /**
     * Constructor.
     */
    explicit SaveService( QObject * parent = 0 );


    /**
     * Set how the image should be scaled to fit the output size.
     * @param mode - method of scaling the image to fit the output size.
     */
    void setAspectRatioMode( Qt::AspectRatioMode mode );

    /**
     * Set the potential data that are layers in the stack if they are visible.
     * @param layers- a list of potential stack layers.
     */
    void setLayers( QList< std::shared_ptr<Layer> > layers);

    /**
     * Set the name of the file where the image should be saved.
     * @param fileName - the absolute path to the file where the image should be
     *      saved.
     */
   void setFileName( const QString& fileName );


    /**
     * Save the image.
     * @return true if the image was saved; false, otherwise.
     */
    bool saveImage(/*const std::vector<int>& frames,
            const Carta::Lib::KnownSkyCS& cs*/const std::shared_ptr<RenderRequest>&);

    /**
     * Destructor.
     */
    ~SaveService();

signals:

    /**
     * Notification that the save has completed.
     * @param result - the success of the save.
     */
    void saveImageResult( bool result );

private slots:

    void _scheduleSave(/*const QImage& qImage,
            const Carta::Lib::VectorGraphics::VGList& vg, const QString& layerName*/
            const std::shared_ptr<RenderResponse>& response);

private:

    QSize _getSaveSize( const std::shared_ptr<RenderRequest>& request ) const;

    /**
     * Returns true if the file is writable and the path is valid.
     * @return - true if the save designation is valid; false otherwise.
     */
    bool _isFileValid() const;

    /**
     * Saves the image to disk.
     * @param img - the image to save.
     */
    void _saveImage( QImage img );

    /// Full path of the output image
    QString m_fileName;

    //Layers in the stack.
    QList<std::shared_ptr<Layer> > m_layers;

    QMap<QString,std::shared_ptr<RenderResponse> > m_images;

    //Top layer of the stack
    int m_selectIndex;

    /// The size of the output image
    QSize m_outputSize;

    /// Determines how the output image will be scaled if an output size is set.
    Qt::AspectRatioMode m_aspectRatioMode;

    // Layered stack view.
    std::unique_ptr<SaveViewLayered> m_view;

    //Total number of images that need to be drawn.
    int m_renderCount;
    //Count of images that have been redrawn to date.
    int m_redrawCount;

    SaveService( const SaveService& other);
    SaveService& operator=( const SaveService& other );
};
}
}



