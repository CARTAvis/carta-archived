#include "Histogram.h"
#include "Render/HistogramRenderService.h"
#include "Data/Clips.h"
#include "Data/Colormap/Colormap.h"
#include "ChannelUnits.h"
#include "Data/Settings.h"
#include "Data/LinkableImpl.h"
#include "Data/Image/Controller.h"
#include "Data/Image/DataSource.h"
#include "Data/Error/ErrorManager.h"
#include "Data/Util.h"
#include "Data/Plotter/Plot2DManager.h"
#include "Plot2D/Plot2DGenerator.h"
#include "Data/Histogram/BinData.h"
#include "Data/Histogram/PlotStyles.h"
#include "Data/Preferences/PreferencesSave.h"
#include "Data/Region/RegionControls.h"
#include "Data/Region/Region.h"
#include "Globals.h"
#include "MainConfig.h"
#include "PluginManager.h"

#include "CartaLib/Hooks/Histogram.h"
#include "CartaLib/AxisInfo.h"
#include "CartaLib/PixelPipeline/IPixelPipeline.h"
#include "CartaLib/Regions/IRegion.h"
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
const QString Histogram::CLIP_MIN_CLIENT = "clipMinClient";
const QString Histogram::CLIP_MAX_CLIENT = "clipMaxClient";
const QString Histogram::CLIP_APPLY = "applyClipToImage";
const QString Histogram::BIN_COUNT = "binCount";
const QString Histogram::BIN_COUNT_MAX = "binCountMax";
const int Histogram::BIN_COUNT_MAX_VALUE = 10000;
const QString Histogram::BIN_WIDTH = "binWidth";
const QString Histogram::COLOR_MIN = "colorMin";
const QString Histogram::COLOR_MAX = "colorMax";
const QString Histogram::COLOR_MIN_PERCENT = "colorMinPercent";
const QString Histogram::COLOR_MAX_PERCENT = "colorMaxPercent";
const QString Histogram::GRAPH_STYLE = "graphStyle";
const QString Histogram::GRAPH_LOG_COUNT = "logCount";
const QString Histogram::GRAPH_COLORED = "colored";
const QString Histogram::PLANE_MODE="planeMode";
const QString Histogram::PLANE_MODE_SINGLE="Current";
const QString Histogram::PLANE_MODE_CHANNEL ="Channel";
const QString Histogram::PLANE_MODE_RANGE="Range";
const QString Histogram::PLANE_MODE_RANGE_VALID = "image3D";
const QString Histogram::PLANE_MODE_ALL="All";
const QString Histogram::PLANE_MIN = "planeMin";
const QString Histogram::PLANE_MAX = "planeMax";
const QString Histogram::PLANE_CHANNEL = "planeChannel";
const QString Histogram::PLANE_CHANNEL_MAX = "planeChannelMax";
const QString Histogram::FOOT_PRINT = "twoDFootPrint";
const QString Histogram::FOOT_PRINT_IMAGE = "Image";
const QString Histogram::FOOT_PRINT_REGION = "Selected Region";
const QString Histogram::FOOT_PRINT_REGION_ALL = "All Regions";
const QString Histogram::FREQUENCY_UNIT = "rangeUnit";
const QString Histogram::CLIP_MIN_PERCENT = "clipMinPercent";
const QString Histogram::CLIP_MAX_PERCENT = "clipMaxPercent";
const QString Histogram::SIZE_ALL_RESTRICT ="limitCubeSize";
const QString Histogram::RESTRICT_SIZE_MAX = "cubeSizeMax";

Clips*  Histogram::m_clips = nullptr;
PlotStyles* Histogram::m_graphStyles = nullptr;
ChannelUnits* Histogram::m_channelUnits = nullptr;
QList<QColor> Histogram::m_curveColors = {Qt::blue, Qt::green, Qt::black, Qt::cyan,
        Qt::magenta, Qt::yellow, Qt::gray };

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
using Carta::Plot2D::Plot2DGenerator;

Histogram::Histogram( const QString& path, const QString& id):
            CartaObject( CLASS_NAME, path, id ),
            m_linkImpl( new LinkableImpl( path )),
            m_preferences( nullptr),
            m_plotManager( new Plot2DManager( path, id ) ),
            m_renderService( new HistogramRenderService()),
            m_stateData( UtilState::getLookup(path, StateInterface::STATE_DATA)) {

    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    Settings* prefObj = objMan->createObject<Settings>();
    m_preferences.reset( prefObj );

    connect( m_renderService.get(),
            SIGNAL(histogramResult(const Carta::Lib::Hooks::HistogramResult& )),
            this,
            SLOT(_histogramRendered(const Carta::Lib::Hooks::HistogramResult& )));
    Plot2DGenerator* gen = new Plot2DGenerator();
    gen->setHistogram( true, 0 );
    m_plotManager->setPlotGenerator( gen );
    m_plotManager->setTitleAxisY( "Count(pixels)" );
    m_plotManager->setTitleAxisX( "Intensity" );
    connect( m_plotManager.get(), SIGNAL(userSelection()), this, SLOT(_zoomToSelection()));
    connect( m_plotManager.get(), SIGNAL(userSelectionColor()), this, SLOT( _updateColorSelection()));

    _initializeStatics();
    _initializeDefaultState();
    _initializeCallbacks();
    _setErrorMargin();

    m_controllerLinked = false;
    m_cubeChannel = 0;

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
                connect(controller, SIGNAL(dataChanged(Controller*)),
                		this, SLOT(_createHistogram(Controller*)));
                connect(controller, SIGNAL(dataChangedRegion(Controller*)),
                               		this, SLOT(_createHistogram(Controller*)));
                connect( controller,SIGNAL(frameChanged(Controller*, Carta::Lib::AxisInfo::KnownType)),
                        this, SLOT(_updateChannel(Controller*, Carta::Lib::AxisInfo::KnownType)));
                connect(controller, SIGNAL(clipsChanged(double,double)),
                        this, SLOT(_updateColorClips(double,double)));
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

void Histogram::_assignColor( std::shared_ptr<BinData> binData ){
    //First go through list of fixed colors & see if there is one available.
    int fixedColorCount = m_curveColors.size();
    int curveCount = m_binDatas.size();
    bool colorAssigned = false;
    for ( int i = 0; i < fixedColorCount; i++ ){
        bool colorAvailable = true;
        QString fixedColorName = m_curveColors[i].name();
        for ( int j = 0; j < curveCount; j++ ){
            if ( m_binDatas[j]->getColor().name() == fixedColorName ){
                colorAvailable = false;
                break;
            }
        }
        if ( colorAvailable ){
            binData->setColor( m_curveColors[i] );
            colorAssigned = true;
            break;
        }
    }

    //If there is no color in the fixed list, assign a random one.
    if ( !colorAssigned ){
        const int MAX_COLOR = 255;
        int redAmount = qrand() % MAX_COLOR;
        int greenAmount = qrand() % MAX_COLOR;
        int blueAmount = qrand() % MAX_COLOR;
        QColor randomColor( redAmount, greenAmount, blueAmount );
        binData->setColor( randomColor.name());
    }
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
       double colorMin = m_stateData.getValue<double>( COLOR_MIN );
       double colorMax = m_stateData.getValue<double>( COLOR_MAX );
       emit colorIntensityBoundsChanged( colorMin, colorMax );
   }
}

