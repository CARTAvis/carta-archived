#include "DataSource.h"
#include "CoordinateSystems.h"
#include "Data/Colormap/Colormaps.h"
#include "Globals.h"
#include "MainConfig.h"
#include "PluginManager.h"
#include "GrayColormap.h"
#include "CartaLib/IImage.h"
#include "Data/Util.h"
#include "Data/Colormap/TransformsData.h"
#include "CartaLib/Hooks/LoadAstroImage.h"
#include "CartaLib/Hooks/GetPersistentCache.h"
#include "CartaLib/Hooks/ConversionSpectralHook.h"
#include "CartaLib/Hooks/PercentileToPixelHook.h"
#include "CartaLib/PixelPipeline/CustomizablePixelPipeline.h"
#include "CartaLib/IPCache.h"
#include "../../ImageRenderService.h"
#include "../../Algorithms/percentileAlgorithms.h"
#include "../Clips.h"
#include <QDebug>
#include <QElapsedTimer>
#include "CartaLib/UtilCASA.h"

using Carta::Lib::AxisInfo;
using Carta::Lib::AxisDisplayInfo;

namespace Carta {

namespace Data {

const QString DataSource::DATA_PATH = "file";
const QString DataSource::CLASS_NAME = "DataSource";
const double DataSource::ZOOM_DEFAULT = 1.0;
const int DataSource::INDEX_LOCATION = 0;
const int DataSource::INDEX_INTENSITY = 1;
const int DataSource::INDEX_PERCENTILE = 2;
const int DataSource::INDEX_FRAME_LOW = 3;
const int DataSource::INDEX_FRAME_HIGH = 4;

CoordinateSystems* DataSource::m_coords = nullptr;

DataSource::DataSource() :
    m_image( nullptr ),
    m_permuteImage( nullptr),
    m_coordinateFormatter( nullptr ),
    m_axisIndexX( 0 ),
    m_axisIndexY( 1 ){
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

        // initialize disk cache
        auto res = Globals::instance()-> pluginManager()
                   -> prepare < Carta::Lib::Hooks::GetPersistentCache > ().first();
        if ( res.isNull() || ! res.val() ) {
            qWarning( "Could not find a disk cache plugin." );
            m_diskCache = nullptr;
            m_diskCacheHelper = nullptr;
        }
        else {
            m_diskCache = res.val();
            m_diskCacheHelper = std::make_shared<Carta::Lib::IntensityCacheHelper>(m_diskCache);
        }
}

std::vector<int> DataSource::_getPermOrder() const
{
    //Build a vector showing the permute order.
    int imageDim = m_image->dims().size();
    std::vector<int> indices( imageDim );
    indices[0] = m_axisIndexX;
    indices[1] = m_axisIndexY;
    int vectorIndex = 2;
    for ( int i = 0; i < imageDim; i++ ){
        if ( i != m_axisIndexX && i != m_axisIndexY ){
            indices[vectorIndex] = i;
            vectorIndex++;
        }
    }

    return indices;
}


int DataSource::_getFrameIndex( int sourceFrameIndex, const std::vector<int>& sourceFrames ) const {
    int frameIndex = 0;
    if (m_image ){
        AxisInfo::KnownType axisType = static_cast<AxisInfo::KnownType>( sourceFrameIndex );
        int axisIndex = Util::getAxisIndex( m_image, axisType );
        //The image doesn't have this particular axis.
        if ( axisIndex >=  0 ) {
            //The image has the axis so make the frame bounded by the image size.
            frameIndex = Carta::Lib::clamp( sourceFrames[sourceFrameIndex], 0, m_image-> dims()[axisIndex] - 1 );
        }
    }
    return frameIndex;
}

std::vector<int> DataSource::_fitFramesToImage( const std::vector<int>& sourceFrames ) const {
    int sourceFrameCount = sourceFrames.size();
    std::vector<int> outputFrames( sourceFrameCount );
    for ( int i = 0; i < sourceFrameCount; i++ ){
        outputFrames[i] = _getFrameIndex( i, sourceFrames );
    }
    return outputFrames;
}


std::vector<AxisInfo::KnownType> DataSource::_getAxisTypes() const {
    std::vector<AxisInfo::KnownType> types;
    casa_mutex.lock();
    CoordinateFormatterInterface::SharedPtr cf(
                   m_image-> metaData()-> coordinateFormatter()-> clone() );
    int axisCount = cf->nAxes();
    for ( int axis = 0 ; axis < axisCount; axis++ ) {
        const AxisInfo & axisInfo = cf-> axisInfo( axis );
        AxisInfo::KnownType axisType = axisInfo.knownType();
        if ( axisType != AxisInfo::KnownType::OTHER ){
            types.push_back( axisInfo.knownType() );
        }
    }
    casa_mutex.unlock();

    return types;
}

void DataSource::_setCoordinateSystem( Carta::Lib::KnownSkyCS cs ){
    casa_mutex.lock();
    m_coordinateFormatter->setSkyCS( cs );
    casa_mutex.unlock();
}

std::vector<AxisInfo> DataSource::_getAxisInfos() const {
    std::vector<AxisInfo> Infos;
    casa_mutex.lock();

    int axisCount = m_coordinateFormatter->nAxes();
    for ( int axis = 0 ; axis < axisCount; axis++ ) {
        const AxisInfo & axisInfo = m_coordinateFormatter-> axisInfo( axis );
        if ( axisInfo.knownType() != AxisInfo::KnownType::OTHER ){
            Infos.push_back( axisInfo );
        }
    }

    casa_mutex.unlock();

    return Infos;
}


AxisInfo::KnownType DataSource::_getAxisType( int index ) const {
    AxisInfo::KnownType type = AxisInfo::KnownType::OTHER;
    casa_mutex.lock();
    CoordinateFormatterInterface::SharedPtr cf(
                       m_image-> metaData()-> coordinateFormatter()-> clone() );
    int axisCount = cf->nAxes();
    if ( index < axisCount && index >= 0 ){
        AxisInfo axisInfo = cf->axisInfo( index );
        type = axisInfo.knownType();
    }
    casa_mutex.unlock();

    return type;
}

AxisInfo::KnownType DataSource::_getAxisXType() const {
    return _getAxisType( m_axisIndexX );
}

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
                if ( type != AxisInfo::KnownType::OTHER ){
                    zTypes.push_back( type );
                }
            }
        }
    }
    return zTypes;
}

