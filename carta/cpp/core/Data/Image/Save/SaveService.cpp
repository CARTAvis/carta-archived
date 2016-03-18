#include "Data/Image/Save/SaveService.h"
#include "Data/Image/Save/SaveViewLayered.h"
#include "Data/Image/Layer.h"

namespace Carta
{
namespace Data
{

SaveService::SaveService( QObject * parent ) :
        QObject( parent ),
        m_view( new SaveViewLayered()){
}

QString SaveService::setFileName( const QString& saveName ){
    QString result;
    QFile file( saveName );
    if ( !file.open( QIODevice::WriteOnly)){
        result = "The file "+saveName+" is not writable.";
    }
    else {
        m_fileName = saveName;
    }
    return result;
}

void SaveService::setLayers( QList< std::shared_ptr<Layer> > layers){
   m_layers = layers;
}


void SaveService::setSelectIndex( int index ){
    m_selectIndex = index;
}


void SaveService::setOutputSize( QSize size ){
    m_outputSize = size;
}


void SaveService::setAspectRatioMode( Qt::AspectRatioMode mode ){
    m_aspectRatioMode = mode;
}

bool SaveService::saveFullImage(){
    bool dataPrepared = true;
    _scheduleFrameRepaint();
    return dataPrepared;
}


void SaveService::_saveFullImageCB( QImage img ){
    QSize imgSize = img.size();
    qDebug() << "outputSize width="<<m_outputSize.width() << " height="<<m_outputSize.height();
    qDebug() << "img height="<<imgSize.height()<<" width="<<imgSize.width();
    qDebug() << "aspect="<<m_aspectRatioMode;
    QImage imgScaled = img.scaled( m_outputSize /** m_renderService->zoom()*/, m_aspectRatioMode );
    QSize scaledSize = imgScaled.size();
    qDebug() << "scaled height="<<scaledSize.height()<<" width="<<scaledSize.width();
    bool result = imgScaled.save( m_fileName );
    emit saveImageResult( result );
}

void SaveService::_scheduleFrameRepaint(){

    m_view->resetLayers();

    //We want the selected index to be the last one in the stack.
    int dataCount = m_layers.size();
    int stackIndex = 0;
    for ( int i = 0; i < dataCount; i++ ){
        int dIndex = ( m_selectIndex + i + 1) % dataCount;
        if ( m_layers[dIndex]->_isVisible() ){

            QImage image = m_layers[dIndex]->_getQImage();
            Carta::Lib::VectorGraphics::VGList graphicsList = m_layers[dIndex]->_getVectorGraphics();
            m_view->setRasterLayer( stackIndex, image );

            m_view->setVectorGraphicsLayer( stackIndex, graphicsList );
            stackIndex++;
        }
    }
    m_view->paintLayers();
    QImage image = m_view->getImage();
    _saveFullImageCB( image );

}


SaveService::~SaveService(){

}
}
}

