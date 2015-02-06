#include "Data/Histogram.h"
#include "Data/Clips.h"
#include "Data/Controller.h"
#include "Data/Util.h"
#include "Globals.h"
#include "ImageView.h"
#include "PluginManager.h"
#include "ErrorManager.h"
#include "CartaLib/Hooks/Histogram.h"
#include "Util.h"
#include <set>
#include "HistogramGenerator.h"
#include <QDebug>

namespace Carta {

namespace Data {

const QString Histogram::CLASS_NAME = "Histogram";

const QString Histogram::CLIP_INDEX = "clipIndex";
const QString Histogram::CLIP_MIN = "clipMin";
const QString Histogram::CLIP_MAX = "clipMax";
const QString Histogram::CLIP_APPLY = "applyClipToImage";
const QString Histogram::BIN_COUNT = "binCount";
const QString Histogram::GRAPH_STYLE = "graphStyle";
const QString Histogram::GRAPH_STYLE_LINE = "Line";
const QString Histogram::GRAPH_STYLE_OUTLINE = "Outline";
const QString Histogram::GRAPH_STYLE_FILL = "Fill";
const QString Histogram::GRAPH_LOG_COUNT = "logCount";
const QString Histogram::GRAPH_COLORED = "colored";
const QString Histogram::PLANE_SINGLE = "selectedPlane";
const QString Histogram::PLANE_MODE="planeMode";
const QString Histogram::PLANE_MODE_SINGLE="Single";
const QString Histogram::PLANE_MODE_RANGE="Range";
const QString Histogram::PLANE_MODE_ALL="All";
const QString Histogram::PLANE_MIN = "planeMin";
const QString Histogram::PLANE_MAX = "planeMax";
const QString Histogram::FOOT_PRINT = "twoDFootPrint";
const QString Histogram::FOOT_PRINT_IMAGE = "Image";
const QString Histogram::FOOT_PRINT_REGION = "Selected Region";
const QString Histogram::FOOT_PRINT_REGION_ALL = "All Regions";
const QString Histogram::CLIP_MIN_PERCENT = "clipMinPercent";
const QString Histogram::CLIP_MAX_PERCENT = "clipMaxPercent";
const QString Histogram::LINK = "links";

std::shared_ptr<Clips>  Histogram::m_clips = nullptr;

class Histogram::Factory : public CartaObjectFactory {
    public:

        CartaObject * create (const QString & path, const QString & id)
        {
            return new Histogram (path, id);
        }
    };

bool Histogram::m_registered =
    ObjectManager::objectManager()->registerClass ( CLASS_NAME, new Histogram::Factory());



Histogram::Histogram( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ),
    m_view(nullptr),
    m_linkImpl( new LinkableImpl( &m_state)),
    m_stateMouse(path + StateInterface::DELIMITER+ImageView::VIEW){
    _initializeDefaultState();
    _initializeCallbacks();
    m_view.reset( new ImageView( path, QColor("yellow"), QImage(), &m_stateMouse));
    registerView(m_view.get());

    //Load the available clips.
    if ( m_clips == nullptr ){
        CartaObject* obj = Util::findSingletonObject( Clips::CLASS_NAME );
        m_clips.reset(dynamic_cast<Clips*>(obj));
    }

}

bool Histogram::addLink( const std::shared_ptr<Controller> & controller){
    bool linkAdded = m_linkImpl->addLink( controller );
    if ( linkAdded ){
        connect(controller.get(), SIGNAL(dataChanged()), this , SLOT(_generateHistogram()));
    }
    return linkAdded;
}



void Histogram::clear(){
    unregisterView();
    m_linkImpl->clear();
}

NdArray::RawViewInterface* Histogram::_findRawData( const QString& fileName, int frameIndex ) const {
    NdArray::RawViewInterface * rawData = nullptr;
    for( std::shared_ptr<Controller> controller : m_linkImpl->m_controllers ){
        rawData = controller->getRawData( fileName, frameIndex);
        if ( rawData != nullptr ){
            break;
        }
    }
    return rawData;
}

double Histogram::_getPercentile( const QString& fileName, int frameIndex, double intensity ) const {
    double percentile = -1;
    NdArray::RawViewInterface* rawData = _findRawData( fileName, frameIndex );
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

        percentile = double(countBelow) / totalCount;
    }
    return percentile;
}

bool Histogram::_getIntensity( const QString& fileName, int frameIndex, double percentile, double* intensity ) const {
    bool intensityFound = false;
    NdArray::RawViewInterface* rawData = _findRawData( fileName, frameIndex );
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
            std::nth_element( allValues.begin(), allValues.begin()+locationIndex, allValues.end() );
            *intensity = allValues[locationIndex];
            intensityFound = true;
        }
    }
    return intensityFound;
}

