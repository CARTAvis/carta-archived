#include "LayerGroup.h"
#include "LayerData.h"
#include "DataSource.h"
#include "Data/Util.h"
#include "Data/Image/LayerCompositionModes.h"
#include "CartaLib/IRemoteVGView.h"
#include "Data/Image/Draw/DrawGroupSynchronizer.h"
#include "State/UtilState.h"

#include <QDebug>
#include <QDir>

using Carta::Lib::AxisInfo;


namespace Carta {

namespace Data {

const QString LayerGroup::CLASS_NAME = "LayerGroup";
const QString LayerGroup::COMPOSITION_MODE="mode";
const QString LayerGroup::GROUP = "Group";
const QString LayerGroup::LAYERS = "layers";


class LayerGroup::Factory : public Carta::State::CartaObjectFactory {

public:

    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new LayerGroup(path, id);
    }
};

bool LayerGroup::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass (CLASS_NAME,
                                                   new LayerGroup::Factory());

LayerGroup::LayerGroup( const QString& path, const QString& id ):
    LayerGroup( CLASS_NAME, path, id ){

}

LayerGroup::LayerGroup(const QString& className, const QString& path, const QString& id) :
    Layer( className, path, id),
    m_drawSync( nullptr ){

    _initializeState();

    // create the synchronizer
    m_drawSync.reset( new DrawGroupSynchronizer( ) );

    // connect its done() slot to our renderingSlot()
    connect( m_drawSync.get(), SIGNAL(done(QImage)),
                            this, SLOT(_renderingDone(QImage)));
}

void LayerGroup::_addContourSet( std::shared_ptr<DataContours> contourSet){
    int childCount = m_children.size();
    for ( int i = 0; i < childCount; i++ ){
        if ( m_children[i]->_isSelected() ){
            m_children[i]->_addContourSet( contourSet );
        }
    }
}

QString LayerGroup::_addData(const QString& fileName, bool* success ) {
    //Find the location of the data, if it already exists.
    int targetIndex = _getIndex( fileName );
    QString result;
    //Add the data if it is not already there.
    if (targetIndex == -1) {
        Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
        LayerData* targetSource = objMan->createObject<LayerData>();

        targetIndex = m_children.size();
        connect( targetSource, SIGNAL(contourSetAdded(Layer*,const QString&)),
                this, SIGNAL(contourSetAdded(Layer*, const QString&)));
        connect( targetSource, SIGNAL(contourSetRemoved(const QString&)),
                        this, SIGNAL(contourSetRemoved(const QString&)));
        connect( targetSource, SIGNAL(colorStateChanged()), this, SIGNAL(colorStateChanged() ));
        m_children.append(std::shared_ptr<Layer>(targetSource));
        _setColorSupport( m_children[targetIndex] );
    }

    result = m_children[targetIndex]->_setFileName(fileName, success );
    return result;
}




bool LayerGroup::_addGroup(){
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    LayerGroup* targetSource = objMan->createObject<LayerGroup>();
    m_children.append( std::shared_ptr<Layer>(targetSource));
    return true;
}


void LayerGroup::_addLayer( std::shared_ptr<Layer> layer ){
    m_children.append( layer );
    int targetIndex = m_children.size() - 1;
    _setColorSupport( m_children[ targetIndex ]);
}

void LayerGroup::_clearColorMap(){
    Layer::_clearColorMap();
    int childCount = m_children.size();
    for ( int i = 0; i < childCount; i++ ){
        m_children[i]->_clearColorMap();
    }
}

std::shared_ptr<DataContours> LayerGroup::_getContour( const QString& name ){
    std::shared_ptr<DataContours> contourSet( nullptr );
    for ( std::shared_ptr<Layer> layer : m_children ){
        contourSet = layer->_getContour( name );
        if ( contourSet ){
            break;
        }
    }
    return contourSet;
}



bool LayerGroup::_closeData( const QString& id ){
    int targetIndex = -1;
    bool dataClosed = false;
    int dataCount = m_children.size();
    for ( int i = 0; i < dataCount; i++ ){
        bool childMatch = m_children[i]->_isMatch( id );
        if ( childMatch ){
            targetIndex = i;
            break;
        }
    }
    /***
     * TODO:  Need to change these to Ids so that we don't have to deal with partial
     * matches. If we get down to one item in the children, or 0 items, to we need to
     * collapse ourselves from a composite to a singleton?
     */
    qDebug() << "Todo";
    if ( targetIndex >= 0 ){
        _removeData( targetIndex );
        dataClosed = true;
    }
    else {
        //See if any of the composite children can remove it.
        for ( int i = 0; i < dataCount; i++ ){
           bool childClosed = m_children[i]->_closeData( id );
           if ( childClosed ){
               dataClosed = true;
           }
        }
    }
    return dataClosed;
}


void LayerGroup::_colorChanged(){
    int childCount = m_children.size();
    for ( int i = 0; i < childCount; i++ ){
        m_children[i]->_colorChanged();
    }
}


void LayerGroup::_displayAxesChanged(std::vector<AxisInfo::KnownType> displayAxisTypes,
        const std::vector<int>& frames ){
    for ( std::shared_ptr<Layer> node : m_children ){
        node ->_displayAxesChanged( displayAxisTypes, frames );
    }
}

Carta::Lib::AxisInfo::KnownType LayerGroup::_getAxisType( int /*index*/ ) const {
    AxisInfo::KnownType axisType = AxisInfo::KnownType::OTHER;
    return axisType;
}

Carta::Lib::AxisInfo::KnownType LayerGroup::_getAxisXType() const {
    AxisInfo::KnownType axisType = AxisInfo::KnownType::OTHER;
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        axisType = m_children[dataIndex]->_getAxisXType();
    }
    return axisType;
}

