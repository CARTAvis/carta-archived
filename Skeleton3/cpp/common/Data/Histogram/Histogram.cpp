#include "Histogram.h"
#include "Data/Clips.h"
#include "Data/Colormap/Colormap.h"
#include "ChannelUnits.h"
#include "Data/Settings.h"
#include "Data/LinkableImpl.h"
#include "Data/Image/Controller.h"
#include "Data/Error/ErrorManager.h"
#include "Data/Util.h"
#include "Histogram/HistogramGenerator.h"
#include "Data/Preferences/PreferencesSave.h"
#include "Globals.h"
#include "MainConfig.h"
#include "ImageView.h"
#include "PluginManager.h"

#include "CartaLib/Hooks/Histogram.h"
#include "CartaLib/PixelPipeline/IPixelPipeline.h"
#include "State/UtilState.h"
#include <set>
#include <QtCore/qmath.h>
#include <QDir>
#include <QDebug>

namespace Carta {

namespace Data {

const QString Histogram::CLASS_NAME = "Histogram";
const QString Histogram::CLIP_BUFFER = "useClipBuffer";
const QString Histogram::CLIP_BUFFER_SIZE = "clipBuffer";
const QString Histogram::CLIP_MIN = "clipMin";
const QString Histogram::CLIP_MAX = "clipMax";
const QString Histogram::CLIP_APPLY = "applyClipToImage";
const QString Histogram::BIN_COUNT = "binCount";
const QString Histogram::BIN_COUNT_MAX = "binCountMax";
const int Histogram::BIN_COUNT_MAX_VALUE = 10000;
const QString Histogram::BIN_WIDTH = "binWidth";
const QString Histogram::COLOR_MIN = "colorMin";
const QString Histogram::COLOR_MAX = "colorMax";
const QString Histogram::COLOR_MIN_PERCENT = "colorMinPercent";
const QString Histogram::COLOR_MAX_PERCENT = "colorMaxPercent";
const QString Histogram::CUSTOM_CLIP = "customClip";
const QString Histogram::DATA_PATH = "dataPath";
const QString Histogram::GRAPH_STYLE = "graphStyle";
const QString Histogram::GRAPH_STYLE_LINE = "Line";
const QString Histogram::GRAPH_STYLE_OUTLINE = "Outline";
const QString Histogram::GRAPH_STYLE_FILL = "Fill";
const QString Histogram::GRAPH_LOG_COUNT = "logCount";
const QString Histogram::GRAPH_COLORED = "colored";
const QString Histogram::PLANE_MODE="planeMode";
const QString Histogram::PLANE_MODE_SINGLE="Current";
const QString Histogram::PLANE_MODE_RANGE="Range";
const QString Histogram::PLANE_MODE_RANGE_VALID = "image3D";
const QString Histogram::PLANE_MODE_ALL="All";
const QString Histogram::PLANE_MIN = "planeMin";
const QString Histogram::PLANE_MAX = "planeMax";

const QString Histogram::FOOT_PRINT = "twoDFootPrint";
const QString Histogram::FOOT_PRINT_IMAGE = "Image";
const QString Histogram::FOOT_PRINT_REGION = "Selected Region";
const QString Histogram::FOOT_PRINT_REGION_ALL = "All Regions";
const QString Histogram::FREQUENCY_UNIT = "rangeUnit";
const QString Histogram::CLIP_MIN_PERCENT = "clipMinPercent";
const QString Histogram::CLIP_MAX_PERCENT = "clipMaxPercent";
const QString Histogram::SIGNIFICANT_DIGITS = "significantDigits";
const QString Histogram::X_COORDINATE = "x";
const QString Histogram::POINTER_MOVE = "pointer-move";

Clips*  Histogram::m_clips = nullptr;
ChannelUnits* Histogram::m_channelUnits = nullptr;

class Histogram::Factory : public Carta::State::CartaObjectFactory {
public:

    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new Histogram (path, id);
    }
};

bool Histogram::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new Histogram::Factory());

using Carta::State::UtilState;
using Carta::State::StateInterface;

Histogram::Histogram( const QString& path, const QString& id):
            CartaObject( CLASS_NAME, path, id ),
            m_view(nullptr),
            m_linkImpl( new LinkableImpl( path )),
            m_preferences( nullptr),
            m_stateData( UtilState::getLookup(path, StateInterface::STATE_DATA)),
            m_stateMouse(UtilState::getLookup(path, ImageView::VIEW)){

    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    Settings* prefObj = objMan->createObject<Settings>();
    m_preferences.reset( prefObj );

    _initializeStatics();
    _initializeDefaultState();
    _initializeCallbacks();
    _setErrorMargin();

    m_view.reset( new ImageView( path, QColor("yellow"), QImage(), &m_stateMouse));
    connect( m_view.get(), SIGNAL(resize(const QSize&)), this, SLOT(_updateSize(const QSize&)));
    registerView(m_view.get());
    m_selectionEnabled = false;
    m_selectionEnabledColor = false;
    m_controllerLinked = false;
    m_cubeChannel = 0;
    m_histogram = new Carta::Histogram::HistogramGenerator();

    //Load the available clips.
    if ( m_clips == nullptr ){
        m_clips = Util::findSingletonObject<Clips>();
    }
}



QString Histogram::addLink( CartaObject*  target){
    Controller* controller = dynamic_cast<Controller*>(target);
    bool linkAdded = false;
    QString result;
    if ( controller != nullptr ){
        if ( !m_controllerLinked ){
            linkAdded = m_linkImpl->addLink( controller );
            if ( linkAdded ){
                connect(controller, SIGNAL(dataChanged(Controller*)), this , SLOT(_createHistogram(Controller*)));
                connect( controller,SIGNAL(channelChanged(Controller*)), this, SLOT(_updateChannel(Controller*)));
                m_controllerLinked = true;
                _createHistogram( controller );
            }
        }
        else {
            CartaObject* obj = m_linkImpl->searchLinks( target->getPath());
            if ( obj != nullptr ){
                linkAdded = true;
            }
            else {
                result = "Histogram only supports linking to a single image source.";
            }
        }
    }
    else {
        result = "Histogram only supports linking to images";
    }
    return result;
}

void Histogram::applyClips(){
   //Get percentiles and normalize to [0,1].
   double clipMinPercent = m_stateData.getValue<double>( COLOR_MIN_PERCENT );
   double clipMaxPercent = m_stateData.getValue<double>(COLOR_MAX_PERCENT);
   if ( clipMinPercent > 0 || clipMaxPercent > 0 ){
       double minPercentile =  clipMinPercent / 100.0;
       double maxPercentile = clipMaxPercent / 100.0;
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
        int significantDigits = m_state.getValue<int>(SIGNIFICANT_DIGITS);
        minIntensity = Util::roundToDigits( minIntensity, significantDigits );
        maxIntensity = Util::roundToDigits( maxIntensity, significantDigits );
        m_stateData.setValue<double>(CLIP_MIN_PERCENT, 0 );
        m_stateData.setValue<double>(CLIP_MAX_PERCENT, 100 );
        m_stateData.setValue<double>(CLIP_MIN, minIntensity);
        m_stateData.setValue<double>(CLIP_MAX, maxIntensity);
        m_stateData.setValue<double>(COLOR_MIN, minIntensity );
        m_stateData.setValue<double>(COLOR_MAX, maxIntensity );

        int frameCount = controller->getChannelUpperBound() - 1;
        bool planeModeValid = false;
        if ( frameCount > 0 ){
            planeModeValid = true;
        }

        bool oldPlaneModeValid = m_stateData.getValue<bool>(PLANE_MODE_RANGE_VALID );
        if ( planeModeValid != oldPlaneModeValid ){
            m_stateData.setValue<bool>(PLANE_MODE_RANGE_VALID, planeModeValid);
        }

        m_stateData.flushState();
    }
    _generateHistogram( true, controller );
}



