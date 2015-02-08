#include "DataSource.h"
#include "Colormaps.h"
#include "Globals.h"
#include "PluginManager.h"
#include "GrayColormap.h"
#include "CartaLib/IImage.h"
#include "CartaLib/Hooks/LoadAstroImage.h"
#include "Algorithms/RawView2QImageConverter.h"

#include <QDebug>

namespace Carta {

namespace Data {

const QString DataSource::CLASS_NAME = "DataSource";

class DataSource::Factory : public CartaObjectFactory {

    public:

        CartaObject * create (const QString & path, const QString & id)
        {
            return new DataSource (path, id);
        }
};

bool DataSource::m_registered =
    ObjectManager::objectManager()->registerClass ( CLASS_NAME,
                                                   new DataSource::Factory());

const QString DataSource::DATA_PATH = "dataPath";



DataSource::DataSource(const QString& path, const QString& id) :
    CartaObject( CLASS_NAME, path, id),
    m_image( nullptr )
    {
        m_cmapUseCaching = true;
        m_cmapUseInterpolatedCaching = true;
        m_cmapCacheSize = 1000;
        m_rawView2QImageConverter.reset( new Carta::Core::RawView2QImageConverter3 );

        // assign a default colormap to the view
        auto rawCmap = std::make_shared < Carta::Core::GrayColormap > ();

        m_rawView2QImageConverter-> setColormap( rawCmap );
        _initializeState();
}

bool DataSource::setFileName( const QString& fileName ){
    m_fileName = fileName.trimmed();
    bool successfulLoad = true;
    if (m_fileName.length() > 0) {
        try {
            m_image = Globals::instance()-> pluginManager()
                      -> prepare <Carta::Lib::Hooks::LoadAstroImage>( m_fileName )
                      .first().val();
            m_coordinateFormatter = m_image-> metaData()-> coordinateFormatter();
            CoordinateFormatterInterface::VD pixel;
            pixel.resize( m_coordinateFormatter->nAxes(), 0 );
            auto fmt = m_coordinateFormatter-> formatFromPixelCoordinate( pixel );
            auto skycs = KnownSkyCS::Galactic;
            m_coordinateFormatter-> setSkyCS( skycs );
            qDebug() << "set skycs to" << int (skycs)
                     << "now it is" << int ( m_coordinateFormatter-> skyCS() );
            fmt = m_coordinateFormatter-> formatFromPixelCoordinate( pixel );
            qDebug() << "0->" << fmt.join( "|" );
        }
        catch( std::logic_error& err ){
            qDebug() << "Failed to load image "<<fileName;
            successfulLoad = false;
        }
    }
    else {
        qDebug() << "Cannot load empty file";
        successfulLoad = false;
    }
    return successfulLoad;
}

void DataSource::setColorMap( const QString& name ){
    ObjectManager* objManager = ObjectManager::objectManager();
    CartaObject* obj = objManager->getObject( Colormaps::CLASS_NAME );
    Colormaps* maps = dynamic_cast<Colormaps*>(obj);
    m_rawView2QImageConverter-> setColormap( maps->getColorMap( name ) );
}

void DataSource::setColorInverted( bool /*inverted*/ ){
    //m_rawView2QImageConverter->setInvert( inverted );
}

void DataSource::setColorReversed( bool /*reversed*/ ){
    //m_rawView2QImageConverter->setReverse( reversed );
}


QStringList DataSource::formatCoordinates( int mouseX, int mouseY, int frameIndex){
    int imageDims = getDimensions();
    auto pixCoords = std::vector<double>( imageDims, 0.0);
    pixCoords[0] = mouseX;
    pixCoords[1] = mouseY;
    if( pixCoords.size() > 2) {

        pixCoords[2] = frameIndex;
    }
    QStringList list = m_coordinateFormatter->formatFromPixelCoordinate( pixCoords);
    return list;
}

NdArray::RawViewInterface * DataSource::getRawData( int channel ) const {
    NdArray::RawViewInterface* rawData = nullptr;
    if ( m_image ){
        auto frameSlice = SliceND().next();
        for( size_t i = 2 ; i < m_image->dims().size() ; i ++) {
            frameSlice.next().index( i == 2 ? channel : 0);
        }

        rawData = m_image->getDataSlice( frameSlice);
    }
    return rawData;
}

QImage DataSource::load(int frameIndex, bool /*forceClipRecompute*/, bool /*autoClip*/, float clipValue){
    QImage qimg;
    NdArray::RawViewInterface * frameView = getRawData( frameIndex );
    if ( frameView != nullptr ){
        m_rawView2QImageConverter-> setView( frameView );
        m_rawView2QImageConverter-> computeClips( clipValue );
        m_rawView2QImageConverter-> setPixelPipelineCacheSize( m_cmapCacheSize);
        m_rawView2QImageConverter-> setPixelPipelineInterpolation( m_cmapUseInterpolatedCaching);
        m_rawView2QImageConverter-> setPixelPipelineCacheEnabled( m_cmapUseCaching);
        m_rawView2QImageConverter-> convert( qimg );
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

QString DataSource::getFileName() const {
    return m_fileName;
}

std::shared_ptr<Image::ImageInterface> DataSource::getImage(){
    return m_image;
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
}
}