Carta::Lib::AxisInfo::KnownType LayerGroup::_getAxisYType() const {
    AxisInfo::KnownType axisType = AxisInfo::KnownType::OTHER;
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        axisType = m_children[dataIndex]->_getAxisYType();
    }
    return axisType;
}


std::vector<Carta::Lib::AxisInfo::KnownType> LayerGroup::_getAxisZTypes() const {
    std::vector<Carta::Lib::AxisInfo::KnownType> zTypes;
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        zTypes = m_children[dataIndex]->_getAxisZTypes();
    }
    return zTypes;
}

std::vector<Carta::Lib::AxisInfo::KnownType> LayerGroup::_getAxisTypes() const {
    std::vector<Carta::Lib::AxisInfo::KnownType> axisTypes;
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        axisTypes = m_children[dataIndex]->_getAxisTypes();
    }
    return axisTypes;
}


QPointF LayerGroup::_getCenterPixel() const {
    int dataIndex = _getIndexCurrent();
    QPointF center = QPointF( nan(""), nan("") );
    if ( dataIndex >= 0 ) {
        center = m_children[dataIndex]->_getCenterPixel();
    }
    return center;
}



QString LayerGroup::_getCompositionMode() const {
    return m_state.getValue<QString>( COMPOSITION_MODE );
}

QStringList LayerGroup::_getCoordinates( double x, double y,
        Carta::Lib::KnownSkyCS system , const std::vector<int>& frames ) const{
    QStringList coordStr;
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        coordStr = m_children[dataIndex]->_getCoordinates( x, y, system, frames );
    }
    return coordStr;
}

Carta::Lib::KnownSkyCS LayerGroup::_getCoordinateSystem() const {
    Carta::Lib::KnownSkyCS cs = Carta::Lib::KnownSkyCS::Unknown;
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0){
        cs = m_children[dataIndex]->_getCoordinateSystem();
    }
    return cs;
}

QString LayerGroup::_getCursorText( int mouseX, int mouseY, const std::vector<int>& frames ){
    QString cursorText;
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        cursorText = m_children[dataIndex]->_getCursorText( mouseX, mouseY, frames );
    }
    return cursorText;

}

