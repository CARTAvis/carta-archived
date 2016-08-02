#include "Data/Image/Save/SaveService.h"
#include "Data/Image/Save/SaveViewLayered.h"
#include "Data/Image/Layer.h"
#include "Data/Image/Render/RenderResponse.h"
#include "Data/Image/Render/RenderRequest.h"


namespace Carta
{
namespace Data
{

SaveService::SaveService( QObject * parent ) :
        QObject( parent ),
        m_view( new SaveViewLayered()){
}

bool SaveService::_isFileValid() const {
    bool fileValid = true;
    QFile file( m_fileName );
    if ( !file.open( QIODevice::WriteOnly)){
        fileValid = false;
    }
    return fileValid;
}

QSize SaveService::_getSaveSize( const std::shared_ptr<RenderRequest>& request ) const {
    QSize saveSize = request->getOutputSize();
    int topIndex = request->getTopIndex();
    if ( 0 <= topIndex && topIndex < m_layers.size() ){
        saveSize = m_layers[topIndex]->_getSaveSize( saveSize, m_aspectRatioMode);
    }
    return saveSize;
}

bool SaveService::saveImage( const std::shared_ptr<RenderRequest>& request){
    m_images.clear();
    int dataCount = m_layers.size();
    bool fileValid = _isFileValid();
    m_selectIndex = request->getTopIndex();
    m_outputSize = request->getOutputSize();
    if ( fileValid ){
        m_renderCount = 0;
        m_redrawCount = dataCount;
        int stackIndex = 0;
        for ( int i = 0; i < dataCount; i++ ){
            bool layerVisible = m_layers[i]->_isVisible();
            if ( layerVisible ){
                connect( m_layers[i].get(),
                        SIGNAL(renderingDone( const std::shared_ptr<RenderResponse>&)),
                        this,
                        SLOT( _scheduleSave( const std::shared_ptr<RenderResponse>&)),
                        Qt::UniqueConnection);
                bool topOfStack = false;
                if ( i == m_selectIndex ){
                    topOfStack = true;
                }
                std::shared_ptr<RenderRequest> layerRequest( new RenderRequest(*request));
                layerRequest->setStackTop( topOfStack );
                m_layers[i]->_render( layerRequest );
                stackIndex++;
            }
        }
    }
    return fileValid;
}

void SaveService::_saveImage( QImage img ){
    QImage imgScaled = img;
    //May need to scale if the image size isn't exactly right and we are ignoring
    //aspect ratio.
    if ( m_aspectRatioMode == Qt::IgnoreAspectRatio ){
        if ( img.width() != m_outputSize.width() || img.height() != m_outputSize.height() ){
            imgScaled = img.scaled( m_outputSize, m_aspectRatioMode );
        }
    }
    bool result = imgScaled.save( m_fileName );
    emit saveImageResult( result );
}

void SaveService::_scheduleSave( const std::shared_ptr<RenderResponse>& response ){
    m_renderCount++;
    m_images[response->getLayerName()] = response;
    if ( m_renderCount != m_redrawCount ) {
        return;
    }

    m_view->resetLayers();

    //We want the selected index to be the last one in the stack.
    int dataCount = m_layers.size();
    int stackIndex = 0;
    for ( int i = 0; i < dataCount; i++ ){
        int dIndex = ( m_selectIndex + i + 1) % dataCount;
        m_layers[dIndex]->disconnect( this );
        if ( m_layers[dIndex]->_isVisible() ){
            QString layerId = m_layers[dIndex]->_getLayerId();
            if ( m_images.contains( layerId ) ){
                QImage image = m_images[layerId]->getImage();
                Carta::Lib::VectorGraphics::VGList graphicsList = m_images[layerId]->getVectorGraphics();
                m_view->setRasterLayer( stackIndex, image );
                m_view->setVectorGraphicsLayer( stackIndex, graphicsList );
                stackIndex++;
            }
        }
    }
    for ( int i = 0; i < dataCount; i++ ){
           m_layers[i]->_renderDone();
       }
    m_view->paintLayers();
    QImage image = m_view->getImage();
    _saveImage( image );
}

void SaveService::setFileName( const QString& saveName ){
   m_fileName = saveName;
}

void SaveService::setLayers( QList< std::shared_ptr<Layer> > layers){
   m_layers = layers;
}


void SaveService::setAspectRatioMode( Qt::AspectRatioMode mode ){
    m_aspectRatioMode = mode;
}


SaveService::~SaveService(){
}
}
}

