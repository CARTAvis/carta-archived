#include "DataSource.h"
#include "Globals.h"
#include "PluginManager.h"
#include "CoordinateFormatter.h"
#include "IImage.h"
#include "Algorithms/RawView2QImageConverter.h"

#include <QDebug>


const QString DataSource::CLASS_NAME = "edu.nrao.carta.DataSource";
bool DataSource::m_registered =
    ObjectManager::objectManager()->registerClass ( CLASS_NAME,
                                                   new DataSource::Factory());

const QString DataSource::DATA_PATH = "dataPath";



DataSource::DataSource(const QString& path, const QString& id) :
    CartaObject( CLASS_NAME, path, id),
    m_state( path),
    m_image( nullptr )
    {
        m_rawView2QImageConverter = std::make_shared<RawView2QImageConverter>();
        _initializeState();
}

void DataSource::setFileName( const QString& fileName ){
    m_fileName = fileName.trimmed();
    if (m_fileName.length() > 0) {
        auto & globals = *Globals::instance();
        auto loadImageHookHelper = globals.pluginManager()->prepare <LoadAstroImage>( m_fileName );
        m_image = loadImageHookHelper.first().val();
    }
}


Nullable<QImage> DataSource::load(int frameIndex, bool forceClipRecompute, bool autoClip, float clipValue){
    Nullable<QImage> qimg;
    if ( m_image ){
        auto frameSlice = SliceND().next();
        for( size_t i = 2 ; i < m_image->dims().size() ; i ++) {
            frameSlice.next().index( i == 2 ? frameIndex : 0);
        }
        NdArray::RawViewInterface * frameView = m_image->getDataSlice( frameSlice);
        //resetClipValue();
        m_rawView2QImageConverter-> setAutoClip( clipValue);
        m_rawView2QImageConverter-> setView( frameView);
        //bool clipRecompute = m_autoClip;
        qimg = m_rawView2QImageConverter-> go(frameIndex, /*clipRecompute*/autoClip || forceClipRecompute);
        delete frameView;
    }

    return qimg;
}

void DataSource::saveState( ) {
    QString oldSavedFile = m_state.getValue<QString>( DATA_PATH );
    if ( m_fileName != oldSavedFile ){
        m_state.setValue<QString>( DATA_PATH, m_fileName );
    }
}

int DataSource::getDimensions() const {
    int imageSize = 0;
    if ( m_image ){
        imageSize = m_image->dims().size();
    }
    return imageSize;
}

void DataSource::_initializeState(){
    m_state.insertValue<QString>( DATA_PATH, "");
}

bool DataSource::contains(const QString& fileName) const {
    bool representsData = false;
    if (m_fileName == fileName) {
        representsData = true;
    }
    return representsData;
}

/*void DataSource::resetClipValue(){
    auto & globals = *Globals::instance();
    IConnector* connector = globals.connector();
    QString val = connector->getState( StateKey::CLIP_VALUE, "");
    val.chop(1);
    bool ok;
    double d = val.toDouble( & ok);
    //Use the default if the stat is not set.
    if( ! ok) {
        m_rawView2QImageConverter-> setAutoClip( 0.95 );
    }
    else {
        d = clamp( d/100, 0.001, 1.0);
        m_rawView2QImageConverter-> setAutoClip( d);
    }
}*/

int DataSource::getFrameCount() const {
    int frameCount = 1;
    if ( m_image ){
        std::vector<int> imageShape  = m_image->dims();
        if ( imageShape.size() > 2 ){
            frameCount = imageShape[2];
        }
    }
    return frameCount;
}

DataSource::~DataSource() {

}
