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

namespace Carta{
namespace Data{

class SaveViewLayered;
class Layer;

class SaveService : public QObject {
    Q_OBJECT

public:

    /**
     * Constructor.
     */
    explicit SaveService( QObject * parent = 0 );

    /**
     * Set the desired output size of the image.
     * @param size - the width and height of the output image.
     */
    void setOutputSize( QSize size );

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
     * @return - an error message if the file does not exist or is not writable.
     */
    QString setFileName( const QString& fileName );

    /**
     * Set the top index in the stack.
     * @param index - the top stack index.
     */
    void setSelectIndex( int index );

    /**
     * Save the image.
     * @return true if the image was saved; false, otherwise.
     */
    bool saveFullImage();

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

    /**
     * Saves the image to disk.
     * @param img - the image to save.
     */
    void _saveFullImageCB( QImage img );

private:

    void _scheduleFrameRepaint();

    /// Full path of the output image
    QString m_fileName;

    //Layers in the stack.
    QList<std::shared_ptr<Layer> > m_layers;

    //Top layer of the stack
    int m_selectIndex;

    /// The size of the output image
    QSize m_outputSize;

    /// Determines how the output image will be scaled if an output size is set.
    Qt::AspectRatioMode m_aspectRatioMode;

    // Layered stack view.
    std::unique_ptr<SaveViewLayered> m_view;

};
}
}



