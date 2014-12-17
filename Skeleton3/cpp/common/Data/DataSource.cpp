#include "DataSource.h"
#include "Colormaps.h"
#include "Globals.h"
#include "PluginManager.h"
#include "GrayColormap.h"
#include "CartaLib/IImage.h"
#include "CartaLib/Hooks/LoadAstroImage.h"
#include "Algorithms/RawView2QImageConverter.h"

#include <QDebug>



const QString DataSource::CLASS_NAME = "DataSource";
bool DataSource::m_registered =
    ObjectManager::objectManager()->registerClass ( CLASS_NAME,
                                                   new DataSource::Factory());

const QString DataSource::DATA_PATH = "dataPath";



DataSource::DataSource(const QString& path, const QString& id) :
    CartaObject( CLASS_NAME, path, id),
    m_image( nullptr )
    {
        m_rawView2QImageConverter = std::make_shared<RawView2QImageConverter>();
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

void DataSource::setColorMap( int index ){
    ObjectManager* objManager = ObjectManager::objectManager();
    CartaObject* obj = objManager->getObject( Colormaps::CLASS_NAME );
    Colormaps* maps = dynamic_cast<Colormaps*>(obj);
    m_rawView2QImageConverter-> setColormap( maps->getColorMap( index ) );
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

Nullable<QImage> DataSource::load(int frameIndex, bool forceClipRecompute, bool autoClip, float clipValue){
    Nullable<QImage> qimg;
    if ( m_image ){


        qDebug() << "Load frameIndex="<<frameIndex<<" clipRecompute="
                <<forceClipRecompute<<" autoClip="<<autoClip<<" clipValue="<<clipValue;
        auto frameSlice = SliceND().next();
        for( size_t i = 2 ; i < m_image->dims().size() ; i ++) {
            frameSlice.next().index( i == 2 ? frameIndex : 0);
        }
        NdArray::RawViewInterface * frameView = m_image->getDataSlice( frameSlice);
        //resetClipValue();
        m_rawView2QImageConverter-> setAutoClip( clipValue);
        m_rawView2QImageConverter-> setView( frameView);
        bool clipRecompute = autoClip || forceClipRecompute;
        qimg = m_rawView2QImageConverter-> go(frameIndex, clipRecompute);
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
