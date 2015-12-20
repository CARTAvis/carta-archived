#include "State/ObjectManager.h"
#include "State/UtilState.h"
#include "Data/Image/Controller.h"
#include "Data/Image/DrawStackSynchronizer.h"
#include "Data/Image/Grid/AxisMapper.h"
#include "Data/Image/Grid/DataGrid.h"
#include "Data/Image/Grid/GridControls.h"
#include "Data/Image/Contour/ContourControls.h"
#include "Data/Image/Contour/DataContours.h"
#include "ControllerData.h"
#include "Data/Settings.h"
#include "Data/DataLoader.h"
#include "DataSource.h"

#include "Data/Error/ErrorManager.h"
#include "Data/Selection.h"
#include "Data/Region.h"
#include "Data/RegionRectangle.h"
#include "Data/Util.h"
#include "ImageView.h"
#include "CartaLib/IImage.h"
#include "CartaLib/IRemoteVGView.h"
#include "CartaLib/PixelPipeline/CustomizablePixelPipeline.h"


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
const QString Controller::REGIONS = "regions";
const QString Controller::PLUGIN_NAME = "CasaImageLoader";
const QString Controller::STACK_SELECT_AUTO = "stackAutoSelect";
const QString Controller::VIEW = "view";

const QString Controller::CLASS_NAME = "Controller";
bool Controller::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass (CLASS_NAME,
                                                   new Controller::Factory());

using Carta::State::UtilState;
using Carta::State::StateInterface;
using Carta::Lib::AxisInfo;

Controller::Controller( const QString& path, const QString& id ) :
        CartaObject( CLASS_NAME, path, id),
        m_selectImage(nullptr),
        m_stackDraw(nullptr),
        m_stateData( UtilState::getLookup(path, StateInterface::STATE_DATA )),
        m_stateMouse(UtilState::getLookup(path, VIEW)){
    QString viewName = Carta::State::UtilState::getLookup( path, VIEW);
    m_stackDraw.reset( new DrawStackSynchronizer(makeRemoteView( viewName)));
    
    m_reloadFrameQueued = false;

    _initializeSelections();

     _initializeState();

     connect( m_stackDraw.get(), SIGNAL(viewResize()), this, SLOT(_viewResize()));

     Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
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

     //Load the view.
     _scheduleFrameReload();

     _initializeCallbacks();
}

void Controller::addContourSet( std::shared_ptr<DataContours> contourSet){
    int dataCount = m_datas.size();
    for ( int i = 0; i < dataCount; i++ ){
        if ( m_datas[i]->_isSelected() ){
            m_datas[i]->_addContourSet( contourSet );
        }
    }
}