QString LayerGroup::_getDefaultName( const QString& id ) const {
    return GROUP + " "+id;
}

int LayerGroup::_getDimension( int coordIndex ) const {
    int dim = -1;
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        dim = m_children[dataIndex]->_getDimension( coordIndex );
    }
    return dim;
}


int LayerGroup::_getDimension() const {
    int imageSize = 0;
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        imageSize = m_children[dataIndex]->_getDimension();
    }
    return imageSize;
}


int LayerGroup::_getFrameCount( AxisInfo::KnownType type ) const {
    int frameCount = 1;
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        frameCount = m_children[dataIndex]->_getFrameCount( type );
    }
    return frameCount;
}

Carta::State::StateInterface LayerGroup::_getGridState() const {
    Carta::State::StateInterface gridState("");
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        gridState = m_children[dataIndex]->_getGridState();
    }
    return gridState;
}

std::shared_ptr<Carta::Lib::Image::ImageInterface> LayerGroup::_getImage(){
    std::shared_ptr<Carta::Lib::Image::ImageInterface> image(nullptr);
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        image = m_children[dataIndex]->_getImage();
    }
    return image;
}


std::shared_ptr<DataSource> LayerGroup::_getDataSource(){
    std::shared_ptr<DataSource> dSource( nullptr );
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        dSource = m_children[dataIndex]->_getDataSource();
    }
    return dSource;
}

std::vector< std::shared_ptr<DataSource> > LayerGroup::_getDataSources() {
    std::vector< std::shared_ptr<DataSource> > dataSources;
    int dataCount = m_children.size();
    //Return the images in stack order.
    int startIndex = _getIndexCurrent();
    for ( int i = 0; i < dataCount; i++ ){
        int dIndex = (startIndex + i) % dataCount;
        if ( m_children[dIndex]->_isVisible() ){
            std::vector< std::shared_ptr<DataSource> > childSources = m_children[dIndex]->_getDataSources();
            for ( std::shared_ptr<DataSource> dSource : childSources ){
                dataSources.push_back( dSource );
            }
        }
    }
    return dataSources;
}

std::vector<int> LayerGroup::_getImageDimensions( ) const {
    std::vector<int> result;
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        result = m_children[dataIndex]->_getImageDimensions();
    }
    return result;
}


QPointF LayerGroup::_getImagePt( QPointF screenPt, bool* valid ) const {
    QPointF imagePt;
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        imagePt = m_children[dataIndex]->_getImagePt( screenPt, valid );
    }
    else {
        *valid = false;
    }
    return imagePt;
}

std::vector< std::shared_ptr<Carta::Lib::Image::ImageInterface> > LayerGroup::_getImages(){
    std::vector<std::shared_ptr<Carta::Lib::Image::ImageInterface> > images;
    int dataCount = m_children.size();
    //Return the images in stack order.
    int startIndex = _getIndexCurrent();
    for ( int i = 0; i < dataCount; i++ ){
        int dIndex = (startIndex + i) % dataCount;
        if ( m_children[dIndex]->_isVisible() ){
            images.push_back( m_children[dIndex]->_getImage());
        }
    }
    return images;
}

int LayerGroup::_getIndex( const QString& fileName) const{
    int dataCount = m_children.size();
    int targetIndex = -1;
    for ( int i = 0; i < dataCount; i++ ){
        QString dataName = m_children[i]->_getId();
        if ( fileName == dataName ){
            targetIndex = i;
            break;
        }
    }
    return targetIndex;
}

int LayerGroup::_getIndexCurrent( ) const {
    int dataIndex = -1;
    return dataIndex;
}

bool LayerGroup::_getIntensity( int frameLow, int frameHigh, double percentile, double* intensity ) const {
    bool intensityFound = false;
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        intensityFound = m_children[dataIndex]->_getIntensity( frameLow, frameHigh, percentile, intensity );
    }
    return intensityFound;
}