int Histogram::_getLinkInfo( QString& link, QString& name ){
    std::shared_ptr<Controller> controller = m_linkImpl->searchControllers(link);
    int index = -1;
    if(controller != NULL){
        index = controller->getSelectImageIndex();
        name = controller-> getImageName(index);
    }
    return index;
}

void Histogram::_initializeDefaultState(){
    m_state.insertValue<int>(CLIP_INDEX, 0 );
    m_state.insertValue<double>( CLIP_MIN, 0 );
    m_state.insertValue<double>(CLIP_MAX, 1);
    m_state.insertValue<bool>(CLIP_APPLY, false );
    m_state.insertValue<int>(BIN_COUNT, 25 );
    m_state.insertValue<QString>(GRAPH_STYLE, GRAPH_STYLE_LINE);
    m_state.insertValue<bool>(GRAPH_LOG_COUNT, true );
    m_state.insertValue<bool>(GRAPH_COLORED, true );
    m_state.insertValue<QString>(PLANE_MODE, PLANE_MODE_SINGLE );
    m_state.insertValue<int>(PLANE_SINGLE, 0 );
    m_state.insertValue<int>(PLANE_MIN, 0 );
    m_state.insertValue<int>(PLANE_MAX, 1 );
    m_state.insertValue<QString>(FOOT_PRINT, FOOT_PRINT_IMAGE );
    m_state.insertValue<double>(CLIP_MIN_PERCENT, 0);
    m_state.insertValue<double>(CLIP_MAX_PERCENT, 0);
    
    m_state.flushState();

    m_stateMouse.insertObject( ImageView::MOUSE );
    m_stateMouse.insertValue<QString>(ImageView::MOUSE_X, 0 );
    m_stateMouse.insertValue<QString>(ImageView::MOUSE_Y, 0 );
    m_stateMouse.flushState();
}

void Histogram::_initializeCallbacks(){
    addCommandCallback( "setBinCount", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
            QString result = _setBinCount( params );
            return result;
        });

    addCommandCallback( "setClipPercent", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
            QString result = _setClipPercent( params );
            return result;
        });

    addCommandCallback( "setClipRange", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) -> QString {
                QString result = _setClipRange( params );
                return result;
        });

    addCommandCallback( "setClipToImage", [=] (const QString & /*cmd*/,
                            const QString & params, const QString & /*sessionId*/) -> QString {
                    QString result = _setClipToImage( params );
                    return result;
                });

    addCommandCallback( "setGraphStyle", [=] (const QString & /*cmd*/,
                            const QString & params, const QString & /*sessionId*/) -> QString {
                    QString result = _setGraphStyle( params );
                    return result;
                });

    addCommandCallback( "setLogCount", [=] (const QString & /*cmd*/,
                                const QString & params, const QString & /*sessionId*/) -> QString {
                        QString result = _setLogCount( params );
                        return result;
                    });

    addCommandCallback( "setColored", [=] (const QString & /*cmd*/,
                                    const QString & params, const QString & /*sessionId*/) -> QString {
                            QString result = _setColored( params );
                            return result;
                        });

    addCommandCallback( "setPlaneMode", [=] (const QString & /*cmd*/,
                                        const QString & params, const QString & /*sessionId*/) -> QString {
                                QString result = _setPlaneMode( params );
                                return result;
                            });

    addCommandCallback( "setPlaneSingle", [=] (const QString & /*cmd*/,
                                        const QString & params, const QString & /*sessionId*/) -> QString {
                                QString result = _setPlaneSingle( params );
                                return result;
                            });

    addCommandCallback( "setPlaneRange", [=] (const QString & /*cmd*/,
                                        const QString & params, const QString & /*sessionId*/) -> QString {
                                QString result = _setPlaneRange( params );
                                return result;
                            });

    addCommandCallback( "set2DFootPrint", [=] (const QString & /*cmd*/,
                                            const QString & params, const QString & /*sessionId*/) -> QString {
                                    QString result = _set2DFootPrint( params );
                                    return result;
                                });

    addCommandCallback( "zoomFull", [=] (const QString & /*cmd*/,
                                            const QString & /*params*/, const QString & /*sessionId*/) -> QString {
                                    qDebug() << "Need to implement histogram zoom full";
                                    return "";
                                });

    addCommandCallback( "zoomRange", [=] (const QString & /*cmd*/,
                                                const QString & /*params*/, const QString & /*sessionId*/) -> QString {
                                        qDebug() << "Need to implement histogram zoom range";
                                        return "";
                                    });
}

