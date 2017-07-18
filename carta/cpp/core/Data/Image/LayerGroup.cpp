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
#include <cmath>

using Carta::Lib::AxisInfo;


namespace Carta {

namespace Data {

const QString LayerGroup::CLASS_NAME = "LayerGroup";
const QString LayerGroup::COMPOSITION_MODE="mode";

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
    connect( m_drawSync.get(), SIGNAL(done(QImage,Carta::Lib::VectorGraphics::VGList)),
                            this, SLOT(_renderingDone(QImage,Carta::Lib::VectorGraphics::VGList)));
}

void LayerGroup::_addContourSet( std::shared_ptr<DataContours> contourSet){
    int childCount = m_children.size();
    for ( int i = 0; i < childCount; i++ ){
        if ( m_children[i]->_isSelected() ){
            m_children[i]->_addContourSet( contourSet );
        }
    }
}

QString LayerGroup::_addData(const QString& fileName, bool* success, int* stackIndex ) {
    QString result;
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    LayerData* targetSource = objMan->createObject<LayerData>();
    connect( targetSource, SIGNAL(contourSetAdded(Layer*,const QString&)),
            this, SIGNAL(contourSetAdded(Layer*, const QString&)));
    connect( targetSource, SIGNAL(contourSetRemoved(const QString&)),
            this, SIGNAL(contourSetRemoved(const QString&)));
    connect( targetSource, SIGNAL(colorStateChanged()), this, SIGNAL(colorStateChanged() ));
    result = targetSource->_setFileName(fileName, success );
    //If we are making a new layer, see if there is a selected group.  If so,
    //add to the group.  If not, add to this group.
    if ( *success ){
        _setColorSupport( targetSource );
        std::shared_ptr<Layer> selectedGroup = _getSelectedGroup();
        if (selectedGroup ){
            selectedGroup->_addLayer( std::shared_ptr<Layer>(targetSource) );
        }
        else {
            m_children.append( std::shared_ptr<Layer>(targetSource) );
            *stackIndex = m_children.size() - 1;
        }
    }
    else {
        delete targetSource;
    }
    return result;
}


bool LayerGroup::_addGroup(){
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    LayerGroup* targetSource = objMan->createObject<LayerGroup>();
    connect( targetSource, SIGNAL(removeLayer(Layer*)),
            this, SLOT( _removeLayer( Layer*)));
    m_children.append( std::shared_ptr<Layer>(targetSource));
    return true;
}


void LayerGroup::_addLayer( std::shared_ptr<Layer> layer, int targetIndex ){
    int tIndex = targetIndex;
    int childCount = m_children.size();
    if ( tIndex == -1 || tIndex == childCount){
        m_children.append( layer );
        tIndex = m_children.size() - 1;
    }
    else if (tIndex >= 0 && tIndex < childCount){
        m_children.insert( tIndex, layer );
    }
    if ( tIndex >= 0 && tIndex < m_children.size() ){
        _setColorSupport( m_children[ tIndex ].get());
        _assignColor( tIndex );
    }
}

void LayerGroup::_assignColor( int index ){

    QString compMode = _getCompositionMode();
    bool colorSupport = m_compositionModes->isColorSupport( compMode );
    if ( colorSupport ){
        int childrenCount = m_children.size();
        if ( index >= 0 && index < childrenCount ){
            bool redUsed = false;
            bool blueUsed = false;
            bool greenUsed = false;
            for ( int i = 0; i < index; i++ ){
                QColor color = m_children[i]->_getMaskColor();
                int redAmount = color.red();
                int greenAmount = color.green();
                int blueAmount = color.blue();
                if ( redAmount == 255 && greenAmount == 0 && blueAmount == 0){
                    redUsed = true;
                }
                else if ( redAmount == 0 && greenAmount == 255 && blueAmount == 0 ){
                    greenUsed = true;
                }
                else if ( redAmount == 0 && greenAmount == 0 && blueAmount == 255 ){
                    blueUsed = true;
                }

            }
            QString id = m_children[index]->_getLayerId();
            if ( !redUsed ){
                m_children[index]->_setMaskColor( id, 255, 0, 0 );
            }
            else if ( !greenUsed ){
                m_children[index]->_setMaskColor( id, 0, 255, 0 );
            }
            else if ( !blueUsed ){
                m_children[index]->_setMaskColor( id, 0, 0, 255 );
            }
        }
    }
    else {
        m_children[index]->_setMaskColorDefault();
    }
}


void LayerGroup::_clearColorMap(){
    Layer::_clearColorMap();
    int childCount = m_children.size();
    for ( int i = 0; i < childCount; i++ ){
        m_children[i]->_clearColorMap();
    }
}

void LayerGroup::_clearChildren(){
    m_children.clear();
}

void LayerGroup::_clearData(){
    int childCount = m_children.size();
    for ( int i = childCount - 1; i>= 0; i-- ){
        _removeData( i );
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

std::vector<Carta::Lib::AxisInfo> LayerGroup::_getAxisInfos() const {
    std::vector<Carta::Lib::AxisInfo> axisInfos;
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        axisInfos = m_children[dataIndex]->_getAxisInfos();
    }
    return axisInfos;
}

QPointF LayerGroup::_getCenterPixel() const {
    int dataIndex = _getIndexCurrent();
    QPointF center = QPointF( nan(""), nan("") );
    if ( dataIndex >= 0 ) {
        center = m_children[dataIndex]->_getCenterPixel();
    }
    return center;
}

QList<std::shared_ptr<Layer> > LayerGroup::_getChildren(){
    return m_children;
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

QString LayerGroup::_getCursorText(bool isAutoClip, double minPercent, double maxPercent, int mouseX, int mouseY,
        const std::vector<int>& frames, const QSize& outputSize) {
    QString cursorText;
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        cursorText = m_children[dataIndex]->_getCursorText(isAutoClip, minPercent, maxPercent, mouseX, mouseY,
                frames, outputSize);
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



QSize LayerGroup::_getDisplaySize() const {
    QSize size;
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        size = m_children[dataIndex]->_getDisplaySize();
    }
    return size;
}


std::vector<int> LayerGroup::_getImageDimensions( ) const {
    std::vector<int> result;
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        result = m_children[dataIndex]->_getImageDimensions();
    }
    return result;
}


QPointF LayerGroup::_getImagePt( const QPointF& screenPt, const QSize& outputSize, bool* valid ) const {
    QPointF imagePt;
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        imagePt = m_children[dataIndex]->_getImagePt( screenPt, outputSize, valid );
    }
    else {
        *valid = false;
    }
    return imagePt;
}

QPointF LayerGroup::_getContextPt( const QPointF& screenPt, const QSize& outputSize, bool* valid ) const {
	QPointF contextPt;
	int dataIndex = _getIndexCurrent();
	if ( dataIndex >= 0 ){
		contextPt = m_children[dataIndex]->_getContextPt( screenPt, outputSize, valid );
	}
	else {
		*valid = false;
	}
	return contextPt;
}

std::vector< std::shared_ptr<Carta::Lib::Image::ImageInterface> > LayerGroup::_getImages(){
    std::vector<std::shared_ptr<Carta::Lib::Image::ImageInterface> > images;
    int dataCount = m_children.size();
    //Return the images in stack order.
    int startIndex = _getIndexCurrent();
    for ( int i = 0; i < dataCount; i++ ){
        int dIndex = (startIndex + i) % dataCount;
        if ( dIndex > -1 && m_children[dIndex]->_isVisible() ){
            images.push_back( m_children[dIndex]->_getImage());
        }
    }
    return images;
}


int LayerGroup::_getIndexCurrent( ) const {
    int dataIndex = -1;
    //The current index should be the first selected one.
    int childCount = m_children.size();
    for ( int i = 0; i < childCount; i++ ){
        if ( m_children[i]->_isSelected()){
            dataIndex = i;
            break;
        }
    }
    //Just choose the top one if nothing is selected
    if ( dataIndex == -1 && childCount > 0 ){
        dataIndex = 0;
    }
    return dataIndex;
}

QRectF LayerGroup::_getInputRect( const QSize& size ) const {
    QRectF rect(0,0,0,0);
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        rect = m_children[dataIndex]->_getInputRect( size );
    }
    return rect;
}

