#include "Data/Histogram.h"
#include "Data/Clips.h"
#include "Data/Colormap.h"
#include "Data/LinkableImpl.h"
#include "Data/Controller.h"
#include "Data/Util.h"
#include "Histogram/HistogramGenerator.h"
#include "Globals.h"
#include "ImageView.h"
#include "PluginManager.h"
#include "ErrorManager.h"
#include "CartaLib/Hooks/Histogram.h"
#include "CartaLib/PixelPipeline/IPixelPipeline.h"
#include "Util.h"
#include <set>
#include <QtCore/qmath.h>
#include <QDebug>

namespace Carta {

namespace Data {

const QString Histogram::CLASS_NAME = "Histogram";

const QString Histogram::CLIP_INDEX = "clipIndex";
const QString Histogram::CLIP_MIN = "clipMin";
const QString Histogram::CLIP_MAX = "clipMax";
const QString Histogram::CLIP_APPLY = "applyClipToImage";
const QString Histogram::BIN_COUNT = "binCount";
const QString Histogram::BIN_WIDTH = "binWidth";
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
const QString Histogram::X_COORDINATE = "x";
const QString Histogram::POINTER_MOVE = "pointer-move";

const double Histogram::CLIP_ERROR_MARGIN = 0.000001;

Clips*  Histogram::m_clips = nullptr;

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
    connect( m_view.get(), SIGNAL(resize(const QSize&)), this, SLOT(_updateSize(const QSize&)));
    registerView(m_view.get());
    m_selectionEnabled = false;
    m_selectionEnded = false;
    m_histogram = new Carta::Histogram::HistogramGenerator();