QString Histogram::_setClipRange( const QString& params ){
    qDebug()<<"inside clip range function";
    QString result;
    std::set<QString> keys = {CLIP_MIN, CLIP_MAX, LINK};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString clipMinStr = dataValues[CLIP_MIN];
    QString clipMaxStr = dataValues[CLIP_MAX];
    QString links = dataValues[LINK];
    bool validRangeMin = false;
    double clipMin = clipMinStr.toDouble( &validRangeMin );
    bool validRangeMax = false;
    double clipMax = clipMaxStr.toDouble( &validRangeMax );
    double oldMin = m_state.getValue<double>(CLIP_MIN);
    double oldMax = m_state.getValue<double>(CLIP_MAX);
    double oldMinPercent = m_state.getValue<double>(CLIP_MIN_PERCENT);
    double oldMaxPercent = m_state.getValue<double>(CLIP_MAX_PERCENT);
    
    if ( validRangeMin && validRangeMax ){
        bool changedState = false;
        bool changedMin = false;
        bool changedMax = false;
        if ( clipMin != oldMin){
            m_state.setValue<double>(CLIP_MIN, clipMin );
            changedState = true;
            changedMin =true;
        }
        if ( clipMax != oldMax ){
            m_state.setValue<double>(CLIP_MAX, clipMax );
            changedState = true;
            changedMax = true;
        }
        if ( changedState ){
            QString filename;
            int index = _getLinkInfo(links, filename);
            if(index >=0 ){
                if(changedMin){
                double clipMinPercent = _getPercentile(filename, 0, clipMin);
                    if(oldMinPercent != clipMinPercent)
                        m_state.setValue<double>(CLIP_MIN_PERCENT, clipMinPercent);
                }
                if(changedMax){
                    double clipMaxPercent = _getPercentile(filename, 0, clipMax);
                    if(oldMaxPercent != clipMaxPercent)
                        m_state.setValue<double>(CLIP_MAX_PERCENT, clipMaxPercent);
                }    
            }
            
            m_state.flushState();
            if(clipMin<clipMax)
                _generateHistogram();
            else{
               result = "Invalid Histogram clip range parameters: "+ params +
               "clip min range has to be less than clip max range"; 
            }
        }
    }
    else {
        result = "Invalid Histogram clip range parameters: "+ params;
    }
    QString origValues = QString::number(oldMin) +","+QString::number(oldMax);
    result = Util::commandPostProcess( result, origValues );
    return result;
}

