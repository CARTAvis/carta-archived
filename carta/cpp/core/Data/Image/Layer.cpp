#include "Layer.h"
#include "Grid/DataGrid.h"
#include "Contour/DataContours.h"
#include "Data/Util.h"
#include "Data/Colormap/ColorState.h"
#include "Data/Image/LayerCompositionModes.h"
#include "State/UtilState.h"

#include <QDebug>

using Carta::Lib::AxisInfo;

namespace Carta {

namespace Data {

const QString Layer::CLASS_NAME = "Layer";
const QString Layer::GROUP = "group";
const QString Layer::LAYER = "layer";
const QString Layer::SELECTED = "selected";
const QString Layer::LAYER_NAME = "name";


LayerCompositionModes* Layer::m_compositionModes = nullptr;


Layer::Layer( const QString& className, const QString& path, const QString& id) :
    CartaObject( className, path, id){
    _initializeSingletons();
    _initializeState();
}


void Layer::_clearColorMap(){
}


bool Layer::_closeData( const QString& /*id*/ ){
    return false;
}


void Layer::_colorChanged(){
    _updateColor();
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


QString Layer::_getId() const {
    return m_state.getValue<QString>( Util::ID );
}

std::vector< std::shared_ptr<Carta::Lib::Image::ImageInterface> > Layer::_getImages(){
    std::vector<std::shared_ptr<Carta::Lib::Image::ImageInterface> > images;
    return images;
}

QString Layer::_getLayerName() const {
    return m_state.getValue<QString>( LAYER_NAME );
}

float Layer::_getMaskAlpha() const {
    return 1.0f;
}

quint32 Layer::_getMaskColor() const {
    QRgb rgbCol = qRgba( 255,255,255,255);
    return rgbCol;
}


QImage Layer::_getQImage() const {
    return m_qimage;
}


Carta::Lib::VectorGraphics::VGList Layer::_getVectorGraphics(){
    return m_vectorGraphics;
}


void Layer::_initializeSingletons( ){
    //Load the available color maps.
    if ( m_compositionModes == nullptr ){
        m_compositionModes = Util::findSingletonObject<LayerCompositionModes>();
    }
}


void Layer::_initializeState(){
    m_state.insertValue<bool>(Util::VISIBLE, true );
    m_state.insertValue<bool>(SELECTED, false );
    m_state.insertValue<QString>( LAYER_NAME, "");
    m_state.insertValue<QString>(Util::ID, "");
}

bool Layer::_isContourDraw() const {
    bool contourDraw = false;
    return contourDraw;
}


bool Layer::_isSelected() const {
    return m_state.getValue<bool>( SELECTED );
}


bool Layer::_isVisible() const {
    return m_state.getValue<bool>(Util::VISIBLE);
}


bool Layer::_isMatch( const QString& name ) const {
    bool matched = false;
    QString id = _getId();
    if ( name == id ){
        matched = true;
    }
    return matched;
}


void Layer::_resetState( const QString& stateStr ){
    Carta::State::StateInterface restoreState("");
    restoreState.setState( stateStr );


    _resetState( restoreState );
}

void Layer::_resetStateContours(const Carta::State::StateInterface& /*restoreState*/ ){

}

void Layer::_resetState( const Carta::State::StateInterface& restoreState ){
    m_state.setValue<bool>(Util::VISIBLE, restoreState.getValue<bool>(Util::VISIBLE) );
    m_state.setValue<bool>(SELECTED, restoreState.getValue<bool>(SELECTED) );
    m_state.setValue<QString>( Util::ID, restoreState.getValue<QString>(Util::ID));
    m_state.setValue<QString>(LAYER_NAME, restoreState.getValue<QString>(LAYER_NAME));
}


bool Layer::_setFileName( const QString& /*fileName*/ ){
    return false;
}


bool Layer::_setCompositionMode( const QString& id, const QString& /*compositionMode*/,
        QString& errorMsg ){
    bool stateChanged = false;
    if ( id == _getId() ){
        errorMsg = "Composition mode is not implemented the layer";
    }
    return stateChanged;
}

void Layer::_setId( const QString& id ){
    QString oldId = m_state.getValue<QString>(Util::ID);
    if ( oldId != id ){
        m_state.setValue<QString>(Util::ID, id );
    }
}


QString Layer::_setImageOrder( const QString& groupId, const std::vector<int>& /*indices*/ ){
    QString result;
    if ( groupId == _getId() ){
        result = "The image order of a single image layer cannot be set.";
    }
    return result;
}



bool Layer::_setSelected( const QStringList& names ){
    bool stateChanged = false;
    bool selected = false;
    if ( names.contains( _getId())){
        selected = true;
    }

    bool oldSelected = m_state.getValue<bool>(SELECTED );
    if ( oldSelected != selected ){
        m_state.setValue<bool>( SELECTED, selected );
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
    if ( id == _getId() ){
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
