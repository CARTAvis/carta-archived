#include "LayerGroup.h"
#include "DataSource.h"
#include "Data/Util.h"
#include "Data/Image/LayerCompositionModes.h"
#include "State/UtilState.h"

#include <QDebug>
#include <QDir>

using Carta::Lib::AxisInfo;


namespace Carta {

namespace Data {

const QString LayerGroup::CLASS_NAME = "LayerGroup";
const QString LayerGroup::COMPOSITION_MODE="mode";
const QString LayerGroup::GROUP = "Group";


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


LayerGroup::LayerGroup(const QString& path, const QString& id) :
    Layer( CLASS_NAME, path, id){
    _initializeState();
}


void LayerGroup::_addLayer( std::shared_ptr<Layer> layer ){
    m_children.append( layer );
    QString childId = _getId() + QDir::separator() + QString::number( m_children.size() - 1 );
    layer->_setId( childId );
}

void LayerGroup::_clearColorMap(){
    Layer::_clearColorMap();
    int childCount = m_children.size();
    for ( int i = 0; i < childCount; i++ ){
        m_children[i]->_clearColorMap();
    }
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


QPointF LayerGroup::_getCenter() const{
    QPointF center;
    if ( m_children.size() > 0 ){
        center = m_children[0]->_getCenter();
    }
    return center;;
}


QString LayerGroup::_getCompositionMode() const {
    return m_state.getValue<QString>( COMPOSITION_MODE );
}

QStringList LayerGroup::_getCoordinates( double x, double y,
        Carta::Lib::KnownSkyCS system, const std::vector<int>& frames ) const{
    QStringList coordStr;
    if ( m_children.size() > 0 ){
        coordStr = m_children[0]->_getCoordinates( x, y, system, frames );
    }
    return coordStr;
}

Carta::Lib::KnownSkyCS LayerGroup::_getCoordinateSystem() const {
    Carta::Lib::KnownSkyCS cs = Carta::Lib::KnownSkyCS::Unknown;
    if ( m_children.size() > 0){
        cs = m_children[0]->_getCoordinateSystem();
    }
    return cs;
}

QString LayerGroup::_getCursorText( int mouseX, int mouseY, const std::vector<int>& frames ){
    QString cursorText;
    if ( m_children.size() > 0 ){
        cursorText = m_children[0]->_getCursorText( mouseX, mouseY, frames );
    }
    return cursorText;

}

QString LayerGroup::_getDefaultName( const QString& id ) const {
    return GROUP + " "+id;
}

int LayerGroup::_getDimension( int coordIndex ) const {
    int dim = -1;
    if ( m_children.size() > 0 ){
        dim = m_children[0]->_getDimension( coordIndex );
    }
    return dim;
}


int LayerGroup::_getDimensions() const {
    int imageSize = 0;
    if ( m_children.size() > 0 ){
        imageSize = m_children[0]->_getDimensions();
    }
    return imageSize;
}



int LayerGroup::_getFrameCount( AxisInfo::KnownType type ) const {
    int frameCount = 1;
    if ( m_children.size() > 0 ){
        frameCount = m_children[0]->_getFrameCount( type );
    }
    return frameCount;
}

Carta::State::StateInterface LayerGroup::_getGridState() const {
    Carta::State::StateInterface gridState("");
    if ( m_children.size() > 0 ){
        gridState = m_children[0]->_getGridState();
    }
    return gridState;
}

std::shared_ptr<Carta::Lib::Image::ImageInterface> LayerGroup::_getImage(){
    std::shared_ptr<Carta::Lib::Image::ImageInterface> image(nullptr);
    if ( m_children.size() > 0 ){
        image = m_children[0]->_getImage();
    }
    return image;
}

std::shared_ptr<DataSource> LayerGroup::_getDataSource(){
    std::shared_ptr<DataSource> dSource( nullptr );
    if ( m_children.size() > 0 ){
        dSource = m_children[0]->_getDataSource();
    }
    return dSource;
}

QPointF LayerGroup::_getImagePt( QPointF screenPt, bool* valid ) const {
    QPointF imagePt;
    if ( m_children.size() > 0 ){
        imagePt = m_children[0]->_getImagePt( screenPt, valid );
    }
    else {
        *valid = false;
    }
    return imagePt;
}


bool LayerGroup::_getIntensity( int frameLow, int frameHigh, double percentile, double* intensity ) const {
    bool intensityFound = false;
    if ( m_children.size() > 0 ){
        intensityFound = m_children[0]->_getIntensity( frameLow, frameHigh, percentile, intensity );
    }
    return intensityFound;
}


QSize LayerGroup::_getOutputSize() const {
    QSize size;
    if ( m_children.size() > 0 ){
        size = m_children[0]-> _getOutputSize();
    }
    return size;
}

double LayerGroup::_getPercentile( int frameLow, int frameHigh, double intensity ) const {
    double percentile = 0;
    if ( m_children.size() > 0 ){
        percentile = m_children[0]->_getPercentile( frameLow, frameHigh, intensity );
    }
    return percentile;
}



QStringList LayerGroup::_getPixelCoordinates( double ra, double dec ) const{
    QStringList result("");
    if ( m_children.size() > 0 ){
        result = m_children[0]->_getPixelCoordinates( ra, dec );
    }
    return result;
}

QString LayerGroup::_getPixelUnits() const {
    QString units;
    if ( m_children.size() > 0 ){
        units = m_children[0]->_getPixelUnits();
    }
    return units;
}

QString LayerGroup::_getPixelValue( double x, double y, const std::vector<int>& frames ) const {
    QString pixelValue = "";
    if ( m_children.size() > 0 ){
        pixelValue = m_children[0]->_getPixelValue( x, y, frames );
    }
    return pixelValue;
}


QPointF LayerGroup::_getScreenPt( QPointF imagePt, bool* valid ) const {
    QPointF screenPt;
    if ( m_children.size() > 0 ){
        screenPt = m_children[0]->_getScreenPt( imagePt, valid );
    }
    else {
        *valid = false;
    }
    return screenPt;
}

QString LayerGroup::_getStateString() const{
    Carta::State::StateInterface copyState( m_state );
    int childCount = m_children.size();
    copyState.insertArray( "layerGroup", childCount );
    for ( int i = 0; i < childCount; i++ ){
        QString key = Carta::State::UtilState::getLookup( "layerGroup", i );
        copyState.setObject( key, m_children[i]->_getStateString());
    }
    QString stateStr = copyState.toString();
    return stateStr;
}


double LayerGroup::_getZoom() const {
    double zoom = DataSource::ZOOM_DEFAULT;
    if ( m_children.size() > 0 ){
        zoom = m_children[0]-> _getZoom();
    }
    return zoom;
}

void LayerGroup::_gridChanged( const Carta::State::StateInterface& state ){
    for ( std::shared_ptr<Layer> layer : m_children ){
        layer->_gridChanged( state );
    }
}

void LayerGroup::_initializeState(){
    QString defaultCompMode = m_compositionModes->getDefault();
    m_state.insertValue<QString>( COMPOSITION_MODE, defaultCompMode );
}


void LayerGroup::_load(vector<int> frames, bool recomputeClipsOnNewFrame,
        double minClipPercentile, double maxClipPercentile ){
    for ( std::shared_ptr<Layer> node : m_children ){
        node -> _load( frames, recomputeClipsOnNewFrame,
                minClipPercentile, maxClipPercentile );
    }
}


void LayerGroup::_render( const std::vector<int>& /*frames*/,
        const Carta::Lib::KnownSkyCS& /*cs*/, bool /*topOfStack*/ ){

}



void LayerGroup::_resetState( const Carta::State::StateInterface& restoreState ){
    Layer::_resetState( restoreState);
    m_state.setValue<QString>( COMPOSITION_MODE, restoreState.getValue<QString>(COMPOSITION_MODE) );
}


void LayerGroup::_resetZoom(){
    for ( std::shared_ptr<Layer> node : m_children ){
        node->_resetZoom();
    }
}


void LayerGroup::_resetPan(){
    for ( std::shared_ptr<Layer> node : m_children ){
        node ->_resetPan();
    }
}


bool LayerGroup::_setCompositionMode( const QString& compositionMode,
        QString& errorMsg ){
    bool stateChanged = false;
    QString actualCompMode;
    bool recognizedMode = m_compositionModes->isCompositionMode( compositionMode, actualCompMode );
    if ( recognizedMode ){
        QString oldMode = m_state.getValue<QString>( COMPOSITION_MODE );
        if ( oldMode != actualCompMode ){
            m_state.setValue<QString>( COMPOSITION_MODE, actualCompMode );
            bool colorSupport = m_compositionModes->isColorSupport( actualCompMode );
            if ( !colorSupport ){
                int childCount = m_children.size();
                for ( int i = 0; i < childCount; i++ ){
                    m_children[i]->_setMaskColorDefault();
                }
            }

            bool alphaSupport = m_compositionModes->isAlphaSupport( actualCompMode );

            if ( !alphaSupport ){
                int childCount = m_children.size();
                for ( int i = 0; i < childCount; i++ ){
                    m_children[i]->_setMaskAlphaDefault();
                }
            }
            stateChanged = true;
        }
    }
    else {
        errorMsg = "Unrecognized layer composition mode: "+compositionMode;
    }
    return stateChanged;
}

void LayerGroup::_setId( const QString& id ){
    QString oldId = _getId();
    Layer::_setId( id );
    QString baseId = _getId();
    if ( baseId != oldId ){
        int childCount = m_children.size();
        for ( int i = 0; i < childCount; i++ ){
            QString childId = baseId + QDir::separator() + QString::number(i);
            m_children[i]->_setId( childId );
        }
        //Change the default layer name if the user has not
        //customized it by setting it to a nondefault value.
        QString oldName = _getLayerName();
        if ( oldName.isEmpty() || oldName  == _getDefaultName( oldId ) ){
            m_state.setValue<QString>(LAYER_NAME, _getDefaultName( baseId) );
        }
    }
}



void LayerGroup::_setPan( double imgX, double imgY ){
    for ( std::shared_ptr<Layer> node : m_children ){
        node -> _setPan( imgX, imgY );
    }
}

bool LayerGroup::_setSelected( const QStringList& names ){
    bool stateChanged = Layer::_setSelected( names );
    if ( !stateChanged ){
        for ( std::shared_ptr<Layer> layer : m_children ){
            bool layerChange = layer->_setSelected( names );
            if ( layerChange ){
                stateChanged = true;
            }
        }
    }
    return stateChanged;
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

void LayerGroup::_viewResize( const QSize& newSize ){
    for ( std::shared_ptr<Layer> node : m_children){
        node->_viewResize( newSize );
    }
}


LayerGroup::~LayerGroup() {
}
}
}
