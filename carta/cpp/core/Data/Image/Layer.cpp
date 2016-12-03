#include "Layer.h"
#include "Grid/DataGrid.h"
#include "Contour/DataContours.h"
#include "Data/Util.h"
#include "Data/Colormap/ColorState.h"
#include "Data/DataLoader.h"
#include "Data/Image/LayerCompositionModes.h"
#include "Data/Image/Render/RenderRequest.h"
#include "State/UtilState.h"

#include <QDebug>

using Carta::Lib::AxisInfo;

namespace Carta {

namespace Data {

const QString Layer::CLASS_NAME = "Layer";
const QString Layer::GROUP = "group";
const QString Layer::LAYER = "layer";


LayerCompositionModes* Layer::m_compositionModes = nullptr;


Layer::Layer( const QString& className, const QString& path, const QString& id) :
    CartaObject( className, path, id){
    m_renderQueued = false;
    _initializeSingletons();
    _initializeState();
}

void Layer::_addLayer( std::shared_ptr<Layer> /*layer*/, int /*targetIndex*/ ){

}


void Layer::_clearChildren(){

}



void Layer::_clearColorMap(){
}


bool Layer::_closeData( const QString& /*id*/ ){
    return false;
}


void Layer::_colorChanged(){
    _updateColor();
}

QList<std::shared_ptr<Layer> > Layer::_getChildren(){
    QList<std::shared_ptr<Layer> > children;
    return children;
}


std::shared_ptr<ColorState> Layer::_getColorState(){
    return std::shared_ptr<ColorState>(nullptr);
}

QString Layer::_getCompositionMode() const {
    return LayerCompositionModes::NONE;
}


std::set<std::shared_ptr<DataContours>> Layer::_getContours() {
    std::set<std::shared_ptr<DataContours> > emptySet;
    return emptySet;
}


std::vector< std::shared_ptr<Carta::Lib::Image::ImageInterface> > Layer::_getImages(){
    std::vector<std::shared_ptr<Carta::Lib::Image::ImageInterface> > images;
    return images;
}

std::shared_ptr<Layer> Layer::_getLayer( const QString& /*name*/ ){
    std::shared_ptr<Layer> layer( nullptr );
    return layer;
}

std::vector<std::shared_ptr<Layer> > Layer::_getLayers(){
    std::vector<std::shared_ptr<Layer> >layers;
    return layers;
}

QString Layer::_getLayerId() const {
    return m_state.getValue<QString>(Util::ID);
}

QStringList Layer::_getLayerIds( ) const {
    QStringList idList( m_state.getValue<QString>( Util::ID));
    return idList;
}

QString Layer::_getLayerName() const {
    return m_state.getValue<QString>( Util::NAME );
}

float Layer::_getMaskAlpha() const {
    return 1.0f;
}

quint32 Layer::_getMaskColor() const {
    QRgb rgbCol = qRgba( 255,255,255,255);
    return rgbCol;
}


void Layer::_initializeSingletons( ){
    //Load the available color maps.
    if ( m_compositionModes == nullptr ){
        m_compositionModes = Util::findSingletonObject<LayerCompositionModes>();
    }
}


void Layer::_initializeState(){
    m_state.insertValue<bool>(Util::VISIBLE, true );
    m_state.insertValue<bool>(Util::SELECTED, false );
    QString idStr = getId();
    idStr = idStr.replace( "c", "");
    m_state.insertValue<QString>(Util::ID, idStr);
    m_state.insertValue<QString>( Util::NAME, "");
}

bool Layer::_isComposite() const {
    return false;
}

bool Layer::_isContourDraw() const {
    bool contourDraw = false;
    return contourDraw;
}

bool Layer::_isDescendant( const QString& id ) const {
    return _isMatch( id );
}

bool Layer::_isEmpty() const {
    return false;
}

bool Layer::_isLoadable( const std::vector<int>& /*frames*/ ) const {
	return false;
}

bool Layer::_isMatch( const QString& name ) const {
    bool matched = false;
    QString id = _getLayerId();
    if ( name == id ){
        matched = true;
    }
    return matched;
}

bool Layer::_isSelected() const {
    return m_state.getValue<bool>( Util::SELECTED );
}

bool Layer::_isSpectralAxis() const {
	return false;
}

bool Layer::_isVisible() const {
    return m_state.getValue<bool>(Util::VISIBLE);
}

void Layer::_render( const std::shared_ptr<RenderRequest>& request ){
    m_renderRequests.push( request );
    if ( !m_renderQueued ){
        _renderStart();
    }
}

void Layer::_renderDone(){
    // schedule a repaint with the connector
    m_renderQueued = false;

    if ( m_renderRequests.size() > 0 ){
        _renderStart();
    }
}


void Layer::_resetStateContours(const Carta::State::StateInterface& /*restoreState*/ ){

}

void Layer::_resetState( const Carta::State::StateInterface& restoreState ){
    m_state.setValue<bool>(Util::VISIBLE, restoreState.getValue<bool>(Util::VISIBLE) );
    m_state.setValue<bool>(Util::SELECTED, restoreState.getValue<bool>( Util::SELECTED) );
    QString layerName = restoreState.getValue<QString>(Util::NAME);
    QString shortName = layerName;
    if ( !layerName.startsWith( GROUP )){
        DataLoader* dLoader = Util::findSingletonObject<DataLoader>();
        shortName = dLoader->getShortName( layerName );
    }
    m_state.setValue<QString>(Util::ID, restoreState.getValue<QString>(Util::ID));
    m_state.setValue<QString>(Util::NAME, shortName);
}


QString Layer::_setFileName( const QString& /*fileName*/, bool* success ){
    QString result = "Incorrect layer for loading image files.";
    *success = false;
    return result;
}


bool Layer::_setCompositionMode( const QString& id, const QString& /*compositionMode*/,
        QString& errorMsg ){
    bool stateChanged = false;
    if ( id == _getLayerId() ){
        errorMsg = "Composition mode is not implemented the layer";
    }
    return stateChanged;
}


bool Layer::_setLayerName( const QString& id, const QString& name ){
    bool nameChanged = false;
    if ( id == _getLayerId() ){
        m_state.setValue<QString>( Util::NAME, name);
        nameChanged = true;
    }
    return nameChanged;
}

bool Layer::_setSelected( QStringList& names ){
    bool stateChanged = false;
    bool selected = false;
    QString layerId = _getLayerId();
    int layerIndex = names.indexOf( layerId );
    if ( layerIndex >= 0 ){
        selected = true;
        names.removeAt( layerIndex );
    }

    bool oldSelected = m_state.getValue<bool>(Util::SELECTED );
    if ( oldSelected != selected ){
        m_state.setValue<bool>( Util::SELECTED, selected );
        stateChanged = true;
    }
    return stateChanged;
}

void Layer::_setSupportAlpha( bool /*supportAlpha*/ ){

}

void Layer::_setSupportColor( bool /*supportColor*/ ){

}


bool Layer::_setVisible( const QString& id, bool visible ){
    bool foundLayer = false;
    if ( id == _getLayerId() ){
        bool oldVisible = m_state.getValue<bool>(Util::VISIBLE);
        if ( visible != oldVisible ){
            m_state.setValue<bool>( Util::VISIBLE, visible );
        }
        foundLayer = true;
    }
    return foundLayer;
}

void Layer::_updateColor(){
    emit colorStateChanged();
}


Layer::~Layer() {
}
}
}
