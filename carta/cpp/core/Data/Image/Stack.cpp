#include "LayerGroup.h"

#include "Data/Image/LayerCompositionModes.h"
#include "Data/Image/Draw/DrawStackSynchronizer.h"
#include "Data/Image/Grid/AxisMapper.h"
#include "Data/Image/Save/SaveService.h"
#include "Data/Preferences/PreferencesSave.h"
#include "Data/Region/Region.h"
#include "Data/Region/RegionFactory.h"
#include "Data/Selection.h"
#include "Data/Util.h"
#include "State/UtilState.h"
#include "State/StateInterface.h"
#include "CartaLib/Hooks/LoadRegion.h"
#include "Globals.h"

#include <QDebug>
#include <QDir>
#include "Stack.h"



using Carta::Lib::AxisInfo;


namespace Carta {

namespace Data {

const QString Stack::CLASS_NAME = "Stack";
const QString Stack::REGIONS = "regions";
const QString Stack::VIEW = "view";


class Stack::Factory : public Carta::State::CartaObjectFactory {

public:

    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new Stack(path, id);
    }
};
bool Stack::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass (CLASS_NAME,
                                                   new Stack::Factory());


Stack::Stack(const QString& path, const QString& id) :
    LayerGroup( CLASS_NAME, path, id),
    m_stackDraw(nullptr),
    m_selectImage(nullptr){
    m_reloadFrameQueued = false;
    _initializeState();
    _initializeSelections();
}

bool Stack::_addData(const QString& fileName, std::shared_ptr<ColorState> colorState ) {
    bool successfulLoad = LayerGroup::_addData( fileName );
    if ( successfulLoad ){
        int lastIndex = m_children.size() - 1;
        m_children[lastIndex]->_setColorMapGlobal( colorState );
        m_children[lastIndex]->_viewResize( m_stackDraw->getClientSize() );
        m_stackDraw->setLayers( m_children );
        _resetFrames( lastIndex );
        _saveState();
    }
    return successfulLoad;
}

QString Stack::_addDataRegion(const QString& fileName ) {
    QString msg;
    int selectIndex = _getSelectImageIndex();
    if ( selectIndex >= 0 ){

        std::shared_ptr<Carta::Lib::Image::ImageInterface> image = m_children[selectIndex]->_getImage();
        auto result = Globals::instance()-> pluginManager()
                                -> prepare <Carta::Lib::Hooks::LoadRegion>(fileName, image );
        auto lam = [=] ( const Carta::Lib::Hooks::LoadRegion::ResultType &data ) {
            int regionCount = data.size();
            for ( int i = 0; i < regionCount; i++ ){
               if ( data[i] ){
                   std::shared_ptr<Region> regionPtr = RegionFactory::makeRegion( data[i] );
                   regionPtr -> _setUserId( fileName, i );
                   m_regions.push_back( regionPtr );
               }
            }
        };
        try {
            result.forEach( lam );
            _saveStateRegions();
        }
        catch( char*& error ){
            msg = QString( error );
        }
    }
    return msg;
}


bool Stack::_addGroup( /*const QString& state*/ ){
    bool groupAdded = LayerGroup::_addGroup();
    if ( groupAdded ){
        m_stackDraw->setLayers( m_children );
    }
    return groupAdded;
}

bool Stack::_closeData( const QString& id ){
    bool dataClosed = LayerGroup::_closeData( id );
    if ( dataClosed ){
        int selectedImage = m_selectImage->getIndex();

        int visibleImageCount = _getStackSizeVisible();
        m_selectImage->setUpperBound( visibleImageCount );

        if ( selectedImage >= visibleImageCount ){
            m_selectImage->setIndex(0);
        }
        //Update the channel upper bound and index if necessary
        int targetData = _getIndexCurrent();
        int selectCount = m_selects.size();
        for ( int i = 0; i < selectCount; i++ ){
            int frameCount = 0;
            AxisInfo::KnownType axisType= static_cast<AxisInfo::KnownType>( i );
            if ( targetData >= 0 ){
                frameCount = m_children[targetData]->_getFrameCount( axisType );
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
        m_stackDraw->setLayers( m_children );
        _scheduleFrameReload( false );
        _saveState();
    }
    return dataClosed;
}

QString Stack::_closeRegion( const QString& regionId ){
    bool regionRemoved = false;
    QString result;
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    //Note that more than one region could be removed, if there are
    //serveral regions that start with the passed in id.
    int regionCount = m_regions.size();
    for ( int i = regionCount - 1; i >= 0; i-- ){
        bool match = m_regions[i]->_isMatch( regionId );
        if ( match ){
            QString id = m_regions[i]->getId();
            objMan->removeObject( id );
            m_regions.removeAt( i );
            regionRemoved = true;
        }
    }
    if ( regionRemoved ){
        _saveStateRegions();
    }
    else {
        result = "Could not find region to remove for id="+regionId;
    }
    return result;
}


void Stack::_displayAxesChanged(std::vector<AxisInfo::KnownType> displayAxisTypes, bool applyAll ){

    std::vector<int> frames = _getFrameIndices();
    if ( !applyAll ){
        int dataIndex = _getIndexCurrent();
        if (dataIndex >= 0 ) {
            if (m_children[dataIndex] != nullptr) {
                std::vector<int> frames = _getFrameIndices();
                m_children[dataIndex]->_displayAxesChanged( displayAxisTypes, frames );
                _scheduleFrameReload( false );
            }
        }
    }
    else {
        int dataCount = m_children.size();
        std::vector<int> frames = _getFrameIndices();
        for ( int i = 0; i < dataCount; i++ ){
            if ( m_children[i] != nullptr ){
                m_children[i]->_displayAxesChanged( displayAxisTypes, frames );
            }
        }
        _scheduleFrameReload( true );
    }

}

std::set<AxisInfo::KnownType> Stack::_getAxesHidden() const {
    int dataCount = m_children.size();
    std::set<AxisInfo::KnownType> axes;
    for ( int i = 0; i < dataCount; i++ ){
        if ( m_children[i]->_isVisible() ){
            std::vector<AxisInfo::KnownType> zAxes = m_children[i]->_getAxisZTypes();
            int zAxesCount = zAxes.size();
            for ( int j = 0; j < zAxesCount; j++ ){
                axes.insert( zAxes[j] );
            }
        }
    }
    return axes;
}


QStringList Stack::getCoordinates( double x, double y,
        Carta::Lib::KnownSkyCS system ) const{
    std::vector<int> indices = _getFrameIndices();
    return _getCoordinates( x, y, system, indices );
}

QString Stack::_getCursorText( int mouseX, int mouseY ){
    int dataIndex = _getIndexCurrent();
    QString cursorText;
    if ( dataIndex >= 0 ){
        std::vector<int> frameIndices = _getFrameIndices();
        cursorText = m_children[dataIndex]->_getCursorText( mouseX, mouseY, frameIndices );
    }
    return cursorText;
}

int Stack::_getFrame( AxisInfo::KnownType axisType ) const {
    int frame = -1;
    if ( axisType != AxisInfo::KnownType::OTHER ){
        std::vector<AxisInfo::KnownType> supportedAxes = _getAxisTypes();

        //Make sure the axis is an axis in the image.
        std::vector<AxisInfo::KnownType>::iterator it  = find( supportedAxes.begin(), supportedAxes.end(), axisType );
        if ( it != supportedAxes.end() ){
            int selectIndex = static_cast<int>( axisType );
            frame = m_selects[selectIndex]->getIndex();
        }
    }
    return frame;
}

std::vector<int> Stack::_getFrameIndices( ) const {
    int selectCount = m_selects.size();
    std::vector<int> frames( selectCount );
    //Determine the index of the frame to load.
    for ( int i = 0; i < selectCount; i++ ){
        frames[i] = m_selects[i]->getIndex();
    }
    return frames;
}

int Stack::_getFrameUpperBound( AxisInfo::KnownType axisType ) const {
    int upperBound = 0;
    if ( axisType != AxisInfo::KnownType::OTHER ){
        int selectIndex = static_cast<int>(axisType );
        upperBound = m_selects[selectIndex]->getUpperBound();
    }
    return upperBound;
}

int Stack::_getIndex( const QString& layerId) const {
    int index = -1;
    int dataCount = m_children.size();
    for ( int i = 0; i < dataCount; i++  ){
        if ( m_children[i]->_getId() == layerId ){
            index = i;
            break;
        }
    }
    return index;
}

int Stack::_getIndexCurrent( ) const {
    int dataIndex = -1;
    if ( m_selectImage ){
        int index = m_selectImage->getIndex();
        int visibleIndex = -1;
        int dataCount = m_children.size();
        for ( int i = 0; i < dataCount; i++ ){
            if ( m_children[i]->_isVisible() ){
                visibleIndex++;
                if ( visibleIndex == index ){
                    dataIndex = i;
                    break;
                }
            }
        }
    }
    return dataIndex;
}

QString Stack::getPixelValue( double x, double y) const {
    std::vector<int> frames = _getFrameIndices();
    return _getPixelValue( x, y, frames );
}

int Stack::_getSelectImageIndex() const {
    int selectImageIndex = -1;
    int stackedImageVisibleCount = _getStackSizeVisible();
    if ( stackedImageVisibleCount >= 1 ){
        selectImageIndex = m_selectImage->getIndex();
    }
    return selectImageIndex;
}

void Stack::_initializeSelections(){
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    m_selectImage = objMan->createObject<Selection>();
    connect( m_selectImage, SIGNAL(indexChanged()), this, SLOT(_scheduleFrameReload()));
    int axisCount = static_cast<int>(AxisInfo::KnownType::OTHER);
    m_selects.resize( axisCount );
    for ( int i = 0; i < axisCount; i++ ){
        m_selects[i] = objMan->createObject<Selection>();
        connect( m_selects[i], SIGNAL(indexChanged()), this, SLOT(_scheduleFrameReload()));
    }
}

void Stack::_initializeState(){
    int regionCount = m_regions.size();
    m_state.insertArray(REGIONS, regionCount );
    m_state.setValue<QString>( LayerGroup::COMPOSITION_MODE, LayerCompositionModes::NONE );
    m_state.flushState();
}

QString Stack::_getCurrentId() const {
    QString id = "";
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        id = m_children[dataIndex]->_getId();
    }
    return id;
}

std::vector<int> Stack::_getImageSlice() const {
    std::vector<int> result;
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        int dimensions = m_children[dataIndex] -> _getDimension();
        result.resize( dimensions );
        Carta::Lib::AxisInfo::KnownType axisXType = _getAxisXType();
        Carta::Lib::AxisInfo::KnownType axisYType = _getAxisYType();
        for ( int i = 0; i < dimensions; i++ ){
            Carta::Lib::AxisInfo::KnownType type  = _getAxisType( i );
            if ( type == axisXType || type == axisYType ){
                result[i] = -1;
            }
            else {
                result[i] = _getFrame( type );
            }
        }
    }
    return result;
}

std::vector<Carta::Lib::RegionInfo> Stack::_getRegions() const {
    int regionCount = m_regions.size();
    std::vector<Carta::Lib::RegionInfo> regionInfos( regionCount );
    for ( int i = 0; i < regionCount; i++ ){
        regionInfos[i] = (*m_regions[i]->getInfo().get());
    }
    return regionInfos;
}

QString Stack::_getStateString() const{
    Carta::State::StateInterface copyState( m_state );
    _saveChildren( copyState, false );
    int regionCount = m_regions.size();
    copyState.resizeArray( REGIONS, regionCount );
    for ( int i = 0; i < regionCount; i++ ){
        QString lookup = Carta::State::UtilState::getLookup( REGIONS, i );
        QString regionStateStr = m_regions[i]->_getStateString();
        copyState.setObject( lookup, regionStateStr );
    }
    copyState.insertValue<QString>( Selection::IMAGE, m_selectImage->getStateString());
    int selectCount = m_selects.size();
    const Carta::Lib::KnownSkyCS cs = _getCoordinateSystem();
    for ( int i = 0; i < selectCount; i++ ){
        QString axisName = AxisMapper::getPurpose( static_cast<AxisInfo::KnownType>(i), cs );
        copyState.insertValue<QString>( axisName, m_selects[i]->getStateString());
    }
    QString stateStr = copyState.toString();
    return stateStr;
}

void Stack::_gridChanged( const Carta::State::StateInterface& state, bool applyAll ){
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        if ( !applyAll ){
            m_children[dataIndex]->_gridChanged( state );
            _scheduleFrameReload( false );
        }
        else {
            int dataCount = m_children.size();
            for ( int i = 0; i < dataCount; i++ ){
                if ( m_children[i] != nullptr ){
                    m_children[i]->_gridChanged( state );
                }
            }
            _scheduleFrameReload( true );
        }
    }
}