void Histogram::clear(){
    unregisterView();
    m_linkImpl->clear();
}


void Histogram::_createHistogram( Controller* controller){
    std::pair<int,int> frameBounds = _getFrameBounds();
    std::vector<double> percentiles(2);
    percentiles[0] = 0;
    percentiles[1] = 1;
    std::vector<std::pair<int,double> > intensities = controller->getIntensity( frameBounds.first, frameBounds.second,
            percentiles );
    if( intensities.size() == 2 && intensities[0].first>= 0 && intensities[1].first >= 0 ){
        int significantDigits = m_state.getValue<int>(Util::SIGNIFICANT_DIGITS);
        double minIntensity = Util::roundToDigits( intensities[0].second, significantDigits );
        double maxIntensity = Util::roundToDigits( intensities[1].second, significantDigits );
        m_stateData.setValue<double>(CLIP_MIN_PERCENT, 0 );
        m_stateData.setValue<double>(CLIP_MAX_PERCENT, 100 );
        m_stateData.setValue<double>(CLIP_MIN, minIntensity);
        m_stateData.setValue<double>(CLIP_MAX, maxIntensity);
        m_stateData.setValue<double>(CLIP_MIN_CLIENT, minIntensity );
        m_stateData.setValue<double>(CLIP_MAX_CLIENT, maxIntensity );

        double binWidth = qAbs( maxIntensity - minIntensity ) / m_state.getValue<int>(BIN_COUNT);
        setBinWidth( binWidth );

        int frameCount = controller->getFrameUpperBound(Carta::Lib::AxisInfo::KnownType::SPECTRAL) - 1;
        bool planeModeValid = false;
        if ( frameCount > 0 ){
            planeModeValid = true;
        }

        bool oldPlaneModeValid = m_stateData.getValue<bool>(PLANE_MODE_RANGE_VALID );
        if ( planeModeValid != oldPlaneModeValid ){
            m_stateData.setValue<bool>(PLANE_MODE_RANGE_VALID, planeModeValid);
        }

        int oldPlaneMax = m_stateData.getValue<int>( PLANE_CHANNEL_MAX );
        if ( oldPlaneMax != frameCount ){
            m_stateData.setValue<int>(PLANE_CHANNEL_MAX, frameCount );
        }

        double colorMinPercent = controller->getClipPercentileMin();
        double colorMaxPercent = controller->getClipPercentileMax();
        _updateColorClips( colorMinPercent, colorMaxPercent );


        m_stateData.flushState();
    }
    _generateHistogram( controller );
}


void Histogram::_finishClips (){
    m_stateData.flushState();
    m_plotManager->clearSelectionColor();
    _generateHistogram( nullptr );
}

void Histogram::_finishColor(){
    double colorMin = m_stateData.getValue<double>(COLOR_MIN);
    double colorMax = m_stateData.getValue<double>(COLOR_MAX);
    m_plotManager->setRangeColor( colorMin, colorMax );
    m_stateData.flushState();
}


void Histogram::_generateHistogram( Controller* controller ){
    Controller* activeController = controller;
    if ( !activeController  ){
        activeController = _getControllerSelected();
    }
    if ( activeController ){
        _loadData( activeController );
    }


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
                std::pair<int,int> frameBounds = _getFrameBounds();
                std::vector<double> percentiles(1);
                percentiles[0] = percentile;
                std::vector<std::pair<int,double> > intensities = controller->getIntensity(
                        frameBounds.first, frameBounds.second, percentiles );
                if ( intensities.size() == 1 && intensities[0].first>= 0 ){
                    intensity = intensities[0].second;
                }
            }
        }
    }
    return intensity;
}

std::pair<double, double> Histogram::getClipRange() const {
    double clipMin = m_stateData.getValue<double>(CLIP_MIN);
    double clipMax = m_stateData.getValue<double>(CLIP_MAX);
    std::pair<double, double> clipRangeValues(clipMin, clipMax);
    return clipRangeValues;
}