bool Controller::addData(const QString& fileName) {
    //Find the location of the data, if it already exists.
    int targetIndex = _getIndex( fileName );

    //Add the data if it is not already there.
    if (targetIndex == -1) {

        Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
        ControllerData* targetSource = objMan->createObject<ControllerData>();

        targetIndex = m_datas.size();
        connect( targetSource, & ControllerData::saveImageResult, this, & Controller::saveImageResultCB );
        connect( targetSource, SIGNAL(contourSetAdded(ControllerData*,const QString&)),
                this, SLOT(_contourSetAdded(ControllerData*, const QString&)));
        connect( targetSource, SIGNAL(contourSetRemoved(const QString&)),
                        this, SLOT(_contourSetRemoved(const QString&)));
        m_datas.append(std::shared_ptr<ControllerData>(targetSource));
        m_stackDraw->setLayers( m_datas );

        targetSource->_viewResize( m_stackDraw->getClientSize() );

        //Colormap
        targetSource->_setColorMapGlobal( m_stateColor );
        connect( targetSource, SIGNAL(colorStateChanged()), this, SLOT( _colorMapChanged() ));

        //Update the data selectors upper bound based on the data.
        int visibleCount = getStackedImageCountVisible();
        m_selectImage->setUpperBound( visibleCount );
    }

    bool successfulLoad = m_datas[targetIndex]->_setFileName(fileName );
    if ( successfulLoad ){
        int selectCount = m_selects.size();
        for ( int i = 0; i < selectCount; i++ ){
            AxisInfo::KnownType type = static_cast<AxisInfo::KnownType>(i);
            int frameCount = m_datas[targetIndex]->_getFrameCount( type );
            m_selects[i]->setUpperBound( frameCount );
        }
        m_selectImage->setIndex(targetIndex);
        if ( isStackSelectAuto() ){
            std::vector<int> selectedLayers(1);
            selectedLayers[0] = targetIndex;
            _setLayersSelected( selectedLayers );
        }

        saveState();

        //Refresh the view of the data.
        _scheduleFrameReload();

        //Notify others there has been a change to the data.
        emit dataChanged( this );
        _updateDisplayAxes( targetIndex );
    }
    else {
        QString error = "Unable to load image: "+fileName+".  Please check the file is a supported image format.";
        Util::commandPostProcess( error );
        _removeData( targetIndex );
    }
    return successfulLoad;
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
            _scheduleFrameReload( true );
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

void Controller::_clearData(){
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    for ( std::shared_ptr<ControllerData> source : m_datas ){
        QString id = source->getId();
        objMan->removeObject( id );
    }
}

void Controller::_clearStatistics(){
    m_stateMouse.setValue<QString>( CURSOR, "" );
    m_stateMouse.flushState();
}

QString Controller::closeImage( const QString& name ){
    int targetIndex = -1;
    QString result;

    int dataCount = m_datas.size();
    for ( int i = 0; i < dataCount; i++ ){
        if ( m_datas[i]->_isMatch( name )){
            targetIndex = i;
            break;
        }
    }
    if ( targetIndex >= 0 ){
        _removeData( targetIndex );
        m_stackDraw->setLayers( m_datas );
        emit dataChanged( this );
    }
    else {
        result = "Could not find data to remove for name="+name;
    }
    return result;
}

void Controller::_colorMapChanged(){
    _renderAll();
}



void Controller::centerOnPixel( double centerX, double centerY ){
    bool panZoomAll = m_state.getValue<bool>( PAN_ZOOM_ALL );
    if ( panZoomAll ){
        int dataCount = m_datas.size();
        for ( int i = 0; i < dataCount; i++ ){
            m_datas[i]->_setPan( centerX, centerY );
        }
        _renderAll();
    }
    else {
        int dataIndex = _getIndexCurrent();
        if ( dataIndex >= 0 ){
            m_datas[dataIndex]->_setPan( centerX, centerY );
        }
        _renderSingle( dataIndex );
    }
}

void Controller::_contourSetAdded( ControllerData* cData, const QString& setName ){
    if ( cData != nullptr ){
        std::shared_ptr<DataContours> addedSet = cData->_getContour( setName );
        if ( addedSet ){
            m_contourControls->_setDrawContours( addedSet );
        }
    }
}

void Controller::_contourSetRemoved( const QString setName ){
    //Remove the contour set from the controls only if nothing in the stack
    //is using it.
    bool stillExists = true;
    int dataCount = m_datas.size();
    for ( int i = 0; i < dataCount; i++ ){
        if ( m_datas[i]->_getContour( setName ) ){
            stillExists = true;
            break;
        }
    }
    if ( !stillExists ){
        m_contourControls->deleteContourSet( setName );
    }
}

void Controller::_contoursChanged(){
    _renderAll();
}

void Controller::_displayAxesChanged(std::vector<AxisInfo::KnownType> displayAxisTypes,
        bool applyAll ){
    if ( !applyAll ){
        int dataIndex = _getIndexCurrent();
        if (dataIndex >= 0 ) {
            if (m_datas[dataIndex] != nullptr) {
                std::vector<int> frames = _getFrameIndices();
                m_datas[dataIndex]->_displayAxesChanged( displayAxisTypes, frames );
            }
        }
    }
    else {
        int dataCount = m_datas.size();
        for ( int i = 0; i < dataCount; i++ ){
            if ( m_datas[i] != nullptr ){
                std::vector<int> frames = _getFrameIndices();
                m_datas[i]->_displayAxesChanged( displayAxisTypes, frames );
            }
        }
    }
    emit axesChanged();
    _scheduleFrameReload( false );
    _updateCursorText( true );
}

std::vector<Carta::Lib::AxisInfo::KnownType> Controller::_getAxisZTypes() const {
    int dataIndex = _getIndexCurrent();
    std::vector<Carta::Lib::AxisInfo::KnownType> zTypes;
    if ( 0 <= dataIndex ){
        zTypes = m_datas[dataIndex]->_getAxisZTypes();
    }
    return zTypes;
}

std::set<AxisInfo::KnownType> Controller::_getAxesHidden() const {
    int dataCount = m_datas.size();
    std::set<AxisInfo::KnownType> axes;
    for ( int i = 0; i < dataCount; i++ ){
        if ( m_datas[i]->_isVisible() ){
            std::vector<AxisInfo::KnownType> zAxes = m_datas[i]->_getAxisZTypes();
            int zAxesCount = zAxes.size();
            for ( int j = 0; j < zAxesCount; j++ ){
                axes.insert( zAxes[j] );
            }
        }
    }
    return axes;
}


QPointF Controller::getCenterPixel() {
    int dataIndex = _getIndexCurrent();
    QPointF center = QPointF( nan(""), nan("") );
    if ( dataIndex >= 0 ) {
        center = m_datas[dataIndex]->_getCenter();
    }
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
    int dataIndex = _getIndexCurrent();
    Carta::Lib::KnownSkyCS cs = Carta::Lib::KnownSkyCS::Unknown;
    if ( 0 <= dataIndex ){
        cs = m_datas[dataIndex]->_getCoordinateSystem();
    }
    return cs;
}

QStringList Controller::getCoordinates( double x, double y, Carta::Lib::KnownSkyCS system) const {
    QStringList result;
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        QStringList coordList = m_datas[dataIndex]->_getCoordinates( x, y, system, _getFrameIndices() );
        for ( int i = 0; i <= 1; i++ ){
            result.append( coordList[i] );
        }
    }
    return result;
}

std::vector< std::shared_ptr <Carta::Lib::Image::ImageInterface> > Controller::getDataSources(){
    //For right now, we are only going to do a histogram of a single image.
    std::vector<std::shared_ptr<Carta::Lib::Image::ImageInterface> > images;
    int dataCount = m_datas.size();
    if ( dataCount > 0 ){
        int dataIndex = _getIndexCurrent();
        if ( 0 <= dataIndex ){
            images.push_back( m_datas[dataIndex]->_getImage());
        }
    }
    return images;
}

std::shared_ptr<ContourControls> Controller::getContourControls() {
    return m_contourControls;
}

int Controller::getFrameUpperBound( AxisInfo::KnownType axisType ) const {
    int upperBound = 0;
    if ( axisType != AxisInfo::KnownType::OTHER ){
        int selectIndex = static_cast<int>(axisType );
        upperBound = m_selects[selectIndex]->getUpperBound();
    }
    return upperBound;
}

int Controller::getFrame( AxisInfo::KnownType axisType ) const {
    int frame = -1;
    if ( axisType != AxisInfo::KnownType::OTHER ){
        int dataIndex = _getIndexCurrent();
        if ( 0 <= dataIndex ){
            std::vector<AxisInfo::KnownType> supportedAxes = m_datas[dataIndex]->_getAxisTypes();
            //Make sure the axis is an axis in the image.
            std::vector<AxisInfo::KnownType>::iterator it  = find( supportedAxes.begin(), supportedAxes.end(), axisType );
            if ( it != supportedAxes.end() ){
                int selectIndex = static_cast<int>( axisType );
                frame = m_selects[selectIndex]->getIndex();
            }
        }
    }
    return frame;
}

std::vector<int> Controller::_getFrameIndices( ) const {
    int selectCount = m_selects.size();
    std::vector<int> frames( selectCount );
    //Determine the index of the frame to load.
    for ( int i = 0; i < selectCount; i++ ){
        frames[i] = m_selects[i]->getIndex();
    }
    return frames;
}


