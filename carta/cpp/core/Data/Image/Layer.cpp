#include "Layer.h"
#include "Grid/DataGrid.h"
#include "Contour/DataContours.h"
#include "Data/Preferences/PreferencesSave.h"
#include "Data/Util.h"
#include "Data/Colormap/ColorState.h"
#include "Data/Image/LayerCompositionModes.h"
#include "State/UtilState.h"
#include "../../ImageSaveService.h"


#include <QDebug>

using Carta::Lib::AxisInfo;
//using Carta::Lib::AxisDisplayInfo;

namespace Carta {

namespace Data {

const QString Layer::CLASS_NAME = "Layer";

const QString Layer::GROUP = "group";
const QString Layer::LAYER = "layer";

const QString Layer::SELECTED = "selected";
const QString Layer::LAYER_ID = "id";
const QString Layer::LAYER_NAME = "name";


LayerCompositionModes* Layer::m_compositionModes = nullptr;



Layer::Layer( const QString& className, const QString& path, const QString& id) :
    CartaObject( className, path, id),
    m_stateColor( nullptr ){
    _initializeSingletons();
    _initializeState();

}

void Layer::_addContourSet( std::shared_ptr<DataContours> /*contour*/ ){

}

void Layer::_clearColorMap(){
    if ( m_stateColor ){
       disconnect( m_stateColor.get() );
       Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
       objMan->removeObject( m_stateColor->getId() );
    }
}


void Layer::_colorChanged(){
    _updateColor();
}


Carta::Lib::AxisInfo::KnownType Layer::_getAxisType( int /*index*/ ) const {
    AxisInfo::KnownType type = AxisInfo::KnownType::OTHER;
    return type;
}

AxisInfo::KnownType Layer::_getAxisXType() const {
    AxisInfo::KnownType axisType = AxisInfo::KnownType::OTHER;
    return axisType;
}

AxisInfo::KnownType Layer::_getAxisYType() const {
    AxisInfo::KnownType axisType = AxisInfo::KnownType::OTHER;
    return axisType;
}

std::vector<AxisInfo::KnownType> Layer::_getAxisZTypes() const {
    std::vector<AxisInfo::KnownType> axisTypes;
    return axisTypes;
}

std::vector<AxisInfo::KnownType> Layer::_getAxisTypes() const {
    std::vector<AxisInfo::KnownType> axisTypes;
    return axisTypes;
}


std::shared_ptr<ColorState> Layer::_getColorState(){
    return m_stateColor;
}

QString Layer::_getCompositionMode() const {
    return LayerCompositionModes::NONE;
}

std::shared_ptr<DataContours> Layer::_getContour( const QString& /*name*/ ){

    std::shared_ptr<DataContours> contourSet( nullptr );
    return contourSet;
}

std::set<std::shared_ptr<DataContours>> Layer::_getContours() {
    std::set<std::shared_ptr<DataContours> > emptySet;
    return emptySet;
}



QString Layer::_getId() const {
    return m_state.getValue<QString>( LAYER_ID );
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
    m_state.insertValue<QString>(LAYER_ID, "");
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



void Layer::_removeContourSet( std::shared_ptr<DataContours> /*contourSet*/ ){
}


void Layer::_resetState( const QString& stateStr ){
    Carta::State::StateInterface restoreState("");
    restoreState.setState( stateStr );

    //Restore the grid
    QString gridStr = restoreState.toString( DataGrid::GRID );
    Carta::State::StateInterface gridState( "" );
    gridState.setState( gridStr );
    _gridChanged( gridState );

    _resetStateContours( restoreState );
    _resetState( restoreState );
}

void Layer::_resetStateContours(const Carta::State::StateInterface& /*restoreState*/ ){

}

void Layer::_resetState( const Carta::State::StateInterface& restoreState ){
    m_state.setValue<bool>(Util::VISIBLE, restoreState.getValue<bool>(Util::VISIBLE) );
    m_state.setValue<bool>(SELECTED, restoreState.getValue<bool>(SELECTED) );
    m_state.setValue<QString>( LAYER_ID, restoreState.getValue<QString>(LAYER_ID));
    m_state.setValue<QString>(LAYER_NAME, restoreState.getValue<QString>(LAYER_NAME));
}



QString Layer::_saveImage( const QString& /*saveName*/, double /*scale*/,
        const std::vector<int>& /*frames*/ ){
    QString result;
   /* if ( m_dataSource ){

        std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> pipeline =
                m_dataSource->_getPipeline();
        m_saveService = new Carta::Core::ImageSaveService::ImageSaveService( saveName,
               pipeline );

        std::shared_ptr<Carta::Lib::NdArray::RawViewInterface> view( m_dataSource->_getRawData( frames ));
        if ( view != nullptr ){
            QString viewId = m_dataSource->_getViewIdCurrent( frames );
            m_saveService->setInputView( view, viewId );
            PreferencesSave* prefSave = Util::findSingletonObject<PreferencesSave>();
            int width = prefSave->getWidth();
            int height = prefSave->getHeight();
            Qt::AspectRatioMode aspectRatioMode = prefSave->getAspectRatioMode();
            m_saveService->setOutputSize( QSize( width, height ) );
            m_saveService->setAspectRatioMode( aspectRatioMode );
            std::pair<int,int> displayDims = m_dataSource->_getDisplayDims();
            m_saveService->setDisplayShape( displayDims.first, displayDims.second );

            m_saveService->setZoom( scale );

            connect( m_saveService, & Carta::Core::ImageSaveService::ImageSaveService::saveImageResult,
                    this, & Layer::_saveImageResultCB );

            bool saveStarted = m_saveService->saveFullImage();
            if ( !saveStarted ){
                result = "Image was not rendered";
            }
        }
        else {
            result = "There was no data to save.";
        }
    }
    else {
        result = "There was no image to save.";
    }*/
    return result;
}


void Layer::_saveImageResultCB( bool result ){
    emit saveImageResult( result );
    m_saveService->deleteLater();
}


bool Layer::_setFileName( const QString& /*fileName*/ ){
    return false;
}


void Layer::_setColorMapGlobal( std::shared_ptr<ColorState> colorState ){

    //Decide if we are going to use our own separate map that is a copy of our current
    //one or reset to a shared global color map based on whether the passed in map
    //is null
    bool colorReset = false;
    if ( colorState ){
        if ( m_stateColor.get() == nullptr || !m_stateColor->_isGlobal() ){
            //Use a common global map
            _clearColorMap();
            m_stateColor = colorState;
            colorReset = true;
        }
    }
    else {
        //We are going to use our own color map
        if ( m_stateColor.get() == nullptr || m_stateColor->_isGlobal() ){
            if ( m_stateColor ){
               disconnect( m_stateColor.get() );
            }
            Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
            ColorState* cObject = objMan->createObject<ColorState>();
            if ( m_stateColor.get() != nullptr ){
                m_stateColor->_replicateTo( cObject );
            }
            cObject->_setGlobal( false );
            m_stateColor.reset (cObject);
            colorReset = true;
        }
    }
    if ( colorReset ){
        _colorChanged( );
        connect( m_stateColor.get(), SIGNAL( colorStateChanged()), this, SLOT(_colorChanged()));
    }
}



bool Layer::_setCompositionMode( const QString& /*compositionMode*/,
        QString& errorMsg ){
    bool stateChanged = false;
    errorMsg = "Composition mode is not implemented for this layer";
    return stateChanged;
}

void Layer::_setId( const QString& id ){
    QString oldId = m_state.getValue<QString>(LAYER_ID);
    if ( oldId != id ){
        m_state.setValue<QString>(LAYER_ID, id );
    }
}


bool Layer::_setMaskColor( int /*redAmount*/,
           int /*greenAmount*/, int /*blueAmount*/, QStringList& /*result*/ ){
    return false;
}


bool Layer::_setMaskAlpha( int /*alphaAmount*/, QString& /*result*/ ){
    return false;
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


void Layer::_setVisible( bool visible ){
    bool oldVisible = m_state.getValue<bool>(Util::VISIBLE);
    if ( visible != oldVisible ){
        m_state.setValue<bool>( Util::VISIBLE, visible );
    }
}

void Layer::_updateColor(){
    emit colorStateChanged();
}


Layer::~Layer() {
}
}
}