QString Histogram::_setClipPercent( const QString& params ){
    QString result;
    std::set<QString> keys = {CLIP_MIN_PERCENT, CLIP_MAX_PERCENT, LINK};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString clipMinPercentStr = dataValues[CLIP_MIN_PERCENT];
    QString clipMaxPercentStr = dataValues[CLIP_MAX_PERCENT];
    QString links = dataValues[LINK];
    bool validRangePercentMin = false;
    double clipMinPercent = clipMinPercentStr.toDouble( &validRangePercentMin );
    bool validRangePercentMax = false;
    double clipMaxPercent = clipMaxPercentStr.toDouble( &validRangePercentMax );
    double oldClipMinPercent = m_state.getValue<double>(CLIP_MIN_PERCENT);
    double oldClipMaxPercent = m_state.getValue<double>(CLIP_MAX_PERCENT);
    double oldClipMin = m_state.getValue<double>(CLIP_MIN);
    double oldClipMax = m_state.getValue<double>(CLIP_MAX);
    
    if (validRangePercentMin && validRangePercentMax){
        bool changedState = false;
        bool changedMinPercent = false;
        bool changedMaxPercent = false;

        if(oldClipMinPercent != clipMinPercent){
            m_state.setValue<double>(CLIP_MIN_PERCENT, clipMinPercent );
            changedState = true;
            changedMinPercent = true;
         }

        if( oldClipMaxPercent != clipMaxPercent ){
            m_state.setValue<double>(CLIP_MAX_PERCENT, clipMaxPercent );
            changedState = true;
            changedMaxPercent = true;
        }

        if(changedState){
            QString filename;
            int index = _getLinkInfo(links, filename);
            double clipMin = 0;
            double clipMax = 0;
            if(index >= 0 ){
                if(changedMinPercent){
                    bool minIntensity = _getIntensity(filename, index, clipMinPercent, &clipMin);
                    if(minIntensity && clipMin != oldClipMin)
                        m_state.setValue<double>(CLIP_MIN, clipMin);
                }
                if(changedMaxPercent){
                    bool maxIntensity = _getIntensity(filename, index, clipMaxPercent, &clipMax);
                    if(maxIntensity && clipMax != oldClipMax)
                        m_state.setValue<double>(CLIP_MAX, clipMax);
                }
            }
            m_state.flushState();
            if(clipMin < clipMax)
                _generateHistogram();
            else {
                result = "Invalid Histogram clip range parameters: "+ params + 
                "clip min range has to be less than clip max range";
            }
        }
    }
    else {
        result = "Invalid clip params: "+params;
    }
    QString origValues = QString::number(oldClipMinPercent)+","+QString::number(oldClipMaxPercent);
    result = Util::commandPostProcess( result, origValues);
    return result;
}


QString Histogram::_setColored( const QString& params ){
    QString result;
    std::set<QString> keys = {GRAPH_COLORED};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString coloredStr = dataValues[*keys.begin()];
    bool validBool = false;
    bool colored = Util::toBool( coloredStr, &validBool );
    bool oldColored = m_state.getValue<bool>(GRAPH_COLORED);
    if ( validBool ){
        if ( colored != oldColored){
            m_state.setValue<bool>(GRAPH_COLORED, colored );
            m_state.flushState();
            _generateHistogram();
        }
    }
    else {
        result = "Invalid Histogram colored parameter; must be a bool: "+ params;
    }
    result = Util::commandPostProcess( result, Util::toString( oldColored ));
    return result;
}

QString Histogram::_setBinCount( const QString& params ){
    QString result;
    std::set<QString> keys = {BIN_COUNT};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString binCountStr = dataValues[*keys.begin()];
    bool validInt = false;
    int binCount = binCountStr.toInt( &validInt );
    int oldBinCount = m_state.getValue<int>(BIN_COUNT );
    if ( validInt ){
        if ( binCount != oldBinCount ){
           m_state.setValue<int>(BIN_COUNT, binCount );
           m_state.flushState();
           _generateHistogram();

        }
    }
    else {
        result = "Invalid Histogram bin count parameters: "+ params;
    }
    result = Util::commandPostProcess( result, QString::number( oldBinCount) );
    return result;
}


QString Histogram::_setLogCount( const QString& params ){
    QString result;
    std::set<QString> keys = {GRAPH_LOG_COUNT};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString logCountStr = dataValues[*keys.begin()];
    bool validBool = false;
    bool logCount = Util::toBool( logCountStr, &validBool );
    bool oldLogCount = m_state.getValue<bool>(GRAPH_LOG_COUNT);
    if ( validBool ){
        if ( logCount != oldLogCount ){
           m_state.setValue<bool>(GRAPH_LOG_COUNT, logCount );
           m_state.flushState();
           _generateHistogram();
        }
    }
    else {
        result = "Invalid Histogram log count must be a bool: "+ params;
    }
    result = Util::commandPostProcess( result, Util::toString( oldLogCount ));
    return result;
}

QString Histogram::_setPlaneMode( const QString& params ){
    QString result;
    std::set<QString> keys = {PLANE_MODE};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString planeModeStr = dataValues[*keys.begin()];
    QString oldPlaneMode = m_state.getValue<QString>(PLANE_MODE);
    if ( planeModeStr == PLANE_MODE_ALL || planeModeStr == PLANE_MODE_SINGLE ||
            planeModeStr == PLANE_MODE_RANGE ){
        if ( planeModeStr != oldPlaneMode ){
           m_state.setValue<QString>(PLANE_MODE, planeModeStr );
           m_state.flushState();
           _generateHistogram();
        }
    }
    else {
        result = "Unrecognized histogram cube mode params: "+ params;
    }
    result = Util::commandPostProcess( result, oldPlaneMode );
    return result;
}