std::shared_ptr<GridControls> Controller::getGridControls() {
    return m_gridControls;
}

std::vector<int> Controller::getImageDimensions( ){
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
}

QString Controller::getImageName(int index) const{
    QString name;
    if ( 0 <= index && index < m_datas.size()){
        name = m_datas[index]->_getFileName();
    }
    return name;
}

int Controller::_getIndex( const QString& fileName) const{
    int dataCount = m_datas.size();
    int targetIndex = -1;
    for ( int i = 0; i < dataCount; i++ ){
        QString dataName = m_datas[i]->_getFileName();
        if ( fileName == dataName ){
            targetIndex = i;
            break;
        }
    }
    return targetIndex;
}

int Controller::_getIndexData( ControllerData* target ) const {
    int targetIndex = -1;
    if ( target != nullptr ){
        QString targetName = target->_getFileName();
        targetIndex = _getIndex( targetName);
    }
    return targetIndex;
}

int Controller::_getIndexCurrent( ) const {
    int index = m_selectImage->getIndex();
    int dataIndex = -1;
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
    return dataIndex;
}


bool Controller::getIntensity( double percentile, double* intensity ) const{
    int currentFrame = getFrame( AxisInfo::KnownType::SPECTRAL);
    bool validIntensity = getIntensity( currentFrame, currentFrame, percentile, intensity );
    return validIntensity;
}

bool Controller::getIntensity( int frameLow, int frameHigh, double percentile, double* intensity ) const{
    bool validIntensity = false;
    int dataIndex = _getIndexCurrent();
    if ( 0 <= dataIndex && percentile >= 0.0 && percentile <= 1.0 ){
        validIntensity = m_datas[dataIndex]->_getIntensity( frameLow, frameHigh, percentile, intensity );
    }
    return validIntensity;
}

QSize Controller::getOutputSize( ){
    QSize result(-1,-1);
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        result = m_datas[dataIndex]->_getOutputSize();
    }
    return result;
}


double Controller::getPercentile( double intensity ) const {
    int currentFrame = getFrame( AxisInfo::KnownType::SPECTRAL );
    return getPercentile( currentFrame, currentFrame, intensity );
}

double Controller::getPercentile( int frameLow, int frameHigh, double intensity ) const {
    double percentile = -1;
    int dataIndex = _getIndexCurrent();
    if ( 0 <= dataIndex ){
        percentile = m_datas[dataIndex]->_getPercentile( frameLow, frameHigh, intensity );
    }
    return percentile;
}

std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> Controller::getPipeline() const {
    std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> pipeline(nullptr);
    //Color map should be based on the selected image rather than the current image.
    int dataIndex = _getIndexCurrent();
    int dataCount = m_datas.size();
    for ( int i = 0; i < dataCount; i++ ){
        if ( m_datas[i]->_isSelected() ){
            dataIndex = i;
            break;
        }
    }
    if ( 0 <= dataIndex ){
        pipeline = m_datas[dataIndex]->_getPipeline();
    }
    return pipeline;
}

QStringList Controller::getPixelCoordinates( double ra, double dec ) const {
    QStringList result("");
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        result = m_datas[dataIndex]->_getPixelCoordinates( ra, dec );
    }
    return result;
}

QString Controller::getPixelValue( double x, double y ) const {
    QString result("");
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        std::vector<int> frames = _getFrameIndices();
        result = m_datas[dataIndex]->_getPixelValue( x, y, frames );
    }
    return result;
}

QString Controller::getPixelUnits() const {
    QString result("");
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        result = m_datas[dataIndex]->_getPixelUnits();
    }
    return result;
}

QString Controller::_getPreferencesId() const {
    QString id;
    if ( m_settings.get() != nullptr ){
        id = m_settings->getPath();
    }
    return id;
}

int Controller::getSelectImageIndex() const {
    int selectImageIndex = -1;
    int stackedImageVisibleCount = getStackedImageCountVisible();
    if ( stackedImageVisibleCount >= 1 ){
        selectImageIndex = m_selectImage->getIndex();
    }
    return selectImageIndex;
}



std::vector< std::shared_ptr<ColorState> >  Controller::getSelectedColorStates(){
    std::vector< std::shared_ptr<ColorState> > colorStates;
    int dataCount = m_datas.size();
    for ( int i = 0; i < dataCount; i++ ){
        if ( m_datas[i]->_isSelected() ){
            colorStates.push_back( m_datas[i]->_getColorState() );
        }
    }
    return colorStates;
}



int Controller::getStackedImageCount() const {
    return m_datas.size();
}

int Controller::getStackedImageCountVisible() const {
    int visibleCount = 0;
    int imageCount = m_datas.size();
    for ( int i = 0; i < imageCount; i++ ){
        if ( m_datas[i]->_isVisible() ){
            visibleCount++;
        }
    }
    return visibleCount;
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
        //Data state should include an array of data of all DataSources (images loaded & grid controls)
        //Data state should include DataSource that is selected
        //Data state should include channel that is selected.
        Carta::State::StateInterface dataState("");
        DataLoader* dataLoader = Util::findSingletonObject<DataLoader>();

        QString rootDir = dataLoader->getRootDir( sessionId );
        int dataCount = m_datas.size();
        dataState.insertArray( DATA, dataCount );
        for ( int i = 0; i < dataCount; i++ ){
            QString lookup = Carta::State::UtilState::getLookup( DATA, i );
            QString dataStateStr = m_datas[i]->_getStateString();
            dataState.setObject( lookup, dataStateStr );
        }
        dataState.setValue<QString>( StateInterface::OBJECT_TYPE, CLASS_NAME + StateInterface::STATE_DATA);
        dataState.setValue<int>(StateInterface::INDEX, getIndex() );
        int selectCount = m_selects.size();
        const Carta::Lib::KnownSkyCS cs = getCoordinateSystem();
        for ( int i = 0; i < selectCount; i++ ){
            QString axisName = AxisMapper::getPurpose( static_cast<AxisInfo::KnownType>(i), cs );
            dataState.insertValue<QString>( axisName, m_selects[i]->getStateString());
        }
        dataState.insertValue<QString>( Selection::IMAGE, m_selectImage->getStateString());
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
    double zoom = DataSource::ZOOM_DEFAULT;
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        zoom = m_datas[dataIndex]->_getZoom( );
    }
    return zoom;
}