    //Load the available clips.
    if ( m_clips == nullptr ){
        CartaObject* obj = Util::findSingletonObject( Clips::CLASS_NAME );
        m_clips = dynamic_cast<Clips*>(obj);
    }

}





bool Histogram::addLink( CartaObject*  target){
    Controller* controller = dynamic_cast<Controller*>(target);
    bool linkAdded = false;
    if ( controller != nullptr ){
        linkAdded = m_linkImpl->addLink( controller );
        if ( linkAdded ){
            connect(controller, SIGNAL(dataChanged(Controller*)), this , SLOT(_createHistogram(Controller*)));
        }
    }
    else {
        Colormap* map = dynamic_cast<Colormap*>(target);
        if ( map != nullptr ){
            linkAdded = m_linkImpl->addLink( map );
            if ( linkAdded ){
                connect( map, SIGNAL(colorMapChanged( Colormap*)), this, SLOT( _updateColorMap( Colormap*)));
                _updateColorMap( map );
            }
        }
        else {
            qWarning() << "Histogram:  unsupported link type.";
        }
    }
    return linkAdded;
}



void Histogram::clear(){
    unregisterView();
    m_linkImpl->clear();
}

void Histogram::_createHistogram( Controller* controller){
    double minIntensity = 0;
    double maxIntensity = 0;

    QString filename = controller->getImageName(0);
    bool minValid = _getIntensity( filename, 0, 0, &minIntensity );
    bool maxValid = _getIntensity( filename, 0, 1, &maxIntensity );

    if(minValid && maxValid){
        m_state.setValue<double>(CLIP_MIN, minIntensity);
        m_state.setValue<double>(CLIP_MAX, maxIntensity);
        m_state.flushState();
    }
    _generateHistogram( true, controller );
}


void Histogram::_endSelection(const QString& params ){
    std::set<QString> keys = {X_COORDINATE};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString xstr = dataValues[X_COORDINATE];
    m_selectionEnd = xstr.toDouble();
    m_selectionEnded = true;
    m_histogram->setSelectionMode( false );
    _generateHistogram( false );
    m_selectionEnabled = false;


}

NdArray::RawViewInterface* Histogram::_findRawData( const QString& fileName, int frameIndex ) const {
    NdArray::RawViewInterface * rawData = nullptr;
    int linkCount = m_linkImpl->getLinkCount();
    for( int i = 0; i < linkCount; i++ ){
        Controller* controller = dynamic_cast<Controller*>( m_linkImpl->getLink(i));
        if ( controller != nullptr ){
            rawData = controller->getRawData( fileName, frameIndex);
            if ( rawData != nullptr ){
                break;
            }
        }
    }
    return rawData;
}

std::vector<std::shared_ptr<Image::ImageInterface>> Histogram::_generateData(Controller* controller){
    std::vector<std::shared_ptr<Image::ImageInterface>> result;
    if ( controller != nullptr ){
        result = controller->getDataSources();
    }
    return result;
}

void Histogram::_generateHistogram( bool newDataNeeded, Controller* controller ){
    if ( newDataNeeded ){
        _loadData( controller );
    }
    //Refresh the view
    //User is not selecting a range
    if ( !m_selectionEnabled ){
        QString style = m_state.getValue<QString>(GRAPH_STYLE);
        bool logCount = m_state.getValue<bool>(GRAPH_LOG_COUNT);
        bool colored = m_state.getValue<bool>(GRAPH_COLORED);
        m_histogram->setStyle(style);
        m_histogram->setLogScale(logCount);
        m_histogram->setColored( colored );
    }
    //User is selecting a range.
    else{
        if ( m_selectionEnded ){
            double minIntensity = m_state.getValue<double>(CLIP_MIN);
            double maxIntensity = m_state.getValue<double>(CLIP_MAX);
            m_histogram->setHistogramSelection(minIntensity, maxIntensity);
        }
        else {
            m_histogram->setHistogramSelection( m_selectionStart, m_selectionEnd );
        }
    }
    QImage * histogramImage = m_histogram->toImage();
    m_view->resetImage( *histogramImage );
    m_view->scheduleRedraw();
}


double Histogram::_getPercentile( const QString& fileName, int frameIndex, double intensity ) const {
    double percentile = 0;
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

        if ( totalCount > 0 ){
            percentile = double(countBelow) / totalCount;
        }
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

int Histogram::_getLinkInfo( const QString& link, QString& name ) const {
    Controller* controller = dynamic_cast<Controller*>(m_linkImpl->searchLinks(link));
    int index = -1;
    if ( controller != nullptr ){
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
    const int DEFAULT_BIN_COUNT = 25;
    m_state.insertValue<int>(BIN_COUNT, DEFAULT_BIN_COUNT );
    double binWidth = _toBinWidth(DEFAULT_BIN_COUNT);
    m_state.insertValue<double>(BIN_WIDTH, binWidth );
    m_state.insertValue<QString>(GRAPH_STYLE, GRAPH_STYLE_LINE);
    m_state.insertValue<bool>(GRAPH_LOG_COUNT, true );
    m_state.insertValue<bool>(GRAPH_COLORED, true );
    m_state.insertValue<QString>(PLANE_MODE, PLANE_MODE_SINGLE );
    m_state.insertValue<int>(PLANE_SINGLE, 0 );
    m_state.insertValue<int>(PLANE_MIN, 0 );
    m_state.insertValue<int>(PLANE_MAX, 1 );
    m_state.insertValue<QString>(FOOT_PRINT, FOOT_PRINT_IMAGE );
    m_state.insertValue<double>(CLIP_MIN_PERCENT, 0);
    m_state.insertValue<double>(CLIP_MAX_PERCENT, 1);

    m_state.flushState();

    m_stateMouse.insertObject( ImageView::MOUSE );
    m_stateMouse.insertValue<QString>(ImageView::MOUSE_X, 0 );
    m_stateMouse.insertValue<QString>(ImageView::MOUSE_Y, 0 );
    m_stateMouse.insertValue<QString>(POINTER_MOVE, "");
    m_stateMouse.flushState();
}

void Histogram::_initializeCallbacks(){
    addCommandCallback( "mouseDown", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result = params;
        _startSelection(params);
        return result;
    });

    addCommandCallback( "mouseUp", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result = params;
        return result;
    });

    addCommandCallback( "setBinCount", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {BIN_COUNT};
        std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
        QString binCountStr = dataValues[*keys.begin()];
        bool validInt = false;
        int binCount = binCountStr.toInt( &validInt );
        if ( validInt ){
            result = setBinCount( binCount );
        }
        else {
            result = "Invalid bin count: "+params+" must be a valid integer";
            Util::commandPostProcess( result );
        }
        return result;
    });

