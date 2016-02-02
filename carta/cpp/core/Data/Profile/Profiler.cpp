#include "Profiler.h"
#include "CurveData.h"
#include "IntensityUnits.h"
#include "SpectralUnits.h"
#include "Data/Clips.h"
#include "Data/Settings.h"
#include "Data/LinkableImpl.h"
#include "Data/Image/Controller.h"
#include "Data/Image/DataSource.h"
#include "Data/Error/ErrorManager.h"
#include "Data/Util.h"
#include "Data/Plotter/Plot2DManager.h"
#include "Data/Plotter/PlotStyles.h"
#include "Plot2D/Plot2DGenerator.h"

#include "CartaLib/Hooks/Plot2DResult.h"
#include "CartaLib/Hooks/ConversionIntensityHook.h"
#include "CartaLib/Hooks/ConversionSpectralHook.h"
#include "CartaLib/AxisInfo.h"
#include "State/UtilState.h"
#include "Globals.h"
#include "PluginManager.h"
#include <QDebug>

namespace Carta {

namespace Data {

const QString Profiler::CLASS_NAME = "Profiler";
const QString Profiler::AXIS_UNITS_BOTTOM = "axisUnitsBottom";
const QString Profiler::AXIS_UNITS_LEFT = "axisUnitsLeft";
const QString Profiler::CLIP_BUFFER = "useClipBuffer";
const QString Profiler::CLIP_BUFFER_SIZE = "clipBuffer";
const QString Profiler::CLIP_MIN = "clipMin";
const QString Profiler::CLIP_MAX = "clipMax";
const QString Profiler::CLIP_MIN_CLIENT = "clipMinClient";
const QString Profiler::CLIP_MAX_CLIENT = "clipMaxClient";
const QString Profiler::CLIP_MIN_PERCENT = "clipMinPercent";
const QString Profiler::CLIP_MAX_PERCENT = "clipMaxPercent";
const QString Profiler::CURVES = "curves";


class Profiler::Factory : public Carta::State::CartaObjectFactory {
public:
    Carta::State::CartaObject * create (const QString & path, const QString & id){
        return new Profiler (path, id);
    }
};

bool Profiler::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new Profiler::Factory());

SpectralUnits* Profiler::m_spectralUnits = nullptr;
IntensityUnits* Profiler::m_intensityUnits = nullptr;

using Carta::State::UtilState;
using Carta::State::StateInterface;
using Carta::Plot2D::Plot2DGenerator;

Profiler::Profiler( const QString& path, const QString& id):
            CartaObject( CLASS_NAME, path, id ),
            m_linkImpl( new LinkableImpl( path )),
            m_preferences( nullptr),
            m_plotManager( new Plot2DManager( path, id ) ),
            m_stateData( UtilState::getLookup(path, StateInterface::STATE_DATA)){

    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    Settings* prefObj = objMan->createObject<Settings>();
    m_preferences.reset( prefObj );

    m_plotManager->setPlotGenerator( new Plot2DGenerator( Plot2DGenerator::PlotType::PROFILE) );
    m_plotManager->setTitleAxisY( "" );

    _initializeStatics();
    _initializeDefaultState();
    _initializeCallbacks();

    m_controllerLinked = false;
}



QString Profiler::addLink( CartaObject*  target){
    Controller* controller = dynamic_cast<Controller*>(target);
    bool linkAdded = false;
    QString result;
    if ( controller != nullptr ){
        if ( !m_controllerLinked ){
            linkAdded = m_linkImpl->addLink( controller );
            if ( linkAdded ){
                connect(controller, SIGNAL(dataChanged(Controller*)), this , SLOT(_generateProfile(Controller*)));
                connect(controller, SIGNAL(channelChanged(Controller*)), this, SLOT( _updateChannel(Controller*)));
                m_controllerLinked = true;
                _generateProfile( controller );
            }
        }
        else {
            CartaObject* obj = m_linkImpl->searchLinks( target->getPath());
            if ( obj != nullptr ){
                linkAdded = true;
            }
            else {
                result = "Profiler only supports linking to a single image source.";
            }
        }
    }
    else {
        result = "Profiler only supports linking to images";
    }
    return result;
}


std::vector<double> Profiler::_convertUnitsX( std::shared_ptr<CurveData> curveData,
        const QString& newUnit ) const {
    QString bottomUnit = newUnit;
    if ( newUnit.isEmpty() ){
        bottomUnit = m_state.getValue<QString>( AXIS_UNITS_BOTTOM );
    }
    std::vector<double> converted = curveData->getValuesX();
    if ( ! m_bottomUnit.isEmpty() ){
        Controller* controller = _getControllerSelected();
        if ( controller ){
            if ( bottomUnit != m_bottomUnit ){
                std::shared_ptr<Carta::Lib::Image::ImageInterface> dataSource = curveData->getSource();
                if ( dataSource ){
                    QString oldUnit = _getUnitUnits( m_bottomUnit );
                    QString newUnit = _getUnitUnits( bottomUnit );
                    auto result = Globals::instance()-> pluginManager()
                                         -> prepare <Carta::Lib::Hooks::ConversionSpectralHook>(dataSource,
                                                 oldUnit, newUnit, converted );
                    auto lam = [&converted] ( const Carta::Lib::Hooks::ConversionSpectralHook::ResultType &data ) {
                        converted = data;
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
            }
        }
    }
    return converted;
}


std::vector<double> Profiler::_convertUnitsY( std::shared_ptr<CurveData> curveData ) const {
    std::vector<double> converted = curveData->getValuesY();
    std::vector<double> plotDataX = curveData->getValuesX();
    QString leftUnit = m_state.getValue<QString>( AXIS_UNITS_LEFT );
    if ( ! m_leftUnit.isEmpty() ){
        Controller* controller = _getControllerSelected();
        if ( controller ){
            if ( leftUnit != m_leftUnit ){
                std::shared_ptr<Carta::Lib::Image::ImageInterface> dataSource =
                        curveData->getSource();
                if ( dataSource > 0 ){
                    //First, we need to make sure the x-values are in Hertz.
                    std::vector<double> hertzVals = _convertUnitsX( curveData, "Hz");
                    bool validBounds = false;
                    std::pair<double,double> boundsY = m_plotManager->getPlotBoundsY( curveData->getName(), &validBounds );
                    if ( validBounds ){
                        QString maxUnit = m_plotManager->getAxisUnitsY();
                        auto result = Globals::instance()-> pluginManager()
                             -> prepare <Carta::Lib::Hooks::ConversionIntensityHook>(dataSource,
                                                         m_leftUnit, leftUnit, hertzVals, converted,
                                                         boundsY.second, maxUnit );;

                        auto lam = [&converted] ( const Carta::Lib::Hooks::ConversionIntensityHook::ResultType &data ) {
                            converted = data;
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
                }
            }
        }
    }
    return converted;
}


void Profiler::_generateProfile(Controller* controller ){
    Controller* activeController = controller;
    if ( activeController == nullptr ){
        activeController = _getControllerSelected();
    }
    if ( activeController ){
        _loadProfile( activeController );
    }
}


Controller* Profiler::_getControllerSelected() const {
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


QString Profiler::getStateString( const QString& sessionId, SnapshotType type ) const{
    QString result("");
    if ( type == SNAPSHOT_PREFERENCES ){
        StateInterface prefState( "");
        prefState.setValue<QString>(Carta::State::StateInterface::OBJECT_TYPE, CLASS_NAME );
        prefState.insertValue<QString>(Util::PREFERENCES, m_state.toString());
        prefState.insertValue<QString>( Settings::SETTINGS, m_preferences->getStateString(sessionId, type) );
        result = prefState.toString();
    }
    else if ( type == SNAPSHOT_LAYOUT ){
        result = m_linkImpl->getStateString(getIndex(), getSnapType( type ));
    }
    return result;
}


QList<QString> Profiler::getLinks() const {
    return m_linkImpl->getLinkIds();
}


QString Profiler::_getPreferencesId() const {
    return m_preferences->getPath();
}


void Profiler::_initializeDefaultState(){
    m_stateData.insertValue<double>( CLIP_MIN, 0 );
    m_stateData.insertValue<double>(CLIP_MAX, 1);
    //Difference between CLIP_MIN and CLIP_MIN_CLIENT is that CLIP_MIN
    //will never be less than the image minimum intensity.  The CLIP_MIN_CLIENT
    //will mostly mirror CLIP_MIN, but may be less than the image minimum intensity
    //if the user wants to zoom out for some reason.
    m_stateData.insertValue<double>( CLIP_MIN_CLIENT, 0 );
    m_stateData.insertValue<double>( CLIP_MAX_CLIENT, 1 );
    m_stateData.insertValue<int>(CLIP_BUFFER_SIZE, 10 );
    m_stateData.insertValue<double>(CLIP_MIN_PERCENT, 0);
    m_stateData.insertValue<double>(CLIP_MAX_PERCENT, 100);
    m_stateData.insertArray( CURVES, 0 );
    m_stateData.flushState();

    //Default units
    m_bottomUnit = m_spectralUnits->getDefault();
    QString unitType = _getUnitType( m_bottomUnit );
    m_plotManager->setTitleAxisX( unitType );
    m_state.insertValue<QString>( AXIS_UNITS_BOTTOM, m_bottomUnit );
    m_state.insertValue<QString>( AXIS_UNITS_LEFT, m_intensityUnits->getDefault());
    m_state.flushState();
}


void Profiler::_initializeCallbacks(){

    addCommandCallback( "setAxisUnitsBottom", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {Util::UNITS};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString unitStr = dataValues[*keys.begin()];
        QString result = setAxisUnitsBottom( unitStr );
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setAxisUnitsLeft", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {Util::UNITS};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString unitStr = dataValues[*keys.begin()];
        QString result = setAxisUnitsLeft( unitStr );
        Util::commandPostProcess( result );
        return result;
    });
    addCommandCallback( "registerPreferences", [=] (const QString & /*cmd*/,
            const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        QString result = _getPreferencesId();
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


void Profiler::_initializeStatics(){
    if ( m_spectralUnits == nullptr ){
        m_spectralUnits = Util::findSingletonObject<SpectralUnits>();
    }
    if ( m_intensityUnits == nullptr ){
        m_intensityUnits = Util::findSingletonObject<IntensityUnits>();
    }
}


bool Profiler::isLinked( const QString& linkId ) const {
    bool linked = false;
    CartaObject* obj = m_linkImpl->searchLinks( linkId );
    if ( obj != nullptr ){
        linked = true;
    }
    return linked;
}


void Profiler::_loadProfile( Controller* controller ){
    if( ! controller) {
        return;
    }
    std::vector<std::shared_ptr<DataSource> > dataSources = controller->getDataSources();

    m_plotCurves.clear();
    int dataCount = dataSources.size();
    for ( int i = 0; i < dataCount; i++ ) {
        std::shared_ptr<Carta::Lib::Image::ImageInterface> image = dataSources[i]->_getImage();
    	std::vector < int > pos( image-> dims().size(), 0 );
        int axis = Util::getAxisIndex( image, Carta::Lib::AxisInfo::KnownType::SPECTRAL );
        Profiles::PrincipalAxisProfilePath path( axis, pos );
        Carta::Lib::NdArray::RawViewInterface * rawView = image-> getDataSlice( SliceND() );
        Profiles::ProfileExtractor * extractor = new Profiles::ProfileExtractor( rawView );
        shared_ptr<Carta::Lib::Image::MetaDataInterface> metaData = image->metaData();
        QString fileName = metaData->title();
        m_leftUnit = image->getPixelUnit().toStr();

        auto profilecb = [ = ] () {
            /**
             * TODO:  We need a finished signal.  Right now we are deleting
             * when the data length is non-zero, which could miss profiles with
             * no data.
             */
            //bool finished = extractor->isFinished();
            //qDebug() << "Extractor finished="<<finished;
            //if ( finished ){
                auto data = extractor->getDataD();

                int dataCount = data.size();
                if ( dataCount > 0 ){
                    std::vector<double> plotDataX( dataCount );
                    std::vector<double> plotDataY( dataCount );

                    for( int i = 0 ; i < dataCount; i ++ ){
                        plotDataX[i] = i;
                        plotDataY[i] = data[i];
                    }

                    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
                    std::shared_ptr<CurveData> profileCurve( objMan->createObject<CurveData>() );
                    m_plotCurves.append( profileCurve );
                    profileCurve->setName( fileName );
                    profileCurve->setSource( image );
                    profileCurve->setData( plotDataX, plotDataY );
                    _updatePlotData();
                    extractor->deleteLater();
                }

                //extractor->deleteLater();
            //}
        };
        connect( extractor, & Profiles::ProfileExtractor::progress, profilecb );
        extractor-> start( path );

    }
}




QString Profiler::removeLink( CartaObject* cartaObject){
    bool removed = false;
    QString result;
    Controller* controller = dynamic_cast<Controller*>( cartaObject );
    if ( controller != nullptr ){
        removed = m_linkImpl->removeLink( controller );
        if ( removed ){
            controller->disconnect(this);
            m_controllerLinked = false;
            //_resetDefaultStateData();
        }
    }
    else {
       result = "Profiler was unable to remove link only image links are supported";
    }
    return result;
}

void Profiler::resetState( const QString& state ){
    StateInterface restoredState( "");
    restoredState.setState( state );

    QString settingStr = restoredState.getValue<QString>(Settings::SETTINGS);
    m_preferences->resetStateString( settingStr );

    QString prefStr = restoredState.getValue<QString>(Util::PREFERENCES);
    m_state.setState( prefStr );
    m_state.flushState();
}

QString Profiler::setAxisUnitsBottom( const QString& unitStr ){
    QString result;
    QString actualUnits = m_spectralUnits->getActualUnits( unitStr );
    if ( !actualUnits.isEmpty() ){
        QString oldBottomUnits = m_state.getValue<QString>( AXIS_UNITS_BOTTOM );
        if ( unitStr != oldBottomUnits ){
            m_state.setValue<QString>( AXIS_UNITS_BOTTOM, actualUnits);
            m_plotManager->setTitleAxisX( _getUnitType( actualUnits ) );
            m_state.flushState();
            _updatePlotData(  );
        }
    }
    else {
        result = "Unrecognized profile bottom axis units: "+unitStr;
    }
    return result;
}

QString Profiler::setAxisUnitsLeft( const QString& unitStr ){
    QString result;
    QString actualUnits = m_intensityUnits->getActualUnits( unitStr );
    if ( !actualUnits.isEmpty() ){
        QString oldLeftUnits = m_state.getValue<QString>( AXIS_UNITS_LEFT );
        if ( oldLeftUnits != actualUnits ){
            m_state.setValue<QString>( AXIS_UNITS_LEFT, actualUnits );
            _updatePlotData();
        }
    }
    else {
        result = "Unrecognized profile left axis units: "+unitStr;
    }
    return result;
}

QString Profiler::setClipBuffer( int bufferAmount ){
    QString result;
    if ( bufferAmount >= 0 && bufferAmount < 100 ){
        int oldBufferAmount = m_stateData.getValue<int>( CLIP_BUFFER_SIZE);
        if ( oldBufferAmount != bufferAmount ){
            m_stateData.setValue<int>( CLIP_BUFFER_SIZE, bufferAmount );
            m_stateData.flushState();
            _generateProfile();
        }
    }
    else {
        result = "Invalid buffer amount (0,100): "+QString::number(bufferAmount);
    }
    return result;
}
QString Profiler::setClipMax( double /*clipMaxClient*/){
    QString result;
    return result;
}

QString Profiler::setClipMin( double /*clipMinClient*/){
    QString result;
    return result;
}

QString Profiler::setClipMaxPercent( double /*clipMaxPercent*/){
    QString result;
    return result;
}

QString Profiler::setClipMinPercent( double /*clipMinPercent*/){
    QString result;
    return result;
}
QString Profiler::setClipRange( double /*clipMin*/, double /*clipMax*/ ){
    QString result;
    return result;
}

QString Profiler::setClipRangePercent( double /*clipMinPercent*/, double /*clipMaxPercent*/ ){
    QString result;
    return result;
}


QString Profiler::setGraphStyle( const QString& /*styleStr*/ ){
    QString result;
    /*QString oldStyle = m_state.getValue<QString>(GRAPH_STYLE);
    QString actualStyle = _getActualGraphStyle( styleStr );
    if ( actualStyle != "" ){
        if ( actualStyle != oldStyle ){
            m_state.setValue<QString>(GRAPH_STYLE, actualStyle );
            m_state.flushState();
            m_plotManager->setStyle( actualStyle );
            m_plotManager->updatePlot();
        }
    }
    else {
        result = "Unrecognized Profiler graph style: "+ styleStr;
    }*/
    return result;
}




QString Profiler::setUseClipBuffer( bool useBuffer ){
    QString result;
    bool oldUseBuffer = m_state.getValue<bool>(CLIP_BUFFER);
    if ( useBuffer != oldUseBuffer ){
        m_state.setValue<bool>(CLIP_BUFFER, useBuffer );
        m_state.flushState();
        _generateProfile();
    }
    return result;
}


QString Profiler::_getUnitType( const QString& unitStr ){
    QString unitType = unitStr;
    int unitStart = unitStr.indexOf( "(");
    if ( unitStart >= 0 ){
        unitType = unitStr.mid( 0, unitStart );
    }
    return unitType;
}


QString Profiler::_getUnitUnits( const QString& unitStr ){
    QString strippedUnit = "";
    int unitStart = unitStr.indexOf( "(");
    if ( unitStart >= 0 ){
        int substrLength = unitStr.length() - unitStart - 2;
        if ( substrLength > 0){
            strippedUnit = unitStr.mid( unitStart + 1, substrLength );
        }
    }
    return strippedUnit;
}


void Profiler::_updateChannel( Controller* controller ){
    int frame = controller->getFrame( Carta::Lib::AxisInfo::KnownType::SPECTRAL );
    m_plotManager->setVLinePosition( frame );
}


void Profiler::_updatePlotData(){
    m_plotManager->clearData();
    int curveCount = m_plotCurves.size();
    for ( int i = 0; i < curveCount; i++ ){
        //Convert the data units, if necessary.
        std::vector<double> convertedX = _convertUnitsX( m_plotCurves[i] );
        std::vector<double> convertedY = _convertUnitsY( m_plotCurves[i] );
        int dataCount = convertedX.size();
        std::vector< std::pair<double,double> > plotData(dataCount);
        for ( int i = 0; i < dataCount; i++ ){
            plotData[i].first  = convertedX[i];
            plotData[i].second = convertedY[i];
        }

        //Put the data into the plot.
        Carta::Lib::Hooks::Plot2DResult plotResult( m_plotCurves[i]->getName(), "", "", plotData );
        m_plotManager->addData( &plotResult );
    }
    QString bottomUnit = m_state.getValue<QString>( AXIS_UNITS_BOTTOM );
    bottomUnit = _getUnitUnits( bottomUnit );
    QString leftUnit = m_state.getValue<QString>( AXIS_UNITS_LEFT );
    m_plotManager->setTitleAxisX( bottomUnit );
    m_plotManager->setTitleAxisY( leftUnit );
    m_plotManager->updatePlot();
}

QString Profiler::_zoomToSelection(){
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
    else {
        _generateProfile();
    }
    return result;
}



Profiler::~Profiler(){
}
}
}