void Stack::_load( bool renderAll, bool recomputeClipsOnNewFrame,
        double minClipPercentile, double maxClipPercentile ){
    m_reloadFrameQueued = false;
    std::vector<int> frames = _getFrameIndices();
    if ( !renderAll) {
        int dataIndex = _getIndexCurrent();
        if ( dataIndex >= 0 && m_children[dataIndex] != nullptr) {
            m_children[dataIndex]->_load(frames, recomputeClipsOnNewFrame,
                    minClipPercentile, maxClipPercentile);
            _renderSingle( dataIndex );
        }
    }
    else {
        int dataCount = m_children.size();
        for ( int i = 0; i < dataCount; i++ ){
            m_children[i]->_load( frames, recomputeClipsOnNewFrame, minClipPercentile, maxClipPercentile );
        }
        _renderAll();
    }
}

void Stack::_render( QList<std::shared_ptr<Layer> > datas, int gridIndex ){
    std::vector<int> frames =_getFrameIndices();
    const Carta::Lib::KnownSkyCS& cs = _getCoordinateSystem();
    m_stackDraw->_render( datas, frames, cs, gridIndex );
}

void Stack::_renderAll(){
    int gridIndex = _getIndexCurrent();
    QList<std::shared_ptr<Layer> > datas;
    int dataCount = m_children.size();
    for ( int i = 0; i < dataCount; i++ ){
        int stackIndex = (gridIndex + i) % dataCount;
        datas.append( m_children[stackIndex] );
    }
    _render( datas, gridIndex );
}