    addCommandCallback( "setBinWidth", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
            QString result;
            std::set<QString> keys = {BIN_WIDTH};
            std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
            QString binWidthStr = dataValues[*keys.begin()];
            bool validDouble = false;
            double binWidth = binWidthStr.toDouble( &validDouble );
            if ( validDouble ){
                result = setBinWidth( binWidth );
            }
            else {
                result = "Invalid bin width: " + params+" must be a valid number";
                Util::commandPostProcess( result );
            }
            return result;
        });

    addCommandCallback( "setClipPercent", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result = _setClipPercent( params );
        return result;
    });

    addCommandCallback( "setClipMax", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result = _setClipMax( params );
        return result;
    });

    // addCommandCallback( "setClipMaxPercent", [=] (const QString & /*cmd*/,
    //                 const QString & params, const QString & /*sessionId*/) -> QString {
    //           qDebug() << "maxPercent params: " << params;
    //         QString result = _setClipMaxPercent( params );
    //         return result;
    //     });

    addCommandCallback( "setClipMin", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result = _setClipMin( params );
        return result;

    });

    // addCommandCallback( "setClipMinPercent", [=] (const QString & /*cmd*/,
    //                 const QString & params, const QString & /*sessionId*/) -> QString {
    //           qDebug() << "minPercent params: " << params;
    //         QString result =_setClipMinPercent( params );
    //         return result;
    //     });

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

    QString pointerPath= getPath() + StateInterface::DELIMITER + ImageView::VIEW +
            StateInterface::DELIMITER + POINTER_MOVE;
    addStateCallback( pointerPath, [=] ( const QString& /*path*/, const QString& value ) {
        QStringList mouseList = value.split( " ");
        if ( mouseList.size() == 2 ){
            bool validX = false;
            int mouseX = mouseList[0].toInt( &validX );
            if ( validX ){
                _updateSelection( mouseX);
            }
        }

    });
}

