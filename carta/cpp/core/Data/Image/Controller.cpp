#include "State/ObjectManager.h"
#include "State/UtilState.h"
#include "Controller.h"
#include "Data/Image/Grid/AxisMapper.h"
#include "Data/Image/Grid/DataGrid.h"
#include "Data/Image/Grid/GridControls.h"
#include "Data/Image/Contour/ContourControls.h"
#include "Data/Image/Contour/DataContours.h"
#include "Data/Settings.h"
#include "Data/DataLoader.h"
#include "ControllerData.h"
#include "DataSource.h"

#include "Data/Error/ErrorManager.h"
#include "Data/Selection.h"
#include "Data/Region.h"
#include "Data/RegionRectangle.h"
#include "Data/Util.h"
#include "ImageView.h"
#include "CartaLib/IImage.h"
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
const QString Controller::POINTER_MOVE = "pointer-move";
const QString Controller::ZOOM = "zoom";
const QString Controller::REGIONS = "regions";
const QString Controller::PLUGIN_NAME = "CasaImageLoader";

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
        m_view(nullptr),
        m_stateData( UtilState::getLookup(path, StateInterface::STATE_DATA )),
        m_stateMouse(UtilState::getLookup(path, ImageView::VIEW)),
        m_viewSize( 400, 400){
    m_view.reset( new ImageView( path, QColor("pink"), QImage(), &m_stateMouse));
    
    m_reloadFrameQueued = false;
    m_repaintFrameQueued = false;

    _initializeSelections();

     _initializeState();
     registerView(m_view.get());

     connect( m_view.get(), SIGNAL(resize(const QSize&)), this, SLOT(_viewResize(const QSize&)));

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
     connect( m_contourControls.get(), SIGNAL(drawContoursChanged()), this, SLOT( _contoursChanged()));

     Settings* settingsObj = objMan->createObject<Settings>();
     m_settings.reset( settingsObj );

     //Load the view.
     _scheduleFrameReload();

     _initializeCallbacks();
}



