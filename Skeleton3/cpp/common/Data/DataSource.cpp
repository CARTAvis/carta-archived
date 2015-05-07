#include "DataSource.h"
#include "Colormap/Colormaps.h"
#include "Globals.h"
#include "PluginManager.h"
#include "GrayColormap.h"
#include "CartaLib/IImage.h"
#include "Util.h"
#include "Colormap/TransformsData.h"
#include "CartaLib/Hooks/LoadAstroImage.h"
#include "CartaLib/PixelPipeline/CustomizablePixelPipeline.h"
#include "../ImageRenderService.h"
#include "../Algorithms/quantileAlgorithms.h"
#include <QDebug>
#include <QDir>
#include <QImageWriter>

namespace Carta {

namespace Data {

const QString DataSource::DATA_PATH = "dataPath";

DataSource::DataSource() :
    m_image( nullptr )
    {
        m_cmapUseCaching = true;
        m_cmapUseInterpolatedCaching = true;
        m_cmapCacheSize = 1000;

        m_renderService.reset( new Carta::Core::ImageRenderService::Service() );
        connect( m_renderService.get(), & Carta::Core::ImageRenderService::Service::done,
                 this, & DataSource::_renderingDone);


        // assign a default colormap to the view
        auto rawCmap = std::make_shared < Carta::Core::GrayColormap > ();

        // initialize pixel pipeline
        m_pixelPipeline = std::make_shared < Carta::Lib::PixelPipeline::CustomizablePixelPipeline > ();
        m_pixelPipeline-> setInvert( false );
        m_pixelPipeline-> setReverse( false );
        m_pixelPipeline-> setColormap( std::make_shared < Carta::Core::GrayColormap > () );
        m_pixelPipeline-> setMinMax( 0, 1 );
        m_renderService-> setPixelPipeline( m_pixelPipeline, m_pixelPipeline-> cacheId());
}

bool DataSource::contains(const QString& fileName) const {
    bool representsData = false;
    if (m_fileName.endsWith(fileName)) {
        representsData = true;
    }
    return representsData;
}

QString DataSource::getCursorText( int mouseX, int mouseY, int frameIndex){
    QString str;
    QTextStream out( & str );
    QPointF lastMouse( mouseX, mouseY );
    
    bool valid = false;
    QPointF imgPt = getImagePt( lastMouse, &valid );
    if ( valid ){
        int imgX = imgPt.x();
        int imgY = imgPt.y();
    
        CoordinateFormatterInterface::SharedPtr cf(
                m_image-> metaData()-> coordinateFormatter()-> clone() );
    
        std::vector < QString > knownSCS2str {
                "Unknown", "J2000", "B1950", "ICRS", "Galactic",
                "Ecliptic"
            };
        std::vector < KnownSkyCS > css {
                KnownSkyCS::J2000, KnownSkyCS::B1950, KnownSkyCS::Galactic,
                KnownSkyCS::Ecliptic, KnownSkyCS::ICRS
            };
         out << "Default sky cs:" << knownSCS2str[static_cast < int > ( cf-> skyCS() )] << "\n";
         out << "Image cursor:" << imgX << "," << imgY << "\n";
    
         for ( auto cs : css ) {
            cf-> setSkyCS( cs );
            out << knownSCS2str[static_cast < int > ( cf-> skyCS() )] << ": ";
            std::vector < Carta::Lib::AxisInfo > ais;
            for ( int axis = 0 ; axis < cf->nAxes() ; axis++ ) {
                const Carta::Lib::AxisInfo & ai = cf-> axisInfo( axis );
                ais.push_back( ai );
            }
            std::vector < double > pixel( m_image-> dims().size(), 0.0 );
            pixel[0] = imgX;
            pixel[1] = imgY;
            if( pixel.size() > 2) {
               pixel[2] = frameIndex;
            }
            auto list = cf-> formatFromPixelCoordinate( pixel );
            for ( size_t i = 0 ; i < ais.size() ; i++ ) {
                out << ais[i].shortLabel().html() << ":" << list[i] << " ";
            }
            out << "\n";
        }

        str.replace( "\n", "<br />" );
    }
    return str;
}

QPointF DataSource::getCenter() const{
    return m_renderService->pan();
}


QPointF DataSource::getImagePt( QPointF screenPt, bool* valid ) const {
    QPointF imagePt;
    if ( m_image != nullptr ){
        imagePt = m_renderService-> screen2img (screenPt);
        *valid = true;
    }
    else {
        *valid = false;
    }
    return imagePt;
}

QPointF DataSource::getScreenPt( QPointF imagePt, bool* valid ) const {
    QPointF screenPt;
    if ( m_image != nullptr ){
        screenPt = m_renderService->img2screen( imagePt );
        *valid = true;
    }
    else {
        *valid = false;
    }
    return screenPt;
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



int DataSource::getDimension( int coordIndex ) const {
    int dim = -1;
    if ( 0 <= coordIndex && coordIndex < getDimensions()){
        dim = m_image-> dims()[coordIndex];
    }
    return dim;
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

QString DataSource::getImageViewName() const {
    QString shortName = m_fileName;
    int sepIndex = m_fileName.lastIndexOf( QDir::separator() );
    if ( sepIndex >= 0 ){
        shortName = m_fileName.right( m_fileName.length() - sepIndex - 1 );
    }
    return shortName;
}

std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> DataSource::getPipeline() const {
    return m_pixelPipeline;

}

bool DataSource::getIntensity( int frameLow, int frameHigh, double percentile, double* intensity ) const {
    bool intensityFound = false;
    NdArray::RawViewInterface* rawData = _getRawData( frameLow, frameHigh );
    if ( rawData != nullptr ){
        NdArray::TypedView<double> view( rawData, false );
        // read in all values from the view into an array
        // we need our own copy because we'll do quickselect on it...
        std::vector < double > allValues;
        view.forEach(
                [& allValues] ( const double  val ) {
            if ( std::isfinite( val ) ) {
                allValues.push_back( val );
            }
        }
        );

        // indicate bad clip if no finite numbers were found
        if ( allValues.size() > 0 ) {
            int locationIndex = allValues.size() * percentile - 1;

            if ( locationIndex < 0 ){
                locationIndex = 0;
            }
            std::nth_element( allValues.begin(), allValues.begin()+locationIndex, allValues.end() );
            *intensity = allValues[locationIndex];
            intensityFound = true;
        }
    }
    return intensityFound;
}

double DataSource::getPercentile( int frameLow, int frameHigh, double intensity ) const {
    double percentile = 0;
    NdArray::RawViewInterface* rawData = _getRawData( frameLow, frameHigh );
    if ( rawData != nullptr ){
        u_int64_t totalCount = 0;
        u_int64_t countBelow = 0;
        NdArray::TypedView<double> view( rawData, false );
        view.forEach([&](const double& val) {
            if( Q_UNLIKELY( std::isnan(val))){
                return;
            }
            totalCount ++;
            if( val <= intensity){
                countBelow++;
            }
            return;
        });

        if ( totalCount > 0 ){
            percentile = double(countBelow) / totalCount;
        }
    }
    return percentile;
}

NdArray::RawViewInterface * DataSource::_getRawData( int channelStart, int channelEnd ) const {
    NdArray::RawViewInterface* rawData = nullptr;
    if ( m_image ){
        auto frameSlice = SliceND().next();
        for( size_t i=2; i < m_image->dims().size(); i++ ){
            if ( i == 2 ){
                SliceND& slice = frameSlice.next();
                if (channelStart>=0 && channelEnd >= 0 ){
                    slice.start( channelStart );
                    slice.end( channelEnd + 1);
                    
                 }
                 else {
                    slice.start( 0 );
                    slice.end( m_image->dims()[2] );
                 }
                 slice.step( 1 );
            }
            else {
                frameSlice.next().index(0);
            }
        }
        rawData = m_image->getDataSlice( frameSlice );
    }
    return rawData;
}

double DataSource::getZoom() const {
    return m_renderService-> zoom();
}

void DataSource::load(int frameIndex, bool /*recomputeClipsOnNewFrame*/, double minClipPercentile, double maxClipPercentile){

    if ( frameIndex < 0 ) {
        frameIndex = 0;
    }
    if ( m_image-> dims().size() <= 2 ) {
        frameIndex = 0;
    }
    else {
        frameIndex = clamp( frameIndex, 0, m_image-> dims()[2] - 1 );
    }

    // prepare slice description corresponding to the entire frame [:,:,frame,0,0,...0]
    auto frameSlice = SliceND().next();
    for ( size_t i = 2 ; i < m_image->dims().size() ; i++ ) {
        frameSlice.next().index( i == 2 ? frameIndex : 0 );
    }

    // get a view of the data using the slice description and make a shared pointer out of it
    NdArray::RawViewInterface::SharedPtr view( m_image-> getDataSlice( frameSlice ) );

    //Update the clip values
    _updateClips( view, frameIndex, minClipPercentile, maxClipPercentile );

    m_renderService-> setPixelPipeline( m_pixelPipeline, m_pixelPipeline-> cacheId());

    // tell the render service to render this job
    QString argStr = QString( "%1//%2").arg(m_fileName).arg(frameIndex);
    m_renderService-> setInputView( view, argStr);
    render();

}

void DataSource::render(){
    m_renderService-> render( 0 );
}

void DataSource::_renderingDone( QImage img, int64_t jobId ){
    Q_UNUSED( jobId );
    emit renderingDone( img );
}

bool DataSource::setFileName( const QString& fileName ){
    m_fileName = fileName.trimmed();
    bool successfulLoad = true;
    if (m_fileName.length() > 0) {
        try {
            auto res = Globals::instance()-> pluginManager()
                                  -> prepare <Carta::Lib::Hooks::LoadAstroImage>( m_fileName )
                                  .first();
            if (!res.isNull()){
                m_image = res.val();

                // reset zoom/pan
                m_renderService-> setZoom( 1.0 );
                m_renderService-> setPan( { m_image-> dims()[0] / 2.0, m_image-> dims()[1] / 2.0 }
                                          );

                // clear quantile cache
                m_quantileCache.resize(0);
                int nf = 1;
                if( m_image-> dims().size() > 2){
                    nf = m_image-> dims()[2];
                }
                m_quantileCache.resize( nf);
            }
            else {
                qWarning( "Could not find any plugin to load image");
                successfulLoad = false;
            }

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
    Carta::State::ObjectManager* objManager = Carta::State::ObjectManager::objectManager();
    Carta::State::CartaObject* obj = objManager->getObject( Colormaps::CLASS_NAME );
    Colormaps* maps = dynamic_cast<Colormaps*>(obj);
    m_pixelPipeline-> setColormap( maps->getColorMap( name ) );
    m_renderService ->setPixelPipeline( m_pixelPipeline, m_pixelPipeline->cacheId());
}

void DataSource::setColorInverted( bool inverted ){
    m_pixelPipeline-> setInvert( inverted );
    m_renderService-> setPixelPipeline( m_pixelPipeline, m_pixelPipeline-> cacheId());
}

void DataSource::setColorReversed( bool reversed ){
    m_pixelPipeline-> setReverse( reversed );
    m_renderService-> setPixelPipeline( m_pixelPipeline, m_pixelPipeline-> cacheId());
}

void DataSource::setColorAmounts( double newRed, double newGreen, double newBlue ){
    std::array<double,3> colorArray;
    colorArray[0] = newRed;
    colorArray[1] = newGreen;
    colorArray[2] = newBlue;
    m_pixelPipeline->setRgbMax( colorArray );
    m_renderService->setPixelPipeline( m_pixelPipeline, m_pixelPipeline->cacheId());
}

void DataSource::setPan( double imgX, double imgY ){
    m_renderService-> setPan( QPointF(imgX,imgY) );
}

void DataSource::setTransformData( const QString& name ){
    Carta::State::CartaObject* transformDataObj = Util::findSingletonObject( TransformsData::CLASS_NAME );
    TransformsData* transformData = dynamic_cast<TransformsData*>(transformDataObj);
    Carta::Lib::PixelPipeline::ScaleType scaleType = transformData->getScaleType( name );
    m_pixelPipeline->setScale( scaleType );
    m_renderService->setPixelPipeline( m_pixelPipeline, m_pixelPipeline->cacheId() );
}

void DataSource::setZoom( double zoomAmount){
    // apply new zoom
    m_renderService-> setZoom( zoomAmount );
}

void DataSource::setPixelCaching( bool cachePixels ){
    Carta::Core::ImageRenderService::PixelPipelineCacheSettings settings = m_renderService-> pixelPipelineCacheSettings();
    settings.enabled = cachePixels;
    m_renderService->setPixelPipelineCacheSettings( settings );
}

void DataSource::setCacheInterpolation( bool enabled ){
    Carta::Core::ImageRenderService::PixelPipelineCacheSettings settings = m_renderService-> pixelPipelineCacheSettings();
    settings.interpolated = enabled;
    m_renderService->setPixelPipelineCacheSettings( settings );
}

void DataSource::setCacheSize( int cacheSize ){
    Carta::Core::ImageRenderService::PixelPipelineCacheSettings settings = m_renderService-> pixelPipelineCacheSettings();
    settings.size = cacheSize;
    m_renderService->setPixelPipelineCacheSettings( settings );
}

void DataSource::setGamma( double gamma ){
    m_pixelPipeline->setGamma( gamma );
    m_renderService->setPixelPipeline( m_pixelPipeline, m_pixelPipeline->cacheId());
}

void DataSource::_updateClips( std::shared_ptr<NdArray::RawViewInterface>& view, int frameIndex,
        double minClipPercentile, double maxClipPercentile ){
    std::vector<double> clips = m_quantileCache[ frameIndex];
    NdArray::Double doubleView( view.get(), false );
    std::vector<double> newClips = Carta::Core::Algorithms::quantiles2pixels(
            doubleView, {minClipPercentile, maxClipPercentile });
    bool clipsChanged = false;
    if ( newClips.size() >= 2 ){
        if( clips.size() < 2 ){
            clipsChanged = true;
        }
        else {
            double ERROR_MARGIN = 0.000001;
            if ( qAbs( newClips[0] - clips[0]) > ERROR_MARGIN ||
                qAbs( newClips[1] - clips[1]) > ERROR_MARGIN ){
                clipsChanged = true;
            }
        }
    }
    if ( clipsChanged ){
        m_quantileCache[ frameIndex ] = newClips;
        m_pixelPipeline-> setMinMax( newClips[0], newClips[1] );
    }

}

void DataSource::viewResize( const QSize& newSize ){
    m_renderService-> setOutputSize( newSize );
}

DataSource::~DataSource() {

}
}
}