void Histogram::_loadData( Controller* controller ){
    int binCount = m_state.getValue<int>(BIN_COUNT);
    int minChannel = m_state.getValue<int>(PLANE_MIN);
    int maxChannel = m_state.getValue<int>(PLANE_MAX);
    int spectralIndex = m_state.getValue<int>(PLANE_SINGLE);
    double minIntensity = m_state.getValue<double>(CLIP_MIN);
    double maxIntensity = m_state.getValue<double>(CLIP_MAX);
    std::vector<std::shared_ptr<Image::ImageInterface>> dataSources;
    if ( controller != nullptr ){
        dataSources = _generateData( controller );
    }
    auto result = Globals::instance()-> pluginManager()
                              -> prepare <Carta::Lib::Hooks::HistogramHook>(dataSources, binCount,
                                      minChannel, maxChannel, spectralIndex, minIntensity, maxIntensity);
    auto lam = [=] ( const Carta::Lib::Hooks::HistogramResult &data ) {
        m_histogram->setData(data, minIntensity, maxIntensity);
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

bool Histogram::removeLink( CartaObject* cartaObject){
    bool removed = false;
    Controller* controller = dynamic_cast<Controller*>( cartaObject );
    if ( controller != nullptr ){
        removed = m_linkImpl->removeLink( controller );
        if ( removed ){
            disconnect(controller);
        }
    }
    else {
        Colormap* map = dynamic_cast<Colormap*>(cartaObject);
        if ( map != nullptr ){
            removed = m_linkImpl->removeLink( map );
            if ( removed ){
                disconnect( map );
            }
        }
        else {
            qWarning() << "Histogram: unrecognized link to remove.";
        }
    }
    return removed;
}

QString Histogram::_setClipMax( const QString& params ){
    QString result;
    std::set<QString> keys = {CLIP_MAX, LINK};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString clipMaxStr = dataValues[CLIP_MAX];
    QString links = dataValues[LINK];
    bool validRangeMax = false;
    double clipMax = clipMaxStr.toDouble( &validRangeMax );
    double oldMax = m_state.getValue<double>(CLIP_MAX);
    double clipMin = m_state.getValue<double>(CLIP_MIN);
    double oldMaxPercent = m_state.getValue<double>(CLIP_MAX_PERCENT);

    if( validRangeMax ){
        if ( qAbs(clipMax - oldMax) > CLIP_ERROR_MARGIN){
            m_state.setValue<double>(CLIP_MAX, clipMax );

            QString filename;
            int index = _getLinkInfo(links, filename);
            if(index >=0 ){
                double clipMaxPercent = _getPercentile(filename, 0, clipMax);
                if(qAbs(oldMaxPercent - clipMaxPercent) > CLIP_ERROR_MARGIN){
                    m_state.setValue<double>(CLIP_MAX_PERCENT, clipMaxPercent);
                }
            }
            m_state.flushState();
            if(clipMin<clipMax){
                _generateHistogram( true );
            }
        }
    }
    else {
        result = "Invalid Histogram clip min parameters: "+ params;
    }
    Util::commandPostProcess( result );
    return result;
}

// QString Histogram::_setClipMaxPercent( const QString& params ){
//     QString result;
//     std::set<QString> keys = {CLIP_MAX_PERCENT, LINK};
//     std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
//     QString clipMaxPercentStr = dataValues[CLIP_MAX_PERCENT];
//     QString links = dataValues[LINK];
//     bool validRangeMax = false;
//     double clipMaxPercent = clipMaxPercentStr.toDouble( &validRangeMax );
//     double oldMaxPercent = m_state.getValue<double>(CLIP_MAX_PERCENT);
//     double clipMin = m_state.getValue<double>(CLIP_MIN);
//     double oldClipMax = m_state.getValue<double>(CLIP_MAX);
//     double clipMax = 0;

//     if( validRangeMax ){
//         if ( qAbs(clipMaxPercent - oldMaxPercent) > CLIP_ERROR_MARGIN){
//              qDebug()<<"I did actually change clipmaxpercent state";
//             m_state.setValue<double>(CLIP_MAX_PERCENT, clipMaxPercent );

//             QString filename;
//             int index = _getLinkInfo(links, filename);
//             if(index >=0 ){
//                 bool validIntensity = _getIntensity(filename, index, clipMaxPercent, &clipMax);
//                 if(validIntensity){
//                     if(qAbs(oldClipMax - clipMax) > CLIP_ERROR_MARGIN)
//                         m_state.setValue<double>(CLIP_MAX, clipMax);
//                 }

//             }
//             m_state.flushState();
//             if(clipMin<clipMax)
//                 _generateHistogram();
//         }
//     }
//     else {
//         result = "Invalid Histogram clip min  percent parameters: "+ params;
//     }
//     QString origValues = QString::number(oldMaxPercent);
//     result = Util::commandPostProcess( result, origValues );
//     return result;
// }

QString Histogram::_setClipMin( const QString& params ){
    QString result;
    std::set<QString> keys = {CLIP_MIN, LINK};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString clipMinStr = dataValues[CLIP_MIN];
    QString links = dataValues[LINK];
    bool validRangeMin = false;
    double clipMin = clipMinStr.toDouble( &validRangeMin );
    double oldMin = m_state.getValue<double>(CLIP_MIN);
    double clipMax = m_state.getValue<double>(CLIP_MAX);
    double oldMinPercent = m_state.getValue<double>(CLIP_MIN_PERCENT);

    if( validRangeMin ){
        if ( qAbs(clipMin - oldMin) > CLIP_ERROR_MARGIN){
            m_state.setValue<double>(CLIP_MIN, clipMin );

            QString filename;
            int index = _getLinkInfo(links, filename);
            if(index >=0 ){
                double clipMinPercent = _getPercentile(filename, 0, clipMin);
                if(qAbs(oldMinPercent - clipMinPercent) > CLIP_ERROR_MARGIN)
                    m_state.setValue<double>(CLIP_MIN_PERCENT, clipMinPercent);
            }
            m_state.flushState();
            if(clipMin<clipMax){
                _generateHistogram( true );
            }
        }
    }
    else {
        result = "Invalid Histogram clip min parameters: "+ params;

    }
    Util::commandPostProcess( result);
    return result;

}


// QString Histogram::_setClipMinPercent( const QString& params ){
//     QString result;
//     std::set<QString> keys = {CLIP_MIN_PERCENT, LINK};
//     std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
//     QString clipMinPercentStr = dataValues[CLIP_MIN_PERCENT];
//     QString links = dataValues[LINK];
//     bool validRangeMin = false;
//     double clipMinPercent = clipMinPercentStr.toDouble( &validRangeMin );
//     double oldMinPercent = m_state.getValue<double>(CLIP_MIN_PERCENT);
//     double clipMax = m_state.getValue<double>(CLIP_MAX);
//     double oldClipMin = m_state.getValue<double>(CLIP_MIN);
//     double clipMin = 0;

//     if( validRangeMin ){
//         if ( qAbs(clipMinPercent - oldMinPercent) > CLIP_ERROR_MARGIN){
//              qDebug()<<"I did actually change clipmaxpercent state";
//             m_state.setValue<double>(CLIP_MIN_PERCENT, clipMinPercent );

//             QString filename;
//             int index = _getLinkInfo(links, filename);
//             if(index >=0 ){
//                 bool validIntensity = _getIntensity(filename, index, clipMinPercent, &clipMin);
//                 if(validIntensity){
//                     if(qAbs(oldClipMin - clipMin) > CLIP_ERROR_MARGIN)
//                         m_state.setValue<double>(CLIP_MIN, clipMin);
//                 }

//             }
//             m_state.flushState();
//             if(clipMin<clipMax)
//                 _generateHistogram();
//         }
//     }
//     else {
//         result = "Invalid Histogram clip min  percent parameters: "+ params;
//     }
//     QString origValues = QString::number(oldMinPercent);
//     result = Util::commandPostProcess( result, origValues );
//     return result;
// }

QString Histogram::_setClipPercent( const QString& params ){

    QString result;
    std::set<QString> keys = {CLIP_MIN_PERCENT, CLIP_MAX_PERCENT, LINK};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString clipMinPercentStr = dataValues[CLIP_MIN_PERCENT];
    QString clipMaxPercentStr = dataValues[CLIP_MAX_PERCENT];
    QString links = dataValues[LINK];
    bool validRangePercentMin = false;
    bool validRangePercentMax = false;
    double clipMinPercent = clipMinPercentStr.toDouble( &validRangePercentMin );
    double clipMaxPercent = clipMaxPercentStr.toDouble( &validRangePercentMax );
    double oldClipMinPercent = m_state.getValue<double>(CLIP_MIN_PERCENT);
    double oldClipMaxPercent = m_state.getValue<double>(CLIP_MAX_PERCENT);
    double oldClipMin = m_state.getValue<double>(CLIP_MIN);
    double oldClipMax = m_state.getValue<double>(CLIP_MAX);


    if (validRangePercentMin && validRangePercentMax){
        bool changedState = false;
        bool changedMinPercent = false;
        bool changedMaxPercent = false;

        if(qAbs(oldClipMinPercent-  clipMinPercent) > CLIP_ERROR_MARGIN){
            m_state.setValue<double>(CLIP_MIN_PERCENT, clipMinPercent );
            changedState = true;
            changedMinPercent = true;
        }

        if( qAbs(oldClipMaxPercent -clipMaxPercent) > CLIP_ERROR_MARGIN ){
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
                    if(minIntensity && qAbs(clipMin -oldClipMin)>CLIP_ERROR_MARGIN){
                        m_state.setValue<double>(CLIP_MIN, clipMin);
                    }
                }
                if(changedMaxPercent){
                    bool maxIntensity = _getIntensity(filename, index, clipMaxPercent, &clipMax);
                    if(maxIntensity && qAbs(clipMax - oldClipMax) > CLIP_ERROR_MARGIN ){
                        m_state.setValue<double>(CLIP_MAX, clipMax);

                    }
                }
            }
            m_state.flushState();
            if(clipMin < clipMax){
                _generateHistogram( true );
            }
            else {
                result = "Invalid Histogram clip range parameters: "+ params + 
                        "clip min range has to be less than clip max range";

            }
        }
    }
    Util::commandPostProcess( result);
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
            _generateHistogram( false );
        }
    }
    else {
        result = "Invalid Histogram colored parameter; must be a bool: "+ params;
    }
    Util::commandPostProcess( result );
    return result;
}