bool Controller::addData(const QString& fileName) {
    //Find the location of the data, if it already exists.
    int targetIndex = -1;
    for (int i = 0; i < m_datas.size(); i++) {
        if (m_datas[i]->_contains(fileName)) {
            targetIndex = i;
            break;
        }
    }

    //Add the data if it is not already there.
    if (targetIndex == -1) {

        Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
        ControllerData* targetSource = objMan->createObject<ControllerData>();
        DataContours* contourObj = objMan->createObject<DataContours>();
        std::shared_ptr<DataContours> contourPtr( contourObj );
        //Controller Data is in charge of drawing the contours.
        targetSource->_setContours( contourPtr );
        //Contour controls is in charge of setting the UI for the contours.
        m_contourControls->_setDrawContours( contourPtr );
        targetIndex = m_datas.size();
        connect( targetSource, SIGNAL(renderingDone(QImage)), this, SLOT(_renderingDone(QImage)));
        connect( targetSource, & ControllerData::saveImageResult, this, & Controller::saveImageResultCB );
        m_datas.append(std::shared_ptr<ControllerData>(targetSource));
        targetSource->_viewResize( m_viewSize );

        //Update the data selectors upper bound based on the data.
        m_selectImage->setUpperBound(m_datas.size());
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
            if ( m_view ){
                _scheduleFrameReload( true );
                double minPercent = m_state.getValue<double>(CLIP_VALUE_MIN);
                double maxPercent = m_state.getValue<double>(CLIP_VALUE_MAX);
                emit clipsChanged( minPercent, maxPercent );
            }
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

QString Controller::closeImage( const QString& name ){
    int targetIndex = -1;
    QString result;
    int dataCount = m_datas.size();
    for ( int i = 0; i < dataCount; i++ ){
        if ( m_datas[i]->_contains( name )){
            targetIndex = i;
            break;
        }
    }

    if ( targetIndex >= 0 ){
        _removeData( targetIndex );
        emit dataChanged( this );
    }
    else {
        result = "Could not find data to remove for name="+name;
    }
    return result;
}

void Controller::centerOnPixel( double centerX, double centerY ){
    int imageIndex = m_selectImage->getIndex();
    if ( imageIndex >= 0 && imageIndex < m_datas.size()){
        m_datas[imageIndex]->_setPan( centerX, centerY );
        _render();
    }
}


void Controller::_contoursChanged(){
    int imageIndex = m_selectImage->getIndex();
    if ( imageIndex >= 0 && imageIndex < m_datas.size() ){
        std::vector<int> frames = _getFrameIndices( imageIndex );
        const Carta::Lib::KnownSkyCS& cs = getCoordinateSystem();
        m_datas[imageIndex]->_render( frames, cs );
    }
}

void Controller::_displayAxesChanged(std::vector<AxisInfo::KnownType> displayAxisTypes,
        bool applyAll ){
    int imageIndex = 0;
    if (m_selectImage != nullptr) {
        imageIndex = m_selectImage->getIndex();
    }
    if ( !applyAll ){
        if (imageIndex >= 0 && imageIndex < m_datas.size()) {
            if (m_datas[imageIndex] != nullptr) {
                std::vector<int> frames = _getFrameIndices( imageIndex );
                m_datas[imageIndex]->_displayAxesChanged( displayAxisTypes, frames );
            }
        }
    }
    else {
        int dataCount = m_datas.size();
        for ( int i = 0; i < dataCount; i++ ){
            if ( m_datas[i] != nullptr ){
                std::vector<int> frames = _getFrameIndices( i );
                m_datas[i]->_displayAxesChanged( displayAxisTypes, frames );
            }
        }
    }
    emit axesChanged();
    this->_loadView();
    _updateCursorText( true );
}

std::vector<Carta::Lib::AxisInfo::KnownType> Controller::_getAxisZTypes() const {
    int imageIndex = m_selectImage->getIndex();
    std::vector<Carta::Lib::AxisInfo::KnownType> zTypes;
    if ( 0 <= imageIndex && imageIndex < m_datas.size() ){
        zTypes = m_datas[imageIndex]->_getAxisZTypes();
    }
    return zTypes;
}

std::set<AxisInfo::KnownType> Controller::_getAxesHidden() const {
    int dataCount = m_datas.size();
    std::set<AxisInfo::KnownType> axes;
    for ( int i = 0; i < dataCount; i++ ){
        std::vector<AxisInfo::KnownType> zAxes = m_datas[i]->_getAxisZTypes();
        int zAxesCount = zAxes.size();
        for ( int j = 0; j < zAxesCount; j++ ){
            axes.insert( zAxes[j] );
        }

    }
    return axes;
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
    int imageIndex = m_selectImage->getIndex();
    Carta::Lib::KnownSkyCS cs = Carta::Lib::KnownSkyCS::Unknown;
    if ( 0 <= imageIndex && imageIndex < m_datas.size() ){
        cs = m_datas[imageIndex]->_getCoordinateSystem();
    }
    return cs;
}

std::shared_ptr<GridControls> Controller::getGridControls() {
    return m_gridControls;
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
        int imageIndex = m_selectImage->getIndex();
        if ( 0 <= imageIndex && imageIndex < m_datas.size()){
            std::vector<AxisInfo::KnownType> supportedAxes = m_datas[imageIndex]->_getAxisTypes();
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

bool Controller::getIntensity( double percentile, double* intensity ) const{
    int currentFrame = getFrame( AxisInfo::KnownType::SPECTRAL);
    bool validIntensity = getIntensity( currentFrame, currentFrame, percentile, intensity );
    return validIntensity;
}

bool Controller::getIntensity( int frameLow, int frameHigh, double percentile, double* intensity ) const{
    bool validIntensity = false;
    int imageIndex = m_selectImage->getIndex();
    if ( 0 <= imageIndex && imageIndex < m_datas.size() && percentile >= 0.0 && percentile <= 1.0 ){
        validIntensity = m_datas[imageIndex]->_getIntensity( frameLow, frameHigh, percentile, intensity );
    }
    return validIntensity;
}

double Controller::getPercentile( double intensity ) const {
    int currentFrame = getFrame( AxisInfo::KnownType::SPECTRAL );
    return getPercentile( currentFrame, currentFrame, intensity );
}

double Controller::getPercentile( int frameLow, int frameHigh, double intensity ) const {
    double percentile = -1;
    int imageIndex = m_selectImage->getIndex();
    if ( 0 <= imageIndex && imageIndex < m_datas.size()){
        percentile = m_datas[imageIndex]->_getPercentile( frameLow, frameHigh, intensity );
    }
    return percentile;
}


std::vector<std::shared_ptr<Image::ImageInterface>> Controller::getDataSources(){
    //For right now, we are only going to do a histogram of a single image.
    std::vector<std::shared_ptr<Image::ImageInterface>> images;
    int dataCount = m_datas.size();
    if ( dataCount > 0 ){
        int imageIndex = m_selectImage->getIndex();
        if ( 0 <= imageIndex && imageIndex < dataCount ){
            images.push_back( m_datas[imageIndex]->_getImage());
        }
    }
    return images;
}

int Controller::getSelectImageIndex() const {
    int selectImageIndex = -1;
    if ( m_datas.size() >= 1 ){
        selectImageIndex = m_selectImage->getIndex();
    }
    return selectImageIndex;
}

QString Controller::getImageName(int index) const{
    QString name;
    if ( 0 <= index && index < m_datas.size()){
        name = m_datas[index]->_getFileName();
    }
    return name;
}

std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> Controller::getPipeline() const {
    std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> pipeline(nullptr);
    int selectImageIndex = m_selectImage->getIndex();
    if ( 0 <= selectImageIndex && selectImageIndex < m_datas.size() ){
        pipeline = m_datas[selectImageIndex]->_getPipeline();
    }
    return pipeline;
}

QStringList Controller::getPixelCoordinates( double ra, double dec ) const {
    QStringList result("");
    int imageIndex = m_selectImage->getIndex();
    if ( imageIndex >= 0 && imageIndex < m_datas.size()){
        result = m_datas[imageIndex]->_getPixelCoordinates( ra, dec );
    }
    return result;
}

QString Controller::getPixelValue( double x, double y ) const {
    QString result("");
    int imageIndex = m_selectImage->getIndex();
    if ( imageIndex >= 0 && imageIndex < m_datas.size()){
        std::vector<int> frames = _getFrameIndices( imageIndex );
        result = m_datas[imageIndex]->_getPixelValue( x, y, frames );
    }
    return result;
}

QString Controller::getPixelUnits() const {
    QString result("");
    int imageIndex = m_selectImage->getIndex();
    if ( imageIndex >= 0 && imageIndex < m_datas.size()){
        result = m_datas[imageIndex]->_getPixelUnits();
    }
    return result;
}

QStringList Controller::getCoordinates( double x, double y, Carta::Lib::KnownSkyCS system) const {
    QStringList result;
    int imageIndex = m_selectImage->getIndex();
    if ( imageIndex >= 0 && imageIndex < m_datas.size()){
        QStringList coordList = m_datas[imageIndex]->_getCoordinates( x, y, system, _getFrameIndices(imageIndex) );
        for ( int i = 0; i <= 1; i++ ){
            result.append( coordList[i] );
        }
    }
    return result;
}

std::vector<int> Controller::_getFrameIndices( int imageIndex ) const {
    int selectCount = m_selects.size();
    std::vector<int> frames( selectCount );
    if ( imageIndex >= 0 && imageIndex < m_datas.size() ){
        if ( m_datas[imageIndex] != nullptr ){
            //Determine the index of the frame to load.
            for ( int i = 0; i < selectCount; i++ ){
                frames[i] = m_selects[i]->getIndex();
            }
        }
    }
    return frames;
}

QString Controller::getPreferencesId() const {
    QString id;
    if ( m_settings.get() != nullptr ){
        id = m_settings->getPath();
    }
    return id;
}

int Controller::getStackedImageCount() const {
    int count = m_datas.size();
    return count;
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
            dataState.setObject( lookup, m_datas[i]->_getStateString());
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
        dataState.insertValue<QString>( ContourControls::CLASS_NAME, m_contourControls->getStateString( sessionId, type));
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
    int imageIndex = m_selectImage->getIndex();
    if ( imageIndex >= 0 && imageIndex < m_datas.size() ){
        zoom = m_datas[imageIndex]->_getZoom( );
        _render();
    }
    return zoom;
}

void Controller::_gridChanged( const StateInterface& state, bool applyAll ){
    int imageIndex = 0;
    if (m_selectImage != nullptr) {
        imageIndex = m_selectImage->getIndex();
    }
    if ( imageIndex >= 0 && imageIndex < m_datas.size() ){
        std::vector<int> frames = _getFrameIndices( imageIndex );
        if ( !applyAll ){
            m_datas[imageIndex]->_gridChanged( state, true, frames );
        }
        else {
            int dataCount = m_datas.size();
            for ( int i = 0; i < dataCount; i++ ){
                bool renderedImage = false;
                if ( i == imageIndex ){
                    renderedImage = true;
                }
                if ( m_datas[i] != nullptr ){
                    m_datas[i]->_gridChanged( state, renderedImage, frames );
                }
            }
        }
        _updateCursorText( true );
    }
}

void Controller::_initializeCallbacks(){
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

    QString pointerPath= UtilState::getLookup( getPath(), UtilState::getLookup( ImageView::VIEW, POINTER_MOVE));
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
        QString result = closeImage( imageName );
                return result;
            });

    addCommandCallback( CENTER, [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) ->QString {
            auto vals = Util::string2VectorDouble( params );
            int count = vals.size();
            if ( count > 1 ) {
                updatePan( vals[0], vals[1]);
            }
            return "";
        });

    addCommandCallback( ZOOM, [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) ->QString {
        auto vals = Util::string2VectorDouble( params );
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
                        QString result = getPreferencesId();
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

    addCommandCallback( "saveImage", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = {DATA_PATH};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString result = saveImage( dataValues[DATA_PATH]);
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

    //Set whether or not to auto clip
    //First the preference state.
    m_state.insertValue<bool>( AUTO_CLIP, true );
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



void Controller::_loadView(  bool newClips ) {
    m_reloadFrameQueued = false;
    //Determine the index of the data to load.
    int imageIndex = 0;
    if (m_selectImage != nullptr) {
        imageIndex = m_selectImage->getIndex();
    }

    if (imageIndex >= 0 && imageIndex < m_datas.size()) {
        if (m_datas[imageIndex] != nullptr) {

            //Determine the indices of the frames to load.
            vector<int> frames = _getFrameIndices( imageIndex );

            //Load the image.
            bool autoClip = m_state.getValue<bool>(AUTO_CLIP);
            if ( newClips ){
                autoClip = true;
            }
            double clipValueMin = m_state.getValue<double>(CLIP_VALUE_MIN);
            double clipValueMax = m_state.getValue<double>(CLIP_VALUE_MAX);
            const Carta::Lib::KnownSkyCS& cs = getCoordinateSystem();
            m_datas[imageIndex]->_load(frames, autoClip, clipValueMin, clipValueMax, cs);
        }
        else {
            qDebug() << "Uninitialized image: "<<imageIndex;
        }
    }
    else {
        qDebug() << "Image index=" << imageIndex << " is out of range";
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

void Controller::_removeData( int index ){
    disconnect( m_datas[index].get());
    int selectedImage = m_selectImage->getIndex();
    QString id = m_datas[index]->getId();
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    objMan->removeObject( id );
    m_datas.removeAt( index );
    m_selectImage->setUpperBound( m_datas.size());
    if ( selectedImage == index ){
       m_selectImage->setIndex( 0 );
    }
    else if ( index < selectedImage ){
       int imageCountDecreased = selectedImage - 1;
       m_selectImage->setIndex( imageCountDecreased );
    }
    //Update the channel upper bound and index if necessary
    int targetImage = m_selectImage->getIndex();
    int selectCount = m_selects.size();
    for ( int i = 0; i < selectCount; i++ ){
        int frameCount = 0;
        AxisInfo::KnownType axisType= static_cast<AxisInfo::KnownType>( i );
        if ( targetImage >= 0 && targetImage < m_datas.size() ){
            frameCount = m_datas[targetImage]->_getFrameCount( axisType );
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
    this->_loadView();

    //Clear the statistics window if there are no images.
    if ( m_datas.size() == 0 ){
        m_stateMouse.setValue<QString>( CURSOR, "" );
        m_stateMouse.flushState();
    }

    saveState();
}

void Controller::_render(){
    int imageIndex = m_selectImage->getIndex();
    if ( imageIndex >= 0 && imageIndex < m_datas.size()){
        std::vector<int> frames = _getFrameIndices( imageIndex );
        const Carta::Lib::KnownSkyCS& cs = getCoordinateSystem();
        m_datas[imageIndex]->_render( frames, cs );
    }
}

void Controller::_renderingDone( QImage img){
    _scheduleFrameRepaint( img );
}

void Controller::_repaintFrameNow(){
    m_view->scheduleRedraw();
    m_repaintFrameQueued = false;
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
    //First we reset the data this controller is displaying
    _clearData();
    m_datas.clear();
    Carta::State::StateInterface dataState( "");
    dataState.setState( state );



    //Reset the image select state.
    QString dataStateStr = dataState.getValue<QString>( Selection::IMAGE );
    m_selectImage ->resetState( dataStateStr );

    int dataCount = dataState.getArraySize(DATA);
    for ( int i = 0; i < dataCount; i++ ){
        QString dataLookup = Carta::State::UtilState::getLookup( DATA, i );
        QString fileLookup = Carta::State::UtilState::getLookup( dataLookup, DataSource::DATA_PATH);
        QString fileName = dataState.getValue<QString>( fileLookup );
        addData( fileName );
        QString gridLookup = Carta::State::UtilState::getLookup( dataLookup, DataGrid::GRID);
        QString gridStr = dataState.toString( gridLookup );
        StateInterface gridState( "" );
        gridState.setState( gridStr );
        std::vector<int> frames = _getFrameIndices( i );
        m_datas[i]->_gridChanged( gridState, false, frames );
    }

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
    int imageIndex = m_selectImage->getIndex();
    if ( 0 <= imageIndex && imageIndex < m_datas.size()){
        StateInterface controlState = m_datas[imageIndex]->_getGridState();
        this->m_gridControls->_resetState( controlState );
    }

    //Restore contours
    QString contourData = dataState.getValue<QString>( ContourControls::CLASS_NAME);
    m_contourControls->resetStateData( contourData );
}

void Controller::resetPan(){
    int dataCount = m_datas.size();
    if ( dataCount > 0 ){
        for ( int i = 0; i < dataCount; i++ ){
            m_datas[i]->_resetPan();
        }
        _render();
    }
}

void Controller::resetZoom(){
    int dataCount = m_datas.size();
    if ( dataCount > 0 ){
        for ( int i = 0; i < dataCount; i++ ){
            m_datas[i]->_resetZoom();
        }
        _render();
    }
}

void Controller::saveState() {
    bool stateChanged = false;
    int dataCount = m_datas.size();
    int oldDataCount = m_stateData.getArraySize(DATA );
    if ( oldDataCount != dataCount ){
        stateChanged = true;
        //Insert the names of the data items for display purposes.
        m_stateData.resizeArray(DATA, dataCount, StateInterface::PreserveAll );
        QStringList longNames;
        for (int i = 0; i < dataCount; i++) {
            longNames.append( m_datas[i]->_getFileName() );
        }

        DataLoader* dataLoader = Util::findSingletonObject<DataLoader>();
        QStringList shortNames = dataLoader->getShortNames( longNames );

        int shortNameCount = shortNames.size();
        for ( int i = 0; i < shortNameCount; i++ ){
            QString dataKey = UtilState::getLookup( DATA, i);
            QString oldViewName;
            if ( i < oldDataCount ){
                oldViewName = m_stateData.getValue<QString>(dataKey);
            }
            if ( shortNames[i] != oldViewName ){
                m_stateData.setValue<QString>( dataKey, shortNames[i] );
            }
        }
    }
    /*int regionCount = m_regions.size();
    m_state.resizeArray( REGIONS, regionCount );
    _saveRegions();*/
    if ( stateChanged ){
        m_stateData.flushState();
    }
}


QString Controller::saveImage( const QString& fileName ){
    int zoomLevel = getZoomLevel();
    return saveImage( fileName, zoomLevel );
}

QString Controller::saveImage( const QString& fileName, double scale ){
    QString result;
    DataLoader* dLoader = Util::findSingletonObject<DataLoader>();
    bool securityRestricted = dLoader->isSecurityRestricted();
    if ( !securityRestricted ){
        int imageIndex = m_selectImage->getIndex();
        if ( 0<= imageIndex && imageIndex < m_datas.size()){
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
                std::vector<int> frames = _getFrameIndices( imageIndex );
                result = m_datas[imageIndex]->_saveImage( fileName, scale, frames );
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

void Controller::_scheduleFrameRepaint( const QImage& img ){
    if ( m_datas.size() > 0 ){
        // if reload is already pending, do nothing
        if ( m_repaintFrameQueued ) {
            return;
        }
        m_view->resetImage( img);
        m_repaintFrameQueued = true;
        QMetaObject::invokeMethod( this, "_repaintFrameNow", Qt::QueuedConnection );
    }
}


void Controller::_scheduleFrameReload( bool newClips ){
    if ( m_datas.size() > 0  ){
        // if reload is already pending, do nothing
        if ( m_reloadFrameQueued ) {
            return;
        }
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


void Controller::setColorInverted( bool inverted ){
    for ( std::shared_ptr<ControllerData> data : m_datas ){
        data->setColorInverted( inverted );
    }
    _render();
}

void Controller::setColorMap( const QString& name ){
    for ( std::shared_ptr<ControllerData> data : m_datas ){
        data->setColorMap( name );
    }
    _render();
}

void Controller::setColorReversed( bool reversed ){
    for ( std::shared_ptr<ControllerData> data : m_datas ){
        data->setColorReversed( reversed );
    }
    _render();
}

void Controller::setColorAmounts( double newRed, double newGreen, double newBlue ){
    for ( std::shared_ptr<ControllerData> data : m_datas ){
        data->setColorAmounts( newRed, newGreen, newBlue );
    }
    _render();
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
            int imageIndex = m_selectImage->getIndex();
            int dataCount = m_datas.size();
            if ( 0 <= imageIndex && imageIndex < dataCount ){
                _updateCursorText( true );
                emit channelChanged( this );
            }
        }
    }
}

void Controller::setFrameImage( int val) {
    if (m_selectImage != nullptr) {
        int oldIndex = m_selectImage->getIndex();
        if ( oldIndex != val ){
            m_selectImage->setIndex(val);
            int imageIndex = m_selectImage->getIndex();
            if ( 0 <= imageIndex && imageIndex < m_datas.size() ){
                int selectCount = m_selects.size();
                for ( int i = 0; i < selectCount; i++ ){
                    AxisInfo::KnownType type = static_cast<AxisInfo::KnownType>(i);
                    int upperBound = m_datas[imageIndex]->_getFrameCount( type );
                    m_selects[i]->setUpperBound( upperBound );
                    if ( m_selects[i]->getIndex() > m_selects[i]->getUpperBound()){
                        m_selects[i]->setIndex( 0 );
                        if ( type == AxisInfo::KnownType::SPECTRAL ){
                            emit channelChanged( this );
                        }
                    }
                }

                Carta::State::StateInterface gridState = m_datas[imageIndex]->_getGridState();
                m_gridControls->_resetState( gridState );
            }
            _updateCursorText( true );
            emit dataChanged( this );
        }
    }
}

void Controller::setGamma( double gamma ){
    for ( std::shared_ptr<ControllerData> data : m_datas ){
        data->setGamma( gamma );
    }
    _render();
}



void Controller::setTransformData( const QString& name ){
    for ( std::shared_ptr<ControllerData> data : m_datas ){
        data->_setTransformData( name );
    }
    _render();
}

void Controller::setZoomLevel( double zoomFactor ){
    int imageIndex = m_selectImage->getIndex();
    if ( imageIndex >= 0 && m_datas.size() > 0 ){
        //Set the zoom
        m_datas[imageIndex]->_setZoom( zoomFactor );
        _render();
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
    int imageIndex = m_selectImage->getIndex();
    if ( 0 <= imageIndex && imageIndex < m_datas.size() ){
        int mouseX = m_stateMouse.getValue<int>(ImageView::MOUSE_X );
        int mouseY = m_stateMouse.getValue<int>(ImageView::MOUSE_Y );
        QString cursorText = m_datas[imageIndex]->_getCursorText( mouseX, mouseY, _getFrameIndices( imageIndex));
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

    int imageIndex = m_selectImage->getIndex();
    if ( imageIndex >= 0 && imageIndex < m_datas.size()){
        //Remember where the user clicked
        QPointF clickPtScreen( centerX, centerY);
        bool validImage = false;
        QPointF clickPtImageOld = m_datas[imageIndex]->_getImagePt( clickPtScreen, &validImage );
        if ( validImage ){
            //Set the zoom
            double newZoom = 1;
            double oldZoom = m_datas[imageIndex]->_getZoom();
            if ( zoomFactor < 0 ) {
                newZoom = oldZoom / 0.9;
            }
            else {
                newZoom = oldZoom * 0.9;
            }
            for (std::shared_ptr<ControllerData> data : m_datas ){
                data->_setZoom( newZoom );
            }

            // what is the new image pixel under the mouse cursor?
            QPointF clickPtImageNew = m_datas[imageIndex]->_getImagePt( clickPtScreen, &validImage );

            // calculate the difference
            QPointF delta = clickPtImageOld - clickPtImageNew;

            // add the delta to the current center
            QPointF currCenter = m_datas[imageIndex]->_getCenter();
            QPointF newCenter = currCenter + delta;
            for ( std::shared_ptr<ControllerData> data : m_datas ){
                data->_setPan( newCenter.x(), newCenter.y() );
            }
            _render();
        }
    }
}

void Controller::updatePan( double centerX , double centerY){
    int imageIndex = m_selectImage->getIndex();
    if ( imageIndex >= 0 && imageIndex < m_datas.size()){
        bool validImage = false;
        QPointF oldImageCenter = m_datas[imageIndex]-> _getImagePt( { centerX, centerY }, &validImage );
        if ( validImage ){

            double imageX = oldImageCenter.x();
            double imageY = oldImageCenter.y();
            for ( std::shared_ptr<ControllerData> data : m_datas ){
                data->_setPan( imageX, imageY );
            }
            _render();
            _updateCursorText( true );
        }
    }
}

QStringList Controller::getCenterPixel() {
    int imageIndex = m_selectImage->getIndex();
    QStringList returnValue = QStringList( "null" );
    if ( imageIndex >= 0 && imageIndex < m_datas.size() ) {
        QPointF center = m_datas[imageIndex]->_getCenter();
        returnValue = QStringList( QString::number( center.x() ) );
        returnValue.append( QString::number( center.y() ) );
    }
    return returnValue;
}

QStringList Controller::getImageDimensions( ){
    QStringList result;
    int imageIndex = m_selectImage->getIndex();
    if ( imageIndex >= 0 && imageIndex < m_datas.size() ){
        int dimensions = m_datas[imageIndex]->_getDimensions();
        for ( int i = 0; i < dimensions; i++ ) {
            int d = m_datas[imageIndex]->_getDimension( i );
            result.append( QString::number( d ) );
        }
    }
    else {
        result = QStringList("");
    }
    return result;
}

QStringList Controller::getOutputSize( ){
    QStringList result;
    int imageIndex = m_selectImage->getIndex();
    if ( imageIndex >= 0 && imageIndex < m_datas.size() ){
        QSize outputSize = m_datas[imageIndex]->_getOutputSize();
        result.append( QString::number( outputSize.width() ) );
        result.append( QString::number( outputSize.height() ) );
    }
    else {
        result = QStringList("");
    }
    return result;
}

void Controller::_viewResize( const QSize& newSize ){
    for ( int i = 0; i < m_datas.size(); i++ ){
        m_datas[i]->_viewResize( newSize );
    }
    m_viewSize = newSize;
    _render();
}

Controller::~Controller(){
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