void Histogram::_endSelection(const QString& params ){
    std::set<QString> keys = {X_COORDINATE};
    std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
    QString xstr = dataValues[X_COORDINATE];
    m_selectionEnd = xstr.toDouble();
    m_histogram->setSelectionMode( false );
    m_selectionEnabled = false;
    _zoomToSelection();
}

void Histogram::_endSelectionColor(const QString& params ){
    std::set<QString> keys = {X_COORDINATE};
    std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
    QString xstr = dataValues[X_COORDINATE];
    m_selectionEnd = xstr.toDouble();
    m_histogram->setSelectionModeColor( false );
    m_selectionEnabledColor = false;
    _updateColorSelection();
}

void Histogram::_finishClips (){
    bool customClip = m_state.getValue<bool>(CUSTOM_CLIP );
    if ( !customClip ){
        m_stateData.setValue<double>(COLOR_MIN, m_stateData.getValue<double>(CLIP_MIN));
        m_stateData.setValue<double>(COLOR_MAX, m_stateData.getValue<double>(CLIP_MAX));
        m_stateData.setValue<double>(COLOR_MIN_PERCENT, m_stateData.getValue<double>(CLIP_MIN_PERCENT));
        m_stateData.setValue<double>(COLOR_MAX_PERCENT, m_stateData.getValue<double>(CLIP_MAX_PERCENT));
    }
    m_stateData.flushState();
    _generateHistogram( true );
}

void Histogram::_finishColor(){
    double colorMin = m_stateData.getValue<double>(COLOR_MIN);
    double colorMax = m_stateData.getValue<double>(COLOR_MAX);
    m_histogram->setRangeIntensityColor( colorMin, colorMax );
    m_stateData.flushState();
}

std::vector<std::shared_ptr<Image::ImageInterface>> Histogram::_generateData(Controller* controller){
    std::vector<std::shared_ptr<Image::ImageInterface>> result;
    if ( controller != nullptr ){
        result = controller->getDataSources();
    }
    return result;
}

void Histogram::_generateHistogram( bool newDataNeeded, Controller* controller ){
    Controller* activeController = controller;
    if ( activeController == nullptr ){
        int linkCount = m_linkImpl->getLinkCount();
        for ( int i = 0; i < linkCount; i++ ){
            CartaObject* link = m_linkImpl->getLink( i );
            Controller* linkedController = dynamic_cast<Controller*>(link);
            if ( linkedController != nullptr){
                activeController = linkedController;
                break;
            }
        }
    }
    if ( newDataNeeded ){
        _loadData( activeController );
    }

    //User is not selecting a range
    if ( !m_selectionEnabled && !m_selectionEnabledColor ){
        QString style = m_state.getValue<QString>(GRAPH_STYLE);
        bool logCount = m_state.getValue<bool>(GRAPH_LOG_COUNT);
        bool colored = m_state.getValue<bool>(GRAPH_COLORED);
        m_histogram->setStyle(style);
        m_histogram->setLogScale(logCount);
        m_histogram->setColored( colored );
    }
    //User is selecting a range.
    else if ( m_selectionEnabled ){
        m_histogram->setRangePixels( m_selectionStart, m_selectionEnd );
    }
    else if ( m_selectionEnabledColor ){
        m_histogram->setRangePixelsColor( m_selectionStart, m_selectionEnd );
    }

    //Refresh the view
    _refreshView();
}

QString Histogram::getStateString( const QString& sessionId, SnapshotType type ) const{
    QString result("");
    if ( type == SNAPSHOT_PREFERENCES ){
        StateInterface prefState( "");
        prefState.setValue<QString>(Carta::State::StateInterface::OBJECT_TYPE, CLASS_NAME );
        prefState.insertValue<QString>( Util::PREFERENCES, m_state.toString());
        prefState.insertValue<QString>( Settings::SETTINGS, m_preferences->getStateString(sessionId, type) );
        result = prefState.toString();
    }
    else if ( type == SNAPSHOT_DATA ){
        StateInterface dataCopy( m_stateData );
        dataCopy.setValue<QString>( StateInterface::OBJECT_TYPE, CLASS_NAME+StateInterface::STATE_DATA);
        dataCopy.setValue<int>( StateInterface::INDEX, getIndex());
        result = dataCopy.toString();
    }
    else if ( type == SNAPSHOT_LAYOUT ){
        result = m_linkImpl->getStateString(getIndex(), getSnapType( type ));
    }
    return result;
}


