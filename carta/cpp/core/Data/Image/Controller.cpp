#include "State/ObjectManager.h"
#include "State/UtilState.h"
#include "Data/Image/Controller.h"
#include "Data/Image/Stack.h"
#include "Data/Image/DataSource.h"
#include "Data/Image/Grid/AxisMapper.h"
#include "Data/Image/Grid/DataGrid.h"
#include "Data/Image/Grid/GridControls.h"
#include "Data/Image/Contour/ContourControls.h"
#include "Data/Image/Contour/DataContours.h"

#include "Data/Settings.h"
#include "Data/DataLoader.h"
#include "Data/Error/ErrorManager.h"

#include "Data/Region/Region.h"


#include "Data/Util.h"
#include "ImageView.h"
#include "CartaLib/IImage.h"
#include "CartaLib/PixelPipeline/CustomizablePixelPipeline.h"
#include "Globals.h"

#include <QtCore/QDebug>
#include <QtCore/QList>
#include <QtCore/QDir>
#include <memory>
#include <set>

using namespace std;

namespace Carta {

namespace Data {

class Controller::Factory : public Carta::State::CartaObjectFactory {

public:

    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new Controller (path, id);
    }
};

const QString Controller::CLIP_VALUE_MIN = "clipValueMin";
const QString Controller::CLIP_VALUE_MAX = "clipValueMax";
const QString Controller::CLOSE_IMAGE = "closeImage";
const QString Controller::AUTO_CLIP = "autoClip";
const QString Controller::DATA = "data";
const QString Controller::DATA_PATH = "dataPath";
const QString Controller::CURSOR = "formattedCursorCoordinates";
const QString Controller::CENTER = "center";
const QString Controller::IMAGE = "image";
const QString Controller::PAN_ZOOM_ALL = "panZoomAll";
const QString Controller::POINTER_MOVE = "pointer-move";
const QString Controller::ZOOM = "zoom";

const QString Controller::PLUGIN_NAME = "CasaImageLoader";
const QString Controller::STACK_SELECT_AUTO = "stackAutoSelect";


const QString Controller::CLASS_NAME = "Controller";
bool Controller::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass (CLASS_NAME,
                                                   new Controller::Factory());

using Carta::State::UtilState;
using Carta::State::StateInterface;
using Carta::Lib::AxisInfo;

Controller::Controller( const QString& path, const QString& id ) :
        CartaObject( CLASS_NAME, path, id),
        m_stateMouse(UtilState::getLookup(path, Stack::VIEW)){

     _initializeState();

     Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();

     //Stack
     Stack* layerGroupRoot = objMan->createObject<Stack>();
     QString viewName = Carta::State::UtilState::getLookup( path, Stack::VIEW);
     layerGroupRoot->_setViewName( viewName );
     m_stack.reset( layerGroupRoot );
     connect( m_stack.get(), SIGNAL( frameChanged(Carta::Lib::AxisInfo::KnownType)),
             this, SLOT(_notifyFrameChange( Carta::Lib::AxisInfo::KnownType)));
     connect( m_stack.get(), SIGNAL( viewLoad(bool)), this, SLOT(_loadViewQueued(bool)));
     connect( m_stack.get(), SIGNAL(contourSetAdded(Layer*,const QString&)),
                     this, SLOT(_contourSetAdded(Layer*, const QString&)));
     connect( m_stack.get(), SIGNAL(contourSetRemoved(const QString&)),
                             this, SLOT(_contourSetRemoved(const QString&)));
     connect( m_stack.get(), SIGNAL(colorStateChanged()), this, SLOT( _colorMapChanged() ));

     GridControls* gridObj = objMan->createObject<GridControls>();
     m_gridControls.reset( gridObj );
     connect( m_gridControls.get(), SIGNAL(gridChanged( const Carta::State::StateInterface&,bool)),
             this, SLOT(_gridChanged( const Carta::State::StateInterface&, bool )));
     connect( m_gridControls.get(), SIGNAL(displayAxesChanged(std::vector<Carta::Lib::AxisInfo::KnownType>,bool )),
                  this, SLOT( _displayAxesChanged(std::vector<Carta::Lib::AxisInfo::KnownType>,bool )));

     ContourControls* contourObj = objMan->createObject<ContourControls>();
     m_contourControls.reset( contourObj );
     m_contourControls->setPercentIntensityMap( this );
     connect( m_contourControls.get(), SIGNAL(drawContoursChanged()),
             this, SLOT( _contoursChanged()));

     Settings* settingsObj = objMan->createObject<Settings>();
     m_settings.reset( settingsObj );

     _initializeCallbacks();
}

void Controller::addContourSet( std::shared_ptr<DataContours> contourSet){
    m_stack->_addContourSet( contourSet );
}

bool Controller::addData(const QString& fileName) {
    //Decide on the type of data we are adding based on the file
    //suffix.
    bool result = false;
    if ( fileName.endsWith( DataLoader::CRTF) ){
        result = _addDataRegion( fileName );
    }
    else {
        result = _addDataImage( fileName );
    }
    return result;
}


bool Controller::_addDataImage(const QString& fileName) {
    bool dataAdded = m_stack->_addData( fileName, m_stateColor );
    if ( dataAdded ){
        if ( isStackSelectAuto() ){
            QStringList selectedLayers;
            QString stackId= m_stack->_getCurrentId();
            selectedLayers.append( stackId );
            _setLayersSelected( selectedLayers );
        }
        _updateDisplayAxes();
        emit dataChanged( this );
    }
    return dataAdded;
}

bool Controller::_addDataRegion(const QString& fileName) {
    QString errorStr = m_stack->_addDataRegion( fileName );
    bool regionLoaded = true;
    if ( errorStr.isEmpty() ){
        emit dataChangedRegion( this );
    }
    else {
        regionLoaded = false;
        ErrorManager* hr = Util::findSingletonObject<ErrorManager>();
        hr->registerError( errorStr );
    }
    return regionLoaded;
}

QString Controller::applyClips( double minIntensityPercentile, double maxIntensityPercentile ){
    QString result;
    bool clipsChangedValue = false;
    if ( minIntensityPercentile < maxIntensityPercentile ){
        const double ERROR_MARGIN = 0.0001;
        if ( 0 <= minIntensityPercentile && minIntensityPercentile <= 1 ){
            double oldMin = m_state.getValue<double>(CLIP_VALUE_MIN );
            if ( qAbs(minIntensityPercentile - oldMin) > ERROR_MARGIN ){
                m_state.setValue<double>(CLIP_VALUE_MIN, minIntensityPercentile );
                clipsChangedValue = true;
            }
        }
        else {
            result = "Minimum intensity percentile invalid [0,1]: "+ QString::number( minIntensityPercentile);
        }
        if ( 0 <= maxIntensityPercentile && maxIntensityPercentile <= 1 ){
            double oldMax = m_state.getValue<double>(CLIP_VALUE_MAX);
            if ( qAbs(maxIntensityPercentile - oldMax) > ERROR_MARGIN ){
                m_state.setValue<double>(CLIP_VALUE_MAX, maxIntensityPercentile );
                clipsChangedValue = true;
            }
        }
        else {
            result = "Maximum intensity percentile invalid [0,1]: "+ QString::number( maxIntensityPercentile);
        }

        if( clipsChangedValue ){
            m_state.flushState();
            m_stack->_scheduleFrameReload( true );
            double minPercent = m_state.getValue<double>(CLIP_VALUE_MIN);
            double maxPercent = m_state.getValue<double>(CLIP_VALUE_MAX);
            emit clipsChanged( minPercent, maxPercent );
        }
    }
    else {
        result = "The minimum percentile: "+QString::number(minIntensityPercentile)+
                " must be less than "+QString::number(maxIntensityPercentile);
    }
    return result;
}


void Controller::clear(){
    unregisterView();
}


void Controller::_clearStatistics(){
    m_stateMouse.setValue<QString>( CURSOR, "" );
    m_stateMouse.flushState();
}

QString Controller::closeImage( const QString& id ){
    QString result;
    bool imageClosed = m_stack->_closeData( id );
    if ( !imageClosed ){
        result = "Could not find data to remove for id="+id;
    }
    else {
        int visibleImageCount = m_stack->_getStackSizeVisible();
        if ( visibleImageCount == 0 ){
            _clearStatistics();
        }
        emit dataChanged( this );
    }
    return result;
}

QString Controller::closeRegion( const QString& regionId ){
    QString result = m_stack->_closeRegion( regionId );
    if ( result.isEmpty() ){
        emit dataChanged( this );
    }
    return result;
}

void Controller::_colorMapChanged(){
    m_stack->_scheduleFrameReload( true );
}



void Controller::centerOnPixel( double centerX, double centerY ){
    bool panZoomAll = m_state.getValue<bool>( PAN_ZOOM_ALL );
    m_stack->_setPan( centerX, centerY, panZoomAll );
}

void Controller::_contourSetAdded( Layer* cData, const QString& setName ){
    if ( cData != nullptr ){
        std::shared_ptr<DataContours> addedSet = cData->_getContour( setName );
        if ( addedSet ){
            m_contourControls->_setDrawContours( addedSet );
        }
    }
}

void Controller::_contourSetRemoved( const QString setName ){
    //Remove the contour set from the controls only if nothing in the stack
    if ( !m_stack->_getContour( setName) ){
        m_contourControls->deleteContourSet( setName );
    }
}

void Controller::_contoursChanged(){
    m_stack->_scheduleFrameReload( true );
}

void Controller::_displayAxesChanged(std::vector<AxisInfo::KnownType> displayAxisTypes,
        bool applyAll ){
    m_stack->_displayAxesChanged( displayAxisTypes, applyAll );
    emit axesChanged();
    _updateCursorText( true );
}

std::vector<Carta::Lib::AxisInfo::KnownType> Controller::_getAxisZTypes() const {
    std::vector<Carta::Lib::AxisInfo::KnownType> zTypes = m_stack->_getAxisZTypes();
    return zTypes;
}

std::set<AxisInfo::KnownType> Controller::_getAxesHidden() const {
    return m_stack->_getAxesHidden();
}


QPointF Controller::getCenterPixel() {
    QPointF center = m_stack->_getCenterPixel();
    return center;
}

double Controller::getClipPercentileMax() const {
    double clipValueMax = m_state.getValue<double>(CLIP_VALUE_MAX);
    return clipValueMax;
}

double Controller::getClipPercentileMin() const {
    double clipValueMin = m_state.getValue<double>(CLIP_VALUE_MIN);
    return clipValueMin;
}

Carta::Lib::KnownSkyCS Controller::getCoordinateSystem() const {
    return m_stack->_getCoordinateSystem();
}

QStringList Controller::getCoordinates( double x, double y, Carta::Lib::KnownSkyCS system) const {
    return m_stack->getCoordinates( x, y, system/*, _getFrameIndices()*/);
}

std::shared_ptr<DataSource> Controller::getDataSource(){
    return m_stack->_getDataSource();
}

std::vector< std::shared_ptr<DataSource> > Controller::getDataSources() {
    return m_stack-> _getDataSources();
}

std::vector< std::shared_ptr<Carta::Lib::Image::ImageInterface> > Controller::getImages() {
    return m_stack->_getImages();
}


std::shared_ptr<ContourControls> Controller::getContourControls() {
    return m_contourControls;
}

int Controller::getFrameUpperBound( AxisInfo::KnownType axisType ) const {
    return m_stack->_getFrameUpperBound( axisType );
}

int Controller::getFrame( AxisInfo::KnownType axisType ) const {
    int frame = m_stack->_getFrame( axisType );
    return frame;
}


std::shared_ptr<GridControls> Controller::getGridControls() {
    return m_gridControls;
}

/*std::vector<int> Controller::getImageDimensions( ) const {
    std::vector<int> result;
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        int dimensions = m_datas[dataIndex]->_getDimensions();
        for ( int i = 0; i < dimensions; i++ ) {
            int d = m_datas[dataIndex]->_getDimension( i );
            result.push_back( d );
        }
    }
    else {
        result.push_back(0);
    }
    return result;
}*/


/*QString Controller::getLayerId(int index) const{
    QString name;
    if ( 0 <= index && index < m_datas.size()){
        name = m_datas[index]->_getId();
    }
    return name;
}*/


std::vector<int> Controller::getImageSlice() const {
    std::vector<int> result = m_stack->_getImageSlice();
    return result;
}


/*int Controller::_getIndex( const QString& fileName) const{
    int dataCount = m_datas.size();
    int targetIndex = -1;
    for ( int i = 0; i < dataCount; i++ ){
        QString dataName = m_datas[i]->_getId();
        if ( fileName == dataName ){
            targetIndex = i;
            break;
        }
    }
    return targetIndex;
}*/

/*int Controller::_getIndexData( Layer* target ) const {
    int targetIndex = -1;
    if ( target != nullptr ){
        QString targetName = target->_getId();
        targetIndex = _getIndex( targetName);
    }
    return targetIndex;
}*/

/*int Controller::_getIndexCurrent( ) const {
    int dataIndex = -1;
    if ( m_selectImage ){
        int index = m_selectImage->getIndex();
        int visibleIndex = -1;
        int dataCount = m_datas.size();
        for ( int i = 0; i < dataCount; i++ ){
            if ( m_datas[i]->_isVisible() ){
                visibleIndex++;
                if ( visibleIndex == index ){
                    dataIndex = i;
                    break;
                }
            }
        }
    }
    return dataIndex;
}*/


bool Controller::getIntensity( double percentile, double* intensity ) const{
    int currentFrame = getFrame( AxisInfo::KnownType::SPECTRAL);
    bool validIntensity = getIntensity( currentFrame, currentFrame, percentile, intensity );
    return validIntensity;
}

bool Controller::getIntensity( int frameLow, int frameHigh, double percentile, double* intensity ) const{
    bool validIntensity = m_stack->_getIntensity( frameLow, frameHigh, percentile, intensity );
    return validIntensity;
}

QSize Controller::getOutputSize( ){
    QSize result = m_stack->_getOutputSize();
    return result;
}


double Controller::getPercentile( double intensity ) const {
    int currentFrame = getFrame( AxisInfo::KnownType::SPECTRAL );
    return getPercentile( currentFrame, currentFrame, intensity );
}

double Controller::getPercentile( int frameLow, int frameHigh, double intensity ) const {
    return m_stack->_getPercentile( frameLow, frameHigh, intensity );
}


QStringList Controller::getPixelCoordinates( double ra, double dec ) const {
    QStringList result = m_stack->_getPixelCoordinates( ra, dec );
    return result;
}

QString Controller::getPixelValue( double x, double y ) const {
    QString result = m_stack->getPixelValue( x, y );
    return result;
}

QString Controller::getPixelUnits() const {
    QString result = m_stack->_getPixelUnits();
    return result;
}

QString Controller::_getPreferencesId() const {
    QString id;
    if ( m_settings.get() != nullptr ){
        id = m_settings->getPath();
    }
    return id;
}

std::vector<Carta::Lib::RegionInfo> Controller::getRegions() const {
    std::vector<Carta::Lib::RegionInfo> regionInfos = m_stack->_getRegions();
    return regionInfos;
}


int Controller::getSelectImageIndex() const {
    return m_stack->_getSelectImageIndex();
}


std::vector< std::shared_ptr<ColorState> >  Controller::getSelectedColorStates(){
    std::vector< std::shared_ptr<ColorState> > colorStates = m_stack->_getSelectedColorStates();
    return colorStates;
}


int Controller::getStackedImageCount() const {
    return m_stack->_getStackSize();
}


int Controller::getStackedImageCountVisible() const {
    return m_stack->_getStackSizeVisible();
}

QString Controller::getStateString( const QString& sessionId, SnapshotType type ) const{
    QString result("");
    if ( type == SNAPSHOT_PREFERENCES ){
        StateInterface prefState( "");
        prefState.setValue<QString>(Carta::State::StateInterface::OBJECT_TYPE, CLASS_NAME );
        prefState.insertValue<QString>(Util::PREFERENCES, m_state.toString());
        prefState.insertValue<QString>(Settings::SETTINGS, m_settings->getStateString(sessionId, type) );
        result = prefState.toString();
    }
    else if ( type == SNAPSHOT_DATA ){

        Carta::State::StateInterface dataState("");
        //DataLoader* dataLoader = Util::findSingletonObject<DataLoader>();

        /*QString rootDir = dataLoader->getRootDir( sessionId );
        int dataCount = m_datas.size();
        dataState.insertArray( DATA, dataCount );
        for ( int i = 0; i < dataCount; i++ ){
            QString lookup = Carta::State::UtilState::getLookup( DATA, i );
            QString dataStateStr = m_datas[i]->_getStateString();
            dataState.setObject( lookup, dataStateStr );
        }*/
        dataState.setState( m_stack->_getStateString() );


        dataState.setValue<QString>( StateInterface::OBJECT_TYPE, CLASS_NAME + StateInterface::STATE_DATA);
        dataState.setValue<int>(StateInterface::INDEX, getIndex() );

        result = dataState.toString();
    }
    return result;
}



QString Controller::getSnapType(CartaObject::SnapshotType snapType) const {
    QString objType = CartaObject::getSnapType( snapType );
    if ( snapType == SNAPSHOT_DATA ){
        objType = objType + Carta::State::StateInterface::STATE_DATA;
    }
    return objType;
}

double Controller::getZoomLevel( ){
    return m_stack->_getZoom();
}

void Controller::_gridChanged( const StateInterface& state, bool applyAll ){
    m_stack->_gridChanged( state, applyAll );
}

void Controller::_initializeCallbacks(){
    addCommandCallback( "hideImage", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {Util::ID};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString idStr = dataValues[*keys.begin()];
        QString result = setImageVisibility( /*imageIndex*/idStr, false );
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setGroup", [=] (const QString & /*cmd*/,
                                const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = {Layer::GROUP};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString groupSelects = dataValues[*keys.begin()];
            bool validBool = false;
            bool group = Util::toBool( groupSelects, &validBool );
            QString result;
            if ( validBool ){
                result = setSelectedLayersGrouped( group );
            }
            else {
                result = "Grouping layers must be true/false: "+params;
            }
            Util::commandPostProcess( result );
            return result;
        });

