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
    m_fullImage = false;
}

bool SaveService::_isFileValid() const {
    bool fileValid = true;
    QFile file( m_fileName );
    if ( !file.open( QIODevice::WriteOnly)){
        fileValid = false;
    }
    return fileValid;
}

bool SaveService::saveImage(const std::vector<int>& frames,
        const Carta::Lib::KnownSkyCS& cs){
    int dataCount = m_layers.size();
    bool fileValid = _isFileValid();
    if ( fileValid ){
        m_renderCount = 0;
        m_redrawCount = dataCount;
        int stackIndex = 0;
        for ( int i = 0; i < dataCount; i++ ){
            if ( m_layers[i]->_isVisible() ){
                connect( m_layers[i].get(), SIGNAL(renderingDone()),
                        this, SLOT(_scheduleSave()), Qt::UniqueConnection);
                bool topOfStack = false;
                if ( i == m_selectIndex ){
                    topOfStack = true;
                }

                if ( m_fullImage ){
                    m_layers[i]->_viewResizeFullSave( m_outputSize);
                }
                m_layers[i]->_render( frames, cs, topOfStack );
                stackIndex++;
            }
        }
    }
    return fileValid;
}

void SaveService::_saveImage( QImage img ){
    QSize imgSize = img.size();
    qDebug() << "img height="<<imgSize.height()<<" width="<<imgSize.width();
    qDebug() << "aspect="<<m_aspectRatioMode;
    QImage imgScaled = img.scaled( m_outputSize, m_aspectRatioMode );
    QSize scaledSize = imgScaled.size();
    qDebug() << "scaled height="<<scaledSize.height()<<" width="<<scaledSize.width();
    bool result = imgScaled.save( m_fileName );
    emit saveImageResult( result );
}

void SaveService::_scheduleSave(){
    m_renderCount++;
    if ( /*m_repaintFrameQueued ||*/ (m_renderCount != m_redrawCount ) ) {
        return;
    }

    m_view->resetLayers();

    //We want the selected index to be the last one in the stack.
    int dataCount = m_layers.size();
    int stackIndex = 0;
    for ( int i = 0; i < dataCount; i++ ){
        int dIndex = ( m_selectIndex + i + 1) % dataCount;
        m_layers[dIndex]->disconnect( this );
        m_layers[dIndex]->_viewReset();
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
    _saveImage( image );
}

void SaveService::setFileName( const QString& saveName ){
   m_fileName = saveName;
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


void SaveService::setFullImage( bool fullImage ){
    m_fullImage = fullImage;
}


SaveService::~SaveService(){
}
}
}

