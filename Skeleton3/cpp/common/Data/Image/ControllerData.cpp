#include "ControllerData.h"
#include "DataGrid.h"
#include "DataContours.h"
#include "DataSource.h"
#include "DrawSynchronizer.h"
#include "Data/Preferences/PreferencesSave.h"
#include "CartaLib/IImage.h"
#include "Data/Util.h"
#include "CartaLib/PixelPipeline/CustomizablePixelPipeline.h"
#include "../../ImageRenderService.h"
#include "../../ImageSaveService.h"
#include "CartaLib/IWcsGridRenderService.h"

#include <QDebug>
#include <QTime>

namespace Carta {

namespace Data {

const QString ControllerData::CLASS_NAME = "ControllerData";
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
    m_dataSource( new DataSource() ),
    m_drawSync( nullptr ){

        Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
        DataGrid* gridObj = objMan->createObject<DataGrid>();
        m_dataGrid.reset( gridObj );
        m_dataGrid->_initializeGridRenderer();
        _initializeState();


        DataContours* contourObj = objMan->createObject<DataContours>();
        m_dataContours.reset( contourObj );

        std::shared_ptr<Carta::Lib::IWcsGridRenderService> gridService = m_dataGrid->_getRenderer();
        std::shared_ptr<Carta::Core::ImageRenderService::Service> imageService = m_dataSource->_getRenderer();
        std::shared_ptr<Carta::Lib::IContourGeneratorService> contourService = m_dataContours->_getRenderer();

        // create the synchronizer
        m_drawSync.reset( new DrawSynchronizer( imageService, gridService, contourService, this ) );

        // connect its done() slot to our renderingSlot()
        connect( m_drawSync.get(), & DrawSynchronizer::done,
                 this, & ControllerData::_renderingDone );

}



bool ControllerData::_contains(const QString& fileName) const {
    bool representsData = false;
    if ( m_dataSource ){
        representsData = m_dataSource->_contains( fileName );
    }
    return representsData;
}

QString ControllerData::_getCursorText( int mouseX, int mouseY, int frameIndex){
    QString cursorText;
    if ( m_dataSource ){
        Carta::Lib::KnownSkyCS cs = m_dataGrid->_getSkyCS();
        cursorText = m_dataSource->_getCursorText( mouseX, mouseY, frameIndex, cs );
    }
    return cursorText;

}

QPointF ControllerData::_getCenter() const{
    QPointF center;
    if ( m_dataSource ){
        center = m_dataSource->_getCenter();
    }
    return center;;
}


Carta::State::StateInterface ControllerData::_getGridState() const {
    return m_dataGrid->_getState();
}

QPointF ControllerData::_getImagePt( QPointF screenPt, bool* valid ) const {
    QPointF imagePt;
    if ( m_dataSource ){
        m_dataSource->_getImagePt( screenPt, valid );
    }
    else {
        *valid = false;
    }
    return imagePt;
}

QString ControllerData::_getPixelValue( double x, double y, int frameIndex ) const {
    QString pixelValue = "";
    if ( m_dataSource ){
        pixelValue = m_dataSource->_getPixelValue( x, y, frameIndex );
    }
    return pixelValue;
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

int ControllerData::_getFrameCount() const {
    int frameCount = 1;
    if ( m_dataSource ){
        frameCount = m_dataSource->_getFrameCount();
    }
    return frameCount;
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

std::shared_ptr<Image::ImageInterface> ControllerData::_getImage(){
    std::shared_ptr<Image::ImageInterface> image;
    if ( m_dataSource ){
        image = m_dataSource->_getImage();
    }
    return image;
}


std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> ControllerData::_getPipeline() const {
    std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> pipeline;
    if ( m_dataSource ){
        pipeline = m_dataSource->_getPipeline();
    }
    return pipeline;

}

bool ControllerData::_getIntensity( int frameLow, int frameHigh, double percentile, double* intensity ) const {
    bool intensityFound = false;
    if ( m_dataSource ){
        intensityFound = m_dataSource->_getIntensity( frameLow, frameHigh, percentile, intensity );
    }
    return intensityFound;
}

double ControllerData::_getPercentile( int frameLow, int frameHigh, double intensity ) const {
    double percentile = 0;
    if ( m_dataSource ){
        percentile = m_dataSource->_getPercentile( frameLow, frameHigh, intensity );
    }
    return percentile;
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

/*NdArray::RawViewInterface * ControllerData::_getRawData( int channelStart, int channelEnd ) const {
    NdArray::RawViewInterface* rawData = nullptr;
    if ( m_dataSource ){
        m_dataSource->_getRawData( channelStart, channelEnd );
    }
    return rawData;
}*/

QString ControllerData::_getStateString() const{
    QString stateStr = m_state.toString();
    return stateStr;
}

double ControllerData::_getZoom() const {
    double zoom = DataSource::ZOOM_DEFAULT;
    if ( m_dataSource ){
        zoom = m_dataSource-> _getZoom();
    }
    return zoom;
}

QSize ControllerData::_getOutputSize() const {
    QSize size;
    if ( m_dataSource ){
        size = m_dataSource-> _getOutputSize();
    }
    return size;
}

void ControllerData::_gridChanged( const Carta::State::StateInterface& state, bool renderImage, int frameIndex ){
    bool stateChanged = m_dataGrid->_resetState( state );
    if ( stateChanged ){
        m_state.setObject(DataGrid::GRID, m_dataGrid->_getState().toString());
        if ( renderImage ){
            _render( frameIndex );
        }
    }
}

void ControllerData::_initializeState(){
    m_state.insertValue<QString>(DataSource::DATA_PATH, "");
    QString gridState = _getGridState().toString();
    m_state.insertObject(DataGrid::GRID, gridState );
}

void ControllerData::_renderingDone(
        QImage image,
        Carta::Lib::VectorGraphics::VGList gridVG,
        Carta::Lib::VectorGraphics::VGList contourVG,
        int64_t /*jobId*/){
    /// \todo we should make sure the jobId matches the last submitted job...

    m_qimage = image;

    // draw the grid over top
    //QTime t;
    //t.restart();
    QPainter painter( & m_qimage );
    painter.setRenderHint( QPainter::Antialiasing, true );
    Carta::Lib::VectorGraphics::VGListQPainterRenderer vgRenderer;
    if ( m_dataGrid->_isGridVisible() ){
        if ( ! vgRenderer.render( gridVG, painter ) ) {
            qWarning() << "could not render grid vector graphics";
        }
        //qDebug() << "Grid VG rendered in" << t.elapsed() / 1000.0 << "sec" << "xyz";
    }
    //t.restart();

    if ( m_dataContours->isContourDraw()){
        QPen lineColor( QColor( "red" ), 1 );
        lineColor.setCosmetic( true );
        painter.setPen( lineColor );

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
                painter.setTransform( tf );
            }
        }

        if ( ! vgRenderer.render( contourVG, painter ) ) {
            qWarning() << "could not render contour vector graphics";
        }
    //qDebug() << "Contour VG rendered in" << t.elapsed() / 1000.0 << "sec" << "xyz";
    }


    // schedule a repaint with the connector
    emit renderingDone( m_qimage );
}


void ControllerData::_load(int frameIndex, bool recomputeClipsOnNewFrame,
        double minClipPercentile, double maxClipPercentile){
    if ( m_dataSource ){
        m_dataSource->_load( frameIndex, recomputeClipsOnNewFrame,
                minClipPercentile, maxClipPercentile );
        if ( m_dataGrid->_isGridVisible() ){
            std::shared_ptr<Carta::Lib::IWcsGridRenderService> gridService = m_dataGrid->_getRenderer();
            gridService-> setInputImage( m_dataSource->_getImage() );
        }
        _render( frameIndex );
    }
}

void ControllerData::_render( int frameIndex ){
    // erase current grid
    std::shared_ptr<Carta::Lib::IWcsGridRenderService> gridService = m_dataGrid->_getRenderer();
    std::shared_ptr<Carta::Core::ImageRenderService::Service> imageService = m_dataSource->_getRenderer();
    //std::shared_ptr<Carta::Lib::IContourGeneratorService> contourService = m_dataContours->_getRenderer();
    QSize renderSize = imageService-> outputSize();
    gridService-> setOutputSize( renderSize );

    int leftMargin = 50;
    int rightMargin = 10;
    int bottomMargin = 50;
    int topMargin = 10;

    QRectF outputRect( leftMargin, topMargin,
                       renderSize.width() - leftMargin - rightMargin,
                       renderSize.height() - topMargin - bottomMargin );

    QRectF inputRect(
        imageService-> screen2img( outputRect.topLeft() ),
        imageService-> screen2img( outputRect.bottomRight() ) );

    gridService-> setImageRect( inputRect );
    gridService-> setOutputRect( outputRect );

    std::shared_ptr<NdArray::RawViewInterface> rawData( m_dataSource->_getRawData( frameIndex, frameIndex) );
    m_drawSync->setInput( rawData );
    m_drawSync->setPens( m_dataContours->getPens() );

    bool contourDraw = m_dataContours->isContourDraw();
    bool gridDraw = m_dataGrid->_isGridVisible();
    m_drawSync-> start( contourDraw, gridDraw );
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

void ControllerData::_saveImage( const QString& saveName, double scale,
        int frameIndex ){
    if ( m_dataSource ){
        QString fileName = m_dataSource->_getFileName();
        std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> pipeline =
                m_dataSource->_getPipeline();
        std::shared_ptr<Image::ImageInterface> image = m_dataSource->_getImage();
        m_saveService = new Carta::Core::ImageSaveService::ImageSaveService( saveName,
                image, pipeline, fileName );
        PreferencesSave* prefSave = Util::findSingletonObject<PreferencesSave>();
        int width = prefSave->getWidth();
        int height = prefSave->getHeight();
        Qt::AspectRatioMode aspectRatioMode = prefSave->getAspectRatioMode();
        m_saveService->setOutputSize( QSize( width, height ) );
        m_saveService->setAspectRatioMode( aspectRatioMode );
        m_saveService->setFrameIndex( frameIndex );

        m_saveService->setZoom( scale );

        connect( m_saveService, & Carta::Core::ImageSaveService::ImageSaveService::saveImageResult,
                this, & ControllerData::_saveImageResultCB );

        m_saveService->saveFullImage();
    }
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

void ControllerData::setColorMap( const QString& name ){
    if ( m_dataSource ){
        m_dataSource->setColorMap( name );
    }
}

void ControllerData::setColorInverted( bool inverted ){
    if ( m_dataSource ){
        m_dataSource->setColorInverted( inverted );
    }
}

void ControllerData::setColorReversed( bool reversed ){
    if ( m_dataSource ){
        m_dataSource->setColorReversed( reversed );
    }
}

void ControllerData::setColorAmounts( double newRed, double newGreen, double newBlue ){
    if ( m_dataSource ){
        m_dataSource->setColorAmounts( newRed, newGreen, newBlue );
    }
}

void ControllerData::_setPan( double imgX, double imgY ){
    if ( m_dataSource ){
        m_dataSource-> _setPan( imgX, imgY );
    }
}

void ControllerData::_setTransformData( const QString& name ){
    if ( m_dataSource ){
        m_dataSource->_setTransformData( name );
    }
}

void ControllerData::_setZoom( double zoomAmount){
    if ( m_dataSource ){
        m_dataSource-> _setZoom( zoomAmount );
    }
}



void ControllerData::setGamma( double gamma ){
    if ( m_dataSource ){
        m_dataSource->setGamma( gamma );
    }
}

void ControllerData::_updateClips( std::shared_ptr<NdArray::RawViewInterface>& view, int frameIndex,
        double minClipPercentile, double maxClipPercentile ){
    if ( m_dataSource ){
        m_dataSource->_updateClips( view, frameIndex, minClipPercentile, maxClipPercentile );
    }
}

void ControllerData::_viewResize( const QSize& newSize ){
    if ( m_dataSource ){
        m_dataSource->_viewResize( newSize );
    }
}


QStringList ControllerData::_getCoordinates( double x, double y, int frameIndex,
        Carta::Lib::KnownSkyCS system ) const{
    QStringList coordStr;
    if ( m_dataSource ){
        coordStr = m_dataSource->_getCoordinates( x, y, frameIndex, system );
    }
    return coordStr;
}



ControllerData::~ControllerData() {
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    if ( m_dataGrid != nullptr){
        objMan->removeObject(m_dataGrid->getId());
    }
}
}
}