QStringList LayerGroup::_getLayerIds( ) const {
    QStringList idList = Layer::_getLayerIds();
    for ( std::shared_ptr<Layer> layer : m_children ){
        idList.append( layer->_getLayerIds());
    }
    return idList;
}


QSize LayerGroup::_getOutputSize() const {
    QSize size;
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        size = m_children[dataIndex]-> _getOutputSize();
    }
    return size;
}

double LayerGroup::_getPercentile( int frameLow, int frameHigh, double intensity ) const {
    double percentile = 0;
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        percentile = m_children[dataIndex]->_getPercentile( frameLow, frameHigh, intensity );
    }
    return percentile;
}



QStringList LayerGroup::_getPixelCoordinates( double ra, double dec ) const{
    QStringList result("");
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        result = m_children[dataIndex]->_getPixelCoordinates( ra, dec );
    }
    return result;
}

QString LayerGroup::_getPixelUnits() const {
    QString units;
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        units = m_children[dataIndex]->_getPixelUnits();
    }
    return units;
}

QString LayerGroup::_getPixelValue( double x, double y, const std::vector<int>& frames ) const {
    QString pixelValue = "";
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        pixelValue = m_children[dataIndex]->_getPixelValue( x, y, frames );
    }
    return pixelValue;
}


QPointF LayerGroup::_getScreenPt( QPointF imagePt, bool* valid ) const {
    QPointF screenPt;
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        screenPt = m_children[dataIndex]->_getScreenPt( imagePt, valid );
    }
    else {
        *valid = false;
    }
    return screenPt;
}

int LayerGroup::_getStackSize() const {
    return m_children.size();
}

int LayerGroup::_getStackSizeVisible() const {
    int visibleCount = 0;
    int imageCount = m_children.size();
    for ( int i = 0; i < imageCount; i++ ){
        if ( m_children[i]->_isVisible() ){
            visibleCount++;
        }
    }
    return visibleCount;
}

QString LayerGroup::_getStateString( bool truncatePaths ) const{
    Carta::State::StateInterface copyState( m_state );
    int childCount = m_children.size();
    copyState.resizeArray( LAYERS, childCount );
    for ( int i = 0; i < childCount; i++ ){
        QString key = Carta::State::UtilState::getLookup( LAYERS, i );
        copyState.setObject( key, m_children[i]->_getStateString( truncatePaths ));
    }
    QString stateStr = copyState.toString();
    return stateStr;
}

Carta::Lib::VectorGraphics::VGList LayerGroup::_getVectorGraphics(){
    Carta::Lib::VectorGraphics::VGComposer comp = Carta::Lib::VectorGraphics::VGComposer( );
    for ( std::shared_ptr<Layer> layer : m_children ){
        comp.appendList( layer->_getVectorGraphics() );
    }
    return comp.vgList();
}


double LayerGroup::_getZoom() const {
    double zoom = DataSource::ZOOM_DEFAULT;
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        zoom = m_children[dataIndex]-> _getZoom();
    }
    return zoom;
}

void LayerGroup::_gridChanged( const Carta::State::StateInterface& state ){
    int dataCount = m_children.size();
    for ( int i = 0; i < dataCount; i++ ){
        if ( m_children[i] != nullptr ){
            m_children[i]->_gridChanged( state );
        }
    }
}


void LayerGroup::_initializeState(){
    QString defaultCompMode = m_compositionModes->getDefault();
    m_state.insertValue<QString>( COMPOSITION_MODE, defaultCompMode );
    m_state.insertArray( LayerGroup::LAYERS, 0 );
    m_state.setValue<QString>( LAYER_NAME, "Group"+getId());
}


void LayerGroup::_load(vector<int> frames, bool recomputeClipsOnNewFrame,
        double minClipPercentile, double maxClipPercentile ){
    int childCount = m_children.size();
    for ( int i = 0; i < childCount; i++ ){
        m_children[i]->_load( frames, recomputeClipsOnNewFrame, minClipPercentile, maxClipPercentile );
    }
}

