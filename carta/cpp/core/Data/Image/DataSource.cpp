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
#include "CartaLib/PixelPipeline/CustomizablePixelPipeline.h"
#include "CartaLib/IPCache.h"
#include "../../ImageRenderService.h"
#include "../../Algorithms/percentileAlgorithms.h"
#include "../../Algorithms/cacheUtils.h"
#include "../Clips.h"
#include <QDebug>
#include <QElapsedTimer>

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
        }
        else {
            m_diskCache = res.val();
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
    return types;
}


std::vector<AxisInfo> DataSource::_getAxisInfos() const {
    std::vector<AxisInfo> Infos;
    CoordinateFormatterInterface::SharedPtr cf(
                   m_image-> metaData()-> coordinateFormatter()-> clone() );
    int axisCount = cf->nAxes();
    for ( int axis = 0 ; axis < axisCount; axis++ ) {
        const AxisInfo & axisInfo = cf-> axisInfo( axis );
        if ( axisInfo.knownType() != AxisInfo::KnownType::OTHER ){
            Infos.push_back( axisInfo );
        }
    }
    return Infos;
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

QString DataSource::_getSkyCS(){

    CoordinateFormatterInterface::SharedPtr cf(
            m_image-> metaData()-> coordinateFormatter()-> clone() );

    QString coordName = m_coords->getName( cf->skyCS() );
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


std::pair<bool, double> DataSource::_isSameValue(double inputValue, std::vector<double> comparedValue, double threshold) const {
    std::pair<bool, double> result = std::make_pair(false, -1);
    for ( auto iter : comparedValue ) {
        if (fabs(inputValue - iter) < threshold) {
            result = std::make_pair(true, iter);
            break;
        }
    }
    return result;
}

// TODO: create a helper struct for this with named value anf error members to make the code more legible.
std::pair<double, double> DataSource::_readIntensityCache(int frameLow, int frameHigh, double percentile, int stokeFrame, QString transformation_label) const {
    std::pair<double, double> result = std::make_pair(-1, -1);
    if (m_diskCache) {
        bool intensityInCache = false;
        double value = -1; // define intensity
        double error = -1; // define intensity error order, i.e. (max-min)*[error order]
        QString intensityKey = QString("%1/%2/%3/%4/%5/%6/intensity").arg(m_fileName).arg(frameLow).arg(frameHigh).arg(stokeFrame).arg(percentile).arg(transformation_label);
        QByteArray intensityVal, intensityError;
        intensityInCache = m_diskCache->readEntry(intensityKey.toUtf8(), intensityVal, intensityError);
        if (intensityInCache) {
            value = qb2d(intensityVal);
            error = qb2d(intensityError);
        }
        //qDebug() << "[read intensity] intensity=" << value << "intensity error order=" << error;
        result = std::make_pair(value, error);
    }
    return result;
}

// TODO: have to add the transformation label
void DataSource::_setIntensityCache(double intensity, double error, int frameLow, int frameHigh, double percentile, int stokeFrame, QString transformation_label) const {
    if (m_diskCache) {
        QString intensityKey = QString("%1/%2/%3/%4/%5/%6/intensity").arg(m_fileName).arg(frameLow).arg(frameHigh).arg(stokeFrame).arg(percentile).arg(transformation_label);
        m_diskCache->setEntry(intensityKey.toUtf8(), d2qb(intensity), d2qb(error));
    }
}

// 2017/05/16    C.C. Chiang: Modify this function that it can get the intensity (pixel) for different stokes (I, Q, U and V)
std::vector<double> DataSource::_getIntensity(int frameLow, int frameHigh,
        const std::vector<double>& percentiles, int stokeFrame,
        Carta::Lib::IntensityUnitConverter::SharedPtr converter) {
    
    int percentileCount = percentiles.size();
    std::vector<double> intensities(percentileCount, 0);
    std::vector<std::pair<double, double>> intensityCache(percentileCount, std::pair<double, double>(-1, -1));
    int foundCount = 0;
    std::vector<bool> found(percentileCount, false);
    
    QString transformation_label = converter ? converter->label : "NONE";

    // TODO: approximate algorithm(s) should be delegated to plugins; this function should know nothing about their implementation.
    // TODO: plugins should return the error order for each value as well as the value
    // TODO: separate hooks for min/max and other values? default implementation should delegate to built-in exact functions?
    // TODO: integrate the cache into algorithm internals
    // * i.e. look up min/max instead of calculating if it's required in the algorithm, as in histogram approximation
    // * we'll need to pass in a converter, so we may as well also pass in cache set / get functions or objects?

    // get the default setting whether to turn on the approximation algorithm for percentile calculations from "config.json"
    bool isApproximation = Globals::instance() -> mainConfig() -> isPercentileApproximation();
    qDebug() << "++++++++ [config.json] percentileApproximation:" << isApproximation;

    // get the default setting from "config.json" to define the pixel bin size = (max-min)/percentApproxDividedNum
    // for percentile approximation algorithm
    unsigned int percentApproxDividedNum = Globals::instance() -> mainConfig() -> getPercentApproxDividedNum();
    qDebug() << "++++++++ [config.json] percentApproxDividedNum:" << percentApproxDividedNum;

    double chooseError;
    if (isApproximation == true) {
        // if we apply approximation algorithm for percentile to pixel calculation,
        // then we have a non-zero error bar
        chooseError = 1 / static_cast<double>(percentApproxDividedNum);
    } else {
        // if not, by default we use the absolute precise algorithm for percentile to pixel calculation,
        // the error bar is zero
        chooseError = 0;
    }
    qDebug() << "++++++++ [config.json] choose error order=" << chooseError;

    // If the disk cache exists, try to look up cached intensity and location values
    for (int i = 0; i < percentileCount; i++) {
        intensityCache[i] = _readIntensityCache(frameLow, frameHigh, percentiles[i], stokeFrame, transformation_label);
        
        if (intensityCache[i].second != -1 /* this intensity cache exists */ &&
            intensityCache[i].second <= chooseError /* already has an intensity error order smaller than the current choice */ ) {
            intensities[i] = intensityCache[i].first;
            // We only cache statistics for each distinct frame-dependent calculation
            // But we need to apply any constant multipliers
            if (converter) {
                intensities[i] = intensities[i] * converter->multiplier;
            }
            foundCount++;
            found[i] = true;
            qDebug() << "++++++++ [find cache for the most precise intensity] for percentile" << percentiles[i]
                     << ", intensity=" << intensities[i] << "+/- (max-min)*" << intensityCache[i].second;
        }
    }

    //Not all percentiles were in the cache.  We are going to have to look some up.
    if (foundCount < percentileCount) {
        qDebug() << "++++++++ Calculating percentile to pixel values..";

        // the SPECTRAL index is the last index of image dimension, which is corresponding to the channel-axis
        int spectralIndex = Util::getAxisIndex( m_image, AxisInfo::KnownType::SPECTRAL );
        int stokeIndex = Util::getAxisIndex( m_image, AxisInfo::KnownType::STOKES );
        qDebug() << "++++++++ Spectral Index No. is " << spectralIndex << "; Stoke Index No. is " << stokeIndex;

        // get raw data (for all stokes if any) in order to sort the raw data set by selection algorithm
        // Carta::Lib::NdArray::RawViewInterface* rawData = _getRawData( frameLow, frameHigh, spectralIndex );

        // get raw data (for the specific stoke) in order to sort the raw data set by selection algorithm
        Carta::Lib::NdArray::RawViewInterface* rawData = _getRawDataForStoke(frameLow, frameHigh, spectralIndex, stokeIndex, stokeFrame);
        qDebug() << "++++++++ frameLow=" << frameLow << ", frameHigh=" << frameHigh;
        qDebug() << "++++++++ set the Stoke Index for Percentile=" << stokeFrame
                 << "(-1: no stoke, 0: stoke I, 1: stoke Q, 2: stoke U, 3: stoke V)";

        if (rawData == nullptr) {
            qCritical() << "Error: could not retrieve image data to calculate missing intensities.";
            return intensities;
        }

        // load raw data through "Carta::Lib::NdArray::RawViewInterface" shared pointer
        std::shared_ptr<Carta::Lib::NdArray::RawViewInterface> view(rawData);

        // get raw data as double variables
        // please refer to IImage.h:186  TypedView( RawViewInterface * rawView, bool keepOwnership = false )
        Carta::Lib::NdArray::Double doubleView(view.get(), false);

        // check the raw data size
        std::vector<int> dataDims = doubleView.dims();
        size_t dataSize = 1;
        for ( auto dim : dataDims ) dataSize *= dim;
        qDebug() << "++++++++ check the raw data size:" << dataSize << "pixel elements, for raw data shape" << dataDims;

        // Make a list of the percentiles which have to be calculated
        std::vector<double> percentiles_to_calculate;
        for (int i = 0; i < percentileCount; i++) {
            if (!found[i]) {
                percentiles_to_calculate.push_back(percentiles[i]);
            }
        }
        
        // TODO: what is this for?
        if (percentiles_to_calculate.size() != 2) {
            qWarning() << "the number of percentiles to calculate is not equal to 2 (check cache save procedure)!!";
        }

        // define the priority number (error order) for choosing (key, value) from SQLite3
        double error;
        
        
        // Find Hz values if they are required for the unit transformation
        std::vector<double> hertzValues;
        
        if (converter && converter->frameDependent) {
            hertzValues = _getHertzValues(doubleView.dims(), spectralIndex);
        }

        // Calculate only the required percentiles
        std::map<double, double> clips_map;

        // TODO: use the minmax algorithm to calculate the min and/or max even if they are not both present and not the only percentiles
        // And not necessarily in the same order
        // TODO: adjust the lambda used in the minmax algorithm so that it can do one or the other and not necessarily both

        if (percentiles_to_calculate.size() == 2 && percentiles_to_calculate[0] == 0 && percentiles_to_calculate[1] == 1) {

            // if percentiles = 0% or 100%, use the iteration algorithm
            qDebug() << "++++++++ [apply] Carta::Core::Algorithms::minMax2pixels() function !!";
            clips_map = Carta::Core::Algorithms::minMax2pixels(doubleView, percentiles_to_calculate, spectralIndex, converter, hertzValues);

            // set the priority number (error order) to be the first priority
            error = 0;

        } else if (isApproximation) {
            // if percentiles != 0% or 100%, use the approximate algorithm
            qDebug() << "++++++++ [apply] Carta::Core::Algorithms::percentile2pixels_approximation() function !!";
            
            // get all clip settings for approximate percentile calculations
            std::shared_ptr<Carta::Data::Clips> m_clips;
            std::vector<double> percentiles_from_all_clips = m_clips -> getAllClips2percentiles();

            // get extra clipping values from UI which is not equal to current clipping ones
            // TODO: we check before caching these at the end, but we don't check if they are cached now.
            // TODO: this logic could be clearer
            // TODO: make a separate vector; combine the two vectors before passing them in; use the separate vector to find extra values to cache below
            // TODO: can use a much simpler function to check for approximate membership in the original vector
            std::vector<double> percentiles_to_calculate_plus;
            
            for (auto& p : percentiles_to_calculate) {
                percentiles_to_calculate_plus.push_back(p);
            }
            
            std::pair<bool, double> parse_percentiles_from_all_clips;
            
            for (auto& p : percentiles_from_all_clips) {
                // the same clipping value from different sources are not absolute equal !!
                // so we need to make the following checks !!
                parse_percentiles_from_all_clips = _isSameValue(p, percentiles_to_calculate, 1e-6);
                if (parse_percentiles_from_all_clips.first == true) {
                    // TODO: why do we add the same value repeatedly?
//                     percentiles_to_calculate_plus.push_back(parse_percentiles_from_all_clips.second);
                    continue; // we already added this
                } else {
                    percentiles_to_calculate_plus.push_back(p);
                }
            }
            
            std::sort(percentiles_to_calculate_plus.begin(), percentiles_to_calculate_plus.end());
        
            for (auto& p : percentiles_to_calculate_plus) {
                qDebug() << p;
            }

            // get minimum and maximum pixel values from the cache or from the raw data
            // This should call the block above
            std::vector<double> minMaxIntensities = _getIntensity(frameLow, frameHigh, std::vector<double>({0, 1}), stokeFrame, converter);

            // apply approximate percentile algorithm for clipping values
            clips_map = Carta::Core::Algorithms::percentile2pixels_approximation(doubleView, minMaxIntensities,
                    percentApproxDividedNum, percentiles_to_calculate_plus, spectralIndex, converter, hertzValues);

            // set the priority number (error order) to be the inverse of percentApproxDividedNum
            error = 1 / static_cast<double>(percentApproxDividedNum);
            
            // if we use the approximation algorithm to get percentiles with respect to pixels values,
            // we can calculate all Clipping values listed on the UI at one loop.
            // In such case, we can set the extra percentiles with respect to pixels values in the other caches.
            // The advantage of this method is that we don't need to use approximation algorithm again if we want
            // to get another Clipping values in the UI.
            // TODO: this logic could be clearer
            std::pair<bool, double> check_repetition;
            if (clips_map.size() > percentiles_to_calculate.size()) {
                for (size_t i = 0; i < percentiles_to_calculate_plus.size(); i++) {
                    // check if the percentile to pixel value to store is already done in previous loop
                    check_repetition = _isSameValue(percentiles_to_calculate_plus[i], percentiles_to_calculate, 1e-6);
                    if (check_repetition.first == true) continue;
                    _setIntensityCache(clips_map[percentiles_to_calculate_plus[i]], error, frameLow, frameHigh, percentiles_to_calculate_plus[i], stokeFrame, transformation_label);
                    qDebug() << "++++++++ [set extra cache] for percentile" << percentiles_to_calculate_plus[i]
                            << ", intensity=" << clips_map[percentiles_to_calculate_plus[i]] << "+/- (max-min)*" << error;
                }
            }

        } else {

            // if percentiles != 0% or 100%, use the precise algorithm
            qDebug() << "++++++++ [apply] Carta::Core::Algorithms::percentile2pixels() function !!";

            // apply std::nth_element for precise percentile calculations
            clips_map = Carta::Core::Algorithms::percentile2pixels(doubleView, percentiles_to_calculate, spectralIndex, converter, hertzValues);

            // set the priority number (error order) to be the first priority
            error = 0;

        }

        // set return values and cache
        for (int i = 0; i < percentileCount; i++) {
            if (!found[i]) {
                intensities[i] = clips_map[percentiles[i]];
                found[i] = true; // for completeness, in case we test this later

                // put calculated values in the disk cache if it exists
                _setIntensityCache(intensities[i], error, frameLow, frameHigh, percentiles[i], stokeFrame, transformation_label);
                
                // apply any constant multiplier *after* caching the frame-dependent portion of the calculation
                if (converter) {
                    intensities[i] = intensities[i] * converter->multiplier;
                }
                
                qDebug() << "++++++++ [set cache] for percentile" << percentiles[i]
                         << ", intensity=" << intensities[i] << "+/- (max-min)*" << error;

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
        
        percentiles = Carta::Core::Algorithms::intensities2percentiles(view, intensities, spectralIndex, converter, hertzValues);
    }
    return percentiles;
}

QPointF DataSource::_getPixelCoordinates( double ra, double dec, bool* valid ) const{
    QPointF result;
    CoordinateFormatterInterface::SharedPtr cf( m_image-> metaData()-> coordinateFormatter()-> clone() );
    const CoordinateFormatterInterface::VD world { ra, dec };
    CoordinateFormatterInterface::VD pixel;
    *valid = cf->toPixel( world, pixel );
    if ( *valid ){
        result = QPointF( pixel[0], pixel[1]);
    }
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

// TODO: this function should be eliminated; it's a duplicate of _getIntensity
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
    std::pair<double, double> minClipInCache = _readIntensityCache(setChannelIndex, setChannelIndex, minClipPercentile, stokeIndex[1], "NONE");
    std::pair<double, double> maxClipInCache = _readIntensityCache(setChannelIndex, setChannelIndex, maxClipPercentile, stokeIndex[1], "NONE");
    // if both of caches exist, we get their values
    if (minClipInCache.second == 0 /* minimum intensity cache exists and has a zero error order */ &&
        maxClipInCache.second == 0 /* maximum intensity cache exists and has a zero error order */) {
        clips.push_back(minClipInCache.first);
        clips.push_back(maxClipInCache.first);
        if (showMesg == true) qDebug() << "++++++++ [find cache] for clips= [" << clips[0] << "," << clips[1] << "]";
    }


    // If the clips were not found in the cache, calculate them
    if (clips.size() < 2) {
        Carta::Lib::NdArray::Double doubleView( view.get(), false );

        // start the timer for computing percentile per frame
        QElapsedTimer timer;
        timer.start();

        // calculate pixel values with respect to percentiles per frame
        std::map<double, double> clips_map = Carta::Core::Algorithms::percentile2pixels(doubleView, { minClipPercentile, maxClipPercentile });

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
        qDebug() << "++++++++ [set cache] for clips= [" << clips[0] << "," << clips[1] << "]";

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