std::vector<std::pair<int,double> > LayerGroup::_getIntensity( int frameLow, int frameHigh,
        const std::vector<double>& percentiles, int stokeFrame ) const{
    std::vector<std::pair<int,double> > results;
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        results = m_children[dataIndex]->_getIntensity( frameLow, frameHigh,
                percentiles, stokeFrame );
    }
    return results;
}

std::shared_ptr<Layer> LayerGroup::_getLayer( const QString& name ){
    std::shared_ptr<Layer> layer(nullptr);
    int dataIndex = -1;
    //Use the current layer
    if ( name.isEmpty() || name.trimmed().length() == 0 ){
    	dataIndex = _getIndexCurrent();
    }
    else {
    	//See if one of the children has a matching id.
    	int childCount = m_children.size();
    	for ( int i = 0; i < childCount; i++ ){
    		if ( !m_children[i]->_isComposite() ){
    			if ( m_children[i]->_getLayerName() == name ){
    				dataIndex = i;
    				break;
    			}
    		}
    	}
    }
    if ( dataIndex >= 0 ){
    	layer = m_children[dataIndex];
    }
    //See if any of the composite children have matching layers
    else {
    	int childCount = m_children.size();
    	for ( int i = 0; i < childCount; i++ ){
    		if ( m_children[i]->_isComposite() ){
    			layer = m_children[i]->_getLayer( name );
    			if ( layer ){
    				break;
    			}
    		}
        }

    }

    return layer;
}