void LayerGroup::_removeData( int index ){
    disconnect( m_children[index].get());
    QString id = m_children[index]->getId();
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    objMan->removeObject( id );
    m_children.removeAt( index );
}


void LayerGroup::_removeContourSet( std::shared_ptr<DataContours> contourSet ){
    for ( std::shared_ptr<Layer> layer : m_children ){
        layer->_removeContourSet( contourSet );
    }
}


void LayerGroup::_render( const std::vector<int>& frames,
        const Carta::Lib::KnownSkyCS& cs, bool topOfStack ){
    m_drawSync->setLayers( m_children );
    m_drawSync->setCombineMode( _getCompositionMode() );
    m_drawSync->render( frames, cs, topOfStack );
}


void LayerGroup ::_renderingDone( QImage image ){
    m_qimage = image;
    emit renderingDone();
}


void LayerGroup::_resetState( const Carta::State::StateInterface& restoreState ){
    Layer::_resetState( restoreState);
    m_state.setValue<QString>( COMPOSITION_MODE, restoreState.getValue<QString>(COMPOSITION_MODE) );

    //State of children
    int dataCount = restoreState.getArraySize(LAYERS);
    QStringList loadedFiles;
    for ( int i = 0; i < dataCount; i++ ){
        QString dataLookup = Carta::State::UtilState::getLookup( LAYERS, i );
        QString typeLookup = Carta::State::UtilState::getLookup( dataLookup, Util::TYPE );
        QString layerType = restoreState.getValue<QString>( typeLookup );
        QString idLookup = Carta::State::UtilState::getLookup( dataLookup, Util::ID );
        QString id = restoreState.getValue<QString>( idLookup );
        int dataIndex = _getIndex( id );
        if ( dataIndex == -1 ){
            if ( layerType == LayerGroup::CLASS_NAME ){
                _addGroup();
            }
            else {
                QString fileLookup = Carta::State::UtilState::getLookup( dataLookup, Util::NAME);
                QString fileName = restoreState.getValue<QString>( fileLookup );
                if ( !fileName.isEmpty()){
                    bool success = false;
                    _addData( fileName, &success );
                }
            }
            dataIndex = m_children.size()-1;
            m_children[dataIndex]->_resetState( restoreState.toString( dataLookup));

        }
        loadedFiles.append( id );
        if ( dataIndex >= 0 ){
            m_children[dataIndex]->_resetState(restoreState.toString(dataLookup));
        }
    }

    //Remove any data that should not be there
    int stackCount = m_children.size();
    for ( int i = stackCount-1; i>= 0; i--){
        QString id = m_children[i]->_getId();
        if ( !loadedFiles.contains( id )){
            _removeData( i );
        }
    }
}


void LayerGroup::_resetZoom(){
    int dataCount = m_children.size();
    for ( int i = 0; i < dataCount; i++ ){
        m_children[i]->_resetZoom();
    }
}


void LayerGroup::_resetPan( ){
    int dataCount = m_children.size();
    for ( int i = 0; i < dataCount; i++ ){
        m_children[i]->_resetPan();
    }
}


void LayerGroup::_setColorMapGlobal( std::shared_ptr<ColorState> colorState ){
    for ( std::shared_ptr<Layer> layer : m_children ){
        layer->_setColorMapGlobal( colorState );
    }
}

void LayerGroup::_setColorSupport( std::shared_ptr<Layer> layer ){
    QString compMode = _getCompositionMode();
    bool colorSupport = m_compositionModes->isColorSupport( compMode );
    bool alphaSupport = m_compositionModes->isAlphaSupport( compMode );
    layer->_setSupportColor( colorSupport );
    layer->_setSupportAlpha( alphaSupport );
}