    addCommandCallback( "showImage", [=] (const QString & /*cmd*/,
                            const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {Util::ID};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString idStr = dataValues[*keys.begin()];
        QString result = setImageVisibility( idStr, true );
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setImageOrder", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
        bool parseError = false;
        const QString indexStr( "indices");
        std::set<QString> keys = {Util::ID, indexStr};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString idStr = dataValues[Util::ID ];
        std::vector<int> vals = Util::string2VectorInt( dataValues[indexStr], &parseError, ";" );
        QString result;
        if ( !parseError ){
            result = setImageOrder( idStr, vals );
        }
        else {
            result = "Image order must be expressed as nonnegative integers: "+params;
        }
        Util::commandPostProcess( result );
        return result;
    });


    //Listen for updates to the clip and reload the frame.
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
            result = setClipValue( clipVal );
        }
        else {
            result = "Invalid clip value: "+params;
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setAutoClip", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {"autoClip"};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString clipKey = *keys.begin();
        bool validBool = false;
        bool autoClip = Util::toBool( dataValues[clipKey], &validBool );
        QString result;
        if ( validBool ){
            setAutoClip( autoClip );
        }
        else {
            result = "Auto clip must be true/false: "+params;
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setPanZoomAll", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = {"panZoomAll"};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString panZoomKey = *keys.begin();
            bool validBool = false;
            bool panZoomAll = Util::toBool( dataValues[panZoomKey], &validBool );
            QString result;
            if ( validBool ){
                setPanZoomAll( panZoomAll );
            }
            else {
                result = "Pan/Zoom All must be true/false: "+params;
            }
            Util::commandPostProcess( result );
            return result;
        });

    QString pointerPath= UtilState::getLookup( getPath(), UtilState::getLookup( Stack::VIEW, POINTER_MOVE));
    addStateCallback( pointerPath, [=] ( const QString& /*path*/, const QString& value ) {
        QStringList mouseList = value.split( " ");
        if ( mouseList.size() == 2 ){
            bool validX = false;
            int mouseX = mouseList[0].toInt( &validX );
            bool validY = false;
            int mouseY = mouseList[1].toInt( &validY );
            if ( validX && validY ){
                _updateCursor( mouseX, mouseY );
            }
        }

    });

    addCommandCallback( CLOSE_IMAGE, [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) ->QString {
        std::set<QString> keys = {IMAGE};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString imageId = dataValues[*keys.begin()];
        QString result = closeImage( imageId );
                return result;
    });

    addCommandCallback( "closeRegion", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) ->QString {
        std::set<QString> keys = {"region"};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString regionId = dataValues[*keys.begin()];
        QString result = closeRegion( regionId );
        return result;
    });

    addCommandCallback( CENTER, [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) ->QString {
        bool parseError = false;
        QString result;
        auto vals = Util::string2VectorDouble( params, &parseError );
        int count = vals.size();
        if ( count > 1 ) {
            updatePan( vals[0], vals[1]);
        }
        else {
            result = "Center command must include doubles specifying the point to center: "+params;
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( ZOOM, [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) ->QString {
        bool error = false;
        auto vals = Util::string2VectorDouble( params, &error );
        if ( vals.size() > 2 ) {
            double centerX = vals[0];
            double centerY = vals[1];
            double z = vals[2];
            updateZoom( centerX, centerY, z );
        }
        return "";
    });

    addCommandCallback( "registerContourControls", [=] (const QString & /*cmd*/,
                            const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        QString result;
        if ( m_contourControls.get() != nullptr ){
            result = m_contourControls->getPath();
        }
        return result;
    });

    addCommandCallback( "registerStack", [=] (const QString & /*cmd*/,
                                const QString & /*params*/, const QString & /*sessionId*/) -> QString {
            QString result;
            if ( m_stack.get() != nullptr ){
                result = m_stack->getPath();
            }
            return result;
        });

    addCommandCallback( "registerGridControls", [=] (const QString & /*cmd*/,
                        const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        QString result;
        if ( m_gridControls.get() != nullptr ){
            result = m_gridControls->getPath();
        }
        return result;
    });

    addCommandCallback( "registerPreferences", [=] (const QString & /*cmd*/,
                            const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        QString result = _getPreferencesId();
        return result;
   });

    addCommandCallback( "registerShape", [=] (const QString & /*cmd*/,
                                const QString & params, const QString & /*sessionId*/) -> QString {
        const QString INDEX( "index");
        std::set<QString> keys = {Util::TYPE, INDEX};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString shapePath;
        /*bool validIndex = false;
        int index = dataValues[INDEX].toInt( &validIndex );
        if ( validIndex ){
            int regionCount = m_regions.size();
            if ( 0 <= index && index < regionCount ){
                //Measure the index from the end.
                shapePath = m_regions[index]->getPath();
            }
            else {
                Carta::Lib::RegionInfo::RegionType regionType = Region::getRegionType( dataValues[Util::TYPE] );
                std::shared_ptr<Region> region = RegionFactory::makeRegion( regionType );
                if ( region ){
                    m_regions.append( region );
                    shapePath = region->getPath();
                }
                else {
                    qDebug()<<"Error unsupported region: "<<params;
                }

            }
        }*/
        return shapePath;
    });

    addCommandCallback( "resetPan", [=] (const QString & /*cmd*/,
                            const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        QString result;
        resetPan();
        return result;
    });


    addCommandCallback( "resetZoom", [=] (const QString & /*cmd*/,
                        const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        QString result;
        resetZoom();
        return result;
    });

    addCommandCallback( "setLayersSelected", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        QStringList names = params.split(";");
        if ( names.size() == 0 ){
            result = "Please specify the layers to select.";
        }
        else {
            result = _setLayersSelected( names );
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "saveImage", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {DATA_PATH};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString result = saveImage( dataValues[DATA_PATH]);
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setMaskColor", [=] (const QString & /*cmd*/,
                                const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {Util::ID, Util::RED, Util::GREEN, Util::BLUE};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString result;
        QString id = dataValues[Util::ID];
        QString redStr = dataValues[Util::RED];
        bool validRed = false;
        int redAmount = redStr.toInt( &validRed );
        QString greenStr = dataValues[Util::GREEN];
        bool validGreen = false;
        int greenAmount = greenStr.toInt( &validGreen );
        QString blueStr = dataValues[Util::BLUE];
        bool validBlue = false;
        int blueAmount = blueStr.toInt( &validBlue );
        if ( validRed && validGreen && validBlue ){
            QStringList errorList = setMaskColor( id, redAmount, greenAmount, blueAmount );
            result = errorList.join(";");
        }
        else {
            result = "Invalid mask color(s): "+params;
        }

        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setMaskAlpha", [=] (const QString & /*cmd*/,
                                    const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = { Util::ID, Util::ALPHA };
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString result;
        QString idStr = dataValues[Util::ID];
        QString alphaStr = dataValues[Util::ALPHA];
        bool validAlpha = false;
        int alphaAmount = alphaStr.toInt( &validAlpha );
        if ( validAlpha ){
            result = setMaskAlpha( idStr, alphaAmount );
        }
        else {
            result = "Invalid mask opacity: "+params;
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setStackSelectAuto", [=] (const QString & /*cmd*/,
                       const QString & params, const QString & /*sessionId*/) -> QString {
       std::set<QString> keys = {STACK_SELECT_AUTO};
       std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
       QString autoModeStr = dataValues[STACK_SELECT_AUTO];
       bool validBool = false;
       bool autoSelect = Util::toBool( autoModeStr, &validBool );
       QString result;
       if ( validBool ){
           setStackSelectAuto( autoSelect );
       }
       else {
           result = "Please specify true/false when setting whether stack selection should be automatic: "+autoModeStr;
       }
       Util::commandPostProcess( result );
       return result;
   });

    addCommandCallback( "setCompositionMode", [=] (const QString & /*cmd*/,
                            const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {Util::ID, LayerGroup::COMPOSITION_MODE};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString compMode = dataValues[LayerGroup::COMPOSITION_MODE];
        QString idStr = dataValues[Util::ID];
        QString result = setCompositionMode( idStr, compMode );
        Util::commandPostProcess( result );
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
}


void Controller::_initializeState(){
    //First the preference state.
    m_state.insertValue<bool>( AUTO_CLIP, true );
    m_state.insertValue<bool>(PAN_ZOOM_ALL, true );
    m_state.insertValue<bool>( STACK_SELECT_AUTO, true );
    m_state.insertValue<double>( CLIP_VALUE_MIN, 0.025 );
    m_state.insertValue<double>( CLIP_VALUE_MAX, 0.975 );
    //Default Tab
    m_state.insertValue<int>( Util::TAB_INDEX, 0 );
    m_state.flushState();

    m_stateMouse.insertObject( ImageView::MOUSE );
    m_stateMouse.insertValue<QString>(CURSOR, "");
    m_stateMouse.insertValue<QString>(POINTER_MOVE, "");
    m_stateMouse.insertValue<int>(ImageView::MOUSE_X, 0 );
    m_stateMouse.insertValue<int>(ImageView::MOUSE_Y, 0 );
    m_stateMouse.flushState();
}

bool Controller::isStackSelectAuto() const {
    return m_state.getValue<bool>( STACK_SELECT_AUTO );
}


void Controller::_loadViewQueued( bool newClips ){
    QMetaObject::invokeMethod( this, "_loadView", Qt::QueuedConnection, Q_ARG(bool, newClips) );
}

void Controller::_loadView( bool renderAll ){
    //Load the image.
    bool autoClip = m_state.getValue<bool>(AUTO_CLIP);
    double clipValueMin = m_state.getValue<double>(CLIP_VALUE_MIN);
    double clipValueMax = m_state.getValue<double>(CLIP_VALUE_MAX);
    m_stack->_load( renderAll, autoClip, clipValueMin, clipValueMax );
}


void Controller::_notifyFrameChange( Carta::Lib::AxisInfo::KnownType axis ){
    emit frameChanged( this, axis );
}

void Controller::removeContourSet( std::shared_ptr<DataContours> contourSet ){
    m_stack->_removeContourSet( contourSet );
}


void Controller::resetState( const QString& state ){
    StateInterface restoredState( "");
    restoredState.setState( state );

    QString settingStr = restoredState.getValue<QString>(Settings::SETTINGS);
    m_settings->resetStateString( settingStr );

    QString prefStr = restoredState.getValue<QString>(Util::PREFERENCES);
    m_state.setState( prefStr );
    m_state.flushState();
}

void Controller::resetStateData( const QString& state ){
    Carta::State::StateInterface dataState( "");
    dataState.setState( state );

    //Reset the layers
    m_stack->_resetState( dataState );

    /*std::vector<int> frames = _getFrameIndices();
    int dataCount = dataState.getArraySize(DATA);
    QStringList loadedFiles;
    for ( int i = 0; i < dataCount; i++ ){
        QString dataLookup = Carta::State::UtilState::getLookup( DATA, i );
        QString idLookup = Carta::State::UtilState::getLookup( dataLookup, Util::ID );
        QString id = dataState.getValue<QString>( idLookup );
        int dataIndex = _getIndex( id );
        if ( dataIndex == -1 ){
            QString fileLookup = Carta::State::UtilState::getLookup( dataLookup, DataSource::DATA_PATH);
            QString fileName = dataState.getValue<QString>( fileLookup );
            if ( !fileName.isEmpty()){
                addData( fileName );
            }
            else {
                _addGroup( dataState.toString( dataLookup ));
            }
            dataIndex = m_datas.size() - 1;

        }
        loadedFiles.append( id );
        if ( dataIndex >= 0 ){
            m_datas[dataIndex]->_resetState(dataState.toString(dataLookup));
            _loadView( true, dataIndex );
        }
    }

    //Remove any data that should not be there
    int stackCount = m_datas.size();
    for ( int i = stackCount-1; i>= 0; i--){
        QString id = m_datas[i]->_getId();
        if ( !loadedFiles.contains( id )){
            _removeData( i );
        }
    }
    m_stackDraw->setLayers( m_datas );

    //Reset the image select state.
    QString dataStateStr = dataState.getValue<QString>( Selection::IMAGE );
    m_selectImage ->resetState( dataStateStr );
    m_stackDraw->setSelectIndex( m_selectImage->getIndex());
    _renderAll();
*/


    //Restore the region State
    /*m_regions.clear();
    int regionCount = dataState.getArraySize(REGIONS);
    for ( int i = 0; i < regionCount; i++ ){
        QString regionLookup = Carta::State::UtilState::getLookup( REGIONS, i );
        QString regionState = dataState.toString( regionLookup );
        std::shared_ptr<Region> region = RegionFactory::makeRegion( regionState );
        m_regions.append( region );
    }
    _saveStateRegions();*/

    //Notify others there has been a change to the data.
    emit dataChanged( this );
    emit dataChangedRegion( this );

    //Reset the state of the grid controls based on the selected image.
    StateInterface gridState = m_stack->_getGridState();
    m_gridControls->_resetState( gridState );
    _loadViewQueued( true );
}

void Controller::resetPan(){
    bool panZoomAll = m_state.getValue<bool>( PAN_ZOOM_ALL );
    m_stack->_resetPan( panZoomAll );
}

void Controller::resetZoom(){
    bool panZoomAll = m_state.getValue<bool>( PAN_ZOOM_ALL );
    m_stack->_resetZoom( panZoomAll );
}


QString Controller::saveImage( const QString& fileName ){
    QString result;
    DataLoader* dLoader = Util::findSingletonObject<DataLoader>();
    bool securityRestricted = dLoader->isSecurityRestricted();
    if ( !securityRestricted ){
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
            result = m_stack->_saveImage( fileName );
        }
    }
    else {
        result = "Write access to the file system is not available.";
    }
    return result;
}

void Controller::saveImageResultCB( bool result ){
    if ( !result ){
        QString msg = "There was a problem saving the image.";
        Util::commandPostProcess( msg );
    }
    else {
        QString msg = "Image was successfully saved.";
        ErrorManager* errorMan = Util::findSingletonObject<ErrorManager>();
        errorMan->registerInformation( msg );
    }
    emit saveImageResult( result );
}


void Controller::setAutoClip( bool autoClip ){
    bool oldAutoClip = m_state.getValue<bool>(AUTO_CLIP );
    if ( autoClip != oldAutoClip ){
        m_state.setValue<bool>( AUTO_CLIP, autoClip );
        m_state.flushState();
    }
}

QString Controller::setClipValue( double clipVal  ) {
    QString result;
    if ( 0 <= clipVal && clipVal < 1 ){
        double oldClipValMin = m_state.getValue<double>( CLIP_VALUE_MIN );
        double oldClipValMax = m_state.getValue<double>( CLIP_VALUE_MAX );
        double oldClipVal = oldClipValMax - oldClipValMin;
        const double ERROR_MARGIN = 0.000001;
        if ( qAbs( clipVal - oldClipVal) >= ERROR_MARGIN ){
            double leftOver = 1 - clipVal;
            double clipValMin = leftOver / 2;
            double clipValMax = clipVal + leftOver / 2;
            result = applyClips (clipValMin, clipValMax );
        }
    }
    else {
        result = "Clip value must be in [0,1).";
    }
    return result;
}


void Controller::_setFrameAxis(int value, AxisInfo::KnownType axisType ) {
    m_stack->_setFrameAxis( value, axisType );
    _updateCursorText( true );
}

void Controller::setFrameImage( int val) {
    if ( val < 0 ){
        return;
    }

    QString layerId = m_stack->_setFrameImage( val );
    if ( layerId.length() > 0 ) {
        //Reset the selected layers.
        if ( isStackSelectAuto() ){
            QStringList names;
            names.append( layerId );
            _setLayersSelected( names );
        }

        Carta::State::StateInterface gridState = m_stack->_getGridState();
        m_gridControls->_resetState( gridState );
        _updateCursorText( true );
        emit dataChanged( this );
    }
}


void Controller::_setColorMapGlobal( std::shared_ptr<ColorState> colorState ){
    m_stateColor = colorState;
}

void Controller::_setColorMapUseGlobal( bool global ) {
    //Reset the color maps.
    if ( global ){
        m_stack->_setColorMapGlobal( m_stateColor );
    }
    else {
        m_stack->_setColorMapGlobal( nullptr );
    }
    emit colorChanged( this );
}

QString Controller::setImageOrder( const QString& groupId, const std::vector<int>& indices ){
    QString result = m_stack->_setImageOrder( groupId, indices );
    return result;
}


QString Controller::setImageVisibility( /*int dataIndex*/const QString& idStr, bool visible ){
    QString result;
    if ( idStr.length() > 0 ){
    /*int dataCount = m_datas.size();
    if ( dataIndex >= 0 && dataIndex < dataCount ){
        bool oldVisible = m_datas[dataIndex]->_isVisible();
        if ( oldVisible != visible ){
            m_datas[dataIndex]->_setVisible( visible );

            int selectedImageIndex = _getIndexCurrent();
            //Update the upper bound on the number of images available.
            int visibleCount = getStackedImageCountVisible();
            m_selectImage->setUpperBound( visibleCount );*/
        bool visibilityChanged = m_stack->_setVisible( idStr, visible );
        if ( visibilityChanged ){
            emit dataChanged( this );
            //Render the image if it is the one currently being viewed.
            /*if ( selectedImageIndex == dataIndex  ){
                _scheduleFrameReload( false );
            }*/
            //if ( visibleCount == 0 ){
            if ( m_stack->_getStackSizeVisible() == 0 ){
                _clearStatistics();
            }
            //saveState();
        }
    }
    else {
        result = "Could not set image visibility; invalid identifier.";
    }
    return result;
}

QString Controller::setLayersSelected( const QStringList indices ){
    QString result;
    if ( indices.size() > 0 ){
        bool selectModeAuto = isStackSelectAuto();
        if ( !selectModeAuto ){
            result = _setLayersSelected( indices );
        }
        else {
            result = "Enable manual layer selection mode before setting layers.";
        }
    }
    else {
        result = "Please specify one or more layers to select.";
    }
    return result;
}


QString Controller::_setLayersSelected( const QStringList names ){
    QString result;
    bool stackAutoSelect = m_state.getValue<bool>(STACK_SELECT_AUTO);
    bool selectStateChanged = m_stack->_setSelected( names );
    if ( selectStateChanged ){
        if ( names.size() > 0 ){
            int selectIndex = m_stack->_getIndex(names[0]);
            if (  !stackAutoSelect  ){
                setFrameImage( selectIndex );
            }
        }
        emit colorChanged( this );
    }

    return result;
}

QStringList Controller::setMaskColor( const QString& id, int redAmount, int greenAmount, int blueAmount ){
    QStringList result;
    m_stack->_setMaskColor( id, redAmount, greenAmount, blueAmount, result );

    return result;
}

QString Controller::setMaskAlpha( const QString& id, int alphaAmount ){
    QString result;
    m_stack->_setMaskAlpha( id, alphaAmount, result );
    return result;
}

void Controller::setPanZoomAll( bool panZoomAll ){
    bool oldPanZoomAll = m_state.getValue<bool>(PAN_ZOOM_ALL);
    if ( panZoomAll != oldPanZoomAll ){
        m_state.setValue<bool>( PAN_ZOOM_ALL, panZoomAll );
        m_state.flushState();
    }
}


void Controller::setStackSelectAuto( bool automatic ){
    bool oldStackSelectAuto = m_state.getValue<bool>(STACK_SELECT_AUTO );
    if ( oldStackSelectAuto != automatic ){
        m_state.setValue<bool>( STACK_SELECT_AUTO, automatic );
        m_state.flushState();
    }
}

QString Controller::setCompositionMode( const QString& id, const QString& compMode ){
    QString result;
    m_stack->_setCompositionMode( id, compMode, result );
    return result;
}

QString Controller::setSelectedLayersGrouped( bool grouped ){
    QString result;
    bool operationPerformed = m_stack->_setLayersGrouped( grouped );
    if ( operationPerformed ){
        //Notify others there has been a change to the data.
        emit dataChanged( this );
    }
    else {
        if ( grouped ){
            result = "The selected layer(s) could not be grouped.";
        }
        else {
            result = "Please make sure only one composite layer is selected to ungroup";
        }
    }
    return result;
}

QString Controller::setTabIndex( int index ){
    QString result;
    if ( index >= 0 ){
        int oldIndex = m_state.getValue<int>( Util::TAB_INDEX );
        if ( index != oldIndex ){
            m_state.setValue<int>( Util::TAB_INDEX, index );
            m_state.flushState();
        }
    }
    else {
        result = "Image settings tab index must be nonnegative: "+ QString::number(index);
    }
    return result;
}


void Controller::setZoomLevel( double zoomFactor ){
    bool zoomPanAll = m_state.getValue<bool>(PAN_ZOOM_ALL);
    m_stack->_setZoomLevel( zoomFactor, zoomPanAll );
}


void Controller::_updateCursor( int mouseX, int mouseY ){
    if ( m_stack->_getStackSize() == 0 ){
        return;
    }

    int oldMouseX = m_stateMouse.getValue<int>( ImageView::MOUSE_X );
    int oldMouseY = m_stateMouse.getValue<int>( ImageView::MOUSE_Y );
    if ( oldMouseX != mouseX || oldMouseY != mouseY ){
        m_stateMouse.setValue<int>( ImageView::MOUSE_X, mouseX);
        m_stateMouse.setValue<int>( ImageView::MOUSE_Y, mouseY );
        _updateCursorText( false );
        m_stateMouse.flushState();
    }
}

void Controller::_updateCursorText(bool notifyClients ){
    QString formattedCursor;
    int mouseX = m_stateMouse.getValue<int>(ImageView::MOUSE_X );
    int mouseY = m_stateMouse.getValue<int>(ImageView::MOUSE_Y );
    QString cursorText = m_stack->_getCursorText( mouseX, mouseY);
    if ( !cursorText.isEmpty()){
        if ( cursorText != m_stateMouse.getValue<QString>(CURSOR)){
            m_stateMouse.setValue<QString>( CURSOR, cursorText );
            if ( notifyClients ){
                m_stateMouse.flushState();
            }
        }
    }
}


void Controller::_updateDisplayAxes(){
    if ( m_gridControls ){
        std::vector<AxisInfo::KnownType> supportedAxes = m_stack->_getAxisTypes();
        m_gridControls->_setAxisTypes( supportedAxes );
        AxisInfo::KnownType xType = m_stack->_getAxisXType();
        AxisInfo::KnownType yType = m_stack->_getAxisYType();
        const Carta::Lib::KnownSkyCS cs = getCoordinateSystem();
        QString xPurpose = AxisMapper::getPurpose( xType, cs );
        QString yPurpose = AxisMapper::getPurpose( yType, cs );
        m_gridControls->setAxis( AxisMapper::AXIS_X, xPurpose );
        m_gridControls->setAxis( AxisMapper::AXIS_Y, yPurpose );
    }
}


void Controller::updateZoom( double centerX, double centerY, double zoomFactor ){
    bool zoomPanAll = m_state.getValue<bool>(PAN_ZOOM_ALL);
    m_stack->_updateZoom( centerX, centerY, zoomFactor, zoomPanAll);
}


void Controller::updatePan( double centerX , double centerY){
    bool zoomPanAll = m_state.getValue<bool>(PAN_ZOOM_ALL);
    m_stack->_updatePan( centerX, centerY, zoomPanAll );
    _updateCursorText( true );
}


Controller::~Controller(){
    //unregisterView();
    clear();
}

}
}