std::vector<std::shared_ptr<Layer> > LayerGroup::_getLayers(){
    std::vector<std::shared_ptr<Layer> > layers;
    int dataCount = m_children.size();
    for ( int i = 0; i < dataCount; i++ ){
        if ( !m_children[i]->_isComposite() ){
            layers.push_back( m_children[i] );
        }
        else {
            std::vector<std::shared_ptr<Layer> > childLayers = m_children[i]->_getLayers();
            int childCount = childLayers.size();
            for ( int j = 0; j < childCount; j++ ){
                layers.push_back( childLayers[j] );
            }
        }
    }
    return layers;
}

QStringList LayerGroup::_getLayerIds( ) const {
    QStringList idList = Layer::_getLayerIds();
    for ( std::shared_ptr<Layer> layer : m_children ){
        idList.append( layer->_getLayerIds());
    }
    return idList;
}


double LayerGroup::_getPercentile( int frameLow, int frameHigh, double intensity ) const {
    double percentile = 0;
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        percentile = m_children[dataIndex]->_getPercentile( frameLow, frameHigh, intensity );
    }
    return percentile;
}



QPointF LayerGroup::_getPixelCoordinates( double ra, double dec, bool* valid ) const{
    QPointF result;
    int dataIndex = _getIndexCurrent();
    *valid = false;
    if ( dataIndex >= 0 ){
        result = m_children[dataIndex]->_getPixelCoordinates( ra, dec, valid );
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
    QString pixelValue("");
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        pixelValue = m_children[dataIndex]->_getPixelValue( x, y, frames );
    }
    return pixelValue;
}

Carta::Lib::VectorGraphics::VGList LayerGroup::_getRegionGraphics() const {
	Carta::Lib::VectorGraphics::VGList vgList;
	int dataIndex = _getIndexCurrent();
	if ( dataIndex >= 0 ){
		vgList = m_children[dataIndex]->_getRegionGraphics();
	}
	return vgList;
}