bool LayerGroup::_setCompositionMode( const QString& id, const QString& compositionMode,
        QString& errorMsg ){
    bool stateChanged = false;
    if ( id == _getId() ){
        QString oldMode = m_state.getValue<QString>( COMPOSITION_MODE );
        if ( oldMode != compositionMode ){
            m_state.setValue<QString>( COMPOSITION_MODE, compositionMode );
            bool colorSupport = m_compositionModes->isColorSupport( compositionMode );
            bool alphaSupport = m_compositionModes->isAlphaSupport( compositionMode );
            int childCount = m_children.size();
            for ( int i = 0; i < childCount; i++ ){
                  m_children[i]->_setSupportColor( colorSupport );
                  m_children[i]->_setSupportAlpha( alphaSupport );
            }
            stateChanged = true;
        }
    }
    else {
        //See if any of the children can set it.
        int dataCount = m_children.size();
        for ( int i = 0; i < dataCount; i++ ){
            bool childSettable = m_children[i]->_setCompositionMode( id, compositionMode, errorMsg );
            if ( childSettable ){
                stateChanged = true;
                break;
            }
        }
    }
    return stateChanged;
}


bool LayerGroup::_setLayersGrouped( bool grouped  ){
    bool operationPerformed = false;

    //First see if any of the children can do the operation.
    int dataCount = m_children.size();
    for ( int i = 0; i < dataCount; i++ ){
        bool childPerformed = m_children[i]->_setLayersGrouped( grouped );
        if ( childPerformed ){
            operationPerformed = true;
            break;
        }
    }

    //None of the children could do it so see if we can group the layers ourselves.
    if ( !operationPerformed ){
        //Go through the layers and get the selected ones.
        QList<int> selectIndices;
        for ( int i = 0; i < dataCount; i++ ){
            if ( m_children[i]->_isSelected() ){
                selectIndices.append(i);
            }
        }
        int selectedCount = selectIndices.size();
        if ( grouped ){
            if ( selectedCount >= 2 ){
                //Make a new group layer.
                Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
                LayerGroup* groupLayer = objMan->createObject<LayerGroup>();
                //Add all the selected layers to the group.
                for ( int i = 0; i < selectedCount; i++ ){
                    groupLayer->_addLayer( m_children[selectIndices[i]] );
                }
                //Remove all the selected ones from the list.
                for ( int i = selectedCount - 1; i >= 0; i-- ){
                    _removeData(i);
                }
                //Insert the group layer at the first selected index.
                std::shared_ptr<Layer> group( groupLayer );
                m_children.insert( selectIndices[0], group );
                QStringList selections;
                selections.append( groupLayer->_getId());
                group->_setSelected( selections );
                operationPerformed = true;
            }
        }
        else {
            //Split the selected layers.
            if ( selectedCount == 1 ){
                //Go through the selected layers, split them, and remove
                //the original.
                /*QMap<int, std::shared_ptr<Layer> > newLayers;
                for ( int i = selectedCount - 1; i >= 0; i-- ){
                    QList<std::shared_ptr<Layer> > splitLayers = m_datas[selectIndices[i]]->split();
                    if ( splitLayers.size() > 0 ){
                        newLayers[selectIndices[i]]= splitLayers;
                        _removeData( selectIndices[i] );
                    }
                }
                //Insert the new layers.
                QList<int> keys = newLayers.keys();
                int keyCount = keys.size();
                if ( keyCount > 0 ){
                    datasChanged = true;
                    for ( int i = 0; i < keyCount; i++ ){
                        int insertIndex = keys[i];
                        int layerCount = newLayers[keys[i]].size();
                        for ( int j = 0; j < layerCount; j++ ){
                            m_datas.insert( insertIndex + j, newLayers[keys[i]][j]);
                        }
                    }
                }
                else {
                    result = "At least one composite layer must be selected to ungroup.";
                }*/
            }
        }
        if ( operationPerformed ){
            //_saveState();

            //Refresh the view of the data.
            //_scheduleFrameReload( false );

            //Notify others there has been a change to the data.


            //emit dataChanged( this );
        }
    }
    return operationPerformed;
}