void Controller::_gridChanged( const StateInterface& state, bool applyAll ){
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        if ( !applyAll ){
            m_datas[dataIndex]->_gridChanged( state );
            _renderSingle( dataIndex );
        }
        else {
            int dataCount = m_datas.size();
            for ( int i = 0; i < dataCount; i++ ){
                if ( m_datas[i] != nullptr ){
                    m_datas[i]->_gridChanged( state );
                }
            }
            _renderAll();
        }
        _updateCursorText( true );
    }
}

void Controller::_initializeCallbacks(){
    addCommandCallback( "hideImage", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {IMAGE};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString imageIndexStr = dataValues[*keys.begin()];
        bool validInt = false;
        int imageIndex = imageIndexStr.toInt( &validInt );
        QString result;
        if ( validInt ){
            result = setImageVisibility( imageIndex, false );
        }
        else {
            result = "Invalid image hide index: "+params;
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "showImage", [=] (const QString & /*cmd*/,
                            const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {IMAGE};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString imageIndexStr = dataValues[*keys.begin()];
        bool validInt = false;
        int imageIndex = imageIndexStr.toInt( &validInt );
        QString result;
        if ( validInt ){
            result = setImageVisibility( imageIndex, true );
        }
        else {
            result = "Invalid image show index: "+params;
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setImageOrder", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
        bool parseError = false;
        std::vector<int> vals = Util::string2VectorInt( params, &parseError, ";" );
        QString result;
        if ( !parseError ){
            result = setImageOrder( vals );
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

    QString pointerPath= UtilState::getLookup( getPath(), UtilState::getLookup( VIEW, POINTER_MOVE));
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
        std::set<QString> keys = {"image"};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString imageName = dataValues[*keys.begin()];
        DataLoader* dataLoader = Util::findSingletonObject<DataLoader>();
            QString longName = dataLoader->getLongName( imageName, "");
        QString result = closeImage( longName );
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
        const QString TYPE( "type");
        const QString INDEX( "index");
        std::set<QString> keys = {TYPE, INDEX};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString shapePath;
        bool validIndex = false;
        int index = dataValues[INDEX].toInt( &validIndex );
        if ( validIndex ){
            int regionCount = m_regions.size();
            if ( 0 <= index && index < regionCount ){
                //Measure the index from the end.
                shapePath = m_regions[index]->getPath();
            }
            else {
                shapePath = _makeRegion( dataValues[TYPE]);
                if ( shapePath.size() == 0 ){
                    qDebug()<<"Error registerShape unsupported shape: "<<params;
                }
                else {
                    saveState();
                }
            }
        }
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
        bool error = false;
        std::vector<int> vals = Util::string2VectorInt( params, &error, ";" );
        if ( error ){
            result = "Please specify the layers to select as nonnegative integers";
        }
        else {
            result = setLayersSelected( vals );
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
        std::set<QString> keys = {Util::RED, Util::GREEN, Util::BLUE};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString result;
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
            QStringList errorList = setMaskColor( redAmount, greenAmount, blueAmount );
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
        std::set<QString> keys = { Util::ALPHA };
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString result;
        QString alphaStr = dataValues[Util::ALPHA];
        bool validAlpha = false;
        int alphaAmount = alphaStr.toInt( &validAlpha );
        if ( validAlpha ){
            result = setMaskAlpha( alphaAmount );
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
        std::set<QString> keys = {ControllerData::COMPOSITION_MODE};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString compMode = dataValues[ControllerData::COMPOSITION_MODE];
        QString result = setCompositionMode( compMode );
        Util::commandPostProcess( result );
        return result;
    });
}



void Controller::_initializeSelections(){
    int axisCount = static_cast<int>(AxisInfo::KnownType::OTHER);
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    m_selects.resize( axisCount );
    for ( int i = 0; i < axisCount; i++ ){
        m_selects[i] = objMan->createObject<Selection>();
        connect( m_selects[i], SIGNAL(indexChanged(bool)), this, SLOT(_scheduleFrameReload()));
    }
    m_selectImage = objMan->createObject<Selection>();
    connect( m_selectImage, SIGNAL(indexChanged(bool)), this, SLOT(_scheduleFrameReload()));
}


void Controller::_initializeState(){
    //First the preference state.
    m_state.insertValue<bool>( AUTO_CLIP, true );
    m_state.insertValue<bool>(PAN_ZOOM_ALL, true );
    m_state.insertValue<bool>( STACK_SELECT_AUTO, true );
    m_state.insertValue<double>( CLIP_VALUE_MIN, 0.025 );
    m_state.insertValue<double>( CLIP_VALUE_MAX, 0.975 );
    m_state.flushState();

    //Now the data state.
    m_stateData.insertArray(DATA, 0 );

    //For testing only.
    //_makeRegion( RegionRectangle::CLASS_NAME );
    int regionCount = m_regions.size();
    m_stateData.insertArray(REGIONS, regionCount );
    //_saveRegions();
    m_stateData.flushState();

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

void Controller::_loadView(  bool newClips ) {
    m_reloadFrameQueued = false;
    //Determine the index of the data to load.
    int dataIndex = _getIndexCurrent();
    _loadView( newClips, dataIndex );
}

void Controller::_loadView( bool newClips, int dataIndex ){
    if ( dataIndex >= 0 ) {
        if (m_datas[dataIndex] != nullptr) {

            //Determine the indices of the frames to load.
            vector<int> frames = _getFrameIndices();

            //Load the image.
            bool autoClip = m_state.getValue<bool>(AUTO_CLIP);
            if ( newClips ){
                autoClip = true;
            }
            double clipValueMin = m_state.getValue<double>(CLIP_VALUE_MIN);
            double clipValueMax = m_state.getValue<double>(CLIP_VALUE_MAX);
            m_datas[dataIndex]->_load(frames, autoClip, clipValueMin, clipValueMax);
            _renderSingle( dataIndex );
        }
        else {
            qDebug() << "Uninitialized image: "<<dataIndex;
        }
    }
}


QString Controller::_makeRegion( const QString& regionType ){
    QString shapePath = Region::makeRegion( regionType );
    if ( shapePath.size() > 0 ){
        Carta::State::ObjectManager* objManager = Carta::State::ObjectManager::objectManager();
        Carta::State::CartaObject* shapeObj = objManager->getObject( shapePath );
        shapePath = shapeObj->getPath();
        m_regions.append(dynamic_cast<Region*>(shapeObj));

     }
    return shapePath;
}

void Controller::removeContourSet( std::shared_ptr<DataContours> contourSet ){
    int dataCount = m_datas.size();
    for ( int i = 0; i < dataCount; i++ ){
        m_datas[i]->_removeContourSet( contourSet );
    }
}


void Controller::_removeData( int index ){
    disconnect( m_datas[index].get());
    int selectedImage = m_selectImage->getIndex();
    int selectedDataIndex = _getIndexCurrent();
    QString id = m_datas[index]->getId();
    bool visible = m_datas[index]->_isVisible();
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    objMan->removeObject( id );
    m_datas.removeAt( index );
    int visibleImageCount = getStackedImageCountVisible();
    m_selectImage->setUpperBound( visibleImageCount );
    if ( selectedDataIndex == index ){
       m_selectImage->setIndex( 0 );
    }
    else if ( index < selectedDataIndex && visible ){
       int imageCountDecreased = selectedImage - 1;
       m_selectImage->setIndex( imageCountDecreased );
    }
    //Update the channel upper bound and index if necessary
    int targetData = _getIndexCurrent();
    int selectCount = m_selects.size();
    for ( int i = 0; i < selectCount; i++ ){
        int frameCount = 0;
        AxisInfo::KnownType axisType= static_cast<AxisInfo::KnownType>( i );
        if ( targetData >= 0 ){
            frameCount = m_datas[targetData]->_getFrameCount( axisType );
        }
        int oldIndex = m_selects[i]->getIndex();
        if ( oldIndex >= frameCount && frameCount > 0){
            _setFrameAxis( frameCount - 1, axisType );
        }
        else {
            _setFrameAxis( 0, axisType );
        }
        m_selects[i]->setUpperBound( frameCount );
    }
    _scheduleFrameReload( false );

    //Clear the statistics window if there are no images.
    if ( visibleImageCount == 0 ){
        _clearStatistics();
    }

    saveState();
}

void Controller::_renderAll(){
    int gridIndex = _getIndexCurrent();
    _render( m_datas, gridIndex );
}

void Controller::_renderSingle( int dIndex ){
    if ( dIndex >= 0 && dIndex >= m_datas.size() ){
        QList<std::shared_ptr<ControllerData> > datas;
        datas.append( m_datas[dIndex] );
        int topIndex = _getIndexCurrent();
        int gridIndex = -1;
        if ( dIndex == topIndex ){
            gridIndex = 0;
        }
        _render( datas, gridIndex );
    }
}

void Controller::_render( QList<std::shared_ptr<ControllerData> > datas, int gridIndex ){
    std::vector<int> frames =_getFrameIndices();
    const Carta::Lib::KnownSkyCS& cs = getCoordinateSystem();
    m_stackDraw->_render( datas, frames, cs, gridIndex );
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

    //Reset the loaded images
    std::vector<int> frames = _getFrameIndices();
    int dataCount = dataState.getArraySize(DATA);
    QStringList loadedFiles;
    for ( int i = 0; i < dataCount; i++ ){
        QString dataLookup = Carta::State::UtilState::getLookup( DATA, i );
        QString fileLookup = Carta::State::UtilState::getLookup( dataLookup, DataSource::DATA_PATH);
        QString fileName = dataState.getValue<QString>( fileLookup );
        int dataIndex = _getIndex( fileName );
        if ( dataIndex == -1 ){
            addData( fileName );
            dataIndex = m_datas.size() - 1;
        }
        loadedFiles.append( fileName );
        if ( dataIndex >= 0 ){
            m_datas[dataIndex]->_resetState(dataState.toString(dataLookup));
            _loadView( true, dataIndex );
        }
    }

    //Remove any data that should not be there
    int stackCount = m_datas.size();
    for ( int i = stackCount-1; i>= 0; i--){
        QString fileName = m_datas[i]->_getFileName();
        if ( !loadedFiles.contains( fileName)){
            _removeData( i );
        }
    }
    m_stackDraw->setLayers( m_datas );

    //Reset the image select state.
    QString dataStateStr = dataState.getValue<QString>( Selection::IMAGE );
    m_selectImage ->resetState( dataStateStr );
    m_stackDraw->setSelectIndex( m_selectImage->getIndex());
    _renderAll();

    //Now we need to restore the axis states.
    int selectCount = m_selects.size();
    for ( int i = 0; i < selectCount; i++ ){
        AxisInfo::KnownType axisType = static_cast<AxisInfo::KnownType>( i );
        const Carta::Lib::KnownSkyCS cs = getCoordinateSystem();
        QString axisPurpose = AxisMapper::getPurpose( axisType, cs );
        QString axisState = dataState.getValue<QString>( axisPurpose );
        m_selects[i]->resetState( axisState );
    }

    //Notify others there has been a change to the data.
    emit dataChanged( this );

    //Reset the state of the grid controls based on the selected image.
    int dataIndex = _getIndexCurrent();
    if ( 0 <= dataIndex ){
        StateInterface controlState = m_datas[dataIndex]->_getGridState();
        this->m_gridControls->_resetState( controlState );
    }
}

void Controller::resetPan(){
    bool panZoomAll = m_state.getValue<bool>( PAN_ZOOM_ALL );
    if ( panZoomAll ){
        int dataCount = m_datas.size();
        if ( dataCount > 0 ){
            for ( int i = 0; i < dataCount; i++ ){
                m_datas[i]->_resetPan();
            }
            _renderAll();
        }
    }
    else {
        int dataIndex = _getIndexCurrent();
        if ( dataIndex >= 0 ){
            m_datas[dataIndex]->_resetPan();
            _renderSingle( dataIndex );
        }
    }
}

void Controller::resetZoom(){
    bool panZoomAll = m_state.getValue<bool>( PAN_ZOOM_ALL );
    if ( panZoomAll ){
        int dataCount = m_datas.size();
        if ( dataCount > 0 ){
            for ( int i = 0; i < dataCount; i++ ){
                m_datas[i]->_resetZoom();
            }
            _renderAll();
        }
    }
    else {
        int dataIndex = _getIndexCurrent();
        if ( dataIndex >= 0 ){
            m_datas[dataIndex]->_resetZoom();
            _renderSingle( dataIndex );
        }
    }
}


void Controller::saveState() {
    int dataCount = m_datas.size();
    int oldDataCount = m_stateData.getArraySize(DATA );
    if ( oldDataCount != dataCount ){
        //Insert the names of the data items for display purposes.
        m_stateData.resizeArray(DATA, dataCount, StateInterface::PreserveNone );
    }
    for (int i = 0; i < dataCount; i++) {
        QString layerString = m_datas[i]->_getLayerString();
        QString dataKey = UtilState::getLookup( DATA, i);
        m_stateData.setObject( dataKey, layerString);
    }

    /*int regionCount = m_regions.size();
    m_state.resizeArray( REGIONS, regionCount );
    _saveRegions();*/
    m_stateData.flushState();
}


QString Controller::saveImage( const QString& fileName ){
    double zoomLevel = getZoomLevel();
    return saveImage( fileName, zoomLevel );
}

QString Controller::saveImage( const QString& fileName, double scale ){
    QString result;
    DataLoader* dLoader = Util::findSingletonObject<DataLoader>();
    bool securityRestricted = dLoader->isSecurityRestricted();
    if ( !securityRestricted ){
        int dataIndex = _getIndexCurrent();
        if ( 0 <= dataIndex ){
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
                std::vector<int> frames = _getFrameIndices();
                result = m_datas[dataIndex]->_saveImage( fileName, scale, frames );
            }
        }
        else {
            result = "Please make sure there is an image to save.";
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

void Controller::_saveRegions(){
    int regionCount = m_regions.size();
    for ( int i = 0; i < regionCount; i++ ){
        QString arrayStr = UtilState::getLookup( REGIONS, i);
        QString regionType= m_regions[i]->getType();
        QString regionId = m_regions[i]->getPath();
        m_state.setObject( arrayStr );
        m_state.insertValue<QString>( UtilState::getLookup( arrayStr, "type"), regionType );
        m_state.insertValue<QString>( UtilState::getLookup( arrayStr, "id"), regionId );
    }
}


void Controller::_scheduleFrameReload( bool newClips ){
    if ( m_datas.size() > 0  ){
        // if reload is already pending, do nothing
        if ( m_reloadFrameQueued ) {
            return;
        }
        int selectIndex=m_selectImage->getIndex();
        m_stackDraw->setSelectIndex( selectIndex);
        _renderAll();
        m_reloadFrameQueued = true;
        QMetaObject::invokeMethod( this, "_loadView", Qt::QueuedConnection, Q_ARG(bool, newClips) );
    }
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
    int axisIndex = static_cast<int>( axisType );
    int selectCount = m_selects.size();
    if ( 0 <= axisIndex && axisIndex < selectCount ){
        int oldIndex = m_selects[axisIndex]->getIndex();
        if ( value != oldIndex ){
            m_selects[axisIndex]->setIndex(value);
            //We only need to update the cursor if the axis is a hidden axis
            //for the current image.
            int dataIndex = _getIndexCurrent();
            if ( 0 <= dataIndex ){
                _updateCursorText( true );
                emit channelChanged( this );
            }
        }
    }
}

void Controller::setFrameImage( int val) {
    if ( val < 0 ){
        return;
    }
    if (m_selectImage != nullptr) {
        int oldIndex = m_selectImage->getIndex();
        if ( oldIndex != val ){
            m_selectImage->setIndex(val);
            if ( isStackSelectAuto() ){
                std::vector<int> indices(1);
                indices[0] = val;
                _setLayersSelected( indices );
            }
            int dataIndex = _getIndexCurrent();
            if ( 0 <= dataIndex ){
                int selectCount = m_selects.size();
                for ( int i = 0; i < selectCount; i++ ){
                    AxisInfo::KnownType type = static_cast<AxisInfo::KnownType>(i);
                    int upperBound = m_datas[dataIndex]->_getFrameCount( type );
                    m_selects[i]->setUpperBound( upperBound );
                    if ( m_selects[i]->getIndex() > m_selects[i]->getUpperBound()){
                        m_selects[i]->setIndex( 0 );
                        if ( type == AxisInfo::KnownType::SPECTRAL ){
                            emit channelChanged( this );
                        }
                    }
                }

                Carta::State::StateInterface gridState = m_datas[dataIndex]->_getGridState();
                m_gridControls->_resetState( gridState );
            }
            _updateCursorText( true );
            emit dataChanged( this );
        }
    }
}


void Controller::_setColorMapGlobal( std::shared_ptr<ColorState> colorState ){
    m_stateColor = colorState;
}

void Controller::_setColorMapUseGlobal( bool global ) {
    //Loop through the data and reset the color maps.
    int dataCount = m_datas.size();
    for ( int i = 0; i < dataCount; i++ ){
        bool selected = m_datas[i]->_isSelected();
        if ( selected ){
            if ( global ){
                m_datas[i]->_setColorMapGlobal( m_stateColor );
            }
            else {
                m_datas[i]->_setColorMapGlobal( nullptr );
            }
        }
    }
    emit colorChanged( this );
}

QString Controller::setImageOrder( const std::vector<int>& indices ){
    QString result;
    bool imageReordered = false;
    int dataCount = m_datas.size();
    int indexCount = indices.size();
    QList<std::shared_ptr<ControllerData> > reorderedList;
    int selectedIndex = m_selectImage->getIndex();
    int newSelectedIndex = selectedIndex;
    if ( indexCount != dataCount ){
        result = "Reorder image size must match the stack count: "+QString::number(dataCount);
    }
    else {
        for ( int i = 0; i < indexCount; i++ ){
            int targetIndex = indices[i];
            if ( targetIndex < 0  || targetIndex >= dataCount ){
                result = "Reorder failed: unknown image index: "+targetIndex;
                break;
            }
            //Insert the image at the target index at position i.
            else {

                reorderedList.append( m_datas[targetIndex] );
                if ( targetIndex != i ){
                    if ( selectedIndex == targetIndex ){
                        newSelectedIndex = i;
                    }
                    imageReordered = true;
                }
            }
        }
    }
    if ( imageReordered ){
        m_datas = reorderedList;
        if ( selectedIndex != newSelectedIndex ){
            this->setFrameImage( newSelectedIndex );
        }
        _renderAll();
    }
    return result;
}

QString Controller::setImageVisibility( int dataIndex, bool visible ){
    QString result;
    int dataCount = m_datas.size();
    if ( dataIndex >= 0 && dataIndex < dataCount ){
        bool oldVisible = m_datas[dataIndex]->_isVisible();
        if ( oldVisible != visible ){
            m_datas[dataIndex]->_setVisible( visible );

            int selectedImageIndex = _getIndexCurrent();
            //Update the upper bound on the number of images available.
            int visibleCount = getStackedImageCountVisible();
            m_selectImage->setUpperBound( visibleCount );
            emit dataChanged( this );
            //Render the image if it is the one currently being viewed.
            if ( selectedImageIndex == dataIndex  ){
                _scheduleFrameReload( false );
            }
            if ( visibleCount == 0 ){
                _clearStatistics();
            }
            saveState();
        }
    }
    else {
        result = "Could not set image visibility; invalid index: "+ QString::number( dataIndex );
    }
    return result;
}

QString Controller::setLayersSelected( const std::vector<int> indices ){
    QString result;
    bool selectModeAuto = isStackSelectAuto();
    if ( !selectModeAuto ){
        result = _setLayersSelected( indices );
    }
    else {
        result = "Enable manual layer selection mode before setting layers.";
    }
    return result;
}

QString Controller::_setLayersSelected( const std::vector<int> indices ){
    int dataCount = m_datas.size();
    //Note: we could to a careful check of the indices here to make sure
    //the are all nonnegative, and not outside of the required bounds, but
    //that would be slower so we just do a rudimentary check that the size
    //is okay.
    QString result;
    std::vector<int> sortedLayers = indices;
    std::sort( sortedLayers.begin(), sortedLayers.end());
    int selectIndex = 0;
    int indexCount = indices.size();
    int selectedIndex = indexCount;
    if ( indexCount > 0 ){
        selectedIndex = sortedLayers[0];
    }
    std::vector<int> oldSelects;
    bool selectStateChanged = false;
    for ( int i = 0; i < dataCount; i++ ){
        if ( m_datas[i]->_isSelected() ){
            oldSelects.push_back( i );
        }
        if ( i != selectedIndex ){
            bool stateChange = m_datas[i]->_setSelected( false );
            if ( stateChange ){
                selectStateChanged = true;
            }
        }
        else {
            bool stateChange = m_datas[i]->_setSelected( true );
            if ( stateChange ){
                selectStateChanged = true;
            }
            selectIndex++;
            if ( selectIndex < indexCount ){
                selectedIndex = sortedLayers[selectIndex];
            }
        }
    }
    if ( selectIndex != indexCount){
        result = "Invalid selection indices";
    }
    if ( !result.isEmpty( )){
        //Set the selections back the way they were
        setLayersSelected( oldSelects );
    }
    else if ( selectStateChanged ){
        saveState();
        emit colorChanged( this );
    }
    return result;
}

QStringList Controller::setMaskColor( int redAmount, int greenAmount, int blueAmount ){
    QStringList result;
    int dataCount = m_datas.size();
    bool dataSelected = false;
    bool dataChanged = false;
    for ( int i = 0; i < dataCount; i++ ){
        if ( m_datas[i]->_isSelected() ){
            dataSelected = true;
            bool changed = m_datas[i]->_setMaskColor( redAmount,
                            greenAmount, blueAmount, result );
            if ( changed ){
                dataChanged = true;
            }
        }
    }
    if ( !dataSelected ){
        result.append( "Please select one or more data sets for the mask color.");
    }
    if ( dataChanged ){
        saveState();
        _scheduleFrameReload( false );
    }
    return result;
}

QString Controller::setMaskAlpha( int alphaAmount ){
    QString result;
    int dataCount = m_datas.size();
    bool dataSelected = false;
    bool dataChanged = false;
    for ( int i = 0; i < dataCount; i++ ){
        if ( m_datas[i]->_isSelected() ){
            dataSelected = true;
            bool changed = m_datas[i]->_setMaskAlpha( alphaAmount, result );
            if ( changed ){
                dataChanged = true;
            }
        }
    }
    if ( !dataSelected ){
        result = "Please select the data for the mask opacity";
    }
    if ( dataChanged ){
        saveState();
        _scheduleFrameReload( false );
    }
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

QString Controller::setCompositionMode( const QString& compMode ){
    QString result;
    int dataCount = m_datas.size();
    bool dataSelected = false;
    bool dataChanged = false;
    for( int i = 0; i < dataCount; i++ ){
        if ( m_datas[i]->_isSelected() ){
            dataSelected = true;
            bool stateChanged = m_datas[i]->_setCompositionMode( compMode, result );
            if ( stateChanged ){
                dataChanged = true;
            }
        }
    }
    if ( !dataSelected ){
        result = "Data must be selected to apply a layer composition mode.";
    }
    if ( dataChanged ){
        saveState();
        _scheduleFrameReload( false );
    }
    return result;
}

void Controller::setZoomLevel( double zoomFactor ){
    bool zoomPanAll = m_state.getValue<bool>(PAN_ZOOM_ALL);
    if ( zoomPanAll ){
        int dataCount = m_datas.size();
        for ( int i = 0; i < dataCount; i++ ){
            m_datas[i]->_setZoom( zoomFactor );
        }
        _renderAll();
    }
    else {
        int dataIndex = _getIndexCurrent();
        if ( dataIndex >= 0 ){
            m_datas[dataIndex]->_setZoom( zoomFactor );
            _renderSingle( dataIndex );
        }
    }
}


void Controller::_updateCursor( int mouseX, int mouseY ){
    if ( m_datas.size() == 0 ){
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
    int dataIndex = _getIndexCurrent();
    if ( 0 <= dataIndex ){
        int mouseX = m_stateMouse.getValue<int>(ImageView::MOUSE_X );
        int mouseY = m_stateMouse.getValue<int>(ImageView::MOUSE_Y );
        QString cursorText = m_datas[dataIndex]->_getCursorText( mouseX, mouseY, _getFrameIndices());
        if ( cursorText != m_stateMouse.getValue<QString>(CURSOR)){
            m_stateMouse.setValue<QString>( CURSOR, cursorText );
            if ( notifyClients ){
                m_stateMouse.flushState();
            }
        }
    }
}


void Controller::_updateDisplayAxes( int targetIndex ){
    if ( m_gridControls ){
        if ( 0 <= targetIndex && targetIndex < m_datas.size() ){
            std::vector<AxisInfo::KnownType> supportedAxes = m_datas[targetIndex]->_getAxisTypes();
            m_gridControls->_setAxisTypes( supportedAxes );
            AxisInfo::KnownType xType = m_datas[targetIndex]->_getAxisXType();
            AxisInfo::KnownType yType = m_datas[targetIndex]->_getAxisYType();
            const Carta::Lib::KnownSkyCS cs = getCoordinateSystem();
            QString xPurpose = AxisMapper::getPurpose( xType, cs );
            QString yPurpose = AxisMapper::getPurpose( yType, cs );
            m_gridControls->setAxis( AxisMapper::AXIS_X, xPurpose );
            m_gridControls->setAxis( AxisMapper::AXIS_Y, yPurpose );
        }
    }
}


void Controller::updateZoom( double centerX, double centerY, double zoomFactor ){
    bool zoomPanAll = m_state.getValue<bool>(PAN_ZOOM_ALL);
    if ( zoomPanAll ){
        for (std::shared_ptr<ControllerData> data : m_datas ){
            _updateZoom( centerX, centerY, zoomFactor, data );
        }
        _renderAll();
    }
    else {
        int dataIndex = _getIndexCurrent();
        if ( dataIndex >= 0 ){
            _updateZoom( centerX, centerY, zoomFactor, m_datas[dataIndex] );
            _renderSingle( dataIndex );
        }
    }
}

void Controller::_updateZoom( double centerX, double centerY, double zoomFactor,
         std::shared_ptr<ControllerData> data ){
    //Remember where the user clicked
    QPointF clickPtScreen( centerX, centerY);
    bool validImage = false;
    QPointF clickPtImageOld = data->_getImagePt( clickPtScreen, &validImage );
    if ( validImage ){
        //Set the zoom
        double newZoom = 1;
        double oldZoom = data->_getZoom();
        if ( zoomFactor < 0 ) {
            newZoom = oldZoom / 0.9;
        }
        else {
            newZoom = oldZoom * 0.9;
        }
        data->_setZoom( newZoom );

        // what is the new image pixel under the mouse cursor?
        QPointF clickPtImageNew = data ->_getImagePt( clickPtScreen, &validImage );

        // calculate the difference
        QPointF delta = clickPtImageOld - clickPtImageNew;

        // add the delta to the current center
        QPointF currCenter = data ->_getCenter();
        QPointF newCenter = currCenter + delta;
        data->_setPan( newCenter.x(), newCenter.y() );
    }
}

void Controller::updatePan( double centerX , double centerY){
    bool zoomPanAll = m_state.getValue<bool>(PAN_ZOOM_ALL);
    if ( zoomPanAll ){
        for ( std::shared_ptr<ControllerData> data : m_datas ){
            _updatePan( centerX, centerY, data );
        }
        _renderAll();
    }
    else {
        int dataIndex = _getIndexCurrent();
        if ( dataIndex >= 0 ){
            _updatePan( centerX, centerY, m_datas[dataIndex] );
            _renderSingle( dataIndex );
        }
    }
    _updateCursorText( true );
}

void Controller::_updatePan( double centerX , double centerY,
        std::shared_ptr<ControllerData> data){
    bool validImage = false;
    QPointF imagePt = data -> _getImagePt( { centerX, centerY }, &validImage );
    if ( validImage ){
        double imageX = imagePt.x();
        double imageY = imagePt.y();
        data->_setPan( imageX, imageY );
    }
}

void Controller::_viewResize( ){
    QSize clientSize = m_stackDraw->getClientSize();
    for ( int i = 0; i < m_datas.size(); i++ ){
        m_datas[i]->_viewResize( clientSize );
    }
    _renderAll();
}

Controller::~Controller(){
    //unregisterView();
    clear();
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    int selectCount = m_selects.size();
    for ( int i = 0; i < selectCount; i++ ){
        if ( m_selects[i] != nullptr){
            objMan->destroyObject(m_selects[i]->getId());
            m_selects[i] = nullptr;
        }
    }
    m_selects.clear();
    if ( m_selectImage != nullptr ){
        objMan->destroyObject( m_selectImage->getId());
        m_selectImage = nullptr;
    }

    if ( m_gridControls != nullptr ){
        objMan->removeObject( m_gridControls->getId());
    }

    if ( m_contourControls != nullptr ){
        objMan->removeObject( m_contourControls->getId());
    }

    if ( m_settings != nullptr ){
        objMan->removeObject( m_settings->getId());
    }

    _clearData();

    for ( Region* region : m_regions ){
        objMan->destroyObject( region->getId());
    }
    m_regions.clear();
}

}
}
