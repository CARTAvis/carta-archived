#include "LayerGroup.h"
#include "Data/Image/DataSource.h"
#include "Data/Image/LayerCompositionModes.h"
#include "Data/Image/Draw/DrawStackSynchronizer.h"
#include "Data/Image/Draw/DrawImageViewsSynchronizer.h"
#include "Data/Image/Grid/AxisMapper.h"
#include "Data/Image/Save/SaveService.h"
#include "Data/Preferences/PreferencesSave.h"
#include "Data/Region/Region.h"
#include "Data/Region/RegionFactory.h"
#include "Data/Selection.h"
#include "Data/Units/UnitsIntensity.h"
#include "Data/Util.h"
#include "State/UtilState.h"
#include "State/StateInterface.h"
#include "Globals.h"

#include <QDebug>
#include <QDir>
#include "Stack.h"



using Carta::Lib::AxisInfo;


namespace Carta {

namespace Data {

const QString Stack::CLASS_NAME = "Stack";




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
    m_imageDraws( new DrawImageViewsSynchronizer() ),
    m_selectImage(nullptr){
    _initializeState();
    _initializeSelections();
}

QString Stack::_addDataImage(const QString& fileName, bool* success ) {
    int stackIndex = -1;
    QString result = _addData( fileName, success, &stackIndex);
    if ( *success && stackIndex >= 0 ){
        _resetFrames( stackIndex );
        _saveState();
    }
    return result;
}

bool Stack::_addGroup( ){
    bool groupAdded = LayerGroup::_addGroup();
    if ( groupAdded ){
        _saveState();
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
            m_selectImage->setIndex(visibleImageCount - 1);
        }
        //Update the channel upper bound and index if necessary
        int targetData = _getIndexCurrent();
        int selectCount = m_selects.size();
        for ( int i = 0; i < selectCount; i++ ){
            int frameCount = 1;
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
        emit viewLoad( );
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
    emit viewLoad( );
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


QStringList Stack::_getCoords( double x, double y,
        Carta::Lib::KnownSkyCS system ) const{
    std::vector<int> indices = _getFrameIndices();
    return _getCoordinates( x, y, system, indices );
}


QString Stack::_getCurrentId() const {
    QString id = "";
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        id = m_children[dataIndex]->_getLayerId();
    }
    return id;
}

QString Stack::_getCursorText( int mouseX, int mouseY ){
    int dataIndex = _getIndexCurrent();
    QString cursorText;
    if ( dataIndex >= 0 ){
        std::vector<int> frameIndices = _getFrameIndices();
        QSize outputSize = m_stackDraw->getClientSize();
        cursorText = m_children[dataIndex]->_getCursorText( mouseX, mouseY,
                frameIndices, outputSize );
    }
    return cursorText;
}

QList<std::shared_ptr<Layer> > Stack::_getDrawChildren() const {
    QList<std::shared_ptr<Layer> > datas;
    int dataCount = m_children.size();
    int currentIndex = _getIndexCurrent();
    if ( currentIndex >= 0 ){
		for ( int i = 0; i < dataCount; i++ ){
			int childIndex = (currentIndex + i ) % dataCount;
			bool visible = m_children[childIndex]->_isVisible();
			bool empty = m_children[childIndex]->_isEmpty();
			if ( visible && !empty ){
				datas.append( m_children[childIndex] );
			}
		}
    }
    return datas;
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

int Stack::_getIndex( const QString& layerId) const {
    int index = -1;
    int dataCount = m_children.size();
    for ( int i = 0; i < dataCount; i++  ){
        if ( m_children[i]->_isDescendant( layerId ) ){
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
            if ( m_children[i]->_isVisible() && !m_children[i]->_isEmpty()){
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

QRectF Stack::_getInputRectangle( ) const {
    QSize output = m_stackDraw->getClientSize();
    QRectF rect = _getInputRect( output );
    return rect;
}

QStringList Stack::_getLayerIds( ) const {
    QStringList idList;
    for ( std::shared_ptr<Layer> layer : m_children ){
        idList.append( layer->_getLayerIds());
    }
    return idList;
}

QSize Stack::_getOutputSize() const {
    return m_stackDraw->getClientSize();
}

QString Stack::_getPixelVal( double x, double y) const {
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

QString Stack::_getStateString() const{
    Carta::State::StateInterface copyState( m_state );
    _saveChildren( copyState, false );
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
        }
        else {
            int dataCount = m_children.size();
            for ( int i = 0; i < dataCount; i++ ){
                if ( m_children[i] != nullptr ){
                    m_children[i]->_gridChanged( state );
                }
            }
        }
        emit viewLoad( );
    }
}

void Stack::_initializeSelections(){
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    m_selectImage = objMan->createObject<Selection>();

    int axisCount = static_cast<int>(AxisInfo::KnownType::OTHER);
    m_selects.resize( axisCount );
    for ( int i = 0; i < axisCount; i++ ){
        m_selects[i] = objMan->createObject<Selection>();
        connect( m_selects[i], SIGNAL(indexChanged()), this, SIGNAL(viewLoad()));
    }
}


void Stack::_initializeState(){
    m_state.setValue<QString>( LayerGroup::COMPOSITION_MODE, LayerCompositionModes::NONE );
    m_state.flushState();
}


QString Stack::_moveSelectedLayers( bool moveDown ){
    QString result;
    QList<int> selectIndices;
    int dataCount = m_children.size();
    for ( int i = 0; i < dataCount; i++ ){
        if ( m_children[i]->_isSelected() ){
            selectIndices.append(i);
        }
    }
    int selectedCount = selectIndices.size();
    bool stackChanged = false;
    if ( selectedCount == 0 ){
        result = "Please make sure at least one, non-internal layer is selected.";
    }
    else {
        if ( moveDown ){
            for ( int i = selectedCount - 1; i >= 0; i-- ){
                int index = selectIndices[i];
                if ( index == dataCount - 1 ){
                    //If it is the last index, we can't move it down, so just skip it
                    //unless it is the only one selected, in which case we should generate
                    //an error message.
                    if ( selectedCount == 1 ){
                        result = "The last image in the stack can not be moved down any further.";
                    }
                }
                else {
                    int newIndex = index + 1;
                    std::shared_ptr<Layer> moveLayer = m_children[index];
                    m_children.removeAt( index );
                    m_children.insert( newIndex, moveLayer );
                    stackChanged = true;
                }
            }
        }
        else {
            for ( int i = 0; i < selectedCount; i++ ){
                int index = selectIndices[i];
                if ( index == 0 ){
                    //If it is the first index, we can't move it up, so just skip it
                    //unless it is the only one selected, in which case we should generate
                    //an error message.
                    if ( selectedCount == 1 ){
                        result = "The first image in the stack can not be moved up any further.";
                    }
                }
                else {
                    int newIndex = index - 1;
                    std::shared_ptr<Layer> moveLayer = m_children[index];
                    m_children.removeAt( index );
                    m_children.insert( newIndex, moveLayer );
                    stackChanged = true;
                }
            }
        }
        if ( stackChanged ){
            emit viewLoad( );
            _saveState();
        }
    }
    return result;
}

void Stack::_render( QList<std::shared_ptr<Layer> > datas, int gridIndex,
		bool recomputeClipsOnNewFrame, double minClipPercentile, double maxClipPercentile ){
    std::vector<int> frames =_getFrameIndices();
    const Carta::Lib::KnownSkyCS& cs = _getCoordinateSystem();
    std::shared_ptr<RenderRequest> request( new RenderRequest( frames, cs));
    request->setTopIndex( gridIndex );
    request->setRequestMain( true );
    request->setRequestContext( true );
    request->setData( datas );
    request->setRecomputeClips( recomputeClipsOnNewFrame );
    request->setClipPercents( minClipPercentile, maxClipPercentile );
    m_imageDraws->render( request);
}



void Stack::_renderAll(bool recomputeClipsOnNewFrame,
        double minClipPercentile, double maxClipPercentile){
    int gridIndex = 0;
    QList<std::shared_ptr<Layer> > datas = _getDrawChildren();
    _render( datas, gridIndex, recomputeClipsOnNewFrame, minClipPercentile, maxClipPercentile );
}

void Stack::_renderContext( double zoomFactor ){
    if ( m_imageDraws->isContextView()){
        std::vector<int> frames =_getFrameIndices();
        const Carta::Lib::KnownSkyCS& cs = _getCoordinateSystem();
        std::shared_ptr<RenderRequest> request( new RenderRequest( frames, cs));
        int gridIndex = _getIndexCurrent();
        request->setTopIndex( gridIndex );
        request->setRequestContext( true );
        request->setZoom( zoomFactor );
        QSize imageSize = _getDisplaySize();
        request->setPan(  QPointF(imageSize.width()/2, imageSize.height()/2) );
        QList<std::shared_ptr<Layer> > datas = _getDrawChildren();
        request->setData( datas );
        m_imageDraws->render( request);
    }
}

void Stack::_renderZoom( int mouseX, int mouseY, double zoomFactor ){
    if ( m_imageDraws->isZoomView()){
        bool validPt = false;
        QPointF screenPt( mouseX, mouseY );
        QSize outputSize = m_stackDraw->getClientSize();
        QPointF panPt = _getImagePt( screenPt, outputSize, &validPt );
        std::vector<int> frames =_getFrameIndices();
        const Carta::Lib::KnownSkyCS& cs = _getCoordinateSystem();
        std::shared_ptr<RenderRequest> request( new RenderRequest( frames, cs));
        int gridIndex = _getIndexCurrent();
        request->setTopIndex( gridIndex );
        request->setRequestZoom( true );
        request->setPan( panPt);
        request->setZoom( zoomFactor );
        if ( validPt ){
            QList<std::shared_ptr<Layer> > datas = _getDrawChildren();
            request->setData( datas );
            m_imageDraws->render( request);
        }
        else {
            //Clear the screen.
            QList<std::shared_ptr<Layer> > datas;
            request->setData( datas );
            m_imageDraws->render( request );
        }
    }
}

QString Stack::_resetFrames( int val ){
	//Set the image frame.
	QString layerId;
	if ( 0 <= val && val < m_children.size()){
		//Update the data selectors upper bound based on the data.
		int visibleCount = _getStackSizeVisible();
		m_selectImage->setUpperBound( visibleCount );
		m_selectImage->setIndex(val);
		bool spectralAxis = m_children[val]->_isSpectralAxis();
		UnitsIntensity* uIntensity = Util::findSingletonObject<UnitsIntensity>();
		uIntensity->setSpectralAxisAvailable( spectralAxis );
		layerId = m_children[val]->_getLayerId();
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


void Stack::_resetStack( const Carta::State::StateInterface& restoreState ){
    _resetState( restoreState );
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
    _saveState();
    emit viewLoad();
}

void Stack::_resetPan( bool panZoomAll ){
    if ( panZoomAll ){
        int dataCount = m_children.size();
        if ( dataCount > 0 ){
            for ( int i = 0; i < dataCount; i++ ){
                m_children[i]->_resetPan();
            }
        }
    }
    else {
        int dataIndex = _getIndexCurrent();
        if ( dataIndex >= 0 ){
            m_children[dataIndex]->_resetPan();
        }
    }
    emit viewLoad( );
}


void Stack::_resetZoom( bool panZoomAll ){
    if ( panZoomAll ){
        int dataCount = m_children.size();
        if ( dataCount > 0 ){
            for ( int i = 0; i < dataCount; i++ ){
                m_children[i]->_resetZoom();
            }
        }
    }
    else {
        int dataIndex = _getIndexCurrent();
        if ( dataIndex >= 0 ){
            m_children[dataIndex]->_resetZoom();
        }
    }
    emit viewLoad( );
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
    m_saveService->setFileName( saveName );
    PreferencesSave* prefSave = Util::findSingletonObject<PreferencesSave>();
    int width = prefSave->getWidth();
    int height = prefSave->getHeight();
    Qt::AspectRatioMode aspectRatioMode = prefSave->getAspectRatioMode();
    m_saveService->setAspectRatioMode( aspectRatioMode );
    m_saveService->setLayers( m_children );
    connect( m_saveService, SIGNAL(saveImageResult(bool) ),
            this, SLOT(_saveImageResultCB(bool) ) );
    std::vector<int> frameIndices = _getFrameIndices();
    std::shared_ptr<RenderRequest> request( new RenderRequest( frameIndices, _getCoordinateSystem()));
    request->setOutputSize( QSize(width, height) );
    request->setTopIndex( _getIndexCurrent());
    request->setRequestMain( true );
    bool saveStarted = m_saveService->saveImage(/*frameIndices, _getCoordinateSystem()*/request);
    if ( !saveStarted ){
        result = "Image was not saved.  Please check the file name.";
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

bool Stack::_setCompositionMode( const QString& id, const QString& compositionMode,
        QString& errorMsg ){
    QString actualCompMode;
    bool stateChanged = false;
    bool recognizedMode = m_compositionModes->isCompositionMode( compositionMode, actualCompMode );
    if ( recognizedMode ){
        stateChanged = LayerGroup::_setCompositionMode( id, actualCompMode, errorMsg );
        if ( stateChanged ){
            _saveState();
            emit viewLoad();
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
                emit viewLoad();
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
        emit viewLoad();
    }
    return layerId;
}



bool Stack::_setLayerName( const QString& id, const QString& name ){
    bool nameSet = LayerGroup::_setLayerName( id, name );
    if ( nameSet ){
        _saveState();
    }
    return nameSet;
}

bool Stack::_setLayersGrouped( bool grouped  ){
	QSize clientSize = m_stackDraw->getClientSize();
    bool operationPerformed = LayerGroup::_setLayersGrouped( grouped, clientSize);
    if ( operationPerformed ){
        emit viewLoad();
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
            emit viewLoad();
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
            emit viewLoad();
        }
    }
}

void Stack::_setPan( double imgX, double imgY, bool panZoomAll ){
    if ( panZoomAll ){
       int childCount = m_children.size();
       for ( int i = 0; i < childCount; i++ ){
           m_children[i]->_setPan( imgX, imgY );
       }
   }
   else {
       int dataIndex = _getIndexCurrent();
       if ( dataIndex >= 0 ){
           m_children[dataIndex]->_setPan( imgX, imgY );
       }
   }
   emit viewLoad();
}

bool Stack::_setSelected( QStringList& names){
    bool stateChanged = LayerGroup::_setSelected( names );
    if ( stateChanged ){
        _saveState( true );
    }
    return stateChanged;
}

void Stack::_setViewName( const QString& viewName ){
    m_stackDraw.reset( new DrawStackSynchronizer(makeRemoteView( viewName)));
    m_imageDraws->setViewDraw( m_stackDraw );
    connect( m_stackDraw.get(), SIGNAL(viewResize()), this, SLOT(_viewResize()));
    connect( m_stackDraw.get(), SIGNAL(inputEvent(InputEvent)),
    		this, SIGNAL(inputEvent(InputEvent)));
}

void Stack::_setViewDrawContext( std::shared_ptr<DrawStackSynchronizer> drawContext ){
    m_imageDraws->setViewDrawContext( drawContext );
}

void Stack::_setViewDrawZoom( std::shared_ptr<DrawStackSynchronizer> drawZoom ){
    m_imageDraws->setViewDrawZoom( drawZoom );
}

bool Stack::_setVisible( const QString& id, bool visible ){
    bool layerFound = LayerGroup::_setVisible( id, visible );
    if ( layerFound ){
        int visibleCount = _getStackSizeVisible();
        m_selectImage->setUpperBound( visibleCount );
        emit viewLoad();
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
    }
    else {
        int dataIndex = _getIndexCurrent();
        if ( dataIndex >= 0 ){
            m_children[dataIndex]->_setZoom( zoomFactor );
        }
    }
    emit viewLoad();
}

void Stack::_updatePan( double centerX , double centerY, bool zoomPanAll ){
    if ( zoomPanAll ){
        for ( std::shared_ptr<Layer> data : m_children ){
            _updatePan( centerX, centerY, data );
        }
    }
    else {
        int dataIndex = _getIndexCurrent();
        if ( dataIndex >= 0 ){
            _updatePan( centerX, centerY, m_children[dataIndex] );
        }
    }
    emit viewLoad();
}

void Stack::_updatePan( double centerX , double centerY,
        std::shared_ptr<Layer> data){
    bool validImage = false;
    QSize outputSize = m_stackDraw->getClientSize();
    QPointF imagePt = data -> _getImagePt( { centerX, centerY }, outputSize, &validImage );
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
    }
    else {
        int dataIndex = _getIndexCurrent();
        if ( dataIndex >= 0 ){
            _updateZoom( centerX, centerY, zoomFactor, m_children[dataIndex] );
        }
    }
    emit viewLoad();
}

void Stack::_updateZoom( double centerX, double centerY, double zoomFactor,
         std::shared_ptr<Layer> data ){
    //Remember where the user clicked
    QPointF clickPtScreen( centerX, centerY);
    bool validImage = false;
    QSize outputSize = m_stackDraw->getClientSize();
    QPointF clickPtImageOld = data->_getImagePt( clickPtScreen, outputSize, &validImage );
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
        QSize outputSize = m_stackDraw->getClientSize();
        QPointF clickPtImageNew = data ->_getImagePt( clickPtScreen, outputSize, &validImage );

        // calculate the difference
        QPointF delta = clickPtImageOld - clickPtImageNew;

        // add the delta to the current center
        QPointF currCenter = data ->_getCenterPixel();
        QPointF newCenter = currCenter + delta;
        data->_setPan( newCenter.x(), newCenter.y() );
    }
}



void Stack::_viewResize(){
    emit viewLoad();
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