QStringList DataSource::_getCoordinates( double x, double y,
        Carta::Lib::KnownSkyCS system, const std::vector<int>& frames ) const{
    std::vector<int> mFrames = _fitFramesToImage( frames );
    casa_mutex.lock();
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
    casa_mutex.unlock();
    return list;
}

QString DataSource::_getDefaultCoordinateSystem() const{

    casa_mutex.lock();

    CoordinateFormatterInterface::SharedPtr cf(
            m_image-> metaData()-> coordinateFormatter()-> clone() );

    QString coordName = m_coords->getName( cf->skyCS() );
    casa_mutex.unlock();

    return coordName;
}

// print the pixel value and x-y coordinate for the cursor on the image viewer
QString DataSource::_getCursorText(bool isAutoClip, double minPercent, double maxPercent, int mouseX, int mouseY,
        Carta::Lib::KnownSkyCS cs, const std::vector<int>& frames,
        double zoom, const QPointF& pan, const QSize& outputSize ){
    QString str;
    QTextStream out( & str );
    QPointF lastMouse( mouseX, mouseY );
    bool valid = false;
    QPointF imgPt = _getImagePt( lastMouse, zoom, pan, outputSize, &valid );
    if ( valid ){
        double imgX = imgPt.x();
        double imgY = imgPt.y();

        // set print out values with rounded imgX and imgY
        QString round_imgX = QString::number(imgX, 'f', 2);
        QString round_imgY = QString::number(imgY, 'f', 2);

        casa_mutex.lock();

        CoordinateFormatterInterface::SharedPtr cf(
                m_image-> metaData()-> coordinateFormatter()-> clone() );

        QString coordName = m_coords->getName( cf->skyCS() );
        //out << "Default sky cs:" << coordName << "\n";

        QString pixelValue = _getPixelValue( round(imgX), round(imgY), frames );
        QString pixelUnits = _getPixelUnits();

        out << "Pixel value = " << pixelValue << " " << pixelUnits << " at ";
        out << "(X, Y) = " << "("<< round_imgX << ", " << round_imgY << ")" << "\n";

        // get the Min. and Max. values of intensity for Quantile mode
        if (isAutoClip == true) {
            std::vector<int> mFrames = _fitFramesToImage( frames );
            std::shared_ptr<Carta::Lib::NdArray::RawViewInterface> view ( _getRawData( mFrames ) );
            std::vector<double> intensity = _getQuantileIntensityCache(view, minPercent, maxPercent, frames, false);

            // set print out values with rounded intensities
            QString sci_intensityMin = QString::number(intensity[0], 'E', 3);
            QString sci_intensityMax = QString::number(intensity[1], 'E', 3);

            double percent = (maxPercent - minPercent)*100;
            out << "<span style=\"color: #000000;\">bounds for "
                << percent << "% clipping per frame: "
                << "[" << sci_intensityMin
                << ", " << sci_intensityMax << "] "
                << "</span>"
                << "\n";
        }

        cf-> setSkyCS( cs );
        out << "[ " << m_coords->getName( cs ) << " ] ";
        std::vector <AxisInfo> ais;
        for ( int axis = 0 ; axis < cf->nAxes() ; axis++ ) {
            const AxisInfo & ai = cf-> axisInfo( axis );
            ais.push_back( ai );
        }

        casa_mutex.unlock();

        QStringList coordList = _getCoordinates( imgX, imgY, cs, frames);
        for ( size_t i = 0 ; i < ais.size() ; i++ ) {
            if(ais[i].knownType() == Carta::Lib::AxisInfo::KnownType::SPECTRAL){
                out << coordList[i] << " ";
            }
            else{
                out << ais[i].shortLabel().html() << ":" << coordList[i] << " ";
            }
        }
        out << "\n";

        QString fileName = m_fileName.split("/").last();
        out << fileName;

        str.replace( "\n", "<br />" );
    }
    return str;
}

QPointF DataSource::_getCenter() const{
    QPointF center( nan(""), nan(""));
    if ( m_permuteImage != nullptr ){
         double xCenter =  m_permuteImage-> dims()[0] / 2.0;
         double yCenter = m_permuteImage-> dims()[1] / 2.0;
         // This is due to casa uses [0,0] as the center of the first pixel,
         // so there is 0.5 (image pixel coordinate, not screen pixel coordinate)
         // shift for the center of the whole image
         xCenter -= 0.5;
         yCenter -= 0.5;
         center.setX( xCenter );
         center.setY( yCenter );
     }
    return center;
}

std::vector<AxisDisplayInfo> DataSource::_getAxisDisplayInfo() const {
    std::vector<AxisDisplayInfo> axisInfo;
    //Note that permutations are 1-based whereas the axis
    //index is zero-based.
    if ( m_image ){
        int imageSize = m_image->dims().size();
        axisInfo.resize( imageSize );

        //Indicate the display axes by  putting -1 in for the display frames.
        //We will later fill in fixed frames for the other axes.
        axisInfo[m_axisIndexX].setFrame( -1 );
        axisInfo[m_axisIndexY].setFrame( -1 );

        //Indicate the new axis order.
        axisInfo[m_axisIndexX].setPermuteIndex( 0 );
        axisInfo[m_axisIndexY].setPermuteIndex( 1 );
        int availableIndex = 2;
        for ( int i = 0; i < imageSize; i++ ){
            axisInfo[i].setFrameCount( m_image->dims()[i] );
            axisInfo[i].setAxisType( _getAxisType( i ) );
            if ( i != m_axisIndexX && i != m_axisIndexY ){
                axisInfo[i].setPermuteIndex( availableIndex );
                availableIndex++;
            }
        }
    }
    return axisInfo;
}