void Stack::_renderSingle( int dIndex ){
    if ( dIndex >= 0 && dIndex < m_children.size() ){
        //Render a single data set (the one passed in).
        QList<std::shared_ptr<Layer> > datas;
        datas.append( m_children[dIndex] );
        //No grid unless the data set being passed in is the top one.
        int topIndex = _getIndexCurrent();
        int gridIndex = -1;
        if ( dIndex == topIndex ){
            gridIndex = 0;
        }
        _render( datas, gridIndex );
    }
}

QString Stack::_reorderImages( const std::vector<int> & indices ){
    QString result;
    bool imageReordered = false;
    int dataCount = m_children.size();
    int indexCount = indices.size();
    QList<std::shared_ptr<Layer> > reorderedList;
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
                reorderedList.append( m_children[targetIndex] );
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
        m_children = reorderedList;
        if ( selectedIndex != newSelectedIndex ){
            this->_setFrameImage( newSelectedIndex );
        }
        _scheduleFrameReload( true );
    }
    return result;
}

QString Stack::_resetFrames( int val ){
    //Set the image frame.
    QString layerId;
    if ( 0 <= val && val < m_children.size()){
        //Update the data selectors upper bound based on the data.
        int visibleCount = _getStackSizeVisible();
        m_selectImage->setUpperBound( visibleCount );
        m_selectImage->setIndex(val);
        layerId = m_children[val]->_getId();
        int selectCount = m_selects.size();
        for ( int i = 0; i < selectCount; i++ ){
            AxisInfo::KnownType type = static_cast<AxisInfo::KnownType>(i);
            int upperBound = _getFrameCount( type );
            m_selects[i]->setUpperBound( upperBound );
            if ( m_selects[i]->getIndex() > m_selects[i]->getUpperBound()){
                m_selects[i]->setIndex( 0 );
                emit frameChanged( type );
            }
        }
    }

    return layerId;
}


