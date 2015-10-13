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

using Carta::Lib::AxisInfo;

namespace Carta {

namespace Data {

const QString DataSource::DATA_PATH = "file";
const QString DataSource::CLASS_NAME = "DataSource";
const double DataSource::ZOOM_DEFAULT = 1.0;

CoordinateSystems* DataSource::m_coords = nullptr;

DataSource::DataSource() :
    m_image( nullptr ),
    m_axisIndexX( 0 ),
    m_axisIndexY( 1 ){
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

std::vector<int> DataSource::_fitFramesToImage( const vector<int>& sourceFrames ) const {
    int sourceFrameCount = sourceFrames.size();
    std::vector<int> outputFrames( sourceFrameCount );
    if ( m_image ){
        for ( int i = 0; i < sourceFrameCount; i++ ){
            AxisInfo::KnownType axisType = static_cast<AxisInfo::KnownType>( i );
            int axisIndex = _getAxisIndex( axisType );
            //The image doesn't h_ave this particular axis.
            if ( axisIndex < 0 ) {
                outputFrames[i] = 0;
            }
            //The image has the axis so make the frame bounded by the image size.
            else {
                outputFrames[i] = Carta::Lib::clamp( sourceFrames[i], 0, m_image-> dims()[axisIndex] - 1 );
            }
        }
    }
    return outputFrames;
}


std::vector<AxisInfo::KnownType> DataSource::_getAxisTypes() const {
    std::vector<AxisInfo::KnownType> types;
    CoordinateFormatterInterface::SharedPtr cf(
                   m_image-> metaData()-> coordinateFormatter()-> clone() );
    int axisCount = cf->nAxes();
    for ( int axis = 0 ; axis < axisCount; axis++ ) {
        const AxisInfo & axisInfo = cf-> axisInfo( axis );
        types.push_back( axisInfo.knownType() );
    }
    return types;
}

int DataSource::_getAxisIndex( AxisInfo::KnownType axisType ) const {
    int index = -1;
    CoordinateFormatterInterface::SharedPtr cf(
                           m_image-> metaData()-> coordinateFormatter()-> clone() );
    int axisCount = cf->nAxes();
    for ( int i = 0; i < axisCount; i++ ){
        AxisInfo axisInfo = cf->axisInfo( i );
        if ( axisInfo.knownType() == axisType ){
            index = i;
            break;
        }
    }
    return index;
}

AxisInfo::KnownType DataSource::_getAxisType( int index ) const {
    AxisInfo::KnownType type = AxisInfo::KnownType::OTHER;
    CoordinateFormatterInterface::SharedPtr cf(
                       m_image-> metaData()-> coordinateFormatter()-> clone() );
    int axisCount = cf->nAxes();
    if ( index < axisCount && index >= 0 ){
        AxisInfo axisInfo = cf->axisInfo( index );
        type = axisInfo.knownType();
    }
    return type;
}

AxisInfo::KnownType DataSource::_getAxisXType() const {
    return _getAxisType( m_axisIndexX );
};

AxisInfo::KnownType DataSource::_getAxisYType() const {
    return _getAxisType( m_axisIndexY );
}

std::vector<AxisInfo::KnownType> DataSource::_getAxisZTypes() const {
    std::vector<AxisInfo::KnownType> zTypes;
    if ( m_image ){
        int imageDims = m_image->dims().size();
        for ( int i = 0; i < imageDims; i++ ){
            if ( i != m_axisIndexX && i!= m_axisIndexY ){
                AxisInfo::KnownType type = _getAxisType( i );
                zTypes.push_back( type );
            }
        }
    }
    return zTypes;
}

QStringList DataSource::_getCoordinates( double x, double y,
        Carta::Lib::KnownSkyCS system, const std::vector<int>& frames ) const{
    std::vector<int> mFrames = _fitFramesToImage( frames );
    CoordinateFormatterInterface::SharedPtr cf( m_image-> metaData()-> coordinateFormatter()-> clone() );
    cf-> setSkyCS( system );
    int imageSize = m_image->dims().size();
    std::vector < double > pixel( imageSize, 0.0 );
    for ( int i = 0; i < imageSize; i++ ){
        if ( i == m_axisIndexX ){
            pixel[i] = x;
        }
        else if ( i == m_axisIndexY ){
            pixel[i] = y;
        }
        else {
            AxisInfo::KnownType axisType = _getAxisType( i );
            int axisIndex = static_cast<int>( axisType );
            pixel[i] = mFrames[axisIndex];
        }
    }
    QStringList list = cf-> formatFromPixelCoordinate( pixel );
    return list;
}


QString DataSource::_getCursorText( int mouseX, int mouseY,
        Carta::Lib::KnownSkyCS cs, const std::vector<int>& frames ){
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

        QString pixelValue = _getPixelValue( round(imgX), round(imgY), frames );
        QString pixelUnits = _getPixelUnits();
        out << pixelValue << " " << pixelUnits;
        out <<"Pixel:" << imgX << "," << imgY << "\n";

        cf-> setSkyCS( cs );
        out << m_coords->getName( cs ) << ": ";
        std::vector <AxisInfo> ais;
        for ( int axis = 0 ; axis < cf->nAxes() ; axis++ ) {
            const AxisInfo & ai = cf-> axisInfo( axis );
            ais.push_back( ai );
        }

        QStringList coordList = _getCoordinates( imgX, imgY, cs, frames);
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

std::vector<int> DataSource::_getAxisPerms() const {
    std::vector<int> axisPerms;
    //Note that permutations are 1-based whereas the axis
    //index is zero-based.
    if ( m_image ){
        int imageSize = m_image->dims().size();
        axisPerms.push_back( m_axisIndexX + 1 );
        axisPerms.push_back( m_axisIndexY + 1 );
        for ( int i = 0; i < imageSize; i++ ){
            if ( i != m_axisIndexX && i != m_axisIndexY ){
                axisPerms.push_back( i + 1 );
            }
        }
    }
    return axisPerms;
}


QPointF DataSource::_getImagePt( QPointF screenPt, bool* valid ) const {
    QPointF imagePt;
    if ( m_image ){
        imagePt = m_renderService-> screen2img (screenPt);
        *valid = true;
    }
    else {
        *valid = false;
    }
    return imagePt;
}

QString DataSource::_getPixelValue( double x, double y, const std::vector<int>& frames ) const {
    QString pixelValue = "";
    int valX = (int)(round(x));
    int valY = (int)(round(y));
    if ( valX >= 0 && valX < m_image->dims()[m_axisIndexX] && valY >= 0 && valY < m_image->dims()[m_axisIndexY] ) {
        NdArray::RawViewInterface* rawData = _getRawData( frames );
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

int DataSource::_getFrameCount( AxisInfo::KnownType type ) const {
    int frameCount = 1;
    if ( m_image ){
        int axisIndex = _getAxisIndex( type );

        std::vector<int> imageShape  = m_image->dims();
        int imageDims = imageShape.size();
        if ( imageDims > axisIndex && axisIndex >= 0 ){
            frameCount = imageShape[axisIndex];
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

std::pair<int,int> DataSource::_getDisplayDims() const {
    std::pair<int,int> displayDims(0,0);
    if ( m_image ){
        displayDims.first = m_image->dims()[m_axisIndexX];
        displayDims.second = m_image->dims()[m_axisIndexY ];
    }
    return displayDims;
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
    int spectralIndex = _getAxisIndex( AxisInfo::KnownType::SPECTRAL );
    NdArray::RawViewInterface* rawData = _getRawData( frameLow, frameHigh, spectralIndex );
    if ( rawData != nullptr ){
        NdArray::TypedView<double> view( rawData, false );
        // read in all values from the view into an array
        // we need our own copy because we'll do quickselect on it...
        std::vector < double > allValues;
        view.forEach( [& allValues] ( const double  val ) {
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
    int spectralIndex = _getAxisIndex( AxisInfo::KnownType::SPECTRAL);
    NdArray::RawViewInterface* rawData = _getRawData( frameLow, frameHigh, spectralIndex );
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

NdArray::RawViewInterface* DataSource::_getRawData( int frameStart, int frameEnd, int axisIndex ) const {
    NdArray::RawViewInterface* rawData = nullptr;
    if ( m_image ){
        int imageDim =m_image->dims().size();
        SliceND frameSlice = SliceND().next();
        for ( int i = 0; i < imageDim; i++ ){
            if ( i != m_axisIndexX && i != m_axisIndexY ){
                int sliceSize = m_image->dims()[i];
                SliceND& slice = frameSlice.next();
                //If it is the target axis,
                if ( i == axisIndex ){
                   //Use the passed in frame range
                   if (0 <= frameStart && frameStart < sliceSize &&
                        0 <= frameEnd && frameEnd < sliceSize ){
                        slice.start( frameStart );
                        slice.end( frameEnd + 1);
                   }
                   else {
                       slice.start(0);
                       slice.end( sliceSize);
                   }
                }
                //Or the entire range
                else {
                   slice.start( 0 );
                   slice.end( sliceSize );
                }
                slice.step( 1 );
            }
        }
        rawData = m_image->getDataSlice( frameSlice );
    }
    return rawData;
}


int DataSource::_getQuantileCacheIndex( const std::vector<int>& frames) const {
    int cacheIndex = 0;
    int imageSize = m_image->dims().size();
    int mult = 1;
    for ( int i = imageSize-1; i >= 0; i-- ){
        if ( i != m_axisIndexX && i != m_axisIndexY ){
            AxisInfo::KnownType axisType = _getAxisType( i );
            int index = static_cast<int>( axisType );
            int frame = frames[index];
            cacheIndex = cacheIndex + mult * frame;
            mult = mult * m_image->dims()[i];
        }
    }
    return cacheIndex;
}

NdArray::RawViewInterface* DataSource::_getRawData( const std::vector<int> frames ) const {
    NdArray::RawViewInterface* rawData = nullptr;
    std::vector<int> mFrames = _fitFramesToImage( frames );
    if ( m_image ){
        int imageDim =m_image->dims().size();
        SliceND nextSlice = SliceND();
        SliceND& slice = nextSlice;
        for ( int i = 0; i < imageDim; i++ ){
            if ( i != m_axisIndexX && i != m_axisIndexY ){
                AxisInfo::KnownType type = _getAxisType( i );
                int axisIndex = static_cast<int>( type );
                int frameIndex = mFrames[axisIndex];
                slice.start( frameIndex );
                slice.end( frameIndex + 1);
            }
            if ( i < imageDim - 1 ){
                slice.next();
            }
        }
        rawData = m_image->getDataSlice( nextSlice );
    }
    return rawData;
}

QString DataSource::_getViewIdCurrent( const std::vector<int>& frames ) const {
   // We create an identifier consisting of the file name and -1 for the two display axes
   // and frame indices for the other axes.
   QString renderId = m_fileName;
   if ( m_image ){
       int imageSize = m_image->dims().size();
       for ( int i = 0; i < imageSize; i++ ){
           AxisInfo::KnownType axisType = _getAxisType( i );
           int axisFrame = frames[static_cast<int>(axisType)];
           QString prefix;
           //Hidden axis identified with an "f" and the index of the frame.
           if ( i != m_axisIndexX && i != m_axisIndexY ){
               prefix = "h";
           }
           //Display axis identified by a "d" plus the actual axis in the image.
           else {
               if ( i == m_axisIndexX ){
                   prefix = "dX";
               }
               else {
                   prefix = "dY";
               }
               axisFrame = i;
           }
           renderId = renderId + "//"+prefix + QString::number(axisFrame );
       }
   }
   return renderId;
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


void DataSource::_load(std::vector<int> frames, bool recomputeClipsOnNewFrame,
        double minClipPercentile, double maxClipPercentile){
    int frameSize = frames.size();
    CARTA_ASSERT( frameSize == static_cast<int>(AxisInfo::KnownType::OTHER));
    std::vector<int> mFrames = _fitFramesToImage( frames );
    std::shared_ptr<NdArray::RawViewInterface> view( _getRawData( mFrames ) );

    //Update the clip values
    if ( recomputeClipsOnNewFrame ){
        _updateClips( view,  minClipPercentile, maxClipPercentile, mFrames );
    }

    m_renderService-> setPixelPipeline( m_pixelPipeline, m_pixelPipeline-> cacheId());

    QString renderId = _getViewIdCurrent( mFrames );
    m_renderService-> setInputView( view, renderId, m_axisIndexX, m_axisIndexY );
}


void DataSource::_resetZoom(){
    m_renderService-> setZoom( ZOOM_DEFAULT );
}

void DataSource::_resetPan(){
    if ( m_image != nullptr ){
        double xCenter =  m_image-> dims()[m_axisIndexX] / 2.0;
        double yCenter = m_image-> dims()[m_axisIndexY] / 2.0;
        m_renderService-> setPan({ xCenter, yCenter });
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
                    _resizeQuantileCache();
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


void DataSource::_resizeQuantileCache(){
    m_quantileCache.resize(0);
    int nf = 1;
    int imageSize = m_image->dims().size();
    for ( int i = 0; i < imageSize; i++ ){
        if ( i != m_axisIndexX && i != m_axisIndexY ){
            nf = nf * m_image->dims()[i];
        }
    }
    m_quantileCache.resize( nf);
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

bool DataSource::_setDisplayAxis( AxisInfo::KnownType axisType, int* axisIndex ){
    bool displayAxisChanged = false;
    if ( m_image ){
        int newXAxisIndex = _getAxisIndex(axisType);
        int imageSize = m_image->dims().size();
        if ( newXAxisIndex >= 0 && newXAxisIndex < imageSize ){
            if ( newXAxisIndex != *axisIndex ){
                *axisIndex = newXAxisIndex;
                displayAxisChanged = true;
            }
        }
    }
    return displayAxisChanged;
}

void DataSource::_setDisplayAxes(std::vector<AxisInfo::KnownType> displayAxisTypes,
        const std::vector<int>& frames ){
    int displayAxisCount = displayAxisTypes.size();
    CARTA_ASSERT( displayAxisCount == 2 );
    bool axisXChanged = _setDisplayAxis( displayAxisTypes[0], &m_axisIndexX );
    bool axisYChanged = _setDisplayAxis( displayAxisTypes[1], &m_axisIndexY );
    if ( axisXChanged || axisYChanged ){
        _resetPan();
        _resizeQuantileCache();
        std::vector<int> mFrames = _fitFramesToImage( frames );
        _updateRenderedView( mFrames );
    }
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


void DataSource::_updateClips( std::shared_ptr<NdArray::RawViewInterface>& view,
        double minClipPercentile, double maxClipPercentile, const std::vector<int>& frames ){
    std::vector<int> mFrames = _fitFramesToImage( frames );
    int quantileIndex = this->_getQuantileCacheIndex( mFrames );
    std::vector<double> clips = m_quantileCache[ quantileIndex];
    NdArray::Double doubleView( view.get(), false );
    std::vector<double> newClips = Carta::Core::Algorithms::quantiles2pixels(
            doubleView, {minClipPercentile, maxClipPercentile });
    bool clipsChanged = false;
    int clipSize = newClips.size();
    if ( clipSize >= 2 ){
        if ( clips.size() >= 2 ){
            double ERROR_MARGIN = 0.000001;
            if ( qAbs( newClips[0] - clips[0]) > ERROR_MARGIN ||
                qAbs( newClips[1] - clips[1]) > ERROR_MARGIN ){
                clipsChanged = true;
            }
        }
        else {
            clipsChanged = true;
        }
    }

    if ( clipsChanged ){
        if ( newClips.size() >= 2 && newClips[0] != newClips[1] ){
            m_quantileCache[ quantileIndex ] = newClips;
            m_pixelPipeline-> setMinMax( newClips[0], newClips[1] );
        }
    }
}

std::shared_ptr<NdArray::RawViewInterface> DataSource::_updateRenderedView( const std::vector<int>& frames ){
    // get a view of the data using the slice description and make a shared pointer out of it
    std::shared_ptr<NdArray::RawViewInterface> view( _getRawData( frames ) );
    // tell the render service to render this job
    QString renderId = _getViewIdCurrent( frames );
    m_renderService-> setInputView( view, renderId, m_axisIndexX, m_axisIndexY );
    return view;
}

void DataSource::_viewResize( const QSize& newSize ){
    m_renderService-> setOutputSize( newSize );
}


DataSource::~DataSource() {

}
}
}