QString Histogram::_setPlaneSingle( const QString& params ){
    QString result;
    std::set<QString> keys = {PLANE_SINGLE};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString planeStr = dataValues[*keys.begin()];
    bool validInt = false;
    int plane = planeStr.toInt( &validInt );
    int oldPlane = m_state.getValue<int>(PLANE_SINGLE);
    if ( validInt ){
        if ( plane != oldPlane ){
           m_state.setValue<int>(PLANE_SINGLE, plane );
           m_state.flushState();
           _generateHistogram();
        }
    }
    else {
        result = "Invalid Histogram single plane: "+ params;
    }
    result = Util::commandPostProcess( result, QString::number(oldPlane) );
    return result;
}

QString Histogram::_set2DFootPrint( const QString& params ){
    QString result;
    std::set<QString> keys = {FOOT_PRINT};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString footPrintStr = dataValues[*keys.begin()];
    QString oldFootPrint = m_state.getValue<QString>(FOOT_PRINT);
    if ( footPrintStr == FOOT_PRINT_IMAGE || footPrintStr == FOOT_PRINT_REGION ||
            footPrintStr == FOOT_PRINT_REGION_ALL ){
        if ( footPrintStr != oldFootPrint){
            m_state.setValue<QString>(FOOT_PRINT, footPrintStr );
            m_state.flushState();
            _generateHistogram();
        }
    }
    else {
        result = "Unsupported Histogram footprint: "+ params;
    }
    result = Util::commandPostProcess( result, oldFootPrint );
    return result;
}

QString Histogram::_setPlaneRange( const QString& params ){
    QString result;
    std::set<QString> keys = {PLANE_MIN, PLANE_MAX};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString planeMinStr = dataValues[PLANE_MIN];
    QString planeMaxStr = dataValues[PLANE_MAX];
    bool validRangeMin = false;
    int planeMin = planeMinStr.toInt( &validRangeMin );
    bool validRangeMax = false;
    int planeMax = planeMaxStr.toInt( &validRangeMax );
    int storedMin = m_state.getValue<int>(PLANE_MIN);
    int storedMax = m_state.getValue<int>(PLANE_MAX);
    if ( validRangeMin && validRangeMax ){
        if ( planeMin <= planeMax ){
            bool changedState = false;
            if ( planeMin != storedMin){
                m_state.setValue<int>(PLANE_MIN, planeMin );
                changedState = true;
            }

            if ( planeMax != storedMax){
                m_state.setValue<int>(PLANE_MAX, planeMax );
                changedState = true;
            }
            if ( changedState ){
                m_state.flushState();
                _generateHistogram();
            }
        }
        else {
            result = "The histogram plane range minimum must be less than the maximum: "+params;
        }
    }
    else {
        result = "Invalid Histogram plane range parameters: "+ params;
    }
    QString oldVals = QString::number(storedMin)+","+QString::number( storedMax);
    result = Util::commandPostProcess( result, oldVals );
    return result;
}

QString Histogram::_setGraphStyle( const QString& params ){
    QString result;
    std::set<QString> keys = {GRAPH_STYLE};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString styleStr = dataValues[*keys.begin()];
    QString oldStyle = m_state.getValue<QString>(GRAPH_STYLE);
    if ( styleStr == GRAPH_STYLE_LINE || styleStr == GRAPH_STYLE_OUTLINE ||
            styleStr == GRAPH_STYLE_FILL ){
        if ( styleStr != oldStyle ){
            m_state.setValue<QString>(GRAPH_STYLE, styleStr );
            m_state.flushState();
            _generateHistogram();
        }
    }
    else {
        result = "Unrecognized histogram graph style: "+ params;
    }
    result = Util::commandPostProcess( result, oldStyle );
    return result;
}


QString Histogram::_setClipToImage( const QString& params ){
    QString result;
    std::set<QString> keys = {CLIP_APPLY};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString clipApplyStr = dataValues[*keys.begin()];
    bool validBool = false;
    bool clipApply = Util::toBool( clipApplyStr, &validBool );
    bool oldClipApply = m_state.getValue<bool>(CLIP_APPLY);
    if ( validBool ){
       if ( clipApply != oldClipApply ){
          m_state.setValue<bool>(CLIP_APPLY, clipApply );
          m_state.flushState();
          _generateHistogram();
       }
    }
    else {
       result = "Invalid apply clip to image; must be true/false: " + clipApplyStr;
    }
    result = Util::commandPostProcess( result, Util::toString( oldClipApply ));
    return result;
}


