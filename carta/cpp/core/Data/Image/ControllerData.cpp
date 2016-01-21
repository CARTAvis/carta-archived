#include "ControllerData.h"
#include "Grid/DataGrid.h"
#include "Contour/DataContours.h"
#include "DataSource.h"
#include "DrawSynchronizer.h"
#include "Data/Preferences/PreferencesSave.h"
#include "Data/DataLoader.h"
#include "Data/Util.h"
#include "Data/Colormap/ColorState.h"
#include "Data/Image/Grid/AxisMapper.h"
#include "Data/Image/Grid/LabelFormats.h"
#include "Data/Image/LayerCompositionModes.h"
#include "State/UtilState.h"

#include "CartaLib/PixelPipeline/CustomizablePixelPipeline.h"
#include "CartaLib/IWcsGridRenderService.h"
#include "CartaLib/AxisDisplayInfo.h"
#include "../../ImageRenderService.h"
#include "../../ImageSaveService.h"


#include <QDebug>
#include <QTime>

using Carta::Lib::AxisInfo;
using Carta::Lib::AxisDisplayInfo;

namespace Carta {

namespace Data {

const QString ControllerData::CLASS_NAME = "ControllerData";
const QString ControllerData::COMPOSITION_MODE="mode";
const QString ControllerData::LAYER = "layer";
const QString ControllerData::MASK = "mask";
const QString ControllerData::LAYER_COLOR="colorSupport";
const QString ControllerData::LAYER_ALPHA="alphaSupport";
const QString ControllerData::SELECTED = "selected";


LayerCompositionModes* ControllerData::m_compositionModes = nullptr;

class ControllerData::Factory : public Carta::State::CartaObjectFactory {

public:

    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new ControllerData(path, id);
    }
};
bool ControllerData::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass (CLASS_NAME,
                                                   new ControllerData::Factory());


ControllerData::ControllerData(const QString& path, const QString& id) :
    CartaObject( CLASS_NAME, path, id),
    m_stateColor( nullptr ),
    m_dataSource( new DataSource() ),
    m_drawSync( nullptr ){

        Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
        DataGrid* gridObj = objMan->createObject<DataGrid>();
        m_dataGrid.reset( gridObj );
        m_dataGrid->_initializeGridRenderer();
        _initializeSingletons();
        _initializeState();

        std::shared_ptr<Carta::Lib::IWcsGridRenderService> gridService = m_dataGrid->_getRenderer();
        std::shared_ptr<Carta::Core::ImageRenderService::Service> imageService = m_dataSource->_getRenderer();

        // create the synchronizer
        m_drawSync.reset( new DrawSynchronizer( imageService, gridService, this ) );

        // connect its done() slot to our renderingSlot()
        connect( m_drawSync.get(), & DrawSynchronizer::done,
                         this, & ControllerData::_renderingDone );
}

void ControllerData::_addContourSet( std::shared_ptr<DataContours> contour ){
    if ( contour ){
        QString targetName = contour->getName();
        std::shared_ptr<DataContours> contourSet = _getContour( targetName );
        if ( !contourSet ){
            m_dataContours.insert( contour );
        }
    }
}

void ControllerData::_clearColorMap(){
    if ( m_stateColor ){
       disconnect( m_stateColor.get() );
       Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
       objMan->removeObject( m_stateColor->getId() );
    }
}

void ControllerData::_clearData(){
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    if ( m_dataGrid != nullptr){
        objMan->removeObject(m_dataGrid->getId());
    }
    for ( std::set< std::shared_ptr<DataContours> >::iterator it = m_dataContours.begin();
            it != m_dataContours.end(); it++ ){
        if ( (*it) ){
            objMan->removeObject( (*it)->getId() );
        }
    }
}

void ControllerData::_colorChanged(){
    if ( m_dataSource ){
        QString mapName = m_stateColor->_getColorMap();
        m_dataSource->_setColorMap( mapName );
        m_dataSource->_setTransformData( m_stateColor->_getDataTransform() );
        m_dataSource->_setGamma( m_stateColor->_getGamma() );
        m_dataSource->_setColorReversed( m_stateColor->_isReversed() );
        m_dataSource->_setColorInverted( m_stateColor->_isInverted() );
        double redAmount = m_stateColor->_getMixRed();
        double greenAmount = m_stateColor->_getMixGreen();
        double blueAmount = m_stateColor->_getMixBlue();
        m_dataSource->_setColorAmounts( redAmount, greenAmount, blueAmount );
        bool defaultNan = m_stateColor->_isNanDefault();
        m_dataSource->_setNanDefault( defaultNan );

        //If we aren't using a default nan color, tell the dataSource to use
        //the custom color.
        if ( !defaultNan ){
            int redAmount = m_stateColor->_getNanRed();
            int greenAmount = m_stateColor->_getNanGreen();
            int blueAmount = m_stateColor->_getNanBlue();
            m_dataSource->_setColorNan( redAmount, greenAmount, blueAmount );
        }
        //We are using  the default nan color.  Update the state to the default color.
        else {
            QColor nanColor = m_dataSource->_getNanColor();
            m_stateColor->_setNanColor( nanColor.red(), nanColor.green(), nanColor.blue() );
        }
        emit colorStateChanged();
    }
}



void ControllerData::_displayAxesChanged(std::vector<AxisInfo::KnownType> displayAxisTypes,
        const std::vector<int>& frames ){
    if ( m_dataSource ){
        m_dataSource->_setDisplayAxes( displayAxisTypes, frames );
    }
}

AxisInfo::KnownType ControllerData::_getAxisXType() const {
    AxisInfo::KnownType axisType = AxisInfo::KnownType::OTHER;
    if ( m_dataSource ){
        axisType = m_dataSource->_getAxisXType();
    }
    return axisType;
}

AxisInfo::KnownType ControllerData::_getAxisYType() const {
    AxisInfo::KnownType axisType = AxisInfo::KnownType::OTHER;
    if ( m_dataSource ){
        axisType = m_dataSource->_getAxisYType();
    }
    return axisType;
}

std::vector<AxisInfo::KnownType> ControllerData::_getAxisZTypes() const {
    std::vector<AxisInfo::KnownType> axisTypes;
    if ( m_dataSource ){
        axisTypes = m_dataSource->_getAxisZTypes();
    }
    return axisTypes;
}

std::vector<AxisInfo::KnownType> ControllerData::_getAxisTypes() const {
    std::vector<AxisInfo::KnownType> axisTypes;
    if ( m_dataSource ){
        axisTypes = m_dataSource->_getAxisTypes();
    }
    return axisTypes;
}


QPointF ControllerData::_getCenter() const{
    QPointF center;
    if ( m_dataSource ){
        center = m_dataSource->_getCenter();
    }
    return center;;
}

std::shared_ptr<ColorState> ControllerData::_getColorState(){
    return m_stateColor;
}

QString ControllerData::_getCompositionMode() const {
    QString maskApplyKey = Carta::State::UtilState::getLookup( MASK, COMPOSITION_MODE );
    return m_state.getValue<QString>( maskApplyKey );
}

std::shared_ptr<DataContours> ControllerData::_getContour( const QString& name ){
    std::shared_ptr<DataContours> contourSet;
    for ( std::set<std::shared_ptr<DataContours> >::iterator it= m_dataContours.begin();
            it != m_dataContours.end(); it++ ){
        if ( name == (*it)->getName() ){
            contourSet = (*it);
            break;
        }
    }
    return contourSet;
}

std::set<std::shared_ptr<DataContours>> ControllerData::_getContours() {
    return m_dataContours;
}

QStringList ControllerData::_getCoordinates( double x, double y,
        Carta::Lib::KnownSkyCS system, const std::vector<int>& frames ) const{
    QStringList coordStr;
    if ( m_dataSource ){
        coordStr = m_dataSource->_getCoordinates( x, y, system, frames );
    }
    return coordStr;
}

Carta::Lib::KnownSkyCS ControllerData::_getCoordinateSystem() const {
    Carta::Lib::KnownSkyCS cs = Carta::Lib::KnownSkyCS::Unknown;
    if ( m_dataGrid ){
        cs = m_dataGrid->_getSkyCS();
    }
    return cs;
}

QString ControllerData::_getCursorText( int mouseX, int mouseY, const std::vector<int>& frames ){
    QString cursorText;
    if ( m_dataSource ){
        Carta::Lib::KnownSkyCS cs = m_dataGrid->_getSkyCS();
        cursorText = m_dataSource->_getCursorText( mouseX, mouseY, cs, frames );
    }
    return cursorText;

}

int ControllerData::_getDimension( int coordIndex ) const {
    int dim = -1;
    if ( m_dataSource ){
        dim = m_dataSource->_getDimension( coordIndex );
    }
    return dim;
}


int ControllerData::_getDimensions() const {
    int imageSize = 0;
    if ( m_dataSource ){
        imageSize = m_dataSource->_getDimensions();
    }
    return imageSize;
}

QString ControllerData::_getFileName() const {
    QString fileName = "";
    if ( m_dataSource ){
        fileName = m_dataSource->_getFileName();
    }
    return fileName;
}

int ControllerData::_getFrameCount( AxisInfo::KnownType type ) const {
    int frameCount = 1;
    if ( m_dataSource ){
        frameCount = m_dataSource->_getFrameCount( type );
    }
    return frameCount;
}

Carta::State::StateInterface ControllerData::_getGridState() const {
    return m_dataGrid->_getState();
}

std::shared_ptr<Carta::Lib::Image::ImageInterface> ControllerData::_getImage(){
    std::shared_ptr<Carta::Lib::Image::ImageInterface> image;
    if ( m_dataSource ){
        image = m_dataSource->_getImage();
    }
    return image;
}

QPointF ControllerData::_getImagePt( QPointF screenPt, bool* valid ) const {
    QPointF imagePt;
    if ( m_dataSource ){
        imagePt = m_dataSource->_getImagePt( screenPt, valid );
    }
    else {
        *valid = false;
    }
    return imagePt;
}


bool ControllerData::_getIntensity( int frameLow, int frameHigh, double percentile, double* intensity ) const {
    bool intensityFound = false;
    if ( m_dataSource ){
        intensityFound = m_dataSource->_getIntensity( frameLow, frameHigh, percentile, intensity );
    }
    return intensityFound;
}

QString ControllerData::_getLayerString() const {
    QStringList longNames;
    longNames.append( _getFileName() );

    DataLoader* dataLoader = Util::findSingletonObject<DataLoader>();
    QStringList shortNames = dataLoader->getShortNames( longNames );

    Carta::State::StateInterface layerState( m_state);
    layerState.setValue<QString>( DataSource::DATA_PATH, shortNames[0]);
    return layerState.toString();
}

float ControllerData::_getMaskAlpha() const {
    QString key = Carta::State::UtilState::getLookup( MASK, Util::ALPHA );
    float maskInt = m_state.getValue<int>( key );
    float mask = maskInt / Util::MAX_COLOR;
    return mask;
}

quint32 ControllerData::_getMaskColor() const {
    QString redKey = Carta::State::UtilState::getLookup( MASK, Util::RED );
    int redColor = m_state.getValue<int>( redKey );
    QString greenKey = Carta::State::UtilState::getLookup( MASK, Util::GREEN );
    int greenColor = m_state.getValue<int>( greenKey );
    QString blueKey = Carta::State::UtilState::getLookup( MASK, Util::BLUE );
    int blueColor = m_state.getValue<int>( blueKey );
    QRgb rgbCol = qRgba( redColor, greenColor, blueColor, 255 );
    return rgbCol;
}

QSize ControllerData::_getOutputSize() const {
    QSize size;
    if ( m_dataSource ){
        size = m_dataSource-> _getOutputSize();
    }
    return size;
}

double ControllerData::_getPercentile( int frameLow, int frameHigh, double intensity ) const {
    double percentile = 0;
    if ( m_dataSource ){
        percentile = m_dataSource->_getPercentile( frameLow, frameHigh, intensity );
    }
    return percentile;
}

std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> ControllerData::_getPipeline() const {
    std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> pipeline;
    if ( m_dataSource ){
        pipeline = m_dataSource->_getPipeline();
    }
    return pipeline;
}

QStringList ControllerData::_getPixelCoordinates( double ra, double dec ) const{
    QStringList result("");
    if ( m_dataSource ){
        result = m_dataSource->_getPixelCoordinates( ra, dec );
    }
    return result;
}

QString ControllerData::_getPixelUnits() const {
    QString units;
    if ( m_dataSource ){
        units = m_dataSource->_getPixelUnits();
    }
    return units;
}

QString ControllerData::_getPixelValue( double x, double y, const std::vector<int>& frames ) const {
    QString pixelValue = "";
    if ( m_dataSource ){
        pixelValue = m_dataSource->_getPixelValue( x, y, frames );
    }
    return pixelValue;
}

QImage ControllerData::_getQImage() const {
    return m_qimage;
}

QPointF ControllerData::_getScreenPt( QPointF imagePt, bool* valid ) const {
    QPointF screenPt;
    if ( m_dataSource ){
        screenPt = m_dataSource->_getScreenPt( imagePt, valid );
    }
    else {
        *valid = false;
    }
    return screenPt;
}

QString ControllerData::_getStateString() const{
    Carta::State::StateInterface copyState( m_state );
    copyState.insertObject( DataGrid::GRID, m_dataGrid->_getState().toString() );
    int contourCount = m_dataContours.size();
    copyState.insertArray( DataContours::CONTOURS, contourCount );
    int i = 0;
    for ( std::set< std::shared_ptr<DataContours> >::iterator iter = m_dataContours.begin();
            iter != m_dataContours.end(); iter++ ){
        QString lookup = Carta::State::UtilState::getLookup( DataContours::CONTOURS, i );
        copyState.setObject( lookup, (*iter)->_getState().toString() );
        i++;
    }
    QString stateStr = copyState.toString();
    return stateStr;
}

Carta::Lib::VectorGraphics::VGList ControllerData::_getVectorGraphics(){
    return m_vectorGraphics;
}

double ControllerData::_getZoom() const {
    double zoom = DataSource::ZOOM_DEFAULT;
    if ( m_dataSource ){
        zoom = m_dataSource-> _getZoom();
    }
    return zoom;
}

void ControllerData::_gridChanged( const Carta::State::StateInterface& state ){
    m_dataGrid->_resetState( state );
}

void ControllerData::_initializeSingletons( ){
    //Load the available color maps.
    if ( m_compositionModes == nullptr ){
        m_compositionModes = Util::findSingletonObject<LayerCompositionModes>();
    }
}



void ControllerData::_initializeState(){
    m_state.insertValue<QString>(DataSource::DATA_PATH, "");
    m_state.insertValue<bool>(Util::VISIBLE, true );
    m_state.insertValue<bool>(SELECTED, false );


    //Color mix
    m_state.insertObject( MASK );
    QString compModeKey = Carta::State::UtilState::getLookup( MASK, COMPOSITION_MODE );
    QString defaultCompMode = m_compositionModes->getDefault();
    m_state.insertValue<QString>( compModeKey, defaultCompMode );
    QString redKey = Carta::State::UtilState::getLookup( MASK, Util::RED );
    m_state.insertValue<int>( redKey, 255 );
    QString greenKey = Carta::State::UtilState::getLookup( MASK, Util::GREEN );
    m_state.insertValue<int>( greenKey, 255 );
    QString blueKey = Carta::State::UtilState::getLookup( MASK, Util::BLUE );
    m_state.insertValue<int>( blueKey, 255 );
    QString alphaKey = Carta::State::UtilState::getLookup( MASK, Util::ALPHA );
    m_state.insertValue<int>( alphaKey, 255 );
    QString layerColorKey = Carta::State::UtilState::getLookup( MASK, LAYER_COLOR );
    bool colorSupport = m_compositionModes->isColorSupport( defaultCompMode );
    m_state.insertValue<bool>( layerColorKey, colorSupport );
    QString layerAlphaKey = Carta::State::UtilState::getLookup( MASK, LAYER_ALPHA );
    bool alphaSupport = m_compositionModes->isAlphaSupport( defaultCompMode );
    m_state.insertValue<bool>( layerAlphaKey, alphaSupport );
}

bool ControllerData::_isContourDraw() const {
    bool contourDraw = false;
    for ( std::set< std::shared_ptr<DataContours> >::iterator it = m_dataContours.begin();
            it != m_dataContours.end(); it++ ){
        if ( (*it)->isContourDraw() ){
            contourDraw = true;
            break;
        }
    }
    return contourDraw;
}

bool ControllerData::_isSelected() const {
    return m_state.getValue<bool>( SELECTED );
}

bool ControllerData::_isVisible() const {
    return m_state.getValue<bool>(Util::VISIBLE);
}

bool ControllerData::_isMatch( const QString& name ) const {
    bool matched = false;
    QString fileName = _getFileName();
    if ( name == fileName ){
        matched = true;
    }
    return matched;
}

void ControllerData::_renderingDone(
        QImage image,
        Carta::Lib::VectorGraphics::VGList gridVG,
        Carta::Lib::VectorGraphics::VGList contourVG,
        int64_t /*jobId*/){
    /// \todo we should make sure the jobId matches the last submitted job...

    m_qimage = image;
    Carta::Lib::VectorGraphics::VGComposer comp( gridVG );

    if ( _isContourDraw()){
        // where does 0.5, 0.5 map to?
        if ( m_dataSource ){

            bool valid1 = false;
            QPointF p1 = m_dataSource->_getScreenPt( { 0.5, 0.5 }, &valid1 );

            // where does 1.5, 1.5 map to?
            bool valid2 = false;
            QPointF p2 = m_dataSource->_getScreenPt( { 1.5, 1.5 }, &valid2 );
            if ( valid1 && valid2 ){
                QTransform tf;
                double m11 = p2.x() - p1.x();
                double m22 = p2.y() - p1.y();
                double m33 = 1; // no projection
                double m13 = 0; // no projection
                double m23 = 0; // no projection
                double m12 = 0; // no shearing
                double m21 = 0; // no shearing
                double m31 = p1.x() - m11 * 0.5;
                double m32 = p1.y() - m22 * 0.5;
                tf.setMatrix( m11, m12, m13, m21, m22, m23, m31, m32, m33 );
                Carta::Lib::VectorGraphics::VGComposer contourComp;
                contourComp.append< Carta::Lib::VectorGraphics::Entries::SetTransform >( tf );
                contourComp.appendList( contourVG);
                comp.appendList( contourComp.vgList() );
            }
        }
    }
    m_vectorGraphics = comp.vgList();

    // schedule a repaint with the connector
    emit renderingDone();
}


void ControllerData::_load(vector<int> frames, bool recomputeClipsOnNewFrame,
        double minClipPercentile, double maxClipPercentile ){
    if ( m_dataSource ){
        m_dataSource->_load( frames, recomputeClipsOnNewFrame,
                minClipPercentile, maxClipPercentile );
        if ( m_dataGrid ){
            if ( m_dataGrid->_isGridVisible() ){
                std::shared_ptr<Carta::Lib::IWcsGridRenderService> gridService = m_dataGrid->_getRenderer();
                gridService->setInputImage( m_dataSource->_getImage() );
            }
        }
    }
}


void ControllerData::_removeContourSet( std::shared_ptr<DataContours> contourSet ){
    if ( contourSet ){
        QString targetName = contourSet->getName();
        for ( std::set< std::shared_ptr<DataContours> >::iterator it = m_dataContours.begin();
                        it != m_dataContours.end(); it++ ){
            if ( targetName == (*it)->getName() ){
                m_dataContours.erase(*it);
                break;
            }
        }
    }
}


void ControllerData::_render( const std::vector<int>& frames,
        const Carta::Lib::KnownSkyCS& cs, bool topOfStack ){
    // erase current grid
    std::shared_ptr<Carta::Lib::IWcsGridRenderService> gridService = m_dataGrid->_getRenderer();
    std::shared_ptr<Carta::Core::ImageRenderService::Service> imageService = m_dataSource->_getRenderer();
    QSize renderSize = imageService-> outputSize();
    gridService-> setOutputSize( renderSize );

    int leftMargin = m_dataGrid->_getMargin( LabelFormats::EAST );
    int rightMargin = m_dataGrid->_getMargin( LabelFormats::WEST );
    int topMargin = m_dataGrid->_getMargin( LabelFormats::NORTH );
    int bottomMargin = m_dataGrid->_getMargin( LabelFormats::SOUTH );

    QRectF outputRect( leftMargin, topMargin,
                       renderSize.width() - leftMargin - rightMargin,
                       renderSize.height() - topMargin - bottomMargin );

    QPointF topLeft = outputRect.topLeft();
    QPointF bottomRight = outputRect.bottomRight();

    QPointF topLeftInput = imageService-> screen2img( topLeft );
    QPointF bottomRightInput = imageService->screen2img( bottomRight );

    QRectF inputRect( topLeftInput, bottomRightInput );

    gridService-> setImageRect( inputRect );
    gridService-> setOutputRect( outputRect );

    std::vector<AxisDisplayInfo> axisInfo = m_dataSource->_getAxisDisplayInfo();
    int axisCount = axisInfo.size();
    for ( int i = 0; i < axisCount; i++ ){
        Carta::Lib::AxisInfo::KnownType type = axisInfo[i].getAxisType();
        int frame = axisInfo[i].getFrame();
        //Not one of the display frames so reset it with the current frame.
        if ( frame >= 0 ){
            frame = m_dataSource->_getFrameIndex( static_cast<int>(type), frames );
            axisInfo[i].setFrame( frame );
        }
    }
    gridService->setAxisDisplayInfo( axisInfo );

    std::shared_ptr<Carta::Lib::NdArray::RawViewInterface> rawData( m_dataSource->_getRawData( frames ));
    m_drawSync->setInput( rawData );
    m_drawSync->setContours( m_dataContours );

    //Which display axes will be drawn.
    AxisInfo::KnownType xType = m_dataSource->_getAxisXType();
    AxisInfo::KnownType yType = m_dataSource->_getAxisYType();
    QString displayLabelX = AxisMapper::getPurpose( xType, cs );
    QString displayLabelY = AxisMapper::getPurpose( yType, cs );
    gridService->setAxisLabel( 0, displayLabelX );
    gridService->setAxisLabel( 1, displayLabelY );

    AxisInfo::KnownType horAxisType = m_dataSource->_getAxisXType();
    Carta::Lib::AxisLabelInfo horAxisInfo = m_dataGrid->_getAxisLabelInfo( 0, horAxisType, cs );
    gridService->setAxisLabelInfo( 0, horAxisInfo );
    AxisInfo::KnownType vertAxisType = m_dataSource->_getAxisYType();
    Carta::Lib::AxisLabelInfo vertAxisInfo = m_dataGrid->_getAxisLabelInfo( 1, vertAxisType, cs );
    gridService->setAxisLabelInfo( 1, vertAxisInfo );

    bool contourDraw = _isContourDraw();
    bool gridDraw = false;
    if ( topOfStack ){
        gridDraw = m_dataGrid->_isGridVisible();
    }
    m_drawSync-> start( contourDraw, gridDraw );
}

void ControllerData::_resetState( const QString& stateStr ){
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

void ControllerData::_resetStateContours(const Carta::State::StateInterface& restoreState ){
    int contourCount = restoreState.getArraySize( DataContours::CONTOURS );
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    QStringList supportedContours;

    //Add any contours not there
    for ( int i = 0; i < contourCount; i++ ){
       QString lookup = Carta::State::UtilState::getLookup( DataContours::CONTOURS, i );
       QString nameLookup = Carta::State::UtilState::getLookup( lookup, DataContours::SET_NAME );
       QString contourStr = restoreState.toString( lookup);
       QString contourName = restoreState.getValue<QString>( nameLookup );
       supportedContours.append( contourName );
       bool newContourSet = false;
       std::shared_ptr<DataContours> contourSet = _getContour( contourName );
       if ( !contourSet ){
           newContourSet = true;
          contourSet = std::shared_ptr<DataContours>(objMan->createObject<DataContours>());
          m_dataContours.insert( contourSet );
       }

       QString contourSetState = restoreState.toString( lookup );
       contourSet->resetState( contourSetState );
       if ( newContourSet ){
           emit contourSetAdded( this, contourName );
       }
   }

   //Remove any contours no longer there
    for ( std::set<std::shared_ptr<DataContours> >::iterator it = m_dataContours.begin();
            it != m_dataContours.end(); it++ ){
        QString contourSetName = (*it)->getName();
        if ( !supportedContours.contains( contourSetName )){
            _removeContourSet( (*it) );
            emit contourSetRemoved( contourSetName );
        }
    }
}

void ControllerData::_resetState( const Carta::State::StateInterface& restoreState ){
    //Restore the other state variables
    m_state.setValue<bool>(Util::VISIBLE, restoreState.getValue<bool>(Util::VISIBLE) );
    m_state.setValue<bool>(SELECTED, restoreState.getValue<bool>(SELECTED) );

    //Color mix
    QString compModeKey = Carta::State::UtilState::getLookup( MASK, COMPOSITION_MODE );
    m_state.setValue<QString>( compModeKey, restoreState.getValue<QString>(compModeKey) );
    QString redKey = Carta::State::UtilState::getLookup( MASK, Util::RED );
    m_state.setValue<int>( redKey, restoreState.getValue<int>(redKey) );
    QString greenKey = Carta::State::UtilState::getLookup( MASK, Util::GREEN );
    m_state.setValue<int>( greenKey, restoreState.getValue<int>(greenKey) );
    QString blueKey = Carta::State::UtilState::getLookup( MASK, Util::BLUE );
    m_state.setValue<int>( blueKey, restoreState.getValue<int>(blueKey) );
    QString alphaKey = Carta::State::UtilState::getLookup( MASK, Util::ALPHA );
    m_state.setValue<int>( alphaKey, restoreState.getValue<int>(alphaKey) );
    QString layerColorKey = Carta::State::UtilState::getLookup( MASK, LAYER_COLOR );
    m_state.setValue<bool>( layerColorKey, restoreState.getValue<bool>(layerColorKey) );
    QString layerAlphaKey = Carta::State::UtilState::getLookup( MASK, LAYER_ALPHA );
    m_state.setValue<bool>( layerAlphaKey, restoreState.getValue<bool>(layerAlphaKey) );
}


void ControllerData::_resetZoom(){
    if ( m_dataSource ){
        m_dataSource->_resetZoom();
    }
}


void ControllerData::_resetPan(){
    if ( m_dataSource ){
        m_dataSource->_resetPan();
    }
}


QString ControllerData::_saveImage( const QString& saveName, double scale,
        const std::vector<int>& frames ){
    QString result;
    if ( m_dataSource ){

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
                    this, & ControllerData::_saveImageResultCB );

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
    }
    return result;
}


void ControllerData::_saveImageResultCB( bool result ){
    emit saveImageResult( result );
    m_saveService->deleteLater();
}


bool ControllerData::_setFileName( const QString& fileName ){
    bool successfulLoad = m_dataSource->_setFileName( fileName );
    if ( successfulLoad ){
        m_state.setValue<QString>(DataSource::DATA_PATH, fileName);
    }
    return successfulLoad;
}


void ControllerData::_setColorMapGlobal( std::shared_ptr<ColorState> colorState ){

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



bool ControllerData::_setCompositionMode( const QString& compositionMode,
        QString& errorMsg ){
    bool stateChanged = false;
    QString actualCompMode;
    bool recognizedMode = m_compositionModes->isCompositionMode( compositionMode, actualCompMode );
    if ( recognizedMode ){
        QString key = Carta::State::UtilState::getLookup( MASK, COMPOSITION_MODE );
        QString oldMode = m_state.getValue<QString>( key );
        if ( oldMode != actualCompMode ){
            m_state.setValue<QString>( key, actualCompMode );
            bool colorSupport = m_compositionModes->isColorSupport( actualCompMode );
            QString colorSupportKey = Carta::State::UtilState::getLookup( MASK, LAYER_COLOR );
            m_state.setValue<bool>( colorSupportKey, colorSupport );
            if ( !colorSupport ){
                QStringList result;
                _setMaskColor( 255, 255, 255, result );
            }
            bool alphaSupport = m_compositionModes->isAlphaSupport( actualCompMode );
            QString alphaSupportKey = Carta::State::UtilState::getLookup( MASK, LAYER_ALPHA );
            m_state.setValue<bool>( alphaSupportKey, alphaSupport );
            if ( !alphaSupport ){
                QString result;
                _setMaskAlpha( 255, result );
            }
            stateChanged = true;
        }
    }
    else {
        errorMsg = "Unrecognized layer composition mode: "+compositionMode;
    }
    return stateChanged;
}


bool ControllerData::_setMaskColor( int redAmount,
        int greenAmount, int blueAmount, QStringList& result ){
    bool changed = false;
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
        QString redKey = Carta::State::UtilState::getLookup( MASK, Util::RED );
        int oldRedAmount = m_state.getValue<int>( redKey );
        QString greenKey = Carta::State::UtilState::getLookup( MASK, Util::GREEN );
        int oldGreenAmount = m_state.getValue<int>( greenKey );
        QString blueKey = Carta::State::UtilState::getLookup( MASK, Util::BLUE );
        int oldBlueAmount = m_state.getValue<int>( blueKey );
        if ( redAmount != oldRedAmount || greenAmount != oldGreenAmount ||
                blueAmount != oldBlueAmount ){
            changed = true;
            m_state.setValue<int>( redKey, redAmount );
            m_state.setValue<int>( greenKey, greenAmount );
            m_state.setValue<int>( blueKey, blueAmount );
        }
    }
    return changed;
}


bool ControllerData::_setMaskAlpha( int alphaAmount, QString& result ){
    bool changed = false;
    if ( 0 > alphaAmount || alphaAmount > 255 ){
        result = "Invalid mask opacity [0,255]:"+QString::number( alphaAmount );
    }
    else {
        QString lookup = Carta::State::UtilState::getLookup( MASK, Util::ALPHA );
        int oldAlpha = m_state.getValue<int>( lookup );
        if ( oldAlpha != alphaAmount ){
            changed = true;
            m_state.setValue<int>( lookup, alphaAmount );
        }
    }
    return changed;
}


bool ControllerData::_setSelected( bool selected ){
    bool stateChanged = false;
    bool oldSelected = m_state.getValue<bool>(SELECTED );
    if ( oldSelected != selected ){
        m_state.setValue<bool>( SELECTED, selected );
        stateChanged = true;
    }
    return stateChanged;
}


void ControllerData::_setVisible( bool visible ){
    bool oldVisible = m_state.getValue<bool>(Util::VISIBLE);
    if ( visible != oldVisible ){
        m_state.setValue<bool>( Util::VISIBLE, visible );
    }
}


void ControllerData::_setPan( double imgX, double imgY ){
    if ( m_dataSource ){
        m_dataSource-> _setPan( imgX, imgY );
    }
}


void ControllerData::_setZoom( double zoomAmount){
    if ( m_dataSource ){
        m_dataSource-> _setZoom( zoomAmount );
    }
}


void ControllerData::_updateClips( std::shared_ptr<Carta::Lib::NdArray::RawViewInterface>& view,
        double minClipPercentile, double maxClipPercentile, const std::vector<int>& frames ){
    if ( m_dataSource ){
        m_dataSource->_updateClips( view,  minClipPercentile, maxClipPercentile, frames );
    }
}

void ControllerData::_viewResize( const QSize& newSize ){
    if ( m_dataSource ){
        m_dataSource->_viewResize( newSize );
    }
}


ControllerData::~ControllerData() {
    _clearData();
}
}
}