double Histogram::_getBufferedIntensity( const QString& clipKey, const QString& percentKey ){
    double intensity = m_stateData.getValue<double>(clipKey);
    //Add padding to either side of the intensity if we are not already at our max.
    if ( m_state.getValue<bool>(CLIP_BUFFER) ){
        float bufferPercentile = m_stateData.getValue<int>(CLIP_BUFFER_SIZE ) / 2.0;
        //See how much padding we have on either side.
        float existing = m_stateData.getValue<double>(percentKey);
        float actual = existing - bufferPercentile;
        if ( clipKey == CLIP_MAX){
            actual = existing + bufferPercentile;
        }
        if ( actual < 0 ){
            actual = 0;
        }
        if ( existing != actual ){
            float percentile = actual / 100;
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

bool Histogram::getColored(){
    bool colored = m_state.getValue<bool>(GRAPH_COLORED);
    return colored;
}

Controller* Histogram::_getControllerSelected() const {
    //We are only supporting one linked controller.
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
    std::pair<int,int> bounds( minChannel, maxChannel);
    return bounds;
}

QList<QString> Histogram::getLinks() const {
    return m_linkImpl->getLinkIds();
}

bool Histogram::getLogCount(){
    bool logCount = m_state.getValue<bool>(GRAPH_LOG_COUNT);
    return logCount;
}

QString Histogram::_getActualPlaneMode( const QString& planeModeStr ){
    QString result = "";
    if ( QString::compare( planeModeStr, PLANE_MODE_ALL, Qt::CaseInsensitive) == 0 ){
        result = PLANE_MODE_ALL;
    }
    else if ( QString::compare( planeModeStr, PLANE_MODE_SINGLE, Qt::CaseInsensitive) == 0 ){
        result = PLANE_MODE_SINGLE;
    }
    else if ( QString::compare( planeModeStr, PLANE_MODE_RANGE, Qt::CaseInsensitive) == 0 ){
        result = PLANE_MODE_RANGE;
    }
    return result;
}

QString Histogram::getPreferencesId() const {
    return m_preferences->getPath();
}

QString Histogram::getSnapType(CartaObject::SnapshotType snapType) const {
    QString objType = CartaObject::getSnapType( snapType );
    if ( snapType == SNAPSHOT_DATA ){
        objType = objType + Carta::State::StateInterface::STATE_DATA;
    }
    return objType;
}


void Histogram::_initializeDefaultState(){

    //Data State - likely to change with a different image
    m_stateData.insertValue<double>( CLIP_MIN, 0 );
    m_stateData.insertValue<double>(CLIP_MAX, 1);
    m_stateData.insertValue<int>(CLIP_BUFFER_SIZE, 10 );
    m_stateData.insertValue<double>(COLOR_MIN, 0 );
    m_stateData.insertValue<double>(COLOR_MAX, 1 );
    m_stateData.insertValue<int>(COLOR_MIN_PERCENT, 0 );
    m_stateData.insertValue<int>(COLOR_MAX_PERCENT, 100 );
    m_stateData.insertValue<double>(CLIP_MIN_PERCENT, 0);
    m_stateData.insertValue<double>(CLIP_MAX_PERCENT, 100);
    m_stateData.insertValue<double>(PLANE_MIN, 0 );
    m_stateData.insertValue<double>(PLANE_MAX, 1 );
    m_stateData.insertValue<bool>(PLANE_MODE_RANGE_VALID, true );
    m_stateData.flushState();

    //Preferences - not image specific
    const int DEFAULT_BIN_COUNT = 25;
    m_state.insertValue<int>(BIN_COUNT, DEFAULT_BIN_COUNT );
    //Decide if the user has decided to override the maximum bin count.
    int histMaxBinCount = Globals::instance()->mainConfig()->getHistogramBinCountMax();
    if ( histMaxBinCount < 0 ){
        histMaxBinCount = BIN_COUNT_MAX_VALUE;
    }

    m_state.insertValue<int>(BIN_COUNT_MAX, histMaxBinCount );
    double binWidth = _toBinWidth(DEFAULT_BIN_COUNT);
    m_state.insertValue<double>(BIN_WIDTH, binWidth );
    m_state.insertValue<bool>(CUSTOM_CLIP, false );
    m_state.insertValue<bool>(CLIP_APPLY, false );
    m_state.insertValue<bool>(CLIP_BUFFER, false);
    m_state.insertValue<QString>(GRAPH_STYLE, GRAPH_STYLE_LINE);
    m_state.insertValue<bool>(GRAPH_LOG_COUNT, true );
    m_state.insertValue<bool>(GRAPH_COLORED, false );
    m_state.insertValue<QString>(PLANE_MODE, PLANE_MODE_SINGLE );
    m_state.insertValue<QString>(FREQUENCY_UNIT, m_channelUnits->getDefaultUnit());
    m_state.insertValue<QString>(FOOT_PRINT, FOOT_PRINT_IMAGE );
    m_state.insertValue<int>(SIGNIFICANT_DIGITS, 6 );
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

    addCommandCallback( "mouseDownShift", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
            _startSelectionColor(params);
            return "";
        });

    addCommandCallback( "mouseUp", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        _endSelection( params );
        return "";
    });

    addCommandCallback( "mouseUpShift", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
            _endSelectionColor( params );
            return "";
        });

    addCommandCallback( "setBinCount", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {BIN_COUNT};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
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

    addCommandCallback( "registerPreferences", [=] (const QString & /*cmd*/,
                    const QString & /*params*/, const QString & /*sessionId*/) -> QString {
                QString result = getPreferencesId();
                return result;
            });

    addCommandCallback( "saveImage", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) -> QString {
                std::set<QString> keys = {DATA_PATH};
                std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                QString result = saveHistogram( dataValues[DATA_PATH]);
                if ( !result.isEmpty() ){
                    Util::commandPostProcess( result );
                }
                else {
                    ErrorManager* hr = Util::findSingletonObject<ErrorManager>();
                    hr->registerInformation( "Histogram was successfully saved.");
                }
                return result;
            });

    addCommandCallback( "setBinWidth", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
            QString result;
            std::set<QString> keys = {BIN_WIDTH};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
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
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
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
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString clipMaxStr = dataValues[CLIP_MAX];
        bool validRangeMax = false;
        double clipMax = clipMaxStr.toDouble( &validRangeMax );
        if ( validRangeMax ){
            result = setClipMax( clipMax );
        }
        else {
            result = "Invalid zoom maximum: " + params+" must be a valid number.";
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setClipMaxPercent", [=] (const QString & /*cmd*/,
                     const QString & params, const QString & /*sessionId*/) -> QString {
       QString result;
       std::set<QString> keys = {CLIP_MAX_PERCENT};
       std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
       QString clipMaxPercentStr = dataValues[CLIP_MAX_PERCENT];
       bool validRangeMax = false;
       double clipMaxPercent = clipMaxPercentStr.toDouble( &validRangeMax );
       if ( validRangeMax ){
           result = setClipMaxPercent( clipMaxPercent );
       }
       else {
           result = "Invalid zoom maximum percentile: " + params+", must be a valid number.";
       }
       Util::commandPostProcess( result );
       return result;
    });

    addCommandCallback( "setClipMin", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {CLIP_MIN};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString clipMinStr = dataValues[CLIP_MIN];
        bool validRangeMin = false;
        double clipMin = clipMinStr.toDouble( &validRangeMin );
        if ( validRangeMin ){
            result = setClipMin( clipMin);
        }
        else {
            result = "Invalid zoom minimum: " + params+" must be a valid number.";
        }
        Util::commandPostProcess( result );
        return result;

    });

    addCommandCallback( "setClipMinPercent", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {CLIP_MIN_PERCENT};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString clipMinPercentStr = dataValues[CLIP_MIN_PERCENT];
        bool validRangeMin = false;
        double clipMinPercent = clipMinPercentStr.toDouble( &validRangeMin );
        if ( validRangeMin ){
            result = setClipMinPercent( clipMinPercent);
        }
        else {
            result = "Invalid zoom minimum percentile: " + params+", must be a valid number.";
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setColorMaxPercent", [=] (const QString & /*cmd*/,
                         const QString & params, const QString & /*sessionId*/) -> QString {
           QString result;
           std::set<QString> keys = {COLOR_MAX_PERCENT};
           std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
           QString colorMaxPercentStr = dataValues[COLOR_MAX_PERCENT];
           bool validRangeMax = false;
           double colorMaxPercent = colorMaxPercentStr.toDouble( &validRangeMax );
           if ( validRangeMax ){
               result = setColorMaxPercent( colorMaxPercent, true );
           }
           else {
               result = "Invalid clip maximum percentile: " + params+", must be a valid number.";
           }
           Util::commandPostProcess( result );
           return result;
        });

    addCommandCallback( "setColorMinPercent", [=] (const QString & /*cmd*/,
                         const QString & params, const QString & /*sessionId*/) -> QString {
           QString result;
           std::set<QString> keys = {COLOR_MIN_PERCENT};
           std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
           QString clipMinPercentStr = dataValues[COLOR_MIN_PERCENT];
           bool validRangeMin = false;
           double clipMinPercent = clipMinPercentStr.toDouble( &validRangeMin );
           if ( validRangeMin ){
               result = setColorMinPercent( clipMinPercent, true );
           }
           else {
               result = "Invalid clip minimum percentile: " + params+", must be a valid number.";
           }
           Util::commandPostProcess( result );
           return result;
        });

    addCommandCallback( "setClipToImage", [=] (const QString & /*cmd*/,
            const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        applyClips();
        return "";
    });

    addCommandCallback( "setClipValue", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
            QString result;
            std::set<QString> keys = {"clipValue"};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            bool validClip = false;
            QString clipKey = *keys.begin();
            QString clipWithoutPercent = dataValues[clipKey].remove("%");
            double clipVal = dataValues[clipKey].toDouble(&validClip);
            if ( validClip ){
                double oldClipValMin = m_stateData.getValue<double>(CLIP_MIN_PERCENT );
                double oldClipValMax = m_stateData.getValue<double>( CLIP_MAX_PERCENT );
                double leftOver = 100 - clipVal*100;
                double clipValMin = leftOver / 2.0;
                double clipValMax = clipVal*100 + leftOver / 2.0;
                if ( qAbs( clipValMin - oldClipValMin) >= m_errorMargin  ||
                        qAbs( clipValMax - oldClipValMax) >= m_errorMargin ){
                    result = setClipRangePercent (clipValMin, clipValMax );
                }
            }
            else {
                result = "Invalid clip value: "+params;
            }
            Util::commandPostProcess( result );
            return result;
        });

    addCommandCallback( "setColorMax", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
            QString result;
            std::set<QString> keys = {COLOR_MAX};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString colorMaxStr = dataValues[COLOR_MAX];
            bool validRangeMax = false;
            double colorMax = colorMaxStr.toDouble( &validRangeMax );
            if ( validRangeMax ){
                result = setColorMax( colorMax, true );
            }
            else {
                result = "Invalid color maximum: " + params+" must be a valid number.";
            }
            Util::commandPostProcess( result );
            return result;
        });

    addCommandCallback( "setColorMin", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
                QString result;
                std::set<QString> keys = {COLOR_MIN};
                std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                QString colorMinStr = dataValues[COLOR_MIN];
                bool validRangeMin = false;
                double colorMin = colorMinStr.toDouble( &validRangeMin );
                if ( validRangeMin ){
                    result = setColorMin( colorMin, true );
                }
                else {
                    result = "Invalid color minimum: " + params+" must be a valid number.";
                }
                Util::commandPostProcess( result );
                return result;
            });

    addCommandCallback( "setCustomClip", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {CUSTOM_CLIP};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString clipStr = dataValues[CUSTOM_CLIP];
        bool validBool = false;
        bool customClip = Util::toBool( clipStr, &validBool );
        if ( validBool ){
            result = setCustomClip( customClip );
        }
        else {
            result = "Custom clip must be true/false: " + params;
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setCubeRangeUnit", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = {FREQUENCY_UNIT};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString unitStr = dataValues[*keys.begin()];
            QString result = setChannelUnit( unitStr );
            Util::commandPostProcess( result );
            return result;
        });


    addCommandCallback( "setGraphStyle", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {GRAPH_STYLE};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString styleStr = dataValues[*keys.begin()];
        QString result = setGraphStyle( styleStr );
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setLogCount", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {GRAPH_LOG_COUNT};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
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
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
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
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString planeModeStr = dataValues[*keys.begin()];
        result = setPlaneMode( planeModeStr );
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setPlaneRange", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {PLANE_MIN, PLANE_MAX};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString planeMinStr = dataValues[PLANE_MIN];
        QString planeMaxStr = dataValues[PLANE_MAX];
        bool validRangeMin = false;
        double planeMin = planeMinStr.toDouble( &validRangeMin );
        bool validRangeMax = false;
        double planeMax = planeMaxStr.toDouble( &validRangeMax );
        if ( validRangeMin && validRangeMax ){
            result = setPlaneRange( planeMin, planeMax );
        }
        else {
            result = "Plane range minimum and maximum must be valid frequencies: "+params;
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
       std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
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

    addCommandCallback( "setSignificantDigits", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
            QString result;
            std::set<QString> keys = {SIGNIFICANT_DIGITS};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString digitsStr = dataValues[SIGNIFICANT_DIGITS];
            bool validDigits = false;
            int digits = digitsStr.toInt( &validDigits );
            if ( validDigits ){
                result = setSignificantDigits( digits );
            }
            else {
                result = "Histogram significant digits must be an integer: "+params;
            }
            Util::commandPostProcess( result );
            return result;
        });


    addCommandCallback( "zoomFull", [=] (const QString & /*cmd*/,
            const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        QString result = setClipRangePercent( 0, 100);
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "zoomRange", [=] (const QString & /*cmd*/,
            const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        _zoomToSelection();
        return "";
    });

    QString pointerPath= UtilState::getLookup(getPath(), UtilState::getLookup(ImageView::VIEW, POINTER_MOVE));
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

void Histogram::_initializeStatics(){
    if ( m_channelUnits == nullptr ){
        m_channelUnits = Util::findSingletonObject<ChannelUnits>();
    }
}

bool Histogram::isLinked( const QString& linkId ) const {
    bool linked = false;
    CartaObject* obj = m_linkImpl->searchLinks( linkId );
    if ( obj != nullptr ){
        linked = true;
    }
    return linked;
}


void Histogram::_loadData( Controller* controller ){

    int binCount = m_state.getValue<int>(BIN_COUNT)+1;
    double minFrequency = -1;
    double maxFrequency = -1;
    QString rangeUnits = m_state.getValue<QString>(FREQUENCY_UNIT );
    QString planeMode = m_state.getValue<QString>(PLANE_MODE);
    if ( planeMode == PLANE_MODE_RANGE ){
        minFrequency = m_stateData.getValue<double>(PLANE_MIN);
        maxFrequency = m_stateData.getValue<double>(PLANE_MAX);
    }
    std::pair<int,int> frameBounds = _getFrameBounds();
    int minChannel = frameBounds.first;
    int maxChannel = frameBounds.second;
    double minIntensity = _getBufferedIntensity( CLIP_MIN, CLIP_MIN_PERCENT );
    double maxIntensity = _getBufferedIntensity( CLIP_MAX, CLIP_MAX_PERCENT );

    std::vector<std::shared_ptr<Image::ImageInterface>> dataSources;
    if ( controller != nullptr ){
        int stackedImageCount = controller->getStackedImageCount();
        if ( stackedImageCount > 0 ){
            dataSources = _generateData( controller );
            auto result = Globals::instance()-> pluginManager()
                                      -> prepare <Carta::Lib::Hooks::HistogramHook>(dataSources, binCount,
                                              minChannel, maxChannel, minFrequency, maxFrequency, rangeUnits,
                                              minIntensity, maxIntensity);
            auto lam = [=] ( const Carta::Lib::Hooks::HistogramResult &data ) {
                m_histogram->setData(data);
                double freqLow = data.getFrequencyMin();
                double freqHigh = data.getFrequencyMax();
                setPlaneRange( freqLow, freqHigh);
            };
            try {
                result.forEach( lam );
            }
            catch( char*& error ){
                QString errorStr( error );
                ErrorManager* hr = Util::findSingletonObject<ErrorManager>();
                hr->registerError( errorStr );
            }
        }
        else if ( stackedImageCount == 0 ){
            _resetDefaultStateData();
            const Carta::Lib::Hooks::HistogramResult data;
            m_histogram->setData( data );
        }
    }
}

void Histogram::refreshState() {
    CartaObject::refreshState();
    m_stateData.refreshState();
    m_preferences->refreshState();
    m_linkImpl->refreshState();
}

void Histogram::_refreshView(){
    QImage * histogramImage = m_histogram->toImage();
    m_view->resetImage( *histogramImage );
    m_view->scheduleRedraw();
}

QString Histogram::removeLink( CartaObject* cartaObject){
    bool removed = false;
    QString result;
    Controller* controller = dynamic_cast<Controller*>( cartaObject );
    if ( controller != nullptr ){
        removed = m_linkImpl->removeLink( controller );
        if ( removed ){
            controller->disconnect(this);
            m_controllerLinked = false;
            _resetDefaultStateData();
        }
    }
    else {
       result = "Histogram was unable to remove link only image links are supported";
    }
    return result;
}

bool Histogram::_resetBinCountBasedOnWidth(){
    bool validWidth = false;
    double binWidth = m_state.getValue<double>(BIN_WIDTH );
    int oldBinCount = m_state.getValue<int>(BIN_COUNT);
    int binCount = _toBinCount( binWidth );
    int maxBinCount = m_state.getValue<int>(BIN_COUNT_MAX );
    if ( binCount <= maxBinCount ){
        validWidth = true;
        if ( oldBinCount != binCount ){
            m_state.setValue<int>(BIN_COUNT, binCount );
        }
    }
    return validWidth;
}

void Histogram::_resetDefaultStateData(){
   m_stateData.setValue<double>( CLIP_MIN, 0 );
   m_stateData.setValue<double>(CLIP_MAX, 1);
   m_stateData.setValue<int>(CLIP_BUFFER_SIZE, 10 );
   m_stateData.setValue<double>(COLOR_MIN, 0 );
   m_stateData.setValue<double>(COLOR_MAX, 1 );
   m_stateData.setValue<int>(COLOR_MIN_PERCENT, 0 );
   m_stateData.setValue<int>(COLOR_MAX_PERCENT, 100 );
   m_stateData.setValue<double>(CLIP_MIN_PERCENT, 0);
   m_stateData.setValue<double>(CLIP_MAX_PERCENT, 100);
   m_stateData.setValue<double>(PLANE_MIN, 0 );
   m_stateData.setValue<double>(PLANE_MAX, 1 );
   m_stateData.flushState();
}


void Histogram::resetState( const QString& state ){
    StateInterface restoredState( "");
    restoredState.setState( state );

    QString settingStr = restoredState.getValue<QString>(Settings::SETTINGS);
    m_preferences->resetStateString( settingStr );

    QString prefStr = restoredState.getValue<QString>(Util::PREFERENCES);
    m_state.setState( prefStr );
    m_state.flushState();
}

void Histogram::resetStateData( const QString& state ){
    m_stateData.setState( state );
    m_stateData.flushState();
}

QString Histogram::setClipBuffer( int bufferAmount ){
    QString result;
    if ( bufferAmount >= 0 && bufferAmount < 100 ){
        int oldBufferAmount = m_stateData.getValue<int>( CLIP_BUFFER_SIZE);
        if ( oldBufferAmount != bufferAmount ){
            m_stateData.setValue<int>( CLIP_BUFFER_SIZE, bufferAmount );
            m_stateData.flushState();
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

QString Histogram::setCustomClip( bool customClip ){
    QString result;
    bool oldCustomClip = m_state.getValue<bool>(CUSTOM_CLIP);
    if ( oldCustomClip != customClip ){
        m_state.setValue<bool>(CUSTOM_CLIP, customClip);
        m_state.flushState();
    }
    return result;
}

QString Histogram::setRangeColor( double colorMin, double colorMax ){
    QString result;
    if ( colorMin < colorMax ){
        result = setColorMin( colorMin, false );
        if ( result.isEmpty()){
            result = setColorMax( colorMax, false );
            if ( result.isEmpty() ){
                _finishColor();
            }
        }
    }
    else {
        result = "Minimum color bound, "+QString::number(colorMin)+", must be less than the maximum bound, "+QString::number(colorMax);
    }
    return result;
}

QString Histogram::setClipRangePercent( double clipMinPercent, double clipMaxPercent ){
    QString result;
    if ( 0 <= clipMinPercent && clipMinPercent <= 100 ){
        if ( 0 <= clipMaxPercent && clipMaxPercent <= 100 ){
            if ( clipMinPercent < clipMaxPercent ){
                result = setClipMinPercent( clipMinPercent, false );
                if ( result.isEmpty()){
                    result = setClipMaxPercent( clipMaxPercent, false );
                    if ( result.isEmpty() ){
                        _finishClips();
                    }
                }
            }
            else {
                result = "Zoom minimum percent: "+ QString::number(clipMinPercent)+" must be less than "+QString::number( clipMaxPercent);
            }
        }
        else {
            result = "Invalid zoom right percent [0,100]: "+QString::number(clipMaxPercent);
        }
    }
    else {
        result = "Invalid zoom left percent [0,100]: "+QString::number( clipMinPercent);
    }
    return result;
}

QString Histogram::setClipMax( double clipMax, bool finish ){
    QString result;
    int significantDigits = m_state.getValue<int>(SIGNIFICANT_DIGITS);
    double clipMaxRounded = Util::roundToDigits( clipMax, significantDigits );
    double oldMax = m_stateData.getValue<double>(CLIP_MAX);
    double clipMin = m_stateData.getValue<double>(CLIP_MIN);
    double oldMaxPercent = m_stateData.getValue<double>(CLIP_MAX_PERCENT);
    if ( clipMin < clipMaxRounded ){
        if ( qAbs(clipMaxRounded - oldMax) > m_errorMargin){

            m_stateData.setValue<double>(CLIP_MAX, clipMaxRounded );
            bool validWidth = _resetBinCountBasedOnWidth();
            if ( validWidth ){
                Controller* controller = _getControllerSelected();
                if ( controller != nullptr ){
                    std::pair<int,int> bounds = _getFrameBounds();
                    double clipUpperBound;
                    controller->getIntensity( bounds.first, bounds.second, 1, &clipUpperBound );
                    double clipMaxPercent = controller->getPercentile(bounds.first, bounds.second, clipMaxRounded );
                    if ( clipMaxPercent >= 0 ){
                        clipMaxPercent = Util::roundToDigits(clipMaxPercent * 100, significantDigits);
                        if(qAbs(oldMaxPercent - clipMaxPercent) > m_errorMargin){
                            m_stateData.setValue<double>(CLIP_MAX_PERCENT, clipMaxPercent);
                        }
                    }
                    else {
                        qWarning() << "Could not update zoom max percent!";
                    }
                }

            }
            else {
                m_stateData.setValue<double>(CLIP_MAX, oldMax );
                result = "Resulting histogram bin count exceeded the maximum.";
            }
            if ( finish ){
               _finishClips();
           }
        }
    }
    else {
        result = "Zoom mininum, "+QString::number(clipMin)+" must be less than maximum, "+QString::number(clipMax);
    }
    return result;
}



QString Histogram::setClipMin( double clipMin, bool finish ){
    QString result;
    double oldMin = m_stateData.getValue<double>(CLIP_MIN);
    double clipMax = m_stateData.getValue<double>(CLIP_MAX);
    double oldMinPercent = m_stateData.getValue<double>(CLIP_MIN_PERCENT);
    int significantDigits = m_state.getValue<int>(SIGNIFICANT_DIGITS );
    double clipMinRounded = Util::roundToDigits( clipMin, significantDigits );
    if( clipMinRounded < clipMax ){
        if ( qAbs(clipMinRounded - oldMin) > m_errorMargin){
            m_stateData.setValue<double>(CLIP_MIN, clipMinRounded );
            bool validWidth = _resetBinCountBasedOnWidth();
            if ( validWidth ){
                Controller* controller = _getControllerSelected();
                if ( controller != nullptr ){
                    std::pair<int,int> bounds = _getFrameBounds();
                    double clipMinPercent = controller->getPercentile( bounds.first, bounds.second, clipMinRounded);
                    clipMinPercent = Util::roundToDigits(clipMinPercent * 100, significantDigits);
                    if ( clipMinPercent >= 0 ){
                        if(qAbs(oldMinPercent - clipMinPercent) > m_errorMargin){
                            m_stateData.setValue<double>(CLIP_MIN_PERCENT, clipMinPercent);
                        }
                    }
                    else {
                        qWarning() << "Could not update zoom minimum percent";
                    }
                }
            }
            else {
                m_stateData.setValue<double>(CLIP_MIN, oldMin );
                result = "Resulting histogram bin count exceeded the maximum.";
            }
            if ( finish ){
                _finishClips();
            }
        }
    }
    else {
        result = "Zoom mininum, "+QString::number(clipMin)+" must be less than maximum, "+QString::number(clipMax);
    }
    return result;

}


QString Histogram::setClipMinPercent( double clipMinPercent, bool complete ){
     QString result;
     double oldMinPercent = m_stateData.getValue<double>(CLIP_MIN_PERCENT);
     double clipMaxPercent = m_stateData.getValue<double>(CLIP_MAX_PERCENT);
     int significantDigits = m_state.getValue<int>(SIGNIFICANT_DIGITS );
     double clipMinPercentRounded = Util::roundToDigits( clipMinPercent, significantDigits );
     if( 0 <= clipMinPercentRounded && clipMinPercentRounded <= 100  ){
         if ( clipMinPercentRounded < clipMaxPercent ){
             if ( qAbs(clipMinPercentRounded - oldMinPercent) > m_errorMargin){
                 m_stateData.setValue<double>(CLIP_MIN_PERCENT, clipMinPercentRounded );
                 Controller* controller = _getControllerSelected();
                 if ( controller != nullptr ){
                     double clipMin = 0;
                     double cMin = clipMinPercentRounded / 100.0;
                     std::pair<int,int> bounds = _getFrameBounds();
                     bool validIntensity = controller->getIntensity( bounds.first, bounds.second, cMin, &clipMin);
                     if(validIntensity){
                         double oldClipMin = m_stateData.getValue<double>(CLIP_MIN);
                         if(qAbs(oldClipMin - clipMin) > m_errorMargin){

                             m_stateData.setValue<double>(CLIP_MIN, Util::roundToDigits(clipMin, significantDigits));
                             bool validWidth = _resetBinCountBasedOnWidth();
                             if ( !validWidth ){
                                 m_stateData.setValue<double>(CLIP_MIN, oldClipMin );
                                 m_stateData.setValue<double>(CLIP_MIN_PERCENT, oldMinPercent );
                                 result = "Resulting histogram bin count exceeded the maximum.";
                             }
                         }
                     }
                     else {
                         m_stateData.setValue<double>(CLIP_MIN_PERCENT, oldMinPercent);
                         result = "Invalid intensity for percent: " + QString::number(clipMinPercent);
                     }
                 }
                 if ( complete){
                     _finishClips();
                 }
             }
         }
         else {
             result = "Zoom left percentile, "+QString::number(clipMinPercent)+
                     " must be less than zoom right percentile, "+ QString::number(clipMaxPercent);
         }
     }
     else {
         result = "Invalid Histogram zoom minimum percentile [0,100]: "+ QString::number(clipMinPercent);
     }
     return result;
}

QString Histogram::setClipMaxPercent( double clipMaxPercent, bool complete ){
     QString result;
     double oldMaxPercent = m_stateData.getValue<double>(CLIP_MAX_PERCENT);
     double clipMinPercent = m_stateData.getValue<double>(CLIP_MIN_PERCENT);
     int significantDigits = m_state.getValue<int>(SIGNIFICANT_DIGITS);
     double clipMaxPercentRounded = Util::roundToDigits( clipMaxPercent, significantDigits );
     if( 0 <= clipMaxPercentRounded && clipMaxPercentRounded <= 100  ){
         double lookupPercent = clipMaxPercentRounded;
         if ( clipMinPercent < lookupPercent ){
             if ( qAbs(clipMaxPercentRounded - oldMaxPercent) > m_errorMargin){
                 m_stateData.setValue<double>(CLIP_MAX_PERCENT, clipMaxPercentRounded );
                 Controller* controller = _getControllerSelected();
                 if ( controller != nullptr ){
                     double clipMax = 0;
                     double decPercent = lookupPercent / 100.0;
                     std::pair<int,int> bound = _getFrameBounds();
                     bool validIntensity = controller->getIntensity(bound.first,bound.second, decPercent, &clipMax);
                     if(validIntensity){
                         double oldClipMax = m_stateData.getValue<double>(CLIP_MAX);
                         if(qAbs(oldClipMax - clipMax) > m_errorMargin){
                             m_stateData.setValue<double>(CLIP_MAX, Util::roundToDigits(clipMax, significantDigits));
                             bool validWidth = _resetBinCountBasedOnWidth();
                             if ( !validWidth ){
                                 m_stateData.setValue<double>(CLIP_MAX, oldClipMax );
                                 m_stateData.setValue<double>(CLIP_MAX_PERCENT, oldMaxPercent );
                                 result = "Resulting histogram bin count exceeded the maximum.";
                             }
                         }
                     }
                     else {
                         result = " Could not map "+ QString::number(lookupPercent)+" to intensity";
                     }
                 }
                 if ( complete ){
                     _finishClips();
                 }
             }
         }
         else {
             result = "Zoom left percentile, "+QString::number(clipMinPercent)+
                     " must be less than zoom right percentile, "+ QString::number( clipMaxPercent);
         }
     }
     else {
         result = "Invalid Histogram zoom maximum percentile [0,100]: "+ QString::number(clipMaxPercent);
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



QString Histogram::setColorMin( double colorMin, bool finish ){
    QString result;
    double oldMin = m_stateData.getValue<double>(COLOR_MIN);
    double colorMax = m_stateData.getValue<double>(COLOR_MAX);
    double oldMinPercent = m_stateData.getValue<double>(COLOR_MIN_PERCENT);
    int significantDigits = m_state.getValue<int>(SIGNIFICANT_DIGITS);
    double colorMinRounded = Util::roundToDigits( colorMin, significantDigits );
    if( colorMinRounded < colorMax ){
        if ( qAbs(colorMinRounded - oldMin) > m_errorMargin){
            m_stateData.setValue<double>(COLOR_MIN, colorMinRounded );
            Controller* controller = _getControllerSelected();
            if ( controller != nullptr ){
                std::pair<int,int> bounds = _getFrameBounds();
                double colorMinPercent = controller->getPercentile( bounds.first, bounds.second, colorMinRounded);
                colorMinPercent = colorMinPercent * 100;
                if ( colorMinPercent >= 0 ){
                    if(qAbs(oldMinPercent - colorMinPercent) > m_errorMargin){
                        m_stateData.setValue<double>(COLOR_MIN_PERCENT, Util::roundToDigits(colorMinPercent,significantDigits ));
                    }
                }
                else {
                    qWarning() << "Could not update clip min percent";
                }
            }
            if ( finish ){
                _finishColor();
            }
        }
    }
    else {
        result = "Clip mininum, "+QString::number(colorMin)+" must be less than maximum, "+QString::number(colorMax);
    }
    return result;

}

QString Histogram::setColorMax( double colorMax, bool finish ){
    QString result;
    double oldMax = m_stateData.getValue<double>(COLOR_MAX);
    double colorMin = m_stateData.getValue<double>(COLOR_MIN);
    double oldMaxPercent = m_stateData.getValue<double>(COLOR_MAX_PERCENT);
    int significantDigits = m_state.getValue<int>(SIGNIFICANT_DIGITS);
    double colorMaxRounded = Util::roundToDigits( colorMax, significantDigits );
    if ( colorMin < colorMaxRounded ){
        if ( qAbs(colorMaxRounded - oldMax) > m_errorMargin){
            m_stateData.setValue<double>(COLOR_MAX, colorMaxRounded );
            Controller* controller = _getControllerSelected();
            if ( controller != nullptr ){
                std::pair<int,int> bounds = _getFrameBounds();
                double colorUpperBound;
                controller->getIntensity( bounds.first, bounds.second, 1, &colorUpperBound );
                double colorMaxPercent = controller->getPercentile(bounds.first, bounds.second, colorMaxRounded );
                if ( colorMaxPercent >= 0 ){
                    colorMaxPercent = colorMaxPercent * 100;
                    if(qAbs(oldMaxPercent - colorMaxPercent) > m_errorMargin){
                        m_stateData.setValue<double>(COLOR_MAX_PERCENT, Util::roundToDigits(colorMaxPercent,significantDigits));
                    }
                }
                else {
                    qWarning() << "Could not update clip max percent!";
                }
            }
            if ( finish ){
                _finishColor();
            }
        }
    }
    else {
        result = "Color mininum bound, "+QString::number(colorMin)+" must be less than maximum, "+QString::number(colorMax);
    }
    return result;
}

QString Histogram::setColorMaxPercent( double colorMaxPercent, bool complete ){
     QString result;
     double oldMaxPercent = m_stateData.getValue<double>(COLOR_MAX_PERCENT);
     double colorMinPercent = m_stateData.getValue<double>(COLOR_MIN_PERCENT);
     int significantDigits = m_state.getValue<int>(SIGNIFICANT_DIGITS);
     double colorMaxPercentRounded = Util::roundToDigits( colorMaxPercent, significantDigits);
     if( 0 <= colorMaxPercentRounded && colorMaxPercentRounded <= 100  ){
         double lookupPercent = colorMaxPercentRounded;
         if ( colorMinPercent < lookupPercent ){
             if ( qAbs(colorMaxPercentRounded - oldMaxPercent) > m_errorMargin){
                 m_stateData.setValue<double>(COLOR_MAX_PERCENT, colorMaxPercentRounded );
                 Controller* controller = _getControllerSelected();
                 if ( controller != nullptr ){
                     double colorMax = 0;
                     double decPercent = lookupPercent / 100.0;
                     std::pair<int,int> bound = _getFrameBounds();
                     bool validIntensity = controller->getIntensity(bound.first,bound.second, decPercent, &colorMax);
                     if(validIntensity){
                         double oldColorMax = m_stateData.getValue<double>(COLOR_MAX);
                         if(qAbs(oldColorMax - colorMax) > m_errorMargin){
                             m_stateData.setValue<double>(COLOR_MAX, Util::roundToDigits(colorMax,significantDigits));
                         }
                     }
                     else {
                         qWarning() <<" Could not map "<<lookupPercent<<" to intensity";
                     }
                 }
                 if ( complete ){
                     _finishColor();
                 }
             }
         }
         else {
             result = "Clip left percentile, "+QString::number(colorMinPercent)+
                     " must be less than clip right percentile, "+ QString::number( colorMaxPercent);
         }
     }
     else {
         result = "Invalid Histogram clip maximum percentile [0,100]: "+ QString::number(colorMaxPercent);
     }
     return result;
}

QString Histogram::setColorMinPercent( double colorMinPercent, bool complete ){
     QString result;
     double oldMinPercent = m_stateData.getValue<double>(COLOR_MIN_PERCENT);
     double colorMaxPercent = m_stateData.getValue<double>(COLOR_MAX_PERCENT);
     int significantDigits = m_state.getValue<int>(SIGNIFICANT_DIGITS);
     double colorMinPercentRounded = Util::roundToDigits( colorMinPercent, significantDigits );
     if( 0 <= colorMinPercentRounded && colorMinPercentRounded <= 100  ){
         if ( colorMinPercentRounded < colorMaxPercent ){
             if ( qAbs(colorMinPercentRounded - oldMinPercent) > m_errorMargin){
                 m_stateData.setValue<double>(COLOR_MIN_PERCENT, colorMinPercentRounded );
                 Controller* controller = _getControllerSelected();
                 if ( controller != nullptr ){
                     double colorMin = 0;
                     double cMin = colorMinPercentRounded / 100.0;
                     std::pair<int,int> bounds = _getFrameBounds();
                     bool validIntensity = controller->getIntensity( bounds.first, bounds.second, cMin, &colorMin);
                     if(validIntensity){
                         double oldColorMin = m_stateData.getValue<double>(COLOR_MIN);
                         if(qAbs(oldColorMin - colorMin) > m_errorMargin){
                             m_stateData.setValue<double>(COLOR_MIN, Util::roundToDigits(colorMin,significantDigits));
                         }
                     }
                     else {
                         qWarning() << "Invalid intensity for percent: "<<colorMinPercent;
                     }
                 }
                 if ( complete ){
                     _finishColor();
                 }
             }
         }
         else {
             result = "Zoom left percentile, "+QString::number(colorMinPercent)+
                     " must be less than zoom right percentile, "+ QString::number(colorMaxPercent);
         }
     }
     else {
         result = "Invalid Histogram zoom minimum percentile [0,100]: "+ QString::number(colorMinPercent);
     }
     return result;
}

QString Histogram::setBinWidth( double binWidth ){
    QString result;
    double oldBinWidth = m_state.getValue<double>(BIN_WIDTH);
    int significantDigits = m_state.getValue<int>(SIGNIFICANT_DIGITS);
    double binWidthRounded = Util::roundToDigits( binWidth, significantDigits );
    if ( binWidthRounded > 0 ){
        if ( qAbs( oldBinWidth - binWidthRounded) > m_errorMargin ){
            m_state.setValue<double>( BIN_WIDTH, binWidthRounded );
            bool validWidth = _resetBinCountBasedOnWidth();
            if ( !validWidth ){
                result = "Resulting histogram bin count exceeded the maximum.";
                m_state.setValue<double>( BIN_WIDTH, oldBinWidth );
            }
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
        int binCountMax = m_state.getValue<int>(BIN_COUNT_MAX);
        if ( binCount <= binCountMax ){
            if ( binCount != oldBinCount ){
                m_state.setValue<int>(BIN_COUNT, binCount );
                double binWidth = _toBinWidth( binCount );

                int significantDigits = m_state.getValue<int>(SIGNIFICANT_DIGITS);
                m_state.setValue<double>(BIN_WIDTH, Util::roundToDigits(binWidth,significantDigits) );
                m_state.flushState();
                _generateHistogram( true );
            }
        }
        else {
            result = "The bin count value, "+QString::number(binCount)+" is larger than the bin count limit: "+binCountMax;
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
    QString actualPlaneMode = _getActualPlaneMode( planeModeStr );
    if ( actualPlaneMode != "" ){
        if ( actualPlaneMode != oldPlaneMode ){
            m_state.setValue<QString>(PLANE_MODE, actualPlaneMode );
            m_state.flushState();
            _generateHistogram( true );
        }
    }
    else {
        result = "Unrecognized histogram cube mode: "+ planeModeStr;
    }
    return result;
}



QString Histogram::saveHistogram( const QString& fileName ){
    QString result = "";
    //Check and make sure the directory exists.
    int dirIndex = fileName.lastIndexOf( QDir::separator() );
    QString dirName = fileName;
    if ( dirIndex >= 0 ){
        dirName = fileName.left( dirIndex );
    }
    QDir dir( dirName );
    if ( ! dir.exists() ){
        result = "Please make sure the save path is valid: "+fileName;
    }
    else {
        PreferencesSave* prefSave = Util::findSingletonObject<PreferencesSave>();
        int width = prefSave->getWidth();
        int height = prefSave->getHeight();
        Qt::AspectRatioMode aspectRatioMode = prefSave->getAspectRatioMode();
        QImage* histogramImage = m_histogram->toImage();
        QSize outputSize( width, height );
        QImage imgScaled = histogramImage->scaled( outputSize, aspectRatioMode );
        bool saveSuccessful = imgScaled.save( fileName );
        if ( !saveSuccessful ){
            result = "The image could not be saved; please check the path: "+fileName+" is valid.";
        }
        delete histogramImage;
    }
    return result;
}

QString Histogram::_set2DFootPrint( const QString& params ){
    QString result;
    std::set<QString> keys = {FOOT_PRINT};
    std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
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

QString Histogram::setPlaneRange( double planeMin, double planeMax){
    QString result;
    if ( planeMin >= 0 && planeMax >= 0 ){
        double storedMin = m_stateData.getValue<double>(PLANE_MIN);
        double storedMax = m_stateData.getValue<double>(PLANE_MAX);
        if ( planeMin <= planeMax ){
            int significantDigits = m_state.getValue<int>(SIGNIFICANT_DIGITS);
            bool changedState = false;
            if ( qAbs(planeMin - storedMin) > m_errorMargin){
                m_stateData.setValue<double>(PLANE_MIN, Util::roundToDigits(planeMin, significantDigits ));
                changedState = true;
            }

            if ( qAbs(planeMax - storedMax) > m_errorMargin ){
                m_stateData.setValue<double>(PLANE_MAX, Util::roundToDigits(planeMax, significantDigits) );
                changedState = true;
            }
            if ( changedState ){
                m_stateData.flushState();
                QString planeMode = m_state.getValue<QString>(PLANE_MODE);
                if ( planeMode == PLANE_MODE_RANGE ){
                    _generateHistogram( true );
                }
            }
        }
        else {
            result = "The frequency range minimum " + QString::number(planeMin)+" must be less than or equal to the maximum: "+QString::number(planeMax);
        }
    }
    else {
        result = "Frequency min "+ QString::number(planeMin)+" and max "+QString::number(planeMax)+" must both be nonnegative.";
    }
    return result;
}

QString Histogram::setChannelUnit( const QString& units ){
    QString result;
    QString oldUnits = m_state.getValue<QString>(FREQUENCY_UNIT);
    int unitIndex = m_channelUnits->getIndex( units );
    if ( unitIndex >= 0 ){
        QString unitsProperCase = m_channelUnits->getUnit( unitIndex );
        if ( oldUnits != unitsProperCase ){
            m_state.setValue<QString>(FREQUENCY_UNIT, unitsProperCase );
            double oldPlaneMin = m_stateData.getValue<double>(PLANE_MIN);
            double planeMin = m_channelUnits->convert( oldUnits, unitsProperCase, oldPlaneMin );
            double oldPlaneMax = m_stateData.getValue<double>(PLANE_MAX);
            double planeMax = m_channelUnits->convert( oldUnits, unitsProperCase, oldPlaneMax );
            result = setPlaneRange( planeMin, planeMax );
            m_stateData.flushState();
            m_state.flushState();
        }
    }
    else {
        result = "Unrecognized histogram range units: "+ units;
    }
    return result;
}

QString Histogram::setSignificantDigits( int digits ){
    QString result;
    if ( digits <= 0 ){
        result = "Invalid significant digits; must be positive:  "+QString::number( digits );
    }
    else {
        if ( m_state.getValue<int>(SIGNIFICANT_DIGITS) != digits ){
            m_state.setValue<int>(SIGNIFICANT_DIGITS, digits );
            _setErrorMargin();
        }
    }
    return result;
}

void Histogram::_setErrorMargin(){
    int significantDigits = m_state.getValue<int>(SIGNIFICANT_DIGITS );
    m_errorMargin = 1.0/qPow(10,significantDigits);
}

QString Histogram::setGraphStyle( const QString& styleStr ){
    QString result;
    QString oldStyle = m_state.getValue<QString>(GRAPH_STYLE);
    QString actualStyle = _getActualGraphStyle( styleStr );
    if ( actualStyle != "" ){
        if ( actualStyle != oldStyle ){
            m_state.setValue<QString>(GRAPH_STYLE, actualStyle );
            m_state.flushState();
            _generateHistogram( false );
        }
    }
    else {
        result = "Unrecognized histogram graph style: "+ styleStr;
    }
    return result;
}

QString Histogram::_getActualGraphStyle( const QString& styleStr ){
    QString result = "";
    if ( QString::compare( styleStr, GRAPH_STYLE_LINE, Qt::CaseInsensitive) == 0 ){
        result = GRAPH_STYLE_LINE;
    }
    else if ( QString::compare( styleStr, GRAPH_STYLE_OUTLINE, Qt::CaseInsensitive) == 0 ){
        result = GRAPH_STYLE_OUTLINE;
    }
    else if ( QString::compare( styleStr, GRAPH_STYLE_FILL, Qt::CaseInsensitive) == 0 ){
        result = GRAPH_STYLE_FILL;
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

bool Histogram::getUseClipBuffer(){
    bool useBuffer = m_state.getValue<bool>(CLIP_BUFFER);
    return useBuffer;
}

void Histogram::_startSelection(const QString& params ){
    std::set<QString> keys = {X_COORDINATE};
    std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
    QString xstr = dataValues[X_COORDINATE];
    m_selectionEnabled = true;
    m_selectionStart = xstr.toDouble();
    m_histogram->setSelectionMode( true );
}

void Histogram::_startSelectionColor(const QString& params ){
    bool customClips = m_state.getValue<bool>(CUSTOM_CLIP );
    if ( customClips ){
        std::set<QString> keys = {X_COORDINATE};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString xstr = dataValues[X_COORDINATE];
        m_selectionEnabledColor = true;
        m_selectionStart = xstr.toDouble();
        m_histogram->setSelectionModeColor( true );
    }
}

double Histogram::_toBinWidth( int count ) const {
    double minRange = m_stateData.getValue<double>( CLIP_MIN);
    double maxRange = m_stateData.getValue<double>( CLIP_MAX);
    double width = 0;
    if ( count > 0 ){
        width = qAbs( maxRange - minRange) / count;
    }
    return width;
}

int Histogram::_toBinCount( double width ) const {
    double minRange = m_stateData.getValue<double>(CLIP_MIN );
    double maxRange = m_stateData.getValue<double>(CLIP_MAX);
    int count = 0;
    if ( width > 0 ){
        count = qCeil( qAbs( maxRange - minRange)  / width);
    }
    return count;
}

void Histogram::_updateChannel( Controller* controller ){
    setCubeChannel( controller->getFrameChannel());
    QString mode = m_state.getValue<QString>(PLANE_MODE);
    if ( mode == PLANE_MODE_SINGLE ){
        _generateHistogram(true, controller );
    }
}


void Histogram::_updateSelection(int x){
    m_selectionEnd = x;
    if ( m_selectionEnabled || m_selectionEnabledColor ){
        _generateHistogram( false );
    }
}


void Histogram::updateColorMap( Colormap* map ){
    if ( map != nullptr ){
        Controller* controller = _getControllerSelected();
        if ( controller != nullptr ){
            map->setColorProperties( controller );
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



void Histogram::_updateColorSelection(){
    bool valid = false;
    std::pair<double,double> range = m_histogram->getRangeColor( &valid );
    if ( valid ){
        double minRange = range.first;
        double maxRange = range.second;
        if ( range.first > range.second ){
            minRange = range.second;
            maxRange = range.first;
        }
        setRangeColor( minRange, maxRange );
    }
    else {
        _generateHistogram( valid );
    }
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
    unregisterView();
    if ( m_preferences != nullptr ){
        Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
        QString id = m_preferences->getId();
        objMan->removeObject( id );
    }
    delete m_histogram;
}
}
}