bool Histogram::getColored() const {
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

QString Histogram::getFootPrint2D() const {
	return m_state.getValue<QString>( FOOT_PRINT );
}

QList<QString> Histogram::getLinks() const {
    return m_linkImpl->getLinkIds();
}

bool Histogram::getLogCount() const {
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
    else if ( QString::compare( planeModeStr, PLANE_MODE_CHANNEL, Qt::CaseInsensitive) == 0 ){
        result = PLANE_MODE_CHANNEL;
    }
    else if ( QString::compare( planeModeStr, PLANE_MODE_RANGE, Qt::CaseInsensitive) == 0 ){
        result = PLANE_MODE_RANGE;
    }
    return result;
}

QString Histogram::_getPreferencesId() const {
    return m_preferences->getPath();
}

QString Histogram::getSnapType(CartaObject::SnapshotType snapType) const {
    QString objType = CartaObject::getSnapType( snapType );
    if ( snapType == SNAPSHOT_DATA ){
        objType = objType + Carta::State::StateInterface::STATE_DATA;
    }
    return objType;
}

bool Histogram::getUseClipBuffer(){
    bool useBuffer = m_state.getValue<bool>(CLIP_BUFFER);
    return useBuffer;
}

void Histogram::_histogramRendered(const Carta::Lib::Hooks::HistogramResult& result){
	QString resultName = result.getName();
	if ( resultName.startsWith( Util::ERROR)){
		ErrorManager* hr = Util::findSingletonObject<ErrorManager>();
		hr->registerError( resultName );
	}
	else {
		//Only create a new one if there is not an existing one that
		//matches.
		int targetIndex = -1;
		int dataCount = m_binDatas.size();
		for ( int i = 0; i < dataCount; i++ ){
			if ( m_binDatas[i]->getName() == resultName ){
				targetIndex = i;
				break;
			}
		}
		//Add the result to our list of plots.
		if ( targetIndex == -1 ){
			Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
			CartaObject* binObj = objMan->createObject<BinData>();
			std::shared_ptr<BinData> binData(dynamic_cast<BinData*>(binObj));
			_assignColor( binData );
			binData->setName( resultName );
			m_binDatas.append( binData );
			targetIndex = m_binDatas.size() - 1;
		}
		m_binDatas[targetIndex]->setHistogramResult( result );

		double freqLow = result.getFrequencyMin();
		double freqHigh = result.getFrequencyMax();
		setPlaneRange( freqLow, freqHigh);
		_updatePlots();
	}
}



void Histogram::_initializeDefaultState(){

    //Data State - likely to change with a different image
    m_stateData.insertValue<double>( CLIP_MIN, 0 );
    m_stateData.insertValue<double>(CLIP_MAX, 1);
    //Difference between CLIP_MIN and CLIP_MIN_CLIENT is that CLIP_MIN
    //will never be less than the image minimum intensity.  The CLIP_MIN_CLIENT
    //will mostly mirror CLIP_MIN, but may be less than the image minimum intensity
    //if the user wants to zoom out for some reason.
    m_stateData.insertValue<double>( CLIP_MIN_CLIENT, 0 );
    m_stateData.insertValue<double>( CLIP_MAX_CLIENT, 1 );
    m_stateData.insertValue<int>(CLIP_BUFFER_SIZE, 10 );
    m_stateData.insertValue<double>(COLOR_MIN, 0 );
    m_stateData.insertValue<double>(COLOR_MAX, 1 );
    m_stateData.insertValue<int>(COLOR_MIN_PERCENT, 0 );
    m_stateData.insertValue<int>(COLOR_MAX_PERCENT, 100 );
    m_stateData.insertValue<double>(CLIP_MIN_PERCENT, 0);
    m_stateData.insertValue<double>(CLIP_MAX_PERCENT, 100);
    m_stateData.insertValue<double>(PLANE_MIN, 0 );
    m_stateData.insertValue<double>(PLANE_MAX, 1 );
    m_stateData.insertValue<int>(PLANE_CHANNEL, 0 );
    m_stateData.insertValue<int>(PLANE_CHANNEL_MAX, 0 );
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
    m_state.insertValue<bool>(CLIP_APPLY, false );
    m_state.insertValue<bool>(CLIP_BUFFER, false);
    QString defaultStyle = m_graphStyles->getDefault();
    m_state.insertValue<QString>(GRAPH_STYLE, defaultStyle );
    m_state.insertValue<bool>(GRAPH_LOG_COUNT, true );
    m_state.insertValue<bool>(GRAPH_COLORED, false );
    m_state.insertValue<QString>(PLANE_MODE, PLANE_MODE_ALL );
    m_state.insertValue<QString>(FREQUENCY_UNIT, m_channelUnits->getDefaultUnit());
    m_state.insertValue<bool>(SIZE_ALL_RESTRICT, true );
    m_state.insertValue<int>(RESTRICT_SIZE_MAX, 1000000 );
    m_state.insertValue<QString>(FOOT_PRINT, FOOT_PRINT_IMAGE );
    m_state.insertValue<int>(Util::SIGNIFICANT_DIGITS, 6 );
    //Default Tab
    m_state.insertValue<int>( Util::TAB_INDEX, 1 );
    m_state.flushState();

}

void Histogram::_initializeCallbacks(){

    addCommandCallback( "setBinCount", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {BIN_COUNT};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString binCountStr = dataValues[*keys.begin()];
        bool validInt = false;
        int binCount = binCountStr.toInt( &validInt );
        if ( validInt ){
            //Request was made to put slider on a logarithmic scale.  So for
            //example, bin count should be 1 when the slider is at 0%, at 10
            //when it is at 25%, 100 at 50%, 1000 at 75% and 10000 at 100%.
            //Issue #67
            int maxBinCount = m_state.getValue<int>(BIN_COUNT_MAX);
            double logMaxCount = qLn( maxBinCount ) / qLn(10);
            double percentage  = (binCount*1.0) / maxBinCount;
            double percentLogMax = percentage * logMaxCount;
            int scaledBinCount = static_cast<int>( qPow(10, percentLogMax) );
            result = setBinCount( scaledBinCount );
        }
        else {
            result = "Invalid bin count: "+params+" must be a valid integer";
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "registerPreferences", [=] (const QString & /*cmd*/,
                    const QString & /*params*/, const QString & /*sessionId*/) -> QString {
                QString result = _getPreferencesId();
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
            qDebug() << "grimmer histogram setClipValue";

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

    addCommandCallback( "setCubeLimit", [=] (const QString & /*cmd*/,
               const QString & params, const QString & /*sessionId*/) -> QString {
           QString result;
           std::set<QString> keys = {SIZE_ALL_RESTRICT};
           std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
           QString useCubeLimitStr = dataValues[*keys.begin()];
           bool validBool = false;
           bool cubeLimit = Util::toBool( useCubeLimitStr, &validBool );
           if ( validBool ){
               setLimitCubes( cubeLimit );
           }
           else {
               result = "Placing a limit on cube sizes must be true/false:"+params;
           }
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

    addCommandCallback( "setCubeSizeLimit", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
            QString result;
            std::set<QString> keys = {RESTRICT_SIZE_MAX};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString sizeLimitStr = dataValues[RESTRICT_SIZE_MAX];
            bool validLimit = false;
            int sizeLimit = sizeLimitStr.toInt( &validLimit );
            if ( validLimit ){
                result = setCubeSizeLimit( sizeLimit );
            }
            else {
                result = "Cube size limit must be a positive integer: "+params;
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

    addCommandCallback( "setPlaneChannel", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
            QString result;
            std::set<QString> keys = {PLANE_CHANNEL};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString planeStr = dataValues[PLANE_CHANNEL];
            bool validPlane = false;
            double plane = planeStr.toInt( &validPlane );
            if ( validPlane ){
                result = setPlaneChannel( plane );
            }
            else {
                result = "Plane channel must be a number: "+params;
            }
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

    addCommandCallback( "setTabIndex", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
            QString result;
            std::set<QString> keys = {Util::TAB_INDEX};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString tabIndexStr = dataValues[Util::TAB_INDEX];
            bool validIndex = false;
            int tabIndex = tabIndexStr.toInt( &validIndex );
            if ( validIndex ){
                result = setTabIndex( tabIndex );
            }
            else {
                result = "Please check that the tab index is a number: " + params;
            }
            Util::commandPostProcess( result );
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
            std::set<QString> keys = {Util::SIGNIFICANT_DIGITS};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString digitsStr = dataValues[Util::SIGNIFICANT_DIGITS];
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
        QString result = _zoomToSelection();
        Util::commandPostProcess( result );
        return result;
    });
}

void Histogram::_initializeStatics(){
    if ( m_channelUnits == nullptr ){
        m_channelUnits = Util::findSingletonObject<ChannelUnits>();
    }
    if ( m_graphStyles == nullptr ){
        m_graphStyles = Util::findSingletonObject<PlotStyles>();
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
    if( ! controller) {
        return;
    }

    int binCount = m_state.getValue<int>(BIN_COUNT)+1;
    double minFrequency = -1;
    double maxFrequency = -1;
    QString rangeUnits = m_state.getValue<QString>(FREQUENCY_UNIT );
    QString planeMode = m_state.getValue<QString>(PLANE_MODE);
    if ( planeMode == PLANE_MODE_RANGE ){
        minFrequency = m_stateData.getValue<double>(PLANE_MIN);
        maxFrequency = m_stateData.getValue<double>(PLANE_MAX);
    }
    std::shared_ptr<DataSource> dataSource = controller->getDataSource();
    if ( dataSource ) {
        std::shared_ptr<Carta::Lib::Image::ImageInterface> image= dataSource->_getImage();
        std::vector<std::shared_ptr<Region> > regions;
        QString footPrint = getFootPrint2D();
        std::shared_ptr<RegionControls> regionControls = controller->getRegionControls();
        if ( footPrint == FOOT_PRINT_REGION ){
        	std::shared_ptr<Region> region = regionControls->getRegion("");
        	if ( region ){
        		regions.push_back( region );
        	}
        }
        else if ( footPrint == FOOT_PRINT_REGION_ALL ){
        	regions = regionControls->getRegions();
        }
        std::vector<int> imageDims = image->dims();
        int dimCount = imageDims.size();
        int pixelSize = 1;
        for ( int i = 0; i < dimCount; i++ ){
            pixelSize = pixelSize * imageDims[i];
        }
        std::pair<int,int> frameBounds = _getFrameBounds();
        int minChannel = frameBounds.first;
        int maxChannel = frameBounds.second;
        bool restrictSize = m_state.getValue<bool>(SIZE_ALL_RESTRICT);
        bool sizeExcessive = false;
        if ( m_state.getValue<int>(RESTRICT_SIZE_MAX) <= pixelSize ){
            sizeExcessive = true;
        }
        bool singleDefault = (planeMode == PLANE_MODE_ALL) && restrictSize &&
               sizeExcessive;
        if ( planeMode == PLANE_MODE_CHANNEL || singleDefault ){
            int chan = m_stateData.getValue<int>( PLANE_CHANNEL );
            minChannel = chan;
            maxChannel = chan;
        }
        double minIntensity = _getBufferedIntensity( CLIP_MIN, CLIP_MIN_PERCENT );
        double maxIntensity = _getBufferedIntensity( CLIP_MAX, CLIP_MAX_PERCENT );

        std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> pipeline = dataSource->_getPipeline();

        m_plotManager->setPipeline( pipeline );
        int regionCount = regions.size();
        QString fileName = dataSource->_getFileName();
        std::vector<HistogramRenderRequest> requests;
        if ( regionCount == 0 ){
        	std::shared_ptr<Carta::Lib::Regions::RegionBase> nullRegion( nullptr );
        	HistogramRenderRequest request( image, binCount, minChannel, maxChannel,
        				minFrequency, maxFrequency, rangeUnits, minIntensity, maxIntensity,
        				fileName, nullRegion, "!" );
        	//Only make a new one if we don't already have this one stored since making a
        	//histogram is data intensive.
        	requests.push_back( request );
        }
        else {
        	for ( int i = 0; i < regionCount; i++ ){
        		std::shared_ptr<Carta::Lib::Regions::RegionBase> regionBase = regions[i]->getModel();
        		QString idStr = regions[i]->getId();
        		HistogramRenderRequest request( image, binCount, minChannel, maxChannel,
        		        				minFrequency, maxFrequency, rangeUnits, minIntensity, maxIntensity,
        		        				fileName, regionBase, idStr );
        		requests.push_back( request );
        	}
        }

        //Update the list of bin data which should be removed (no longer needed).  Only return
        //requests that are not already stored.
        std::vector<HistogramRenderRequest> renderRequests = _updateBinDatas( requests );
        int renderRequestCount = renderRequests.size();
        if ( renderRequestCount == 0 ){
        	//Clear the plot manager and put the existing data into it.
        	_updatePlots();
        }
        else {
        	for ( int i = 0; i < renderRequestCount; i++ ){
        		m_renderService->renderHistogram( renderRequests[i] );
        	}
        }
    }
    else {
    	int dataCount = this->m_binDatas.size();
    	for ( int i = (dataCount - 1); i>= 0; i-- ){
    		_removeData( i );
    	}

        _resetDefaultStateData();
        _updatePlots();
    }
}

void Histogram::refreshState() {
    CartaObject::refreshState();
    m_stateData.refreshState();
    m_preferences->refreshState();
    m_linkImpl->refreshState();
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
   m_stateData.setValue<double>(CLIP_MIN_CLIENT, 0 );
   m_stateData.setValue<double>(CLIP_MAX_CLIENT, 1 );
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



void Histogram::_removeData( int index ){
	int dataCount = m_binDatas.size();
	if ( index >= 0 && index < dataCount ){
		Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
		objMan->removeObject( m_binDatas[index]->getId());
		m_binDatas.removeAt( index );
	}
}

QString Histogram::setClipBuffer( int bufferAmount ){
    QString result;
    if ( bufferAmount >= 0 && bufferAmount < 100 ){
        int oldBufferAmount = m_stateData.getValue<int>( CLIP_BUFFER_SIZE);
        if ( oldBufferAmount != bufferAmount ){
            m_stateData.setValue<int>( CLIP_BUFFER_SIZE, bufferAmount );
            m_stateData.flushState();
            _generateHistogram( nullptr );
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



QString Histogram::setCubeSizeLimit(  int sizeLimit ){
    QString result;
    if ( sizeLimit <= 0 ){
        result = "The cube size limit must be a positive integer.";
    }
    else {
        int oldLimit = m_state.getValue<int>( RESTRICT_SIZE_MAX );
        if ( oldLimit != sizeLimit ){
            m_state.setValue<int>( RESTRICT_SIZE_MAX, sizeLimit );
            m_state.flushState();
            _generateHistogram( nullptr );
        }
    }
    return result;
}

void Histogram::setLimitCubes( bool limitCubes ){
    bool oldLimitCubes = m_state.getValue<bool>( SIZE_ALL_RESTRICT );
    if ( limitCubes != oldLimitCubes ){
        m_state.setValue<bool>( SIZE_ALL_RESTRICT, limitCubes );
        m_state.flushState();
        _generateHistogram( nullptr );
    }
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
                    if ( clipMinPercent == 0 && clipMaxPercent == 100 ){
                        m_plotManager->clearSelection();
                    }
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

QString Histogram::setClipMax( double clipMaxClient, bool finish ){
    QString result;

    double oldMax = m_stateData.getValue<double>(CLIP_MAX);
    double oldMaxClient = m_stateData.getValue<double>( CLIP_MAX_CLIENT );
    double clipMinClient = m_stateData.getValue<double>( CLIP_MIN_CLIENT );
    double oldMaxPercent = m_stateData.getValue<double>(CLIP_MAX_PERCENT);
    //Bypass the check that the new max will be larger than the old min if we
    //are not finished and are also planning to set a new min.
    if ( clipMinClient < clipMaxClient || !finish ){
        double adjustedMax = clipMaxClient;
        double imageMaxIntensity = 0;
        Controller* controller = _getControllerSelected();
        if ( controller != nullptr ){
            std::vector<double> percentiles(1);
            percentiles[0] = 1;
            std::vector<std::pair<int,double> > intensities = controller->getIntensity( percentiles);
            if ( intensities.size() == 1 && intensities[0].first>= 0 ){
                imageMaxIntensity = intensities[0].second;
                if ( clipMaxClient < imageMaxIntensity ){
                    adjustedMax = imageMaxIntensity;
                }
            }

            //Set the max clip that will not exceed the maximum intensity of the image.
            int significantDigits = m_state.getValue<int>(Util::SIGNIFICANT_DIGITS);
            double clipMaxRounded = Util::roundToDigits( adjustedMax, significantDigits );
            if ( qAbs(clipMaxRounded - oldMax) > m_errorMargin){
                m_stateData.setValue<double>(CLIP_MAX, clipMaxRounded );
                bool validWidth = _resetBinCountBasedOnWidth();
                if ( validWidth ){
                    Controller* controller = _getControllerSelected();
                    if ( controller != nullptr ){
                        std::pair<int,int> bounds = _getFrameBounds();
                        double clipMaxPercent = controller->getPercentile(bounds.first, bounds.second,
                                clipMaxClient );
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

                //If the max clip value was successfully set, set the one requested by the client.
                if ( result.isEmpty() ){
                   double clientMaxRounded = Util::roundToDigits( clipMaxClient, significantDigits );
                   if ( qAbs(clientMaxRounded - oldMaxClient) > m_errorMargin){
                       m_stateData.setValue<double>(CLIP_MAX_CLIENT, clientMaxRounded );
                       m_plotManager->setAxisXRange( clipMinClient, clientMaxRounded );
                   }
                }

                if ( finish ){
                   _finishClips();
               }
            }
        }
    }
    else {
        result = "Zoom mininum, "+QString::number(clipMinClient)+" must be less than maximum, "+QString::number(clipMaxClient);
    }
    return result;
}



QString Histogram::setClipMin( double clipMinClient, bool finish ){
    QString result;
    double oldMin = m_stateData.getValue<double>(CLIP_MIN);
    double oldMinClient = m_stateData.getValue<double>(CLIP_MIN_CLIENT );
    double clipMaxClient = m_stateData.getValue<double>(CLIP_MAX_CLIENT);
    double oldMinPercent = m_stateData.getValue<double>(CLIP_MIN_PERCENT);
    //Bypass the check that the new min is less than the old max if we are not
    //finished and are also planning to set a new max.
    if ( clipMinClient < clipMaxClient || !finish ){
        //The histogram will segfault if the clip is set less than the minimum intensity, so
        //we adjust what the client wants to what the histogram will take.
        Controller* controller = _getControllerSelected();
        double adjustedMin = clipMinClient;
        if ( controller != nullptr ){
            std::vector<double> percentiles( 1 );
            percentiles[0] = 0;
            std::vector<std::pair<int,double> > intensities = controller->getIntensity( percentiles );
            if ( intensities.size() == 1 && intensities[0].first >= 0 ){
                double imageMinIntensity = intensities[0].second;
                if ( clipMinClient < imageMinIntensity ){
                    adjustedMin = imageMinIntensity;
                }
            }

            //Decide if we need to change the actual clip min.
            int significantDigits = m_state.getValue<int>(Util::SIGNIFICANT_DIGITS );
            double clipMinRounded = Util::roundToDigits( adjustedMin, significantDigits );
            if ( qAbs(clipMinRounded - oldMin) > m_errorMargin){
                m_stateData.setValue<double>(CLIP_MIN, clipMinRounded );
                bool validWidth = _resetBinCountBasedOnWidth();
                if ( validWidth ){
                    std::pair<int,int> bounds = _getFrameBounds();
                    double clipMinPercent = controller->getPercentile( bounds.first, bounds.second, clipMinClient);
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
                else {
                    m_stateData.setValue<double>(CLIP_MIN, oldMin );
                    result = "Resulting histogram bin count exceeded the maximum.";
                }
            }

            //If there was not an error setting the actual clip min, then we can go ahead
            //and set the one the client sees.
            double clipMinClientRounded = Util::roundToDigits( clipMinClient, significantDigits );
            if ( qAbs(clipMinClientRounded - oldMinClient) > m_errorMargin){
                m_stateData.setValue<double>( CLIP_MIN_CLIENT, clipMinClientRounded );
                m_plotManager->setAxisXRange( clipMinClientRounded, clipMaxClient );
            }

            if ( finish ){
                _finishClips();
            }
        }
    }
    else {
        result = "Zoom mininum, "+QString::number(clipMinClient)+" must be less than maximum, "+QString::number(clipMaxClient);
    }
    return result;
}


QString Histogram::setClipMinPercent( double clipMinPercent, bool complete ){
     QString result;
     double oldMinPercent = m_stateData.getValue<double>(CLIP_MIN_PERCENT);
     double clipMaxPercent = m_stateData.getValue<double>(CLIP_MAX_PERCENT);
     int significantDigits = m_state.getValue<int>(Util::SIGNIFICANT_DIGITS );
     double clipMinPercentRounded = Util::roundToDigits( clipMinPercent, significantDigits );
     if( 0 <= clipMinPercentRounded && clipMinPercentRounded <= 100  ){
         if ( clipMinPercentRounded < clipMaxPercent ){
             if ( qAbs(clipMinPercentRounded - oldMinPercent) > m_errorMargin){
                 m_stateData.setValue<double>(CLIP_MIN_PERCENT, clipMinPercentRounded );
                 Controller* controller = _getControllerSelected();
                 if ( controller != nullptr ){
                     double cMin = clipMinPercentRounded / 100.0;
                     std::pair<int,int> bounds = _getFrameBounds();
                     std::vector<double> percentiles(1);
                     percentiles[0] = cMin;
                     std::vector<std::pair<int,double> > intensities = controller->getIntensity(
                             bounds.first, bounds.second, percentiles );
                     if ( intensities.size() == 1 && intensities[0].first >= 0 ){
                         double clipMin = intensities[0].second;
                         double oldClipMin = m_stateData.getValue<double>(CLIP_MIN);
                         if(qAbs(oldClipMin - clipMin) > m_errorMargin){
                             bool validWidth = _resetBinCountBasedOnWidth();
                             if ( !validWidth ){
                                 m_stateData.setValue<double>(CLIP_MIN_PERCENT, oldMinPercent );
                                 result = "Resulting histogram bin count exceeded the maximum.";
                             }
                             else {
                                 double roundedMin = Util::roundToDigits(clipMin, significantDigits);
                                 m_stateData.setValue<double>(CLIP_MIN, roundedMin);
                                 //Update the client min to match it
                                 m_stateData.setValue<double>(CLIP_MIN_CLIENT, roundedMin );
                                 double maxClientClip = m_stateData.getValue<double>( CLIP_MAX_CLIENT );
                                 m_plotManager->setAxisXRange( roundedMin, maxClientClip );
                                 //We must trigger a selection on the histogram, unless we are at full
                                 //percent, in which case, we do not want it.
                                 if ( roundedMin != 0 || maxClientClip != 100 ){
                                     m_plotManager->setRange( roundedMin, maxClientClip );
                                 }
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
     int significantDigits = m_state.getValue<int>(Util::SIGNIFICANT_DIGITS);
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
                     std::vector<double> percentiles(1);
                     percentiles[0] = decPercent;
                     std::pair<int,int> bound = _getFrameBounds();
                     std::vector<std::pair<int,double> > intensities = controller->getIntensity(
                             bound.first, bound.second, percentiles );
                     if( intensities.size() == 1 && intensities[0].first >= 0 ){
                         clipMax = intensities[0].second;
                         double oldClipMax = m_stateData.getValue<double>(CLIP_MAX);
                         if(qAbs(oldClipMax - clipMax) > m_errorMargin){

                             bool validWidth = _resetBinCountBasedOnWidth();
                             if ( !validWidth ){
                                 m_stateData.setValue<double>(CLIP_MAX_PERCENT, oldMaxPercent );
                                 result = "Resulting histogram bin count exceeded the maximum.";
                             }
                             else {
                                 double roundedMax = Util::roundToDigits(clipMax, significantDigits);
                                 m_stateData.setValue<double>(CLIP_MAX, roundedMax );
                                 m_stateData.setValue<double>(CLIP_MAX_CLIENT, roundedMax );
                                 double clipMinClient = m_stateData.getValue<double>( CLIP_MIN_CLIENT );
                                 m_plotManager->setAxisXRange( clipMinClient, roundedMax );

                                 //Trigger selection on the histogram, unless we are at full zoom, then
                                 //we don't want selection.
                                 if ( clipMinPercent != 0 || roundedMax != 100 ){
                                     m_plotManager->setRange( clipMinClient, roundedMax );
                                 }
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
        if ( colored ){
            _generateHistogram( nullptr );
        }
        m_plotManager->setColored( colored );
    }
    return result;
}



QString Histogram::setColorMin( double colorMin, bool finish ){
    QString result;
    double oldMin = m_stateData.getValue<double>(COLOR_MIN);
    double colorMax = m_stateData.getValue<double>(COLOR_MAX);
    double oldMinPercent = m_stateData.getValue<double>(COLOR_MIN_PERCENT);
    int significantDigits = m_state.getValue<int>(Util::SIGNIFICANT_DIGITS);
    double colorMinRounded = Util::roundToDigits( colorMin, significantDigits );
    //Bypass the check that the min is less than the max if we are not finished
    //and planning to set the max.
    if( colorMinRounded < colorMax || !finish ){
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
    int significantDigits = m_state.getValue<int>(Util::SIGNIFICANT_DIGITS);
    double colorMaxRounded = Util::roundToDigits( colorMax, significantDigits );
    //By pass checking that the new max is larger than the min if we are not
    //finished and are planning to set a new min.
    if ( colorMin < colorMaxRounded || !finish ){
        if ( qAbs(colorMaxRounded - oldMax) > m_errorMargin){
            m_stateData.setValue<double>(COLOR_MAX, colorMaxRounded );
            Controller* controller = _getControllerSelected();
            if ( controller != nullptr ){
                std::pair<int,int> bounds = _getFrameBounds();
                double colorMaxPercent = controller->getPercentile(bounds.first, bounds.second, colorMaxRounded );
                if ( colorMaxPercent >= 0 ){
                    colorMaxPercent = colorMaxPercent * 100;
                    if(qAbs(oldMaxPercent - colorMaxPercent) > m_errorMargin){
                        m_stateData.setValue<double>(COLOR_MAX_PERCENT,
                                Util::roundToDigits(colorMaxPercent,significantDigits));
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
     int significantDigits = m_state.getValue<int>(Util::SIGNIFICANT_DIGITS);
     double colorMaxPercentRounded = Util::roundToDigits( colorMaxPercent, significantDigits);
     if( 0 <= colorMaxPercentRounded && colorMaxPercentRounded <= 100  ){
         double lookupPercent = colorMaxPercentRounded;
         if ( colorMinPercent < lookupPercent || !complete ){
             if ( qAbs(colorMaxPercentRounded - oldMaxPercent) > m_errorMargin){
                 m_stateData.setValue<double>(COLOR_MAX_PERCENT, colorMaxPercentRounded );
                 Controller* controller = _getControllerSelected();
                 if ( controller != nullptr ){
                     double decPercent = lookupPercent / 100.0;
                     std::vector<double> percentiles(1);
                     percentiles[0] = decPercent;
                     std::pair<int,int> bound = _getFrameBounds();
                     std::vector<std::pair<int,double> > intensities = controller->getIntensity( bound.first,
                             bound.second, percentiles );
                     if( intensities.size() == 1 && intensities[0].first >= 0 ){
                         double colorMax = intensities[0].second;
                         double oldColorMax = m_stateData.getValue<double>(COLOR_MAX);
                         if(qAbs(oldColorMax - colorMax) > m_errorMargin){
                             double roundedMax = Util::roundToDigits(colorMax,significantDigits);
                             m_stateData.setValue<double>(COLOR_MAX, roundedMax );
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
     int significantDigits = m_state.getValue<int>(Util::SIGNIFICANT_DIGITS);
     double colorMinPercentRounded = Util::roundToDigits( colorMinPercent, significantDigits );
     if( 0 <= colorMinPercentRounded && colorMinPercentRounded <= 100  ){
         if ( colorMinPercentRounded < colorMaxPercent || !complete){
             if ( qAbs(colorMinPercentRounded - oldMinPercent) > m_errorMargin){
                 m_stateData.setValue<double>(COLOR_MIN_PERCENT, colorMinPercentRounded );
                 Controller* controller = _getControllerSelected();
                 if ( controller != nullptr ){
                     double cMin = colorMinPercentRounded / 100.0;
                     std::vector<double> percentiles( 1 );
                     percentiles[0] = cMin;
                     std::pair<int,int> bounds = _getFrameBounds();
                     std::vector<std::pair<int,double> > intensities = controller->getIntensity( bounds.first,
                             bounds.second, percentiles );
                     if( intensities.size() == 1 && intensities[0].first >= 0 ){
                         double colorMin = intensities[0].second;
                         double oldColorMin = m_stateData.getValue<double>(COLOR_MIN);
                         if(qAbs(oldColorMin - colorMin) > m_errorMargin){
                             double minRounded = Util::roundToDigits(colorMin,significantDigits);
                             m_stateData.setValue<double>(COLOR_MIN, minRounded );
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
    int significantDigits = m_state.getValue<int>(Util::SIGNIFICANT_DIGITS);
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
            _generateHistogram( nullptr );
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

                int significantDigits = m_state.getValue<int>(Util::SIGNIFICANT_DIGITS);
                m_state.setValue<double>(BIN_WIDTH, Util::roundToDigits(binWidth,significantDigits) );
                m_state.flushState();
                _generateHistogram( nullptr );
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

QString Histogram::_setCubeChannel( int channel ){
    QString result;
    if ( channel < 0 ){
        result = "Invalid cube channel "+QString::number( channel );
    }
    else {
        if ( m_cubeChannel != channel ){
            m_cubeChannel = channel;
            if ( m_state.getValue<QString>(PLANE_MODE) == PLANE_MODE_SINGLE ){
                _generateHistogram( nullptr );
            }
        }
    }
    return result;
}

QString Histogram::setPlaneChannel( int channel ){
    QString result;
    int channelMax = m_stateData.getValue<int>( PLANE_CHANNEL_MAX);
    if ( channel < 0 ){
        result = "Invalid channel index "+QString::number( channel );
    }
    else if ( channel > channelMax ){
        result = "Channel index must be less than or equal to "+QString::number(channelMax) +": "+QString::number(channel);
    }
    else {
        int oldChannel = m_stateData.getValue<int>( PLANE_CHANNEL );
        if ( oldChannel != channel ){
            m_stateData.setValue<int>( PLANE_CHANNEL, channel );
            if ( m_state.getValue<QString>(PLANE_MODE) == PLANE_MODE_CHANNEL ){
                _generateHistogram( nullptr );
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
        m_plotManager->setLogScale( logCount );
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
            _generateHistogram( nullptr );
        }
    }
    else {
        result = "Unrecognized histogram cube mode: "+ planeModeStr;
    }
    return result;
}


QString Histogram::saveHistogram( const QString& fileName ){
    QString result = m_plotManager->savePlot( fileName );
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

            _generateHistogram( nullptr );
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
            int significantDigits = m_state.getValue<int>(Util::SIGNIFICANT_DIGITS);
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
                    _generateHistogram( nullptr );
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
        if ( m_state.getValue<int>(Util::SIGNIFICANT_DIGITS) != digits ){
            m_state.setValue<int>(Util::SIGNIFICANT_DIGITS, digits );
            _setErrorMargin();
        }
    }
    return result;
}

void Histogram::_setErrorMargin(){
    int significantDigits = m_state.getValue<int>(Util::SIGNIFICANT_DIGITS );
    m_errorMargin = 1.0/qPow(10,significantDigits);
}

QString Histogram::setGraphStyle( const QString& styleStr ){
    QString result;
    QString oldStyle = m_state.getValue<QString>(GRAPH_STYLE);
    QString actualStyle = m_graphStyles->getActualPlotStyle( styleStr );
    if ( actualStyle != "" ){
        if ( actualStyle != oldStyle ){
            m_state.setValue<QString>(GRAPH_STYLE, actualStyle );
            m_state.flushState();
            m_plotManager->setStyle( styleStr );
        }
    }
    else {
        result = "Unrecognized histogram graph style: "+ styleStr;
    }
    return result;
}

QString Histogram::setTabIndex( int index ){
    QString result;
    if ( index >= 0 ){
        int oldIndex = m_state.getValue<int>( Util::TAB_INDEX );
        if ( index != oldIndex ){
            m_state.setValue<int>( Util::TAB_INDEX, index );
            m_state.flushState();
        }
    }
    else {
        result = "Histogram tab index must be nonnegative: "+ QString::number(index);
    }
    return result;
}



QString Histogram::setUseClipBuffer( bool useBuffer ){
    QString result;
    bool oldUseBuffer = m_state.getValue<bool>(CLIP_BUFFER);
    if ( useBuffer != oldUseBuffer ){
        m_state.setValue<bool>(CLIP_BUFFER, useBuffer );
        m_state.flushState();
        _generateHistogram( nullptr );
    }
    return result;
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

std::vector<HistogramRenderRequest> Histogram::_updateBinDatas( std::vector<HistogramRenderRequest> requests ){
	std::vector<HistogramRenderRequest> newRequests;
	//Go through the old histograms and add new requests that we don't already have.
	int dataCount = m_binDatas.size();
	int requestCount = requests.size();

	for ( int i = 0; i < requestCount; i++ ){
		QString requestId = requests[i].getId();
		bool binFound = false;
		for ( int j = 0; j < dataCount; j++ ){
			QString id = m_binDatas[j]->getName();
			if ( requestId.startsWith( id ) ){
				binFound = true;
				break;
			}
		}
		if ( !binFound ){
			newRequests.push_back( requests[i]);
		}
	}

	QList<int> removeIndices;
	//Now go through the histograms and remove those not being requested.
	for ( int j = 0; j < dataCount; j++ ){
		bool requested = false;
		QString binDataId = m_binDatas[j]->getName();
		for ( int i = 0; i < requestCount; i++ ){
			QString requestId = requests[i].getId();
			if ( requestId.startsWith(binDataId)){
				requested = true;
				break;
			}
		}
		if ( !requested ){
			removeIndices.append( j );
		}
	}

	//Remove bin data that we don't need.
	int removeCount = removeIndices.size();
	for ( int i = removeCount-1; i>= 0; i-- ){
		_removeData( removeIndices[i] );
	}
	return newRequests;
}


void Histogram::_updateChannel( Controller* controller, Carta::Lib::AxisInfo::KnownType type ){
    if ( type == Carta::Lib::AxisInfo::KnownType::SPECTRAL ){
        int spectralFrame = controller->getFrame( type );
        _setCubeChannel( spectralFrame );
        QString mode = m_state.getValue<QString>(PLANE_MODE);
        if ( mode == PLANE_MODE_SINGLE ){
            _generateHistogram(controller );
        }
    }
}


void Histogram::updateColorMap(){
    Controller* controller = _getControllerSelected();
    if ( controller ){
        std::shared_ptr<DataSource> dataSource = controller->getDataSource();
        if ( dataSource ){
            std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> pipeline = dataSource->_getPipeline();
            m_plotManager->setPipeline( pipeline );
        }
    }
}


void Histogram::_updateColorClips( double colorMinPercent, double colorMaxPercent ){
    if ( colorMinPercent < colorMaxPercent ){
        double normMin = colorMinPercent * 100;
        double normMax = colorMaxPercent * 100;
        setColorMinPercent( normMin, false );
        setColorMaxPercent( normMax, false );
        double colorMin = m_stateData.getValue<double>(COLOR_MIN);
        double colorMax = m_stateData.getValue<double>(COLOR_MAX);
        emit colorIntensityBoundsChanged( colorMin, colorMax );
        m_stateData.flushState();
    }
}

void Histogram::_updateColorSelection(){
    bool valid = false;
    std::pair<double,double> range = m_plotManager->getRangeColor( &valid );
    if ( valid ){
        double minRange = range.first;
        double maxRange = range.second;
        if ( range.first > range.second ){
            minRange = range.second;
            maxRange = range.first;
        }
        setRangeColor( minRange, maxRange );
    }
}

void Histogram::_updatePlots( ){
	m_plotManager->clearData();
	int dataCount = m_binDatas.size();
	for ( int i = 0; i < dataCount; i++ ){
		Carta::Lib::Hooks::HistogramResult result = m_binDatas[i]->getHistogramResult();
		m_plotManager->addData( &result );
		m_plotManager->setColor( m_binDatas[i]->getColor(), result.getName());
	}
	//Refresh the view
	m_plotManager->setLogScale( m_state.getValue<bool>( GRAPH_LOG_COUNT ) );
	m_plotManager->setStyle( m_state.getValue<QString>( GRAPH_STYLE ) );
	m_plotManager->setColored( m_state.getValue<bool>( GRAPH_COLORED ) );
	m_plotManager->updatePlot();
}

QString Histogram::_zoomToSelection(){
    QString result;
    bool valid = false;
    std::pair<double,double> range = m_plotManager->getRange( & valid );
    if ( valid ){
        double minRange = range.first;
        double maxRange = range.second;
        if ( range.first > range.second ){
            minRange = range.second;
            maxRange = range.first;
        }
        if ( minRange < maxRange ){
            result = setClipRange( minRange, maxRange );
        }
    }
    return result;
}


Histogram::~Histogram(){

}
}
}