QSize LayerGroup::_getSaveSize( const QSize& outputSize,  Qt::AspectRatioMode aspectMode) const {
    QSize saveSize = outputSize;
    int dataIndex = _getIndexCurrent();
    if ( dataIndex >= 0 ){
        saveSize = m_children[dataIndex]->_getSaveSize( outputSize, aspectMode );
    }
    return saveSize;
}

std::pair<double,QString> LayerGroup::_getRestFrequency() const {
	std::pair<double,QString> restFreq( -1, "");
	int dataIndex = _getIndexCurrent();
	if ( dataIndex >= 0 ){
		restFreq = m_children[dataIndex]->_getRestFrequency();
	}
	return restFreq;
}

std::shared_ptr<Layer> LayerGroup::_getSelectedGroup() {
    std::shared_ptr<Layer> group( nullptr );
    int childCount = m_children.size();
    for ( int i = 0; i < childCount; i++ ){
        if ( m_children[i]->_isComposite() ){
            //See if the child is selected.  If so, it is the one
            //we are looking for.
            if ( m_children[i]->_isSelected() ){
                group = m_children[i];
                break;
            }
            else {
                //See if the child has descendants that are groups and
                //selected.
                LayerGroup* childGroup = (LayerGroup*)(m_children[i].get());
                group = childGroup->_getSelectedGroup();
                if ( group ){
                    break;
                }
            }
        }
    }
    return group;
}

int LayerGroup::_getStackSize() const {
    return m_children.size();
}