bool LayerGroup::_setMaskAlpha( const QString& id, int alphaAmount){
    bool changed = false;
    //Groups can't have a mask color, so we just ask the children to set it.
    for ( std::shared_ptr<Layer> layer : m_children ){
        bool layerChanged = layer->_setMaskAlpha( id, alphaAmount );
        if ( layerChanged ){
            changed = true;
            break;
        }
    }
    return changed;
}

bool LayerGroup::_setMaskColor( const QString& id, int redAmount,
        int greenAmount, int blueAmount ){
    bool changed = false;
    //Groups can't have a mask color, so we just ask the children to set it.
    for ( std::shared_ptr<Layer> layer : m_children ){
        bool layerChanged = layer->_setMaskColor( id, redAmount, greenAmount, blueAmount);
        if ( layerChanged ){
            changed = true;
            break;
        }
    }
    return changed;
}

void LayerGroup::_setPan( double imgX, double imgY ){
    for ( std::shared_ptr<Layer> node : m_children ){
        node -> _setPan( imgX, imgY );
    }
}

bool LayerGroup::_setSelected( const QStringList& names){
    bool stateChanged = Layer::_setSelected( names );
    if ( !stateChanged || names.size() == 0 ){
        for ( std::shared_ptr<Layer> layer : m_children ){
            bool layerChange = layer->_setSelected( names );
            if ( layerChange ){
                stateChanged = true;
            }
        }
    }
    return stateChanged;
}

std::vector< std::shared_ptr<ColorState> >  LayerGroup::_getSelectedColorStates(){
    std::vector< std::shared_ptr<ColorState> > colorStates;
    for ( std::shared_ptr<Layer> layer : m_children ){
        std::vector<std::shared_ptr<ColorState> > layerColorStates = layer->_getSelectedColorStates();
        int layerStateCount = layerColorStates.size();
        for ( int i = 0; i < layerStateCount; i++ ){
            colorStates.push_back( layerColorStates[i] );
        }
    }
    return colorStates;
}

void LayerGroup::_setMaskColorDefault(){
    int childCount = m_children.size();
    for ( int i = 0; i < childCount; i++ ){
        m_children[i]->_setMaskColorDefault();
    }
}


void LayerGroup::_setMaskAlphaDefault(){
    QString result;
    int childCount = m_children.size();
    for ( int i = 0; i < childCount; i++ ){
        m_children[i]->_setMaskAlphaDefault();
    }
}


bool LayerGroup::_setVisible( const QString& id, bool visible ){
    bool layerFound = Layer::_setVisible( id, visible );
    if ( !layerFound ){
        int dataCount = m_children.size();
        for ( int i = 0; i < dataCount; i++ ){
            bool layerChildFound  = m_children[i]->_setVisible( id, visible );
            if ( layerChildFound ){
                //layerIndex = i;
                layerFound = true;
                break;
            }
        }
    }
    return layerFound;
}

void LayerGroup::_setZoom( double zoomAmount){
    for ( std::shared_ptr<Layer> node : m_children ){
        node-> _setZoom( zoomAmount );
    }
}


void LayerGroup::_updateClips( std::shared_ptr<Carta::Lib::NdArray::RawViewInterface>& view,
        double minClipPercentile, double maxClipPercentile, const std::vector<int>& frames ){
    for ( std::shared_ptr<Layer> node : m_children ){
        node->_updateClips( view,  minClipPercentile, maxClipPercentile, frames );
    }
}

void LayerGroup::_viewReset(){
    for ( std::shared_ptr<Layer> node : m_children){
        node->_viewReset();
    }
}

void LayerGroup::_viewResize( const QSize& newSize ){
    m_drawSync->viewResize( newSize );
    for ( std::shared_ptr<Layer> node : m_children){
        node->_viewResize( newSize );
    }
}

void LayerGroup::_viewResizeFullSave(const QSize& outputSize){
    for ( std::shared_ptr<Layer> node : m_children ){
        node->_viewResizeFullSave(outputSize);
    }
}

LayerGroup::~LayerGroup() {


}
}
}
