#include "LayerGroup.h"
//#include "DataSource.h"
#include "Data/Util.h"
#include "Data/Image/LayerCompositionModes.h"
#include "Data/Image/DrawStackSynchronizer.h"
#include "Data/Image/Grid/AxisMapper.h"
#include "State/UtilState.h"
#include "State/StateInterface.h"
#include "Data/Selection.h"

#include <QDebug>
#include <QDir>
#include "Stack.h"

using Carta::Lib::AxisInfo;


namespace Carta {

namespace Data {

const QString Stack::CLASS_NAME = "Stack";
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
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    m_selectImage = objMan->createObject<Selection>();
    connect( m_selectImage, SIGNAL(indexChanged(bool)), this, SLOT(_scheduleFrameReload()));
}

bool Stack::_addData(const QString& fileName, std::shared_ptr<ColorState> colorState ) {
    bool successfulLoad = LayerGroup::_addData( fileName/*, colorState*/ );
    if ( successfulLoad ){
        int lastIndex = m_children.size() - 1;
        m_children[lastIndex]->_setColorMapGlobal( colorState );
        m_selectImage->setIndex(lastIndex);
        int selectCount = m_selects.size();
        for ( int i = 0; i < selectCount; i++ ){
            AxisInfo::KnownType type = static_cast<AxisInfo::KnownType>(i);
            int frameCount = m_children[lastIndex]->_getFrameCount( type );
            m_selects[i]->setUpperBound( frameCount );
        }

        /*_updateDisplayAxes( targetIndex );*/

        //Update the data selectors upper bound based on the data.
        int visibleCount = _getStackSizeVisible();
        m_selectImage->setUpperBound( visibleCount );

        m_stackDraw->setLayers( m_children );
        m_children[lastIndex]->_viewResize( m_stackDraw->getClientSize() );

        _saveState();

        //Refresh the view of the data.
        _scheduleFrameReload();
    }

    return successfulLoad;
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

void Stack::_displayAxesChanged(std::vector<AxisInfo::KnownType> displayAxisTypes, bool applyAll ){
    std::vector<int> frames = _getFrameIndices();
    if ( !applyAll ){
        int dataIndex = _getIndexCurrent();
        if (dataIndex >= 0 ) {
            if (m_children[dataIndex] != nullptr) {
                std::vector<int> frames = _getFrameIndices();
                m_children[dataIndex]->_displayAxesChanged( displayAxisTypes, frames );
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
    connect( m_selectImage, SIGNAL(indexChanged(bool)), this, SLOT(_scheduleFrameReload()));
    int axisCount = static_cast<int>(AxisInfo::KnownType::OTHER);
    m_selects.resize( axisCount );
    for ( int i = 0; i < axisCount; i++ ){
        m_selects[i] = objMan->createObject<Selection>();
        connect( m_selects[i], SIGNAL(indexChanged(bool)), this, SLOT(_scheduleFrameReload()));
    }
}

void Stack::_initializeState(){
    m_state.insertArray( LayerGroup::LAYERS, 0 );
    m_state.flushState();
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

QString Stack::_getStateString() const{
    Carta::State::StateInterface copyState( m_state );
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
            _renderSingle( dataIndex );
        }
        else {
            int dataCount = m_children.size();
            for ( int i = 0; i < dataCount; i++ ){
                if ( m_children[i] != nullptr ){
                    m_children[i]->_gridChanged( state );
                }
            }
            _renderAll();
        }
    }
}

void Stack::_load(bool recomputeClipsOnNewFrame,
        double minClipPercentile, double maxClipPercentile ){
    m_reloadFrameQueued = false;
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ) {
          if (m_children[dataIndex] != nullptr) {
              std::vector<int> frames = _getFrameIndices();
              m_children[dataIndex]->_load(frames, recomputeClipsOnNewFrame,
                      minClipPercentile, maxClipPercentile);
              _renderSingle( dataIndex );
          }
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
        QList<std::shared_ptr<Layer> > datas;
        datas.append( m_children[dIndex] );
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
        _renderAll();
    }
    return result;
}

QString Stack::_resetFrames( int val ){
    //Set the image frame.
    int oldIndex = m_selectImage->getIndex();
    QString layerId;
    if ( oldIndex != val ){
        m_selectImage->setIndex(val);
        layerId = _getId();
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
    m_stackDraw->setLayers( m_children );

    m_stackDraw->setSelectIndex( m_selectImage->getIndex());
    _renderAll();
}

void Stack::_resetPan( bool panZoomAll ){
    if ( panZoomAll ){
        int dataCount = m_children.size();
        if ( dataCount > 0 ){
            for ( int i = 0; i < dataCount; i++ ){
                m_children[i]->_resetPan();
            }
            _renderAll();
        }
    }
    else {
        int dataIndex = _getIndexCurrent();
        if ( dataIndex >= 0 ){
            m_children[dataIndex]->_resetPan();
            _renderSingle( dataIndex );
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
            _renderAll();
        }
    }
    else {
        int dataIndex = _getIndexCurrent();
        if ( dataIndex >= 0 ){
            m_children[dataIndex]->_resetZoom();
            _renderSingle( dataIndex );
        }
    }
}

QString Stack::saveImage( const QString& saveName,  double scale){
    QString result = _saveImage( saveName, scale, _getFrameIndices());
    return result;
}


void Stack::_saveState( bool flush ) {
    int dataCount = m_children.size();
    int oldDataCount = m_state.getArraySize( LAYERS );
    if ( oldDataCount != dataCount ){
        m_state.resizeArray(LAYERS, dataCount, Carta::State::StateInterface::PreserveNone );
    }
    for (int i = 0; i < dataCount; i++) {
        QString layerString = m_children[i]->_getStateString();
        QString dataKey = Carta::State::UtilState::getLookup( LAYERS, i);
        m_state.setObject( dataKey, layerString);
    }
    if ( flush ){
        m_state.flushState();
    }
}

void Stack::_scheduleFrameReload( bool newClips ){
    if ( m_children.size() > 0  ){
        // if reload is already pending, do nothing
        if ( m_reloadFrameQueued ) {
            return;
        }

        int selectIndex=m_selectImage->getIndex();
        m_stackDraw->setSelectIndex( selectIndex);

    }
    emit viewLoad( newClips );
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
                _renderAll();
            }
        }
    }
}

QString Stack::_setFrameImage( int val ){
    //Set the image frame.
    int oldIndex = m_selectImage->getIndex();
    QString layerId;
    if ( oldIndex != val ){
        m_selectImage->setIndex(val);
        layerId = _getId();
        _resetFrames( val);
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
       _renderAll();
   }
   else {
       int dataIndex = _getIndexCurrent();
       if ( dataIndex >= 0 ){
           m_children[dataIndex]->_setPan( imgX, imgY );
       }
       _renderSingle( dataIndex );
   }
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
        _renderAll();
    }
    else {
        int dataIndex = _getIndexCurrent();
        if ( dataIndex >= 0 ){
            m_children[dataIndex]->_setZoom( zoomFactor );
            _renderSingle( dataIndex );
        }
    }
}

void Stack::_updatePan( double centerX , double centerY, bool zoomPanAll ){
    if ( zoomPanAll ){
        for ( std::shared_ptr<Layer> data : m_children ){
            _updatePan( centerX, centerY, data );
        }
        _renderAll();
    }
    else {
        int dataIndex = _getIndexCurrent();
        if ( dataIndex >= 0 ){
            _updatePan( centerX, centerY, m_children[dataIndex] );
            _renderSingle( dataIndex );
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
        _renderAll();
    }
    else {
        int dataIndex = _getIndexCurrent();
        if ( dataIndex >= 0 ){
            _updateZoom( centerX, centerY, zoomFactor, m_children[dataIndex] );
            _renderSingle( dataIndex );
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
    _renderAll();
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