QPointF DataSource::_getImagePt( const QPointF& screenPt, double zoom, const QPointF& pan,
            const QSize& outputSize, bool* valid ) const {
    QPointF imagePt;
    if ( m_image ){
        imagePt = m_renderService-> screen2image (screenPt, pan, zoom, outputSize);
        *valid = true;
    }
    else {
        *valid = false;
    }
    return imagePt;
}

QString DataSource::_getPixelValue( double x, double y, const std::vector<int>& frames ) const {
    QString pixelValue( "" );
    int valX = (int)(round(x));
    int valY = (int)(round(y));
    if ( valX >= 0 && valX < m_image->dims()[m_axisIndexX] && valY >= 0 && valY < m_image->dims()[m_axisIndexY] ) {
        Carta::Lib::NdArray::RawViewInterface* rawData = _getRawData( frames );
        if ( rawData != nullptr ){
            Carta::Lib::NdArray::TypedView<double> view( rawData, true );
            double val =  view.get( { valX, valY } );

            // set the rounded pixel value to print out
            pixelValue = QString::number(val, 'E', 3);
            //pixelValue = QString::number( val );
        }
    }
    return pixelValue;
}

int DataSource::_getFrameCount( AxisInfo::KnownType type ) const {
    int frameCount = 1;
    if ( m_image ){
        int axisIndex = Util::getAxisIndex( m_image, type );

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

std::shared_ptr<Carta::Lib::Image::ImageInterface> DataSource::_getImage(){
    return m_image;
}

std::shared_ptr<Carta::Lib::Image::ImageInterface> DataSource::_getPermImage(){
    return m_permuteImage;
}

std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> DataSource::_getPipeline() const {
    return m_pixelPipeline;
}

std::shared_ptr<Carta::Core::ImageRenderService::Service> DataSource::_getRenderer() const {
    return m_renderService;
}

std::shared_ptr<Carta::Lib::IntensityValue> DataSource::_readIntensityCache(int frameLow, int frameHigh, double percentile, int stokeFrame, QString transformationLabel) const {
    if (m_diskCacheHelper) {
        return m_diskCacheHelper->get(m_fileName, frameLow, frameHigh, percentile, stokeFrame, transformationLabel);
    }
    return nullptr;
}

void DataSource::_setIntensityCache(double intensity, double error, int frameLow, int frameHigh, double percentile, int stokeFrame, QString transformationLabel) const {
    if (m_diskCacheHelper) {
        m_diskCacheHelper->set(m_fileName, intensity, error, frameLow, frameHigh, percentile, stokeFrame, transformationLabel);
    }
}

std::vector<double> DataSource::_getIntensity(int frameLow, int frameHigh,
        const std::vector<double>& percentiles, int stokeFrame,
        Carta::Lib::IntensityUnitConverter::SharedPtr converter) {

    // Pick a calculator

    Carta::Lib::IPercentilesToPixels<double>::SharedPtr calculator = nullptr;

    if (percentiles.size() == 2 && percentiles[0] == 0 && percentiles[1] == 1) {
        // Special case: always use the min/max algorithm for min and max
        calculator = std::make_shared<Carta::Core::Algorithms::MinMaxPercentiles<double> >();
    } else {
        // Look for the best approximate plugin
        auto result = Globals::instance()-> pluginManager()-> prepare <Carta::Lib::Hooks::PercentileToPixelHook<double> >(m_image);

        auto lam = [&calculator] ( const Carta::Lib::Hooks::PercentileToPixelHook<double>::ResultType &data ) {
            if (!calculator || data->error < calculator->error) {
                calculator = data;
            }
        };

        result.forEach( lam );

        if (!calculator) {
            // No approximate plugin found; use exact algorithm
            calculator = std::make_shared<Carta::Core::Algorithms::PercentilesToPixels<double> >();
        }
    }

    qDebug() << "++++++++ Chosen percentile calculator:" << calculator->label;

    std::vector<double> intensities(percentiles.size(), 0);
    std::vector<bool> found(percentiles.size(), false);
    size_t foundCount = 0;

    QString transformationLabel = converter ? converter->label : "NONE";


    // If the disk cache exists, try to look up cached intensity values

    for (size_t i = 0; i < percentiles.size(); i++) {
        std::shared_ptr<Carta::Lib::IntensityValue> cachedValue = _readIntensityCache(frameLow, frameHigh, percentiles[i], stokeFrame, transformationLabel);

        if (cachedValue /* this intensity cache exists */ &&
            cachedValue->error <= calculator->error /* already has an intensity error order smaller than the current choice */ ) {
            intensities[i] = cachedValue->value;

            qDebug() << "++++++++ Found percentile" << percentiles[i] << "in cache. Intensity:" << intensities[i] << "+/- (max-min)*" << cachedValue->error;

            // We only cache statistics for each distinct frame-dependent calculation
            // But we need to apply any constant multipliers
            if (converter) {
                intensities[i] = intensities[i] * converter->multiplier;
            }

            found[i] = true;
            foundCount++;
        }
    }

    // Not all percentiles were in the cache.  We are going to have to look some up.
    if (foundCount < percentiles.size()) {
        qDebug() << "++++++++ Calculating intensities for percentiles";

        int spectralIndex = Util::getAxisIndex( m_image, AxisInfo::KnownType::SPECTRAL );
        int stokeIndex = Util::getAxisIndex( m_image, AxisInfo::KnownType::STOKES );

        Carta::Lib::NdArray::RawViewInterface* rawData = _getRawDataForStoke(frameLow, frameHigh, spectralIndex, stokeIndex, stokeFrame);

        qDebug() << "++++++++ Fetched raw image data for:" << "frameLow:" << frameLow << "frameHigh:" << frameHigh << "Spectral index:" << spectralIndex << "Stoke index:" << stokeIndex << "Stoke frame:" << stokeFrame;

        if (rawData == nullptr) {
            qCritical() << "Error: could not retrieve image data to calculate missing intensities.";
            return intensities;
        }

        // Create the view
        std::shared_ptr<Carta::Lib::NdArray::RawViewInterface> view(rawData);
        Carta::Lib::NdArray::Double doubleView(view.get(), false);

        // Make a list of the percentiles which have to be calculated
        std::vector<double> percentilesToCalculate;

        // Add the original missing percentiles
        for (size_t i = 0; i < percentiles.size(); i++) {
            if (!found[i]) {
                percentilesToCalculate.push_back(percentiles[i]);
            }
        }

        // if the algorithm is approximate, add extra percentiles from clips, but only if they are not cached

        if (calculator->isApproximate) {
            std::shared_ptr<Carta::Data::Clips> m_clips;
            std::vector<double> percentilesFromClips = m_clips->getAllClips2percentiles();

            for (auto& p : percentilesFromClips) {
                // TODO check exactly why this is necessary
                // check if extra percentiles are close to any existing percentiles, cached or uncached
                // C.C.Chiang: For approximation method, we can calculate all clipping values which are
                // listed on the UI panel at the same time. This "for loop" is used to fill in all clipping
                // values (as a set) we want to calculate.
                bool isDuplicate = false;
                for (size_t i = 0; i < percentiles.size(); i++) {
                    if (fabs(percentiles[i] - p) < 1e-6) {
                        isDuplicate = true;
                        // Either this was found in the cache already
                        // Or it's already in the list of percentiles to be calculated
                        // Either way, ignore it
                        break;
                    }
                }
                if (!isDuplicate) {
                    // This is a different percentile
                    // Look in the cache first
                    std::shared_ptr<Carta::Lib::IntensityValue> cachedValue = _readIntensityCache(frameLow, frameHigh, p, stokeFrame, transformationLabel);
                    // Add it to the list if it's not in the cache
                    if (!cachedValue) {
                        percentilesToCalculate.push_back(p);
                    }
                }
            }

            std::sort(percentilesToCalculate.begin(), percentilesToCalculate.end());
        }

        // Find Hz values if they are required for the unit transformation
        std::vector<double> hertzValues;

        if (converter && converter->frameDependent) {
            hertzValues = _getHertzValues(doubleView.dims(), spectralIndex);
        }

        // Calculate only the required percentiles
        std::map<double, double> clips_map;

        // Some algorithms need the min and max; we handle this explicitly for now
        if (calculator->needsMinMax) {
            // If an approximate algorithm requires min and max, they will always be calculated exactly
            // Because any approximate values in the cache will not satisfy the error requirement inside this call
            std::vector<double> minMaxIntensities = _getIntensity(frameLow, frameHigh, std::vector<double>({0, 1}), stokeFrame, converter);
            calculator->setMinMax(minMaxIntensities);
        }

        // perform the calculation on all of the percentiles

        clips_map = calculator->percentile2pixels(doubleView, percentilesToCalculate, spectralIndex, converter, hertzValues);

        // add all the calculated values to the cache

        for (auto &m : clips_map) {
            // TODO: check what happens with the close values. Do we also need to cache the value with a different key, or does the serialisation unify them?
            // put calculated values in the disk cache if it exists
            _setIntensityCache(m.second, calculator->error, frameLow, frameHigh, m.first, stokeFrame, transformationLabel);

            qDebug() << "++++++++ [set cache] for percentile" << m.first << ", intensity=" << m.second << "+/- (max-min)*" << calculator->error;
        }

        // set return values (only the intensities which were requested)

        for (size_t i = 0; i < percentiles.size(); i++) {
            if (!found[i]) {
                intensities[i] = clips_map[percentiles[i]];
                found[i] = true; // for completeness, in case we test this later

                // apply any constant multiplier *after* caching the frame-dependent portion of the calculation
                if (converter) {
                    intensities[i] = intensities[i] * converter->multiplier;
                }
            }
        }
    }

    return intensities;
}

QColor DataSource::_getNanColor() const {
    QColor nanColor = m_renderService->getNanColor();
    return nanColor;
}

std::vector<double> DataSource::_getHertzValues(const std::vector<int> dims, const int spectralIndex) const {
    std::vector<double> hertzValues;

    if (spectralIndex >= 0) { // multiple frames
        std::vector<double> Xvalues;

        for (int i = 0; i < dims[spectralIndex]; i++) {
            Xvalues.push_back((double)i);
        }

        // convert frame indices to Hz
        auto result = Globals::instance()-> pluginManager()-> prepare <Carta::Lib::Hooks::ConversionSpectralHook>(m_image, "", "Hz", Xvalues );
        auto lam = [&hertzValues] ( const Carta::Lib::Hooks::ConversionSpectralHook::ResultType &data ) {
            hertzValues = data;
        };

        result.forEach( lam );
    } else {
        qWarning() << "Could not calculate Hertz values. This image has no spectral axis.";
    }

    return hertzValues;
}

std::vector<double> DataSource::_getPercentiles( int frameLow, int frameHigh, std::vector<double> intensities, Carta::Lib::IntensityUnitConverter::SharedPtr converter ) const {
    std::vector<double> percentiles(intensities.size());
    int spectralIndex = Util::getAxisIndex( m_image, AxisInfo::KnownType::SPECTRAL);
    // TODO: do we need to modify this to take the stokes frame into account, like the percentile -> intensity calculation
    Carta::Lib::NdArray::RawViewInterface* rawData = _getRawData( frameLow, frameHigh, spectralIndex );
    if ( rawData != nullptr ){
        Carta::Lib::NdArray::Double view( rawData, false );

        std::vector<double> hertzValues;

        if (converter && converter->frameDependent) {
            hertzValues = _getHertzValues(view.dims(), spectralIndex);
        }

        Carta::Lib::IPixelsToPercentiles<double>::SharedPtr calculator = std::make_shared<Carta::Core::Algorithms::PixelsToPercentiles<double> >();

        percentiles = calculator->pixels2percentiles(view, intensities, spectralIndex, converter, hertzValues);
    }
    return percentiles;
}

QPointF DataSource::_getPixelCoordinates( double ra, double dec, bool* valid ) const{
    QPointF result;
    casa_mutex.lock();
    CoordinateFormatterInterface::SharedPtr cf( m_image-> metaData()-> coordinateFormatter()-> clone() );
    const CoordinateFormatterInterface::VD world { ra, dec };
    CoordinateFormatterInterface::VD pixel;
    *valid = cf->toPixel( world, pixel );
    if ( *valid ){
        result = QPointF( pixel[0], pixel[1]);
    }
    casa_mutex.unlock();

    return result;
}

std::pair<double,QString> DataSource::_getRestFrequency() const {
	std::pair<double,QString> restFreq( -1, "");
	if ( m_image ){
		restFreq = m_image->metaData()->getRestFrequency();
	}
	return restFreq;
}

QPointF DataSource::_getScreenPt( const QPointF& imagePt, const QPointF& pan,
        double zoom, const QSize& outputSize, bool* valid ) const {
    QPointF screenPt;
    if ( m_image != nullptr ){
        screenPt = m_renderService->image2screen( imagePt, pan, zoom, outputSize);
        *valid = true;
    }
    else {
        *valid = false;
    }
    return screenPt;
}

QPointF DataSource::_getWorldCoordinates( double pixelX, double pixelY,
        Carta::Lib::KnownSkyCS coordSys, bool* valid ) const{
    QPointF result;
    casa_mutex.lock();

    CoordinateFormatterInterface::SharedPtr cf( m_image-> metaData()-> coordinateFormatter()-> clone() );
    cf->setSkyCS( coordSys );
    int imageDims = _getDimensions();
    std::vector<double> pixel( imageDims );
    pixel[0] = pixelX;
    pixel[1] = pixelY;
    CoordinateFormatterInterface::VD world;
    *valid = cf->toWorld( pixel, world );
    if ( *valid ){
        result = QPointF( world[0], world[1]);
    }
    casa_mutex.unlock();

    return result;
}

QString DataSource::_getPixelUnits() const {
    QString units = m_image->getPixelUnit().toStr();
    return units;
}

Carta::Lib::NdArray::RawViewInterface* DataSource::_getRawData( int frameStart, int frameEnd, int axisIndex ) const {
    Carta::Lib::NdArray::RawViewInterface* rawData = nullptr;
    if ( m_image ){
        // get the image dimension:
        // if the image dimension=3, then dim[0]: x-axis, dim[1]: y-axis, and dim[2]: channel-axis
        // if the image dimension=4, then dim[0]: x-axis, dim[1]: y-axis, dim[2]: stoke-axis,   and dim[3]: channel-axis
        //                                                            or  dim[2]: channel-axis, and dim[3]: stoke-axis
        int imageDim =m_image->dims().size();

        SliceND frameSlice = SliceND().next();

        for ( int i = 0; i < imageDim; i++ ){

            // only deal with the extra dimensions other than x-axis and y-axis
            if ( i != m_axisIndexX && i != m_axisIndexY ){

                // declare and set the variable "sliceSize" as the total number of channel or stoke
                int sliceSize = m_image->dims()[i];
                qDebug() << "++++++++ For the image dimension: dim[" << i << "], the total number of slices is " << sliceSize;

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

Carta::Lib::NdArray::RawViewInterface* DataSource::_getRawDataForStoke( int frameStart, int frameEnd, int axisIndex,
        int axisStokeIndex, int stokeSliceIndex ) const {

    Carta::Lib::NdArray::RawViewInterface* rawData = nullptr;

    if ( m_image ){
        // get the image dimension:
        // if the image dimension=3, then dim[0]: x-axis, dim[1]: y-axis, and dim[2]: channel-axis
        // if the image dimension=4, then dim[0]: x-axis, dim[1]: y-axis, dim[2]: stoke-axis, and dim[3]: channel-axis
        //                                                            or  dim[2]: channel-axis, and dim[3]: stoke-axis
        int imageDim =m_image->dims().size();
        qDebug() << "++++++++ the dimension of image raw data for percentile calculation=" << imageDim;

        SliceND frameSlice = SliceND().next();

        for ( int i = 0; i < imageDim; i++ ){

            // only deal with the extra dimensions other than x-axis and y-axis
            if ( i != m_axisIndexX && i != m_axisIndexY ){

                // get the number of slice (e.q. channel) in this dimension
                int sliceSize = m_image->dims()[i];
                SliceND& slice = frameSlice.next();

                // If it is the target axis..
                if ( i == axisIndex ){
                   // Use the passed in frame range
                   if ( 0 <= frameStart && frameStart < sliceSize &&
                        0 <= frameEnd && frameEnd < sliceSize ){
                       slice.start( frameStart );
                       slice.end( frameEnd + 1);
                   } else {
                       qDebug() << "++++++++ for spectral axis index=" << i << ", the total slice size is" << sliceSize;
                       slice.start(0);
                       slice.end( sliceSize);
                   }
                } else if ( i == axisStokeIndex && stokeSliceIndex >= 0 && stokeSliceIndex <= 3){
                    // If the stoke-axis is exist (axisStokeIndex != -1),
                    // we only consider one stoke (stokeSliceIndex) for percentile calculation
                    qDebug() << "++++++++ we only consider the stoke" << stokeSliceIndex <<
                                "(-1: no stoke, 0: stoke I, 1: stoke Q, 2: stoke U, 3: stoke V) for percentile calculation" ;
                    slice.start( stokeSliceIndex );
                    slice.end( stokeSliceIndex + 1 );
                } else {

                    // Or the entire range
                    qDebug() << "++++++++ the total number of channel is" << sliceSize;
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

std::vector<int> DataSource::_getStokeIndex( const std::vector<int>& frames ) const {
    std::vector<int> stokeIndex = {-1, -1};
    if ( m_permuteImage ) {
        int imageDim =m_permuteImage->dims().size();
        std::vector<int> indices = _getPermOrder();
        for ( int i = 0; i < imageDim; i++ ) {
            if ( i != 0 && i != 1 ) {
                int frameIndex = 0;
                int axisIndex = -1;
                int thisAxis = indices[i];
                AxisInfo::KnownType type = _getAxisType( thisAxis );
                if ( type == AxisInfo::KnownType::STOKES ) {
                    axisIndex = static_cast<int>( type );
                    frameIndex = frames[axisIndex];
                    stokeIndex[0] = axisIndex;
                    stokeIndex[1] = frameIndex;
                }
            }
        }
    }
    return stokeIndex;
}

std::vector<int> DataSource::_getChannelIndex( const std::vector<int>& frames ) const {
    std::vector<int> channelIndex = {-1, -1};
    if ( m_permuteImage ) {
        int imageDim =m_permuteImage->dims().size();
        std::vector<int> indices = _getPermOrder();
        for ( int i = 0; i < imageDim; i++ ) {
            if ( i != 0 && i != 1 ) {
                int frameIndex = 0;
                int axisIndex = -1;
                int thisAxis = indices[i];
                AxisInfo::KnownType type = _getAxisType( thisAxis );
                if ( type == AxisInfo::KnownType::SPECTRAL ) {
                    axisIndex = static_cast<int>( type );
                    frameIndex = frames[axisIndex];
                    channelIndex[0] = axisIndex;
                    channelIndex[1] = frameIndex;
                }
            }
        }
    }
    return channelIndex;
}

std::shared_ptr<Carta::Lib::Image::ImageInterface> DataSource::_getPermutedImage() const {
    std::shared_ptr<Carta::Lib::Image::ImageInterface> permuteImage(nullptr);
    if ( m_image ){
        //Build a vector showing the permute order.
        std::vector<int> indices = _getPermOrder();
        permuteImage = m_image->getPermuted( indices );
    }
    return permuteImage;
}

Carta::Lib::NdArray::RawViewInterface* DataSource::_getRawData( const std::vector<int> frames ) const {

    Carta::Lib::NdArray::RawViewInterface* rawData = nullptr;
    std::vector<int> mFrames = _fitFramesToImage( frames );

    if ( m_permuteImage ){
        int imageDim =m_permuteImage->dims().size();

        //Build a vector showing the permute order.
        std::vector<int> indices = _getPermOrder();

        SliceND nextSlice = SliceND();
        SliceND& slice = nextSlice;

        for ( int i = 0; i < imageDim; i++ ){

            //Since the image has been permuted the first two indices represent the display axes.
            if ( i != 0 && i != 1 ){

                //Take a slice at the indicated frame.
                int frameIndex = 0;
                int thisAxis = indices[i];
                AxisInfo::KnownType type = _getAxisType( thisAxis );

                // check the type of axis and its indix of slices
                int axisIndex = -1;
                if ( type == AxisInfo::KnownType::SPECTRAL ) {
                    axisIndex = static_cast<int>( type );
                    frameIndex = mFrames[axisIndex];
                    // qDebug() << "++++++++ SPECTRAL axis Index with permutation is" << axisIndex << ", the current frame Index is" << frameIndex;
                } else if ( type == AxisInfo::KnownType::STOKES ) {
                    axisIndex = static_cast<int>( type );
                    frameIndex = mFrames[axisIndex];
                    // qDebug() << "++++++++ STOKE axis Index with permutation is" << axisIndex << ", the current frame Index is" << frameIndex;
                } else if ( type != AxisInfo::KnownType::OTHER ) {
                    axisIndex = static_cast<int>( type );
                    frameIndex = mFrames[axisIndex];
                    // qDebug() << "++++++++ axis Index with permutation is" << axisIndex << ", the current frame Index is" << frameIndex;
                }

                slice.start( frameIndex );
                slice.end( frameIndex + 1);
            }

            if ( i < imageDim - 1 ){
                slice.next();
            }
        }
        rawData = m_permuteImage->getDataSlice( nextSlice );
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


void DataSource::_initializeSingletons( ){
    //Load the available color maps.
    if ( m_coords == nullptr ){
        m_coords = Util::findSingletonObject<CoordinateSystems>();
    }
}


bool DataSource::_isLoadable( std::vector<int> frames ) const {
        int imageDim =m_image->dims().size();
	bool loadable = true;
	for ( int i = 0; i < imageDim; i++ ){
		AxisInfo::KnownType type = _getAxisType( i );
		if ( AxisInfo::KnownType::OTHER != type ){
			int axisIndex = static_cast<int>( type );
			int frameIndex = frames[axisIndex];
                        int frameCount = m_image->dims()[i];
			if ( frameIndex >= frameCount ){
				loadable = false;
				break;
			}
		}
		else {
			loadable = false;
			break;
		}
	}
	return loadable;
}

bool DataSource::_isSpectralAxis() const {
	bool spectralAxis = false;
	int imageSize = m_image->dims().size();
	for ( int i = 0; i < imageSize; i++ ){
		AxisInfo::KnownType axisType = _getAxisType( i );
		if ( axisType == AxisInfo::KnownType::SPECTRAL ){
			spectralAxis = true;
			break;
		}
	}
	return spectralAxis;
}

void DataSource::_load(std::vector<int> frames, bool recomputeClipsOnNewFrame,
        double minClipPercentile, double maxClipPercentile){
	//Only load if the frames make sense for the image.  I.e., the frame index
	//should be less than the image size.
	if ( _isLoadable( frames ) ){
		int frameSize = frames.size();
		CARTA_ASSERT( frameSize == static_cast<int>(AxisInfo::KnownType::OTHER));
		std::vector<int> mFrames = _fitFramesToImage( frames );
		std::shared_ptr<Carta::Lib::NdArray::RawViewInterface> view ( _getRawData( mFrames ) );
		std::vector<int> dimVector = view->dims();

		//Update the clip values
		if ( recomputeClipsOnNewFrame ){
			_updateClips( view,  minClipPercentile, maxClipPercentile, mFrames );
        }
		QString cacheId=m_pixelPipeline-> cacheId();
		m_renderService-> setPixelPipeline( m_pixelPipeline,cacheId );

		QString renderId = _getViewIdCurrent( mFrames );
		m_renderService-> setInputView( view, renderId );
	}
}


void DataSource::_resetZoom(){
    m_renderService-> setZoom( ZOOM_DEFAULT );
}

void DataSource::_resetPan(){
    if ( m_permuteImage != nullptr ){
        double xCenter =  m_permuteImage-> dims()[0] / 2.0;
        double yCenter = m_permuteImage-> dims()[1] / 2.0;
        m_renderService-> setPan({ xCenter, yCenter });
    }
}

QString DataSource::_setFileName( const QString& fileName, bool* success ){
    QString file = fileName.trimmed();
    *success = true;
    QString result;
    if (file.length() > 0) {
        if ( file != m_fileName ){
            try {
                auto res = Globals::instance()-> pluginManager()
                                      -> prepare <Carta::Lib::Hooks::LoadAstroImage>( file )
                                      .first();
                if (!res.isNull()){
                    m_image = res.val();
                    m_permuteImage = m_image;
                    std::shared_ptr<CoordinateFormatterInterface> cf(
                        m_image->metaData()->coordinateFormatter()->clone() );
                    m_coordinateFormatter = cf;
                    // reset zoom/pan
                    _resetZoom();
                    _resetPan();

                    m_fileName = file;
                }
                else {
                    result = "Could not find any plugin to load image";
                    qWarning() << result;
                    *success = false;
                }

            }
            catch( std::logic_error& err ){
                result = "Failed to load image "+fileName;
                qDebug() << result;
                *success = false;
            }
        }
    }
    else {
        result = "Could not load empty file.";
        *success = false;
    }
    return result;
}


void DataSource::_setColorMap( const QString& name ){
    Carta::State::ObjectManager* objManager = Carta::State::ObjectManager::objectManager();
    Carta::State::CartaObject* obj = objManager->getObject( Colormaps::CLASS_NAME );
    Colormaps* maps = dynamic_cast<Colormaps*>(obj);
    m_pixelPipeline-> setColormap( maps->getColorMap( name ) );
    m_renderService ->setPixelPipeline( m_pixelPipeline, m_pixelPipeline->cacheId());
}

void DataSource::_setColorInverted( bool inverted ){
    m_pixelPipeline-> setInvert( inverted );
    m_renderService-> setPixelPipeline( m_pixelPipeline, m_pixelPipeline-> cacheId());
}

void DataSource::_setColorReversed( bool reversed ){
    m_pixelPipeline-> setReverse( reversed );
    m_renderService-> setPixelPipeline( m_pixelPipeline, m_pixelPipeline-> cacheId());
}

void DataSource::_setColorAmounts( double newRed, double newGreen, double newBlue ){
    std::array<double,3> colorArray;
    colorArray[0] = newRed;
    colorArray[1] = newGreen;
    colorArray[2] = newBlue;
    m_pixelPipeline->setRgbMax( colorArray );
    m_renderService->setPixelPipeline( m_pixelPipeline, m_pixelPipeline->cacheId());
}

void DataSource::_setColorNan( double red, double green, double blue ){
    QColor nanColor( red, green, blue );
    m_renderService->setNanColor( nanColor );
}

bool DataSource::_setDisplayAxis( AxisInfo::KnownType axisType, int* axisIndex ){
    bool displayAxisChanged = false;
    if ( m_image ){
        int newXAxisIndex = Util::getAxisIndex(m_image, axisType);

        // invalid and let caller handle this case
        if (newXAxisIndex<0) {
            *axisIndex = newXAxisIndex;
             displayAxisChanged = true;
        }

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

    int m_axisIndexX_copy = m_axisIndexX;
    int m_axisIndexY_copy = m_axisIndexY;

    int displayAxisCount = displayAxisTypes.size();
    CARTA_ASSERT( displayAxisCount == 2 );
    bool axisXChanged = false;
    bool axisYChanged = false;
    //We could have an image with two linear display axes.  In this case, we can't
    //distinguish by the type of axis as we do below.
    if ( displayAxisTypes[0] == AxisInfo::KnownType::LINEAR &&
            displayAxisTypes[1] == AxisInfo::KnownType::LINEAR ){
        if ( m_axisIndexX != 0 ){
            m_axisIndexX = 0;
            axisXChanged = true;
        }
        if ( m_axisIndexY != 1 ){
            m_axisIndexY = 1;
            axisYChanged = true;
        }
    }
    else {
        axisXChanged = _setDisplayAxis( displayAxisTypes[0], &m_axisIndexX );
        axisYChanged = _setDisplayAxis( displayAxisTypes[1], &m_axisIndexY );
    }

    // invalid displayAxisTypes
    if (m_axisIndexX == -1 || m_axisIndexY == -1 ){
        m_axisIndexX = m_axisIndexX_copy;
        m_axisIndexY = m_axisIndexY_copy;

        axisXChanged = false;
        axisYChanged = false;
    }

    if ( axisXChanged || axisYChanged ){
        m_permuteImage = _getPermutedImage();
        _resetPan();
    }
    std::vector<int> mFrames = _fitFramesToImage( frames );
    _updateRenderedView( mFrames );
}

void DataSource::_setNanDefault( bool nanDefault ){
    m_renderService->setDefaultNan( nanDefault );
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


void DataSource::_setGamma( double gamma ){
    m_pixelPipeline->setGamma( gamma );
    m_renderService->setPixelPipeline( m_pixelPipeline, m_pixelPipeline->cacheId());
}

// TODO: should this function be eliminated in favour of _getIntensity?
std::vector<double> DataSource::_getQuantileIntensityCache(std::shared_ptr<Carta::Lib::NdArray::RawViewInterface>& view,
        double minClipPercentile, double maxClipPercentile, const std::vector<int>& frames, bool showMesg) {
    std::vector<int> mFrames = _fitFramesToImage( frames );
    std::vector<int> stokeIndex = _getStokeIndex( mFrames );
    std::vector<int> channelIndex = _getChannelIndex( mFrames );


    int setChannelIndex;
    if (channelIndex[1] == -1) {
        // channelIndex[1] == -1 means there is only one channel in the image file,
        // in such case we rename the channel index (setChannelIndex) = 0
        setChannelIndex = 0;
    } else {
        setChannelIndex = channelIndex[1];
    }

    std::vector<double> clips;

    // If the disk cache exists, try to find the clips in the cache first
    std::shared_ptr<Carta::Lib::IntensityValue> minClipInCache = _readIntensityCache(setChannelIndex, setChannelIndex, minClipPercentile, stokeIndex[1], "NONE");
    std::shared_ptr<Carta::Lib::IntensityValue> maxClipInCache = _readIntensityCache(setChannelIndex, setChannelIndex, maxClipPercentile, stokeIndex[1], "NONE");
    // if both of caches exist, we get their values
    if (minClipInCache && minClipInCache->error == 0 /* minimum intensity cache exists and has a zero error order */ &&
        maxClipInCache && maxClipInCache->error == 0 /* maximum intensity cache exists and has a zero error order */) {
        clips.push_back(minClipInCache->value);
        clips.push_back(maxClipInCache->value);
        if (showMesg == true) {
            qDebug() << "++++++++ [find cache] for percentile (per frame)= [" << minClipPercentile << "," << maxClipPercentile << "], intensity= [" << clips[0] << "," << clips[1] << "]";
        }
    }


    // If the clips were not found in the cache, calculate them
    if (clips.size() < 2) {
        Carta::Lib::NdArray::Double doubleView( view.get(), false );

        // start the timer for computing percentile per frame
        QElapsedTimer timer;
        timer.start();

        // calculate pixel values with respect to percentiles per frame
        Carta::Lib::IPercentilesToPixels<double>::SharedPtr calculator = std::make_shared<Carta::Core::Algorithms::PercentilesToPixels<double> >();

        std::map<double, double> clips_map = calculator->percentile2pixels(doubleView, {minClipPercentile, maxClipPercentile}, -1, nullptr, {});

        // end of timer for computing percentile per frame
        int elapsedTime = timer.elapsed();

        if (CARTA_RUNTIME_CHECKS) {
            if (elapsedTime > 5) {
                // only save the elapsed time to log file if it is greater than 5 ms
                qCritical() << "<> Time to get the percentile per frame:" << elapsedTime << "ms";
            } else {
                qDebug() << "++++++++ [percentile per frame] calculating time (for std::nth_element):" << elapsedTime << "ms";
            }
        }

        clips = {clips_map[minClipPercentile], clips_map[maxClipPercentile]};

        // If the disk cache exists, put the calculated clips in it
        // this step is done in DataSource::_getCursorText() first !!
        // the intensity error is zero, because we use percentile2pixels() --> "std::nth_element" algorithm
        // for precise percentile calculation
        _setIntensityCache(clips[0], 0, setChannelIndex, setChannelIndex, minClipPercentile, stokeIndex[1], "NONE");
        _setIntensityCache(clips[1], 0, setChannelIndex, setChannelIndex, maxClipPercentile, stokeIndex[1], "NONE");
        qDebug() << "++++++++ [find cache] for percentile (per frame)= [" << minClipPercentile << "," << maxClipPercentile << "], intensity= [" << clips[0] << "," << clips[1] << "]";
    }
    return clips;
}

void DataSource::_updateClips( std::shared_ptr<Carta::Lib::NdArray::RawViewInterface>& view,
        double minClipPercentile, double maxClipPercentile, const std::vector<int>& frames ){
    // get quantile intensity cache
    std::vector<double> clips = _getQuantileIntensityCache(view, minClipPercentile, maxClipPercentile, frames, true);
    m_pixelPipeline-> setMinMax( clips[0], clips[1] );
    m_renderService-> setPixelPipeline( m_pixelPipeline, m_pixelPipeline-> cacheId());
}

std::shared_ptr<Carta::Lib::NdArray::RawViewInterface> DataSource::_updateRenderedView( const std::vector<int>& frames ){
    // get a view of the data using the slice description and make a shared pointer out of it
    std::shared_ptr<Carta::Lib::NdArray::RawViewInterface> view( _getRawData( frames ) );
    // tell the render service to render this job
    QString renderId = _getViewIdCurrent( frames );
    m_renderService-> setInputView( view, renderId/*, m_axisIndexX, m_axisIndexY*/ );
    return view;
}

void DataSource::_viewResize( const QSize& newSize ){
    m_renderService-> setOutputSize( newSize );
}


DataSource::~DataSource() {

}
}
}