void Stack::_resetState( const Carta::State::StateInterface& restoreState ){
    LayerGroup::_resetState( restoreState );
    QString dataStateStr = restoreState.getValue<QString>( Selection::IMAGE );
    m_selectImage ->resetState( dataStateStr );
    int selectCount = m_selects.size();
    for ( int i = 0; i < selectCount; i++ ){
        AxisInfo::KnownType axisType = static_cast<AxisInfo::KnownType>( i );
        const Carta::Lib::KnownSkyCS cs = _getCoordinateSystem();
        QString axisPurpose = AxisMapper::getPurpose( axisType, cs );
        QString axisState = restoreState.getValue<QString>( axisPurpose );
        m_selects[i]->resetState( axisState );
    }

    m_regions.clear();
    int regionCount = m_state.getArraySize(REGIONS);
    for ( int i = 0; i < regionCount; i++ ){
        QString regionLookup = Carta::State::UtilState::getLookup( REGIONS, i );
        QString regionState = m_state.toString( regionLookup );
        std::shared_ptr<Region> region = RegionFactory::makeRegion( regionState );
        m_regions.append( region );
    }
    _saveStateRegions();

    _saveState();

    m_stackDraw->setLayers( m_children );
    m_stackDraw->setSelectIndex( m_selectImage->getIndex());
    _scheduleFrameReload( true );
}

