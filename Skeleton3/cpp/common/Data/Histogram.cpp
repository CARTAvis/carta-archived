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
const QString Histogram::CLIP_BUFFER = "useClipBuffer";
const QString Histogram::CLIP_BUFFER_SIZE = "clipBuffer";
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
const QString Histogram::PLANE_MODE="planeMode";
const QString Histogram::PLANE_MODE_SINGLE="Current";
const QString Histogram::PLANE_MODE_RANGE="Range";
const QString Histogram::PLANE_MODE_ALL="All";
const QString Histogram::PLANE_MIN = "planeMin";
const QString Histogram::PLANE_MAX = "planeMax";
const QString Histogram::PLANE_RANGE_UPPER_BOUND = "planeRangeMax";
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
    m_cubeChannel = 0;
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
    std::pair<int,int> frameBounds = _getFrameBounds();
    bool minValid = controller->getIntensity( frameBounds.first, frameBounds.second, 0, &minIntensity );
    bool maxValid = controller->getIntensity( frameBounds.first, frameBounds.second, 1, &maxIntensity );

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
    m_histogram->setSelectionMode( false );
    m_selectionEnabled = false;
    _zoomToSelection();
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
        m_histogram->setRangePixels( m_selectionStart, m_selectionEnd );
    }

    //Refresh the view
    _refreshView();
}

void Histogram::_refreshView(){
    QImage * histogramImage = m_histogram->toImage();
    m_view->resetImage( *histogramImage );
    m_view->scheduleRedraw();
}


void Histogram::_applyClips() const {
    if ( m_state.getValue<bool>(CLIP_APPLY) ){
        //Get percentiles and normalize to [0,1].
        double clipMinPercent = m_state.getValue<double>( CLIP_MIN_PERCENT );
        double clipMaxPercent = m_state.getValue<double>(CLIP_MAX_PERCENT);
        if ( clipMinPercent > 0 || clipMaxPercent > 0 ){
            double minPercentile =  clipMinPercent / 100.0;
            double maxPercentile = (100 - clipMaxPercent ) / 100.0;
            int linkCount = m_linkImpl->getLinkCount();
            for ( int i = 0; i < linkCount; i++ ){
                CartaObject* obj = m_linkImpl->getLink( i );
                Controller* controller = dynamic_cast<Controller*>(obj);
                if ( controller != nullptr ){
                    controller->applyClips( minPercentile, maxPercentile );
                }
            }
        }
    }
}

void Histogram::_finishClips (){
    m_state.flushState();
    _applyClips();
    _generateHistogram( true );
}

Controller* Histogram::_getControllerSelected() const {
    //TODO: Make more sophisticated.  Right now we just find the first linked
    //controller.   Eventually we want to do this based on user selection.
    Controller* controller = nullptr;
    int linkCount = m_linkImpl->getLinkCount();
    for ( int i = 0; i < linkCount; i++ ){
        CartaObject* obj = m_linkImpl->getLink(i );
        Controller* control = dynamic_cast<Controller*>( obj);
        if ( control != nullptr){
            controller = control;
            break;
        }
    }
    return controller;
}

std::pair<int,int> Histogram::_getFrameBounds() const {
    int minChannel = -1;
    int maxChannel = -1;
    QString planeMode = m_state.getValue<QString>(PLANE_MODE );
    if ( planeMode == PLANE_MODE_SINGLE ){
        minChannel = m_cubeChannel;
        maxChannel = m_cubeChannel;
    }
    else if ( planeMode == PLANE_MODE_RANGE ){
        minChannel = m_state.getValue<int>(PLANE_MIN );
        maxChannel = m_state.getValue<int>(PLANE_MAX);
    }
    std::pair<int,int> bounds( minChannel, maxChannel);
    return bounds;
}