QString Histogram::setBinWidth( double binWidth ){
    QString result;
    double oldBinWidth = m_state.getValue<double>(BIN_WIDTH);
    if ( binWidth > 0 ){
        const double ERROR_MARGIN = 0.00001;
        if ( qAbs( oldBinWidth - binWidth) > ERROR_MARGIN ){
            m_state.setValue<double>( BIN_WIDTH, binWidth );
            int binCount = _toBinCount( binWidth );
            m_state.setValue<int>(BIN_COUNT, binCount );
            m_state.flushState();
            _generateHistogram( true );
        }

    }
    else {
        if ( binWidth < 0 ){
            result = "Bin width must be positive: "+QString::number(binWidth);
        }
    }
    Util::commandPostProcess( result);
    return result;
}

QString Histogram::setBinCount( int binCount ){
    QString result;
    int oldBinCount = m_state.getValue<int>(BIN_COUNT );
    if ( binCount > 0  ){
        if ( binCount != oldBinCount ){
            m_state.setValue<int>(BIN_COUNT, binCount );
            double binWidth = _toBinWidth( binCount );
            m_state.setValue<double>(BIN_WIDTH, binWidth );
            m_state.flushState();
            _generateHistogram( true );

        }
    }
    else {
        result = "Invalid Histogram bin count: "+QString::number( binCount);
    }
    Util::commandPostProcess( result );
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
            _generateHistogram( false );
        }
    }
    else {
        result = "Invalid Histogram log count must be a bool: "+ params;
    }
    Util::commandPostProcess( result );
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
            _generateHistogram( true );
        }
    }
    else {
        result = "Unrecognized histogram cube mode params: "+ params;
    }
    Util::commandPostProcess( result );
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
            _generateHistogram( true );
        }
    }
    else {
        result = "Invalid Histogram single plane: "+ params;
    }
    Util::commandPostProcess( result );
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
            _generateHistogram( true );
        }
    }
    else {
        result = "Unsupported Histogram footprint: "+ params;
    }
    Util::commandPostProcess( result );
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
                _generateHistogram( true );
            }
        }
        else {
            result = "The histogram plane range minimum must be less than the maximum: "+params;
        }
    }
    else {
        result = "Invalid Histogram plane range parameters: "+ params;
    }
    Util::commandPostProcess( result );
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
            _generateHistogram( false );
        }
    }
    else {
        result = "Unrecognized histogram graph style: "+ params;
    }
    Util::commandPostProcess( result );
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
            _generateHistogram( false );
        }
    }
    else {
        result = "Invalid apply clip to image; must be true/false: " + clipApplyStr;
    }
    Util::commandPostProcess( result);
    return result;
}