std::vector<std::shared_ptr<Image::ImageInterface>> Histogram::_generateData(){
    std::vector<std::shared_ptr<Image::ImageInterface>> result;
    for( std::shared_ptr<Controller> controller : m_linkImpl->m_controllers ){
        std::vector<std::shared_ptr<Image::ImageInterface>> images = controller->getDataSources();
        int imageCount = images.size();
        for( int j = 0; j < imageCount; j++){
            result.push_back(images[j]);
        }
    }
    return result;
}

void Histogram::_generateHistogram(){

    int binCount = m_state.getValue<int>(BIN_COUNT);
    int minChannel = m_state.getValue<int>(PLANE_MIN);
    int maxChannel = m_state.getValue<int>(PLANE_MAX);
    int spectralIndex = m_state.getValue<int>(PLANE_SINGLE);
    double minIntensity = m_state.getValue<double>(CLIP_MIN);
    double maxIntensity = m_state.getValue<double>(CLIP_MAX);
    QString style = m_state.getValue<QString>(GRAPH_STYLE);
    bool logCount = m_state.getValue<bool>(GRAPH_LOG_COUNT);
    // bool colored = m_state.getValue<bool>(GRAPH_COLORED);

    std::vector<std::shared_ptr<Image::ImageInterface>> dataSource = _generateData();

    if(dataSource.size()>=1){
        auto result = Globals::instance()-> pluginManager()
                          -> prepare <Carta::Lib::Hooks::HistogramHook>(dataSource, binCount,
                            minChannel, maxChannel, spectralIndex, minIntensity, maxIntensity);
        auto lam = [=] ( const Carta::Lib::HistogramResult &data ) {

                HistogramGenerator * histogram = new HistogramGenerator();
                histogram->setStyle(style);
                histogram->setLogScale(logCount); 
                histogram->setData(data);
                QImage * histogramImage = histogram->toImage();
                m_view->resetImage(*histogramImage);
                refreshView(m_view.get());
            };
        try {
            result.forEach( lam );
        }
        catch( char*& error ){
            QString errorStr( error );
            ErrorManager* hr = dynamic_cast<ErrorManager*>(Util::findSingletonObject( ErrorManager::CLASS_NAME ));
            hr->registerError( errorStr );
        }

    }

    //Test code for percentage<->intensity conversions
    /*double per1 = _getPercentile( "/scratch/Images/Orion.methanol.cbc.contsub.image.fits", 0, 0 );
    qDebug() << "per 0 = "<<per1;
    double per2 = _getPercentile( "/scratch/Images/Orion.methanol.cbc.contsub.image.fits", 0, .2 );
    qDebug() << "per .2 = "<<per2;
    double per3 = _getPercentile( "/scratch/Images/Orion.methanol.cbc.contsub.image.fits", 0, .4 );
    qDebug() << "per .4 = "<<per3;
    double per4 = _getPercentile( "/scratch/Images/Orion.methanol.cbc.contsub.image.fits", 0, .6 );
    qDebug() << "per .6 = "<<per4;
    
    double intensity = 0;
    bool intensityFound = _getIntensity( "/scratch/Images/Orion.methanol.cbc.contsub.image.fits", 0, .5, &intensity );
    if ( intensityFound ){
        qDebug() << "Intensity at 50%="<<intensity;
    }
    intensityFound = _getIntensity( "/scratch/Images/Orion.methanol.cbc.contsub.image.fits", 0, .75, &intensity );
    if ( intensityFound ){
        qDebug() << "Intensity at 75%="<<intensity;
    }
    intensityFound = _getIntensity( "/scratch/Images/Orion.methanol.cbc.contsub.image.fits", 0, .9, &intensity );
    if ( intensityFound ){
        qDebug() << "Intensity at 90%="<<intensity;
    }*/
}

bool Histogram::removeLink( const std::shared_ptr<Controller> & controller){
    bool removed = m_linkImpl->removeLink( controller );
    if ( removed ){
        disconnect(controller.get(), SIGNAL(dataChanged()), this , SLOT(_generateHistogram()));
    }
    return removed;
}

Histogram::~Histogram(){

}
}
}
