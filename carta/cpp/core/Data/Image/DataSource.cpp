#include "DataSource.h"
#include "CoordinateSystems.h"
#include "Data/Colormap/Colormaps.h"
#include "Globals.h"
#include "PluginManager.h"
#include "GrayColormap.h"
#include "CartaLib/IImage.h"
#include "Data/Util.h"
#include "Data/Colormap/TransformsData.h"
#include "CartaLib/Hooks/LoadAstroImage.h"
#include "CartaLib/PixelPipeline/CustomizablePixelPipeline.h"
#include "../../ImageRenderService.h"
#include "../../Algorithms/quantileAlgorithms.h"
#include <QDebug>

namespace Carta {

namespace Data {

const QString DataSource::DATA_PATH = "file";
const QString DataSource::CLASS_NAME = "DataSource";
const double DataSource::ZOOM_DEFAULT = 1.0;

CoordinateSystems* DataSource::m_coords = nullptr;

DataSource::DataSource() :
    m_image( nullptr ){
        m_cmapUseCaching = true;
        m_cmapUseInterpolatedCaching = true;
        m_cmapCacheSize = 1000;

        _initializeSingletons();

        //Initialize the rendering service
        m_renderService.reset( new Carta::Core::ImageRenderService::Service() );

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

bool DataSource::_contains(const QString& fileName) const {
    bool representsData = false;
    if ( m_fileName.endsWith(fileName)) {
        representsData = true;
    }
    return representsData;
}

QString DataSource::_getCursorText( int mouseX, int mouseY, int frameIndex,
        Carta::Lib::KnownSkyCS cs ){
    QString str;
    QTextStream out( & str );
    QPointF lastMouse( mouseX, mouseY );
    bool valid = false;
    QPointF imgPt = _getImagePt( lastMouse, &valid );
    if ( valid ){
        double imgX = imgPt.x();
        double imgY = imgPt.y();
    
        CoordinateFormatterInterface::SharedPtr cf(
                m_image-> metaData()-> coordinateFormatter()-> clone() );


        QString coordName = m_coords->getName( cf->skyCS() );
        //out << "Default sky cs:" << coordName << "\n";

        QString pixelValue = _getPixelValue( round(imgX), round(imgY), frameIndex );
        QString pixelUnits = _getPixelUnits();
        out << pixelValue << " " << pixelUnits;
        out <<"Pixel:" << imgX << "," << imgY << "\n";

        cf-> setSkyCS( cs );
        out << m_coords->getName( cs ) << ": ";
        std::vector < Carta::Lib::AxisInfo > ais;
        for ( int axis = 0 ; axis < cf->nAxes() ; axis++ ) {
            const Carta::Lib::AxisInfo & ai = cf-> axisInfo( axis );
            ais.push_back( ai );
        }

        QStringList coordList = _getCoordinates( imgX, imgY, frameIndex, cs);
        for ( size_t i = 0 ; i < ais.size() ; i++ ) {
            out << ais[i].shortLabel().html() << ":" << coordList[i] << " ";
        }
        out << "\n";

        str.replace( "\n", "<br />" );
    }
    return str;
}

QPointF DataSource::_getCenter() const{
    return m_renderService->pan();
}


QPointF DataSource::_getImagePt( QPointF screenPt, bool* valid ) const {
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

QString DataSource::_getPixelValue( double x, double y, int frameIndex ) const {
    QString pixelValue = "";
    int valX = (int)(round(x));
    int valY = (int)(round(y));
    if ( valX >= 0 && valX < m_image->dims()[0] && valY >= 0 && valY < m_image->dims()[1] ) {
        NdArray::RawViewInterface* rawData = _getRawData( frameIndex, frameIndex );
        if ( rawData != nullptr ){
            NdArray::TypedView<double> view( rawData, false );
            double val =  view.get( { valX, valY } );
            pixelValue = QString::number( val );
        }
    }
    return pixelValue;
}


QPointF DataSource::_getScreenPt( QPointF imagePt, bool* valid ) const {
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

int DataSource::_getFrameCount() const {
    int frameCount = 1;
    if ( m_image ){
        std::vector<int> imageShape  = m_image->dims();
        if ( imageShape.size() > 2 ){
            frameCount = imageShape[2];
        }
    }
    return frameCount;
}



int DataSource::_getDimension( int coordIndex ) const {
    int dim = -1;
    if ( 0 <= coordIndex && coordIndex < _getDimensions()){
        dim = m_image-> dims()[coordIndex];
    }
    return dim;
}


int DataSource::_getDimensions() const {
    int imageSize = 0;
    if ( m_image ){
        imageSize = m_image->dims().size();
    }
    return imageSize;
}

QString DataSource::_getFileName() const {
    return m_fileName;
}

std::shared_ptr<Image::ImageInterface> DataSource::_getImage(){
    return m_image;
}



std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> DataSource::_getPipeline() const {
    return m_pixelPipeline;
}

std::shared_ptr<Carta::Core::ImageRenderService::Service> DataSource::_getRenderer() const {
    return m_renderService;
}

bool DataSource::_getIntensity( int frameLow, int frameHigh, double percentile, double* intensity ) const {
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

double DataSource::_getPercentile( int frameLow, int frameHigh, double intensity ) const {
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

QStringList DataSource::_getPixelCoordinates( double ra, double dec ) const{
    QStringList result("");
    CoordinateFormatterInterface::SharedPtr cf( m_image-> metaData()-> coordinateFormatter()-> clone() );
    const CoordinateFormatterInterface::VD world { ra, dec };
    CoordinateFormatterInterface::VD pixel;
    bool valid = cf->toPixel( world, pixel );
    if ( valid ){
        result = QStringList( QString::number( pixel[0] ) );
        result.append( QString::number( pixel[1] ) );
    }
    return result;
}

QString DataSource::_getPixelUnits() const {
    QString units = m_image->getPixelUnit().toStr();
    return units;
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



double DataSource::_getZoom() const {
    double zoom = ZOOM_DEFAULT;
    if ( m_renderService != nullptr ){
        zoom = m_renderService-> zoom();
    }
    return zoom;
}

QSize DataSource::_getOutputSize() const {
    QSize size;
    if ( m_renderService != nullptr ){
        size = m_renderService-> outputSize();
    }
    return size;
}


void DataSource::_initializeSingletons( ){
    //Load the available color maps.
    if ( m_coords == nullptr ){
        m_coords = Util::findSingletonObject<CoordinateSystems>();
    }
}

void DataSource::_load(int frameIndex, bool /*recomputeClipsOnNewFrame*/, double minClipPercentile, double maxClipPercentile){

    if ( frameIndex < 0 ) {
        frameIndex = 0;
    }
    if ( m_image-> dims().size() <= 2 ) {
        frameIndex = 0;
    }
    else {
        frameIndex = Carta::Lib::clamp( frameIndex, 0, m_image-> dims()[2] - 1 );
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
    QString argStr = QString( "%1//%2").arg( m_fileName ).arg(frameIndex);
    m_renderService-> setInputView( view, argStr);
}


void DataSource::_resetZoom(){
    m_renderService-> setZoom( ZOOM_DEFAULT );
}

void DataSource::_resetPan(){
    if ( m_image != nullptr ){
        m_renderService-> setPan(
                { m_image-> dims()[0] / 2.0, m_image-> dims()[1] / 2.0 }
        );
    }
}



bool DataSource::_setFileName( const QString& fileName ){
    QString file = fileName.trimmed();
    bool successfulLoad = true;
    if (file.length() > 0) {
        if ( file != m_fileName ){
            try {
                auto res = Globals::instance()-> pluginManager()
                                      -> prepare <Carta::Lib::Hooks::LoadAstroImage>( file )
                                      .first();
                if (!res.isNull()){
                    m_image = res.val();

                    // reset zoom/pan
                    _resetZoom();
                    _resetPan();

                    // clear quantile cache
                    m_quantileCache.resize(0);
                    int nf = 1;
                    if( m_image-> dims().size() > 2){
                        nf = m_image-> dims()[2];
                    }
                    m_quantileCache.resize( nf);
                    m_fileName = file;
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
    }
    else {
        qDebug() << "Could not load empty file.";
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

void DataSource::_setPan( double imgX, double imgY ){
    m_renderService-> setPan( QPointF(imgX,imgY) );
}

void DataSource::_setTransformData( const QString& name ){
    TransformsData* transformData = Util::findSingletonObject<TransformsData>();
    Carta::Lib::PixelPipeline::ScaleType scaleType = transformData->getScaleType( name );
    m_pixelPipeline->setScale( scaleType );
    m_renderService->setPixelPipeline( m_pixelPipeline, m_pixelPipeline->cacheId() );
}

void DataSource::_setZoom( double zoomAmount){
    // apply new zoom
    m_renderService-> setZoom( zoomAmount );
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

void DataSource::_viewResize( const QSize& newSize ){
    m_renderService-> setOutputSize( newSize );
}


QStringList DataSource::_getCoordinates( double x, double y, int frameIndex,
        Carta::Lib::KnownSkyCS system) const{
    CoordinateFormatterInterface::SharedPtr cf( m_image-> metaData()-> coordinateFormatter()-> clone() );
    cf-> setSkyCS( system );
    std::vector < double > pixel( m_image-> dims().size(), 0.0 );
    pixel[0] = x;
    pixel[1] = y;
    int pixelCount = pixel.size();
    if ( pixelCount > 2 ){
        pixel[2] = frameIndex;
    }
    QStringList list = cf-> formatFromPixelCoordinate( pixel );
    return list;
}



DataSource::~DataSource() {

}
}
}