void Histogram::_startSelection(const QString& params ){
    std::set<QString> keys = {X_COORDINATE};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString xstr = dataValues[X_COORDINATE];
    // bool validRangePercentMax = false;
    // double clipMinPercent = clipMinPercentStr.toDouble( &validRangePercentMin );
    m_selectionEnabled = true;
    m_selectionEnded = false;
    m_selectionStart = xstr.toDouble();
    m_histogram->setSelectionMode( true );
}

double Histogram::_toBinWidth( int count ) const {
    double minRange = m_state.getValue<double>( CLIP_MIN);
    double maxRange = m_state.getValue<double>( CLIP_MAX);
    double width = qAbs( maxRange - minRange) / count;
    return width;
}

int Histogram::_toBinCount( double width ) const {
    double minRange = m_state.getValue<double>(CLIP_MIN );
    double maxRange = m_state.getValue<double>(CLIP_MAX);
    int count = qFloor( qAbs( maxRange - minRange) ) / width;
    return count;
}

// void Histogram::_updateSelection(const QString& params ){
void Histogram::_updateSelection(int x){
    // std::set<QString> keys = {X_COORDINATE};
    // std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    // QString xstr = dataValues[X_COORDINATE];
    m_selectionEnd = x;
    m_selectionEnabled = true;

    _generateHistogram( false );
}


void Histogram::_updateColorMap( Colormap* map ){
    if ( map != nullptr ){
        std::shared_ptr<Carta::Lib::PixelPipeline::IColormapNamed> cMap= map->getColorMap();
        m_histogram->setColorMap( cMap );
    }
    _generateHistogram( false );
}

void Histogram::_updateSize( const QSize& size ){
    m_histogram->setSize( size.width(), size.height());
    _generateHistogram( false );
}

QList<QString> Histogram::getLinks() const {
    return m_linkImpl->getLinkIds();
}

Histogram::~Histogram(){
    delete m_histogram;
}
}
}