void Stack::_resetPan( bool panZoomAll ){
    if ( panZoomAll ){
        int dataCount = m_children.size();
        if ( dataCount > 0 ){
            for ( int i = 0; i < dataCount; i++ ){
                m_children[i]->_resetPan();
            }
            _scheduleFrameReload( true );
        }
    }
    else {
        int dataIndex = _getIndexCurrent();
        if ( dataIndex >= 0 ){
            m_children[dataIndex]->_resetPan();
            _scheduleFrameReload( false );
        }
    }
}


void Stack::_resetZoom( bool panZoomAll ){
    if ( panZoomAll ){
        int dataCount = m_children.size();
        if ( dataCount > 0 ){
            for ( int i = 0; i < dataCount; i++ ){
                m_children[i]->_resetZoom();
            }
            _scheduleFrameReload( true );
        }
    }
    else {
        int dataIndex = _getIndexCurrent();
        if ( dataIndex >= 0 ){
            m_children[dataIndex]->_resetZoom();
            _scheduleFrameReload( false );
        }
    }
}


void Stack::_saveChildren( Carta::State::StateInterface& state, bool truncate ) const {
    int dataCount = m_children.size();
    int oldDataCount = state.getArraySize( LAYERS );
    if ( oldDataCount != dataCount ){
        state.resizeArray(LAYERS, dataCount, Carta::State::StateInterface::PreserveNone );
    }
    for (int i = 0; i < dataCount; i++) {
        QString layerString = m_children[i]->_getStateString( truncate );
        QString dataKey = Carta::State::UtilState::getLookup( LAYERS, i);
        state.setObject( dataKey, layerString);
    }
}

QString Stack::_saveImage( const QString& saveName ){
    QString result;
    m_saveService = new SaveService();
    result = m_saveService->setFileName( saveName );
    if ( result.isEmpty()){
        PreferencesSave* prefSave = Util::findSingletonObject<PreferencesSave>();
        int width = prefSave->getWidth();
        int height = prefSave->getHeight();
        Qt::AspectRatioMode aspectRatioMode = prefSave->getAspectRatioMode();
        m_saveService->setOutputSize( QSize( width, height ) );
        m_saveService->setAspectRatioMode( aspectRatioMode );
        m_saveService->setLayers( m_children );
        m_saveService->setSelectIndex( _getIndexCurrent() );
        connect( m_saveService, SIGNAL(saveImageResult() ),
                this, SLOT(_saveImageResultCB() ) );

        bool saveStarted = m_saveService->saveFullImage();
        if ( !saveStarted ){
            result = "Image was not rendered";
        }
    }
    return result;
}

void Stack::_saveImageResultCB( bool result ){
    emit saveImageResult( result );
    m_saveService->deleteLater();
}



