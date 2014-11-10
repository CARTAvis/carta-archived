#include "CartaLib/IImage.h"
#include "DataSource.h"
#include "Globals.h"
#include "PluginManager.h"
#include "IConnector.h"
#include "Algorithms/RawView2QImageConverter.h"

#include <QDebug>

DataSource::DataSource(const QString& fileName) :
    m_image( nullptr ){
    m_fileName = fileName;

    m_rawView2QImageConverter = std::make_shared<RawView2QImageConverter>();
    if (m_fileName.trimmed().length() > 0) {
        auto & globals = *Globals::instance();
        auto loadImageHookHelper = globals.pluginManager()->prepare <LoadAstroImage>( m_fileName );
        m_image = loadImageHookHelper.first().val();
    }
}

Nullable<QImage> DataSource::load(int frameIndex, bool forceClipRecompute){
    Nullable<QImage> qimg;
    if ( m_image ){
        auto frameSlice = SliceND().next();
        for( size_t i = 2 ; i < m_image->dims().size() ; i ++) {
            frameSlice.next().index( i == 2 ? frameIndex : 0);
        }
        NdArray::RawViewInterface * frameView = m_image->getDataSlice( frameSlice);
        resetClipValue();
        m_rawView2QImageConverter-> setView( frameView);
        bool clipRecompute = false;
        auto & globals = *Globals::instance();
        IConnector* connector = globals.connector();
        QString clipRecomputeStr = connector->getState(StateKey::AUTO_CLIP, "" );
        if ( clipRecomputeStr == "1"){
            clipRecompute = true;
        }
        qimg = m_rawView2QImageConverter-> go(frameIndex, clipRecompute || forceClipRecompute);
        delete frameView;
    }

    return qimg;
}

void DataSource::saveState(QString winId, int index ) {
    auto & globals = *Globals::instance();
    IConnector* connector = globals.connector();
    QString indexStr( winId + "-" + QString::number(index));
    QString savedFile = connector->getState( StateKey::DATA_PATH, indexStr);
    if ( savedFile != m_fileName ){
        connector->setState(StateKey::DATA_PATH, indexStr, m_fileName);
    }
}

bool DataSource::contains(const QString& fileName) const {
    bool representsData = false;
    if (m_fileName == fileName) {
        representsData = true;
    }
    return representsData;
}

void DataSource::resetClipValue(){
    auto & globals = *Globals::instance();
    IConnector* connector = globals.connector();
    QString val = connector->getState( StateKey::CLIP_VALUE, "");
    val.chop(1);
    bool ok;
    double d = val.toDouble( & ok);
    //Use the default if the stat is not set.
    if( ! ok) {
        m_rawView2QImageConverter-> setAutoClip( 0.95 /* 95% */);
    }
    else {
        d = clamp( d/100, 0.001, 1.0);
        m_rawView2QImageConverter-> setAutoClip( d);
    }
}

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