int LayerGroup::_getStackSizeVisible() const {
    int visibleCount = 0;
    int imageCount = m_children.size();
    for ( int i = 0; i < imageCount; i++ ){
        if ( m_children[i]->_isVisible() && !m_children[i]->_isEmpty()){
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


QPointF LayerGroup::_getWorldCoordinates( double pixelX, double pixelY,
        Carta::Lib::KnownSkyCS coordSys, bool* valid ) const{
    QPointF result;
    int dataIndex = _getIndexCurrent();
    *valid = false;
    if ( dataIndex >= 0 ){
        result = m_children[dataIndex]->_getWorldCoordinates( pixelX, pixelY, coordSys, valid );
    }
    return result;
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
    m_state.setValue<QString>( Util::NAME, Layer::GROUP+_getLayerId());
}

bool LayerGroup::_isComposite() const {
    return true;
}

bool LayerGroup::_isDescendant( const QString& id ) const {
    bool descendant = _isMatch( id );
    if ( !descendant ){
        int childCount = m_children.size();
        for ( int i = 0; i < childCount; i++ ){
            if ( m_children[i]->_isDescendant( id )){
                descendant = true;
                break;
            }
        }
    }
    return descendant;
}

bool LayerGroup::_isEmpty() const {
    bool empty = true;
    int childCount = m_children.size();
    for ( int i = 0; i < childCount; i++ ){
        if ( !m_children[i]->_isEmpty() ){
            empty = false;
            break;
        }
    }
    return empty;
}

bool LayerGroup::_isLoadable( const std::vector<int>& frames ) const {
	//A group is loadable if there is one loadable image in the group.
	bool loadable = false;
	int childCount = m_children.size();
	for ( int i = 0; i < childCount; i++ ){
		if ( m_children[i]->_isLoadable( frames ) ){
			loadable = true;
			break;
		}
	}
	return loadable;
}

bool LayerGroup::_isSpectralAxis() const {
	bool spectralAxis = false;

	//All children must have a spectral axis if the group is to have one.
	int childCount = m_children.size();
	if ( childCount > 0 ){
		spectralAxis = true;
		for ( int i = 0; i < childCount; i++ ){
			spectralAxis = m_children[i]->_isSpectralAxis();
			if ( !spectralAxis ){
				break;
			}
		}
	}
	return spectralAxis;
}


void LayerGroup::_removeData( int index ){
    int childCount = m_children.size();
    if ( 0 <= index && index < childCount ){
        disconnect( m_children[index].get());
        QString id = m_children[index]->getId();
        Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
        objMan->removeObject( id );
        m_children.removeAt( index );
    }
}


void LayerGroup::_removeContourSet( std::shared_ptr<DataContours> contourSet ){
    for ( std::shared_ptr<Layer> layer : m_children ){
        layer->_removeContourSet( contourSet );
    }
}

void LayerGroup::_removeLayer( Layer* group ){
    if ( group ){
        QString id = group->getId();
        //Find the index of the group to remove.
        int childCount = m_children.size();

        int targetIndex = -1;
        for ( int i = 0; i < childCount; i++ ){
            if ( m_children[i]->getId() == id ){
                targetIndex = i;
                break;

            }
        }
        if ( targetIndex >= 0 ){
            QList<std::shared_ptr<Layer> > children = group->_getChildren();
            group->_clearChildren();
            _removeData( targetIndex );
            int addCount = children.size();
            for ( int i = 0; i < addCount; i++ ){
                int addIndex = targetIndex + i;
                _addLayer( children[i], addIndex );
            }
        }
    }
}


void LayerGroup::_renderStart( ){
    if ( m_renderRequests.size() > 0 ){
        m_renderQueued = true;
        std::shared_ptr<RenderRequest> request = m_renderRequests.pop();
        //Only load the layers which have the required frames.
        std::vector<int> frames = request->getFrames();
        QList<std::shared_ptr<Layer> > loadables;
        int childCount = m_children.size();
        for ( int i = 0; i < childCount; i++ ){
        	if ( m_children[i]->_isLoadable( frames ) ){
        		loadables.append( m_children[i] );
        	}
        }
        m_drawSync->setLayers( loadables );
        m_drawSync->setCombineMode( _getCompositionMode() );
        int topIndex = -1;
        if ( request->isStackTop()){
            topIndex = _getIndexCurrent();
        }
        request->setTopIndex( topIndex );
        m_drawSync->render( request );
    }
}


void LayerGroup ::_renderingDone( QImage image, Carta::Lib::VectorGraphics::VGList graphics ){
    std::shared_ptr<RenderResponse> response( new RenderResponse(image, graphics, _getLayerId()));
    emit renderingDone( response );
}


void LayerGroup::_resetState( const Carta::State::StateInterface& restoreState ){
    Layer::_resetState( restoreState);
    m_state.setValue<QString>( COMPOSITION_MODE, restoreState.getValue<QString>(COMPOSITION_MODE) );

    //State of children
    int dataCount = restoreState.getArraySize(LAYERS);
    _clearData();
    for ( int i = 0; i < dataCount; i++ ){
        QString dataLookup = Carta::State::UtilState::getLookup( LAYERS, i );
        QString typeLookup = Carta::State::UtilState::getLookup( dataLookup, Util::TYPE );
        QString layerType = restoreState.getValue<QString>( typeLookup );
        int dataIndex = -1;
        if ( layerType == LayerGroup::CLASS_NAME ){
            _addGroup();
            dataIndex = m_children.size()-1;
        }
        else {
            QString fileLookup = Carta::State::UtilState::getLookup( dataLookup, Util::NAME);
            QString fileName = restoreState.getValue<QString>( fileLookup );
            if ( !fileName.isEmpty()){
                bool success = false;
                _addData( fileName, &success, &dataIndex );
            }
        }
        int childCount = m_children.size();
        if ( dataIndex >= 0 && dataIndex < childCount){
            QString dataStateStr = restoreState.toString( dataLookup );
            Carta::State::StateInterface childState( "" );
            childState.setState( dataStateStr );
            m_children[dataIndex]->_resetState( childState );
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


void LayerGroup::_setColorSupport( Layer* layer ){
    QString compMode = _getCompositionMode();
    bool colorSupport = m_compositionModes->isColorSupport( compMode );
    bool alphaSupport = m_compositionModes->isAlphaSupport( compMode );
    layer->_setSupportColor( colorSupport );
    layer->_setSupportAlpha( alphaSupport );
}

bool LayerGroup::_setCompositionMode( const QString& id, const QString& compositionMode,
        QString& errorMsg ){
    bool stateChanged = false;
    if ( id == _getLayerId() ){
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


bool LayerGroup::_setLayersGrouped( bool grouped, const QSize& clientSize  ){
    bool operationPerformed = false;
    int dataCount = m_children.size();
    if ( !grouped ){
        //First see if any of the children can do the operation.
        //For now, it only makes sense to allow groups one deep.
        for ( int i = 0; i < dataCount; i++ ){
            bool childPerformed = m_children[i]->_setLayersGrouped( grouped, clientSize );
            if ( childPerformed ){
                operationPerformed = true;
                break;
            }
        }
    }

    //None of the children could do it so see if we can group the layers ourselves.
    if ( !operationPerformed ){
        //Go through the layers and get the selected ones.
        QList<int> selectIndices;
        for ( int i = 0; i < dataCount; i++ ){
            if ( m_children[i]->_isSelected() && !m_children[i]->_isComposite()){
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
                    disconnect( m_children[selectIndices[i]].get());
                    m_children.removeAt( selectIndices[i] );
                }
                //Insert the group layer at the first selected index.
                m_children.insert( selectIndices[0], std::shared_ptr<Layer>(groupLayer) );
                connect( groupLayer, SIGNAL(removeLayer(Layer*)),
                            this, SLOT( _removeLayer( Layer*)));
                QStringList selections;
                selections.append( groupLayer->_getLayerId());
                groupLayer->_setViewSize( clientSize );
                groupLayer->_setSelected( selections );
                operationPerformed = true;
            }
        }
        else {
            //Split the selected layers.
            if ( _isSelected() ){
                emit removeLayer( this );
                operationPerformed = true;
            }
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

bool LayerGroup::_setLayerName( const QString& id, const QString& name ){
    bool nameSet = Layer::_setLayerName( id, name );
    if ( !nameSet ){
        int childCount = m_children.size();
        for ( int i = 0; i < childCount; i++ ){
            nameSet = m_children[i]->_setLayerName( id, name );
            if ( nameSet ){
                break;
            }
        }
    }
    return nameSet;
}

void LayerGroup::_setPan( double imgX, double imgY ){
    for ( std::shared_ptr<Layer> node : m_children ){
        node -> _setPan( imgX, imgY );
    }
}

void LayerGroup::_setRegionGraphics( const Carta::Lib::VectorGraphics::VGList& regionVGList){
	//Only the top layer needs them, but we store in all layers.
	for( std::shared_ptr<Layer> layer : m_children ){
		layer->_setRegionGraphics( regionVGList );
	}
}

bool LayerGroup::_setSelected( QStringList& names){
    bool stateChanged = Layer::_setSelected( names );
    for ( std::shared_ptr<Layer> layer : m_children ){
        bool layerChange = layer->_setSelected( names );
        if ( layerChange ){
            stateChanged = true;
        }
    }
    return stateChanged;
}

std::vector< std::shared_ptr<ColorState> >  LayerGroup::_getSelectedColorStates( bool global ){
    std::vector< std::shared_ptr<ColorState> > colorStates;
    int childCount = m_children.size();
    int currentIndex = _getIndexCurrent();
    for ( int j = 0; j < childCount; j++  ){
        int index = (currentIndex + j) % childCount;
        std::vector<std::shared_ptr<ColorState> > layerColorStates = m_children[index]->_getSelectedColorStates( global );
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

void LayerGroup::_setViewSize( const QSize& size ){
	if ( m_drawSync ){
		m_drawSync->viewResize( size );
	}
}

bool LayerGroup::_setVisible( const QString& id, bool visible ){
    bool layerFound = Layer::_setVisible( id, visible );
    if ( !layerFound ){
        int dataCount = m_children.size();
        for ( int i = 0; i < dataCount; i++ ){
            bool layerChildFound  = m_children[i]->_setVisible( id, visible );
            if ( layerChildFound ){
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


LayerGroup::~LayerGroup() {
    _clearData();

}
}
}