void Stack::_saveState( bool flush ) {
    _saveChildren( m_state, true );
    if ( flush ){
        m_state.flushState();
    }
}

void Stack::_saveStateRegions(){
    //Regions
    int regionCount = m_regions.size();
    int oldRegionCount = m_state.getArraySize( REGIONS);
    if ( regionCount != oldRegionCount){
        m_state.resizeArray( REGIONS, regionCount, Carta::State::StateInterface::PreserveNone );
    }
    for ( int i = 0; i < regionCount; i++ ){
        QString regionKey = Carta::State::UtilState::getLookup( REGIONS, i);
        QString regionTypeStr= m_regions[i]->_getStateString();
        m_state.setObject( regionKey, regionTypeStr );
    }
    m_state.flushState();
}

void Stack::_scheduleFrameReload( bool renderAll ){
    if ( m_children.size() > 0  ){
        // if reload is already pending, do nothing
        if ( m_reloadFrameQueued ) {
            return;
        }
        m_reloadFrameQueued = true;

        int selectIndex=m_selectImage->getIndex();
        m_stackDraw->setSelectIndex( selectIndex);
        emit viewLoad( renderAll );
    }

}


bool Stack::_setCompositionMode( const QString& id, const QString& compositionMode,
        QString& errorMsg ){
    QString actualCompMode;
    bool stateChanged = false;
    bool recognizedMode = m_compositionModes->isCompositionMode( compositionMode, actualCompMode );
    if ( recognizedMode ){
        stateChanged = LayerGroup::_setCompositionMode( id, actualCompMode, errorMsg );
        if ( stateChanged ){
            _saveState();
            _scheduleFrameReload( false );
        }
    }
    else {
        errorMsg = "Unrecognized layer composition mode: "+compositionMode;
    }
    return stateChanged;
}

void Stack::_setFrameAxis(int value, AxisInfo::KnownType axisType ) {
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
                emit frameChanged( axisType );
                _scheduleFrameReload( true );
            }
        }
    }
}

QString Stack::_setFrameImage( int val ){
    //Set the image frame.
    int oldIndex = m_selectImage->getIndex();
    QString layerId;
    if ( oldIndex != val ){
        layerId = _resetFrames( val);
    }
    return layerId;
}


bool Stack::_setLayersGrouped( bool grouped  ){
    bool operationPerformed = LayerGroup::_setLayersGrouped( grouped );
    if ( operationPerformed ){
        _scheduleFrameReload( false );
        _saveState();
    }
    return operationPerformed;
}


void Stack::_setMaskColor( const QString& id, int redAmount,
        int greenAmount, int blueAmount, QStringList& result ){
    if ( 0 > redAmount || redAmount > 255 ){
        result.append( "Invalid red mask color [0,255]: "+QString::number( redAmount ) );
    }
    if ( 0 > greenAmount || greenAmount > 255 ){
        result.append( "Invalid green mask color [0,255]: "+QString::number( greenAmount ) );
    }
    if ( 0 > blueAmount || blueAmount > 255 ){
        result.append( "Invalid blue mask color [0,255]: "+QString::number( blueAmount ) );
    }
    if ( result.length() == 0 ){
        bool changed = LayerGroup::_setMaskColor( id, redAmount, greenAmount, blueAmount);
        if ( changed ){
            _saveState();
            _scheduleFrameReload( false );
        }
    }
}

void Stack::_setMaskAlpha( const QString& id, int alphaAmount, QString& result ){
    if ( 0 > alphaAmount || alphaAmount > 255 ){
        result = "Invalid mask opacity [0,255]:"+QString::number( alphaAmount );
    }
    else {
        bool changed = LayerGroup::_setMaskAlpha( id, alphaAmount );
        if ( changed ){
            _saveState();
            _scheduleFrameReload( false );
        }
    }
}

void Stack::_setPan( double imgX, double imgY, bool panZoomAll ){
    if ( panZoomAll ){
       int childCount = m_children.size();
       for ( int i = 0; i < childCount; i++ ){
           m_children[i]->_setPan( imgX, imgY );
       }
       _scheduleFrameReload( true );
   }
   else {
       int dataIndex = _getIndexCurrent();
       if ( dataIndex >= 0 ){
           m_children[dataIndex]->_setPan( imgX, imgY );
       }
       _scheduleFrameReload( false );
   }
}