void Histogram::_initializeDefaultState(){
    m_state.insertValue<int>(CLIP_INDEX, 0 );
    m_state.insertValue<double>( CLIP_MIN, 0 );
    m_state.insertValue<double>(CLIP_MAX, 1);
    m_state.insertValue<bool>(CLIP_APPLY, false );
    m_state.insertValue<bool>(CLIP_BUFFER, false);
    m_state.insertValue<int>(CLIP_BUFFER_SIZE, 10 );
    const int DEFAULT_BIN_COUNT = 25;
    m_state.insertValue<int>(BIN_COUNT, DEFAULT_BIN_COUNT );
    double binWidth = _toBinWidth(DEFAULT_BIN_COUNT);
    m_state.insertValue<double>(BIN_WIDTH, binWidth );
    m_state.insertValue<QString>(GRAPH_STYLE, GRAPH_STYLE_LINE);
    m_state.insertValue<bool>(GRAPH_LOG_COUNT, true );
    m_state.insertValue<bool>(GRAPH_COLORED, false );
    m_state.insertValue<QString>(PLANE_MODE, PLANE_MODE_SINGLE );
    m_state.insertValue<int>(PLANE_MIN, 0 );
    m_state.insertValue<int>(PLANE_MAX, 1 );
    m_state.insertValue<int>(PLANE_RANGE_UPPER_BOUND, 1000000000);
    m_state.insertValue<QString>(FOOT_PRINT, FOOT_PRINT_IMAGE );
    m_state.insertValue<double>(CLIP_MIN_PERCENT, 0);
    m_state.insertValue<double>(CLIP_MAX_PERCENT, 0);
    m_state.insertValue<bool>(Util::STATE_FLUSH, false );

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
        _startSelection(params);
        return "";
    });

    addCommandCallback( "mouseUp", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        _endSelection( params );
        return "";
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
        }
        Util::commandPostProcess( result );
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
            }
            Util::commandPostProcess( result );
            return result;
        });

    addCommandCallback( "setClipBuffer", [=] (const QString & /*cmd*/,
                   const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {CLIP_BUFFER_SIZE};
        std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
        QString clipBufferStr = dataValues[*keys.begin()];
        bool validInt = false;
        double clipBuffer = clipBufferStr.toInt( &validInt );
        if ( validInt ){
            result = setClipBuffer( clipBuffer );
        }
        else {
            result = "Invalid clip buffer size: " + params+" must be a valid integer.";
        }
        Util::commandPostProcess( result );
        return result;
    });



    addCommandCallback( "setClipMax", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {CLIP_MAX};
        std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
        QString clipMaxStr = dataValues[CLIP_MAX];
        bool validRangeMax = false;
        double clipMax = clipMaxStr.toDouble( &validRangeMax );
        if ( validRangeMax ){
            result = setClipMax( clipMax );
        }
        else {
            result = "Invalid clip maximum: " + params+" must be a valid number.";
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setClipMaxPercent", [=] (const QString & /*cmd*/,
                     const QString & params, const QString & /*sessionId*/) -> QString {
       QString result;
       std::set<QString> keys = {CLIP_MAX_PERCENT};
       std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
       QString clipMaxPercentStr = dataValues[CLIP_MAX_PERCENT];
       bool validRangeMax = false;
       double clipMaxPercent = clipMaxPercentStr.toDouble( &validRangeMax );
       if ( validRangeMax ){
           result = setClipMaxPercent( clipMaxPercent );
       }
       else {
           result = "Invalid clip maximum percentile: " + params+", must be a valid number.";
       }
       Util::commandPostProcess( result );
       return result;
    });

    addCommandCallback( "setClipMin", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {CLIP_MIN};
        std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
        QString clipMinStr = dataValues[CLIP_MIN];
        bool validRangeMin = false;
        double clipMin = clipMinStr.toDouble( &validRangeMin );
        if ( validRangeMin ){
            result = setClipMin( clipMin);
        }
        else {
            result = "Invalid clip minimum: " + params+" must be a valid number.";
        }
        Util::commandPostProcess( result );
        return result;

    });

    addCommandCallback( "setClipMinPercent", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {CLIP_MIN_PERCENT};
        std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
        QString clipMinPercentStr = dataValues[CLIP_MIN_PERCENT];
        bool validRangeMin = false;
        double clipMinPercent = clipMinPercentStr.toDouble( &validRangeMin );
        if ( validRangeMin ){
            result = setClipMinPercent( clipMinPercent);
        }
        else {
            result = "Invalid clip minimum percentile: " + params+", must be a valid number.";
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setClipToImage", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {CLIP_APPLY};
        std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
        QString clipApplyStr = dataValues[*keys.begin()];
        bool validBool = false;
        bool clipApply = Util::toBool( clipApplyStr, &validBool );
        if ( validBool ){
            result = setClipToImage( clipApply );
        }
        else {
            result = "Invalid clip to image: "+params+", must be true/false.";
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setGraphStyle", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {GRAPH_STYLE};
        std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
        QString styleStr = dataValues[*keys.begin()];
        QString result = setGraphStyle( styleStr );
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setLogCount", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {GRAPH_LOG_COUNT};
        std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
        QString logCountStr = dataValues[*keys.begin()];
        bool validBool = false;
        bool logCount = Util::toBool( logCountStr, &validBool );
        if ( validBool ){
            result = setLogCount( logCount );
        }
        else {
            result = "Displaying a log of histogram counts must be true/false:"+params;
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setColored", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {GRAPH_COLORED};
        std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
        QString coloredStr = dataValues[*keys.begin()];
        bool validBool = false;
        bool colored = Util::toBool( coloredStr, &validBool );
        if ( validBool ){
            result = setColored( colored );
        }
        else {
            result = "The colored status of the histogram must be true/false: "+params;
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setPlaneMode", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {PLANE_MODE};
        std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
        QString planeModeStr = dataValues[*keys.begin()];
        result = setPlaneMode( planeModeStr );
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setPlaneRange", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {PLANE_MIN, PLANE_MAX};
        std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
        QString planeMinStr = dataValues[PLANE_MIN];
        QString planeMaxStr = dataValues[PLANE_MAX];
        bool validRangeMin = false;
        int planeMin = planeMinStr.toInt( &validRangeMin );
        bool validRangeMax = false;
        int planeMax = planeMaxStr.toInt( &validRangeMax );
        if ( validRangeMin && validRangeMax ){
            result = setPlaneRange( planeMin, planeMax );
        }
        else {
            result = "Plane range minimum and maximum must be valid integers: "+params;
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "set2DFootPrint", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result = _set2DFootPrint( params );
        return result;
    });

    addCommandCallback( "setUseClipBuffer", [=] (const QString & /*cmd*/,
               const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
       std::set<QString> keys = {CLIP_BUFFER};
       std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
       QString clipBufferStr = dataValues[*keys.begin()];
       bool validBool = false;
       bool useClipBuffer = Util::toBool(clipBufferStr, &validBool );
       if ( validBool ){
           result = setUseClipBuffer( useClipBuffer );
       }
       else {
           result = "Use clip buffer must be true/false: " + params;
       }
       Util::commandPostProcess( result );
       return result;
   });


    addCommandCallback( "zoomFull", [=] (const QString & /*cmd*/,
            const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        QString result = setClipRangePercent( 0, 0);
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "zoomRange", [=] (const QString & /*cmd*/,
            const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        _zoomToSelection();
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

double Histogram::_getBufferedIntensity( const QString& clipKey, const QString& percentKey ){
    double intensity = m_state.getValue<double>(clipKey);
    //Add padding to either side of the intensity if we are not already at our max.
    if ( m_state.getValue<bool>(CLIP_BUFFER) ){
        float bufferPercentile = m_state.getValue<int>(CLIP_BUFFER_SIZE ) / 2.0;
        //See how much padding we have on either side.
        float existing = m_state.getValue<double>(percentKey);
        float actual = existing - bufferPercentile;
        if ( actual < 0 ){
            actual = 0;
        }
        if ( existing != actual ){
            float percentile = actual / 100;
            if ( clipKey == CLIP_MAX ){
                percentile = 1 - percentile;
            }
            Controller* controller = _getControllerSelected();
            if ( controller != nullptr ){
                double actualIntensity = intensity;
                std::pair<int,int> frameBounds = _getFrameBounds();
                bool intensityValid = controller->getIntensity( frameBounds.first, frameBounds.second, percentile, &actualIntensity );
                if ( intensityValid ){
                    intensity = actualIntensity;
                }
            }
        }
    }
    return intensity;
}

void Histogram::_loadData( Controller* controller ){

    int binCount = m_state.getValue<int>(BIN_COUNT);
    std::pair<int,int> frameBounds = _getFrameBounds();
    int minChannel = frameBounds.first;
    int maxChannel = frameBounds.second;
    double minIntensity = _getBufferedIntensity( CLIP_MIN, CLIP_MIN_PERCENT );
    double maxIntensity = _getBufferedIntensity( CLIP_MAX, CLIP_MAX_PERCENT );

    std::vector<std::shared_ptr<Image::ImageInterface>> dataSources;
    if ( controller != nullptr ){
        dataSources = _generateData( controller );
    }
    auto result = Globals::instance()-> pluginManager()
                              -> prepare <Carta::Lib::Hooks::HistogramHook>(dataSources, binCount,
                                      minChannel, maxChannel, minIntensity, maxIntensity);
    auto lam = [=] ( const Carta::Lib::Hooks::HistogramResult &data ) {
        m_histogram->setData(data);
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

void Histogram::refreshState(){
    m_state.setValue<bool>(Util::STATE_FLUSH, true );
    m_state.flushState();
    m_state.setValue<bool>(Util::STATE_FLUSH, false );
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

QString Histogram::setClipBuffer( int bufferAmount ){
    QString result;
    if ( bufferAmount >= 0 && bufferAmount < 100 ){
        int oldBufferAmount = m_state.getValue<int>( CLIP_BUFFER_SIZE);
        if ( oldBufferAmount != bufferAmount ){
            m_state.setValue<int>( CLIP_BUFFER_SIZE, bufferAmount );
            m_state.flushState();
            _generateHistogram( true );
        }
    }
    else {
        result = "Invalid buffer amount (0,100): "+QString::number(bufferAmount);
    }
    return result;
}

QString Histogram::setClipRange( double clipMin, double clipMax ){
    QString result;
    if ( clipMin < clipMax ){
        result = setClipMin( clipMin, false );
        if ( result.isEmpty()){
            result = setClipMax( clipMax, false );
            if ( result.isEmpty() ){
                _finishClips();
            }
        }
    }
    else {
        result = "Minimum clip, "+QString::number(clipMin)+", must be less than maximum clip, "+QString::number(clipMax);
    }
    return result;
}

QString Histogram::setClipRangePercent( int clipMinPercent, int clipMaxPercent ){
    QString result;
    if ( 0 <= clipMinPercent && clipMinPercent < 100 ){
        if ( 0 <= clipMaxPercent && clipMaxPercent < 100 ){
            if ( clipMinPercent < 100 - clipMaxPercent ){
                result = setClipMinPercent( clipMinPercent, false );
                if ( result.isEmpty()){
                    result = setClipMaxPercent( clipMaxPercent, false );
                    if ( result.isEmpty() ){
                        _finishClips();
                    }
                }
            }
            else {
                result = "Clip min percent: "+ QString::number(clipMinPercent)+" must be less than "+QString::number( 100-clipMaxPercent);
            }
        }
        else {
            result = "Invalid clip right percent [0,100): "+QString::number(clipMaxPercent);
        }
    }
    else {
        result = "Invalid clip left percent [0,100): "+QString::number( clipMinPercent);
    }
    return result;
}

QString Histogram::setClipMax( double clipMax, bool finish ){
    QString result;
    double oldMax = m_state.getValue<double>(CLIP_MAX);
    double clipMin = m_state.getValue<double>(CLIP_MIN);
    double oldMaxPercent = m_state.getValue<double>(CLIP_MAX_PERCENT);
    if ( clipMin < clipMax ){
        if ( qAbs(clipMax - oldMax) > CLIP_ERROR_MARGIN){
            m_state.setValue<double>(CLIP_MAX, clipMax );
            Controller* controller = _getControllerSelected();
            if ( controller != nullptr ){
                std::pair<int,int> bounds = _getFrameBounds();
                double clipUpperBound;
                controller->getIntensity( bounds.first, bounds.second, 1, &clipUpperBound );
                double clipMaxPercent = controller->getPercentile(bounds.first, bounds.second, clipMax );
                if ( clipMaxPercent >= 0 ){
                    clipMaxPercent = clipMaxPercent * 100;
                    clipMaxPercent = 100 - clipMaxPercent;
                    if(qAbs(oldMaxPercent - clipMaxPercent) > CLIP_ERROR_MARGIN){
                        m_state.setValue<double>(CLIP_MAX_PERCENT, clipMaxPercent);
                    }
                }
                else {
                    qWarning() << "Could not update clip max percent!";
                }
            }
            if ( finish ){
                _finishClips();
            }
        }
    }
    else {
        result = "Clip mininum, "+QString::number(clipMin)+" must be less than maximum, "+QString::number(clipMax);
    }
    return result;
}



QString Histogram::setClipMin( double clipMin, bool finish ){
    QString result;
    double oldMin = m_state.getValue<double>(CLIP_MIN);
    double clipMax = m_state.getValue<double>(CLIP_MAX);
    double oldMinPercent = m_state.getValue<double>(CLIP_MIN_PERCENT);

    if( clipMin < clipMax ){
        if ( qAbs(clipMin - oldMin) > CLIP_ERROR_MARGIN){
            m_state.setValue<double>(CLIP_MIN, clipMin );
            Controller* controller = _getControllerSelected();
            if ( controller != nullptr ){
                std::pair<int,int> bounds = _getFrameBounds();
                double clipMinPercent = controller->getPercentile( bounds.first, bounds.second, clipMin);
                clipMinPercent = clipMinPercent * 100;
                if ( clipMinPercent >= 0 ){
                    if(qAbs(oldMinPercent - clipMinPercent) > CLIP_ERROR_MARGIN){
                        m_state.setValue<double>(CLIP_MIN_PERCENT, clipMinPercent);
                    }
                }
                else {
                    qWarning() << "Could not update clip min percent";
                }
            }
            if ( finish ){
                _finishClips();
            }
        }
    }
    else {
        result = "Clip mininum, "+QString::number(clipMin)+" must be less than maximum, "+QString::number(clipMax);
    }
    return result;

}


QString Histogram::setClipMinPercent( double clipMinPercent, bool complete ){
     QString result;
     double oldMinPercent = m_state.getValue<double>(CLIP_MIN_PERCENT);
     double clipMaxPercent = m_state.getValue<double>(CLIP_MAX_PERCENT);
     if( 0 <= clipMinPercent && clipMinPercent < 100  ){
         if ( clipMinPercent < 100 - clipMaxPercent ){
             if ( qAbs(clipMinPercent - oldMinPercent) > CLIP_ERROR_MARGIN){
                 m_state.setValue<double>(CLIP_MIN_PERCENT, clipMinPercent );
                 Controller* controller = _getControllerSelected();
                 if ( controller != nullptr ){
                     double clipMin = 0;
                     double cMin = clipMinPercent / 100.0;
                     std::pair<int,int> bounds = _getFrameBounds();
                     bool validIntensity = controller->getIntensity( bounds.first, bounds.second, cMin, &clipMin);
                     if(validIntensity){
                         double oldClipMin = m_state.getValue<double>(CLIP_MIN);
                         if(qAbs(oldClipMin - clipMin) > CLIP_ERROR_MARGIN){
                             m_state.setValue<double>(CLIP_MIN, clipMin);
                         }
                     }
                     else {
                         qWarning() << "Invalid intensity for percent: "<<clipMinPercent;
                     }
                 }
                 if ( complete ){
                     _finishClips();
                 }
             }
         }
         else {
             result = "Clip left percentile, "+QString::number(clipMinPercent)+
                     " must be less than clip right percentile, "+ QString::number( 1 - clipMaxPercent);
         }
     }
     else {
         result = "Invalid Histogram clip minimum percentile [0,1): "+ QString::number(clipMinPercent);
     }
     return result;
}

QString Histogram::setClipMaxPercent( double clipMaxPercent, bool complete ){
     QString result;
     double oldMaxPercent = m_state.getValue<double>(CLIP_MAX_PERCENT);
     double clipMinPercent = m_state.getValue<double>(CLIP_MIN_PERCENT);
     if( 0 <= clipMaxPercent && clipMaxPercent < 100  ){
         double lookupPercent = 100 - clipMaxPercent;
         if ( clipMinPercent < lookupPercent ){
             if ( qAbs(clipMaxPercent - oldMaxPercent) > CLIP_ERROR_MARGIN){
                 m_state.setValue<double>(CLIP_MAX_PERCENT, clipMaxPercent );
                 Controller* controller = _getControllerSelected();
                 if ( controller != nullptr ){
                     double clipMax = 0;
                     double decPercent = lookupPercent / 100.0;
                     std::pair<int,int> bound = _getFrameBounds();
                     bool validIntensity = controller->getIntensity(bound.first,bound.second, decPercent, &clipMax);
                     if(validIntensity){
                         double oldClipMax = m_state.getValue<double>(CLIP_MAX);
                         if(qAbs(oldClipMax - clipMax) > CLIP_ERROR_MARGIN){
                             m_state.setValue<double>(CLIP_MAX, clipMax);
                         }
                     }
                     else {
                         qWarning() <<" Could not map "<<lookupPercent<<" to intensity";
                     }
                 }
                 if ( complete ){
                     _finishClips();
                 }
             }
         }
         else {
             result = "Clip left percentile, "+QString::number(clipMinPercent)+
                     " must be less than clip right percentile, "+ QString::number( 1 - clipMaxPercent);
         }
     }
     else {
         result = "Invalid Histogram clip maximum percentile [0,1): "+ QString::number(clipMaxPercent);
     }
     return result;
}


QString Histogram::setColored( bool colored ){
    QString result;
    bool oldColored = m_state.getValue<bool>(GRAPH_COLORED);
    if ( colored != oldColored){
        m_state.setValue<bool>(GRAPH_COLORED, colored );
        m_state.flushState();
        _generateHistogram( false );
    }
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
    return result;
}

QString Histogram::setCubeChannel( int channel ){
    QString result;
    if ( channel < 0 ){
        result = "Invalid cube channel "+QString::number( channel );
    }
    else {
        if ( m_cubeChannel != channel ){
            m_cubeChannel = channel;
            if ( m_state.getValue<QString>(PLANE_MODE) == PLANE_MODE_SINGLE ){
                _generateHistogram( true );
            }
        }
    }
    return result;
}


QString Histogram::setLogCount( bool logCount ){
    QString result;
    bool oldLogCount = m_state.getValue<bool>(GRAPH_LOG_COUNT);
    if ( logCount != oldLogCount ){
        m_state.setValue<bool>(GRAPH_LOG_COUNT, logCount );
        m_state.flushState();
        _generateHistogram( false );
    }
    return result;
}

QString Histogram::setPlaneMode( const QString& planeModeStr ){
    QString result;
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
        result = "Unrecognized histogram cube mode: "+ planeModeStr;
    }
    return result;
}

QString Histogram::setPlaneRangeUpperBound( int bound ){
    QString result;
    int actualMax = bound - 1;
    if ( actualMax >= 0 ){
        int oldBound = m_state.getValue<int>(PLANE_RANGE_UPPER_BOUND );
        if ( actualMax != oldBound ){
            m_state.setValue<int>( PLANE_RANGE_UPPER_BOUND, actualMax );
            m_state.flushState();
        }
    }
    else {
        result = "Plane range upper bound must be positive: "+QString::number( bound);
    }
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

QString Histogram::setPlaneRange( int planeMin, int planeMax ){
    QString result;
    if ( planeMin >= 0 && planeMax >= 0 ){
        int storedMin = m_state.getValue<int>(PLANE_MIN);
        int storedMax = m_state.getValue<int>(PLANE_MAX);
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
            result = "The plane range minimum " + QString::number(planeMin)+" must be less than or equal to the maximum: "+QString::number(planeMax);
        }
    }
    else {
        result = "Plane min "+ QString::number(planeMin)+" and max "+QString::number(planeMax)+" must both be nonnegative.";
    }
    return result;
}

QString Histogram::setGraphStyle( const QString& styleStr ){
    QString result;
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
        result = "Unrecognized histogram graph style: "+ styleStr;
    }
    return result;
}


QString Histogram::setClipToImage( bool clipApply ){
    QString result;
    bool oldClipApply = m_state.getValue<bool>(CLIP_APPLY);
    if ( clipApply != oldClipApply ){
        m_state.setValue<bool>(CLIP_APPLY, clipApply );
        m_state.flushState();
        if ( clipApply ){
            _applyClips();
        }
        else {
            //Unapply any clips that might have been set.
            int linkCount = m_linkImpl->getLinkCount();
            for ( int i = 0; i < linkCount; i++ ){
                CartaObject* obj = m_linkImpl->getLink( i );
                Controller* controller = dynamic_cast<Controller*>(obj);
                if ( controller != nullptr ){
                    controller->applyClips( 0, 100 );
                }
            }
        }
    }
    return result;
}

QString Histogram::setUseClipBuffer( bool useBuffer ){
    QString result;
    bool oldUseBuffer = m_state.getValue<bool>(CLIP_BUFFER);
    if ( useBuffer != oldUseBuffer ){
        m_state.setValue<bool>(CLIP_BUFFER, useBuffer );
        m_state.flushState();
        _generateHistogram( true );
    }
    return result;
}


void Histogram::_startSelection(const QString& params ){
    std::set<QString> keys = {X_COORDINATE};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString xstr = dataValues[X_COORDINATE];
    m_selectionEnabled = true;
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

void Histogram::_updateSelection(int x){
    m_selectionEnd = x;
    m_selectionEnabled = true;
    _generateHistogram( false );
}


void Histogram::_updateColorMap( Colormap* map ){
    if ( map != nullptr ){
        Controller* controller = map->getControllerSelected();
        if ( controller != nullptr ){
            std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> pipeline = controller->getPipeline();
            m_histogram->setPipeline( pipeline );
        }
    }
    _generateHistogram( false );
}

void Histogram::_updateSize( const QSize& size ){
    m_histogram->setSize( size.width(), size.height());
    _generateHistogram( false );
}

void Histogram::_zoomToSelection(){
    bool valid = false;
    std::pair<double,double> range = m_histogram->getRange( &valid );
    if ( valid ){
        double minRange = range.first;
        double maxRange = range.second;
        if ( range.first > range.second ){
            minRange = range.second;
            maxRange = range.first;
        }
        setClipRange( minRange, maxRange );
    }
    else {
        _generateHistogram( valid );
    }
}


Histogram::~Histogram(){
    delete m_histogram;
}
}
}