bool Stack::_setSelected( const QStringList& names){
    bool stateChanged = LayerGroup::_setSelected( names );
    if ( stateChanged ){
        _saveState( true );
    }
    return stateChanged;
}

void Stack::_setViewName( const QString& viewName ){
    m_stackDraw.reset( new DrawStackSynchronizer(makeRemoteView( viewName)));
    connect( m_stackDraw.get(), SIGNAL(viewResize()), this, SLOT(_viewResize()));
}

bool Stack::_setVisible( const QString& id, bool visible ){
    bool layerFound = LayerGroup::_setVisible( id, visible );
    if ( layerFound ){
        int visibleCount = _getStackSizeVisible();
        m_selectImage->setUpperBound( visibleCount );
        _scheduleFrameReload( false );
        _saveState();
    }
    return layerFound;
}

void Stack::_setZoomLevel( double zoomFactor, bool zoomPanAll ){
    if ( zoomPanAll ){
        int dataCount = m_children.size();
        for ( int i = 0; i < dataCount; i++ ){
            m_children[i]->_setZoom( zoomFactor );
        }
        _scheduleFrameReload( true );
    }
    else {
        int dataIndex = _getIndexCurrent();
        if ( dataIndex >= 0 ){
            m_children[dataIndex]->_setZoom( zoomFactor );
            _scheduleFrameReload( false );
        }
    }
}

void Stack::_updatePan( double centerX , double centerY, bool zoomPanAll ){
    if ( zoomPanAll ){
        for ( std::shared_ptr<Layer> data : m_children ){
            _updatePan( centerX, centerY, data );
        }
        _scheduleFrameReload( true );
    }
    else {
        int dataIndex = _getIndexCurrent();
        if ( dataIndex >= 0 ){
            _updatePan( centerX, centerY, m_children[dataIndex] );
            _scheduleFrameReload( false );
        }
    }

}

void Stack::_updatePan( double centerX , double centerY,
        std::shared_ptr<Layer> data){
    bool validImage = false;
    QPointF imagePt = data -> _getImagePt( { centerX, centerY }, &validImage );
    if ( validImage ){
        double imageX = imagePt.x();
        double imageY = imagePt.y();
        data->_setPan( imageX, imageY );
    }
}

void Stack::_updateZoom( double centerX, double centerY, double zoomFactor, bool zoomPanAll ){
    if ( zoomPanAll ){
        for (std::shared_ptr<Layer> data : m_children ){
            _updateZoom( centerX, centerY, zoomFactor, data );
        }
        _scheduleFrameReload( true );
    }
    else {
        int dataIndex = _getIndexCurrent();
        if ( dataIndex >= 0 ){
            _updateZoom( centerX, centerY, zoomFactor, m_children[dataIndex] );
            _scheduleFrameReload( false );
        }
    }
}

void Stack::_updateZoom( double centerX, double centerY, double zoomFactor,
         std::shared_ptr<Layer> data ){
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
        QPointF currCenter = data ->_getCenterPixel();
        QPointF newCenter = currCenter + delta;
        data->_setPan( newCenter.x(), newCenter.y() );
    }
}

void Stack::_viewResize(){
    QSize clientSize = m_stackDraw->getClientSize();
    for ( int i = 0; i < m_children.size(); i++ ){
        m_children[i]->_viewResize( clientSize );
    }
    _scheduleFrameReload( true );
}


Stack::~Stack() {
    if ( m_selectImage != nullptr ){
        Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
        objMan->destroyObject( m_selectImage->getId());
        m_selectImage = nullptr;
    }
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
      int selectCount = m_selects.size();
      for ( int i = 0; i < selectCount; i++ ){
          if ( m_selects[i] != nullptr){
              objMan->destroyObject(m_selects[i]->getId());
              m_selects[i] = nullptr;
          }
      }
      m_selects.clear();
}
}
}
