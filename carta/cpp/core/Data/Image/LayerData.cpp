#include "Grid/DataGrid.h"
#include "Contour/DataContours.h"
#include "DataSource.h"
#include "Data/Image/Draw/DrawSynchronizer.h"
#include "Data/DataLoader.h"
#include "Data/Util.h"
#include "Data/Colormap/ColorState.h"
#include "Data/Image/Grid/AxisMapper.h"
#include "Data/Image/Grid/LabelFormats.h"
#include "State/UtilState.h"

#include "CartaLib/PixelPipeline/CustomizablePixelPipeline.h"
#include "CartaLib/IWcsGridRenderService.h"
#include "CartaLib/AxisDisplayInfo.h"
#include "../../ImageRenderService.h"

#include <QDebug>
#include <QTime>
#include "LayerData.h"

using Carta::Lib::AxisInfo;
using Carta::Lib::AxisDisplayInfo;

namespace Carta {

namespace Data {

const QString LayerData::CLASS_NAME = "LayerData";
const QString LayerData::MASK = "mask";
const QString LayerData::LAYER_COLOR="colorSupport";
const QString LayerData::LAYER_ALPHA="alphaSupport";

const QString LayerData::PAN = "pan";


class LayerData::Factory : public Carta::State::CartaObjectFactory {

public:

    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new LayerData(path, id);
    }
};
bool LayerData::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass (CLASS_NAME,
                                                   new LayerData::Factory());


LayerData::LayerData(const QString& path, const QString& id) :
    Layer( CLASS_NAME, path, id),
    m_dataSource( new DataSource()),
    m_drawSync( nullptr ),
    m_stateColor( nullptr ){

        m_renderQueued = false;

        _initializeState();

        Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
        ColorState* colorObj = objMan->createObject<ColorState>();
        m_stateColor.reset( colorObj );
        connect( m_stateColor.get(), SIGNAL( colorStateChanged()), this, SLOT(_colorChanged()));


        DataGrid* gridObj = objMan->createObject<DataGrid>();
        m_dataGrid.reset( gridObj );
        m_dataGrid->_initializeGridRenderer();
        _colorChanged();

        std::shared_ptr<Carta::Lib::IWcsGridRenderService> gridService = m_dataGrid->_getRenderer();
        std::shared_ptr<Carta::Core::ImageRenderService::Service> imageService = m_dataSource->_getRenderer();

        // create the synchronizer
        m_drawSync.reset( new DrawSynchronizer( imageService, gridService, this ) );


        // connect its done() slot to our renderingSlot()
        connect( m_drawSync.get(), & DrawSynchronizer::done,
                         this, & LayerData::_renderingDone );

}

void LayerData::_addContourSet( std::shared_ptr<DataContours> contour ){
    if ( contour ){
        QString targetName = contour->getName();
        std::shared_ptr<DataContours> contourSet = _getContour( targetName );
        if ( !contourSet ){
            m_dataContours.insert( contour );
        }
    }
}

void LayerData::_clearColorMap(){
    if ( m_stateColor ){
       disconnect( m_stateColor.get() );
       Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
       objMan->removeObject( m_stateColor->getId() );
    }
}

void LayerData::_colorChanged(){
    _updateColor();
    Layer::_colorChanged();
}


void LayerData::_displayAxesChanged(std::vector<AxisInfo::KnownType> displayAxisTypes,
        const std::vector<int>& frames ){
    if ( m_dataSource ){
        m_dataSource->_setDisplayAxes( displayAxisTypes, frames );
    }
}




Carta::Lib::AxisInfo::KnownType LayerData::_getAxisType( int index ) const {
    AxisInfo::KnownType type = AxisInfo::KnownType::OTHER;
    if ( m_dataSource ){
        type = m_dataSource->_getAxisType( index );
    }
    return type;
}

AxisInfo::KnownType LayerData::_getAxisXType() const {
    AxisInfo::KnownType axisType = AxisInfo::KnownType::OTHER;
    if ( m_dataSource ){
        axisType = m_dataSource->_getAxisXType();
    }
    return axisType;
}

AxisInfo::KnownType LayerData::_getAxisYType() const {
    AxisInfo::KnownType axisType = AxisInfo::KnownType::OTHER;
    if ( m_dataSource ){
        axisType = m_dataSource->_getAxisYType();
    }
    return axisType;
}

std::vector<AxisInfo::KnownType> LayerData::_getAxisZTypes() const {
    std::vector<AxisInfo::KnownType> axisTypes;
    if ( m_dataSource ){
        axisTypes = m_dataSource->_getAxisZTypes();
    }
    return axisTypes;
}

std::vector<AxisInfo::KnownType> LayerData::_getAxisTypes() const {
    std::vector<AxisInfo::KnownType> axisTypes;
    if ( m_dataSource ){
        axisTypes = m_dataSource->_getAxisTypes();
    }
    return axisTypes;
}


QPointF LayerData::_getCenterPixel() const {
    QPointF center;
    QString centerXKey = Carta::State::UtilState::getLookup( PAN, Util::XCOORD );
    QString centerYKey = Carta::State::UtilState::getLookup( PAN, Util::YCOORD );
    center.setX( m_state.getValue<double>( centerXKey) );
    center.setY( m_state.getValue<double>( centerYKey) );
    return center;
}

std::shared_ptr<ColorState> LayerData::_getColorState(){
    return m_stateColor;
}

std::vector< std::shared_ptr<ColorState> >  LayerData::_getSelectedColorStates( bool global){
    std::vector< std::shared_ptr<ColorState> > colorStates;
    if ( _isSelected() || global ){
        colorStates.push_back( m_stateColor );
    }
    return colorStates;
}


std::shared_ptr<DataContours> LayerData::_getContour( const QString& name ){
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

std::set<std::shared_ptr<DataContours>> LayerData::_getContours() {
    return m_dataContours;
}

QStringList LayerData::_getCoordinates( double x, double y,
        Carta::Lib::KnownSkyCS system, const std::vector<int>& frames ) const{
    QStringList coordStr;
    if ( m_dataSource ){
        coordStr = m_dataSource->_getCoordinates( x, y, system, frames );
    }
    return coordStr;
}

Carta::Lib::KnownSkyCS LayerData::_getCoordinateSystem() const {
    Carta::Lib::KnownSkyCS cs = Carta::Lib::KnownSkyCS::Unknown;
    if ( m_dataGrid ){
        cs = m_dataGrid->_getSkyCS();
    }
    return cs;
}

QString LayerData::_getCursorText( int mouseX, int mouseY, const std::vector<int>& frames,
        const QSize& outputSize ){
    QString cursorText;
    if ( m_dataSource ){
        Carta::Lib::KnownSkyCS cs = m_dataGrid->_getSkyCS();
        QPointF pan = _getPan();
        double zoom = _getZoom();
        cursorText = m_dataSource->_getCursorText( mouseX, mouseY, cs, frames, zoom, pan, outputSize );
    }
    return cursorText;
}

std::shared_ptr<DataSource> LayerData::_getDataSource(){
    return m_dataSource;
}


int LayerData::_getDimension( int coordIndex ) const {
    int dim = -1;
    if ( m_dataSource ){
        dim = m_dataSource->_getDimension( coordIndex );
    }
    return dim;
}


int LayerData::_getDimension() const {
    int imageSize = 0;
    if ( m_dataSource ){
        imageSize = m_dataSource->_getDimensions();
    }
    return imageSize;
}



QSize LayerData::_getDisplaySize() const {
    Carta::Lib::AxisInfo::KnownType xType = _getAxisXType();
    Carta::Lib::AxisInfo::KnownType yType = _getAxisYType();
    int frameCountX = _getFrameCount( xType );
    int frameCountY = _getFrameCount( yType );
    return QSize( frameCountX, frameCountY );
}


std::vector<int> LayerData::_getImageDimensions( ) const {
    std::vector<int> result;
    if ( m_dataSource ){
        int dimensions = m_dataSource->_getDimensions();
        for ( int i = 0; i < dimensions; i++ ) {
            int d = m_dataSource->_getDimension( i );
            result.push_back( d );
        }
    }
    return result;
}


int LayerData::_getFrameCount( AxisInfo::KnownType type ) const {
    int frameCount = 1;
    if ( m_dataSource ){
        frameCount = m_dataSource->_getFrameCount( type );
    }
    return frameCount;
}

Carta::State::StateInterface LayerData::_getGridState() const {
    return m_dataGrid->_getState();
}

std::shared_ptr<Carta::Lib::Image::ImageInterface> LayerData::_getImage(){
    std::shared_ptr<Carta::Lib::Image::ImageInterface> image;
    if ( m_dataSource ){
        image = m_dataSource->_getImage();
    }
    return image;
}

QPointF LayerData::_getContextPt( const QPointF& screenPt, const QSize& outputSize, bool* valid ) const {
	QPointF contextPt;
	if ( m_dataSource ){
		double zoom = 1;
		QPointF imageCenter = m_dataSource->_getCenter();
		contextPt = m_dataSource->_getImagePt( screenPt, zoom, imageCenter, outputSize, valid );
	}
	else {
		*valid = false;
	}
	return contextPt;
}

QPointF LayerData::_getImagePt( const QPointF& screenPt, const QSize& outputSize,  bool* valid ) const {
    QPointF imagePt;
    if ( m_dataSource ){
        double zoom = m_state.getValue<double>( Util::ZOOM );

        QPointF pan = _getPan();
        imagePt = m_dataSource->_getImagePt( screenPt, zoom, pan, outputSize, valid );
    }
    else {
        *valid = false;
    }
    return imagePt;
}


QRectF LayerData::_getInputRect( const QSize& outputSize ) const {
    QPointF pan = _getPan();
    double zoom = _getZoom();
    return _getInputRectangle( pan, zoom, outputSize);
}

QRectF LayerData::_getInputRectangle( const QPointF& pan, double zoom, const QSize& outputSize ) const {
    QRectF outputRect = _getOutputRectangle( outputSize, false, false );

    //Bottom and top point of image in output pixels.
    std::shared_ptr<Carta::Core::ImageRenderService::Service> imageService = m_dataSource->_getRenderer();
    QPointF tl =imageService->image2screen( QPointF(0,0), pan, zoom, outputSize );
    QSize imageSize = _getDisplaySize();
    QPointF br = imageService->image2screen( QPointF(imageSize.width(),imageSize.height()), pan, zoom, outputSize );
    QRectF imageRect( tl, br );

    //Where image is in pixels
    QRectF inputRect = outputRect.intersected( imageRect );

    //Translate back to image points.
    QPointF tlImage = imageService->screen2image( inputRect.topLeft(), pan, zoom, outputSize );
    QPointF brImage = imageService->screen2image( inputRect.bottomRight(), pan, zoom, outputSize );

    //Translate back to pixels with no zoom and pan, preserving image size.
    QPointF center( imageSize.width()/2, imageSize.height()/2 );
    QPointF tlPixel = imageService->image2screen( tlImage, center, 1, imageSize );
    QPointF brPixel = imageService->image2screen( brImage, center, 1, imageSize );
    QRectF visibleRect( tlPixel, brPixel );
    return visibleRect;
}

QRectF LayerData::_getInputRectangle( const QPointF& pan, double zoom, const QRectF& outputRect,
        const QSize& outputSize ) const {
    QPointF topLeft = outputRect.topLeft();
    QPointF bottomRight = outputRect.bottomRight();
    std::shared_ptr<Carta::Core::ImageRenderService::Service> imageService =
            m_dataSource->_getRenderer();
    QPointF topLeftInput = imageService-> screen2image( topLeft, pan, zoom, outputSize );
    QPointF bottomRightInput = imageService->screen2image( bottomRight, pan, zoom, outputSize );
    //QSize size( qAbs( topLeftInput.x() - bottomRightInput.x()), qAbs( topLeftInput.y() - bottomRightInput.y()));
    QRectF inputRect( topLeftInput, bottomRightInput );
    return inputRect;
}

std::vector<std::pair<int,double> > LayerData::_getIntensity( int frameLow, int frameHigh,
        const std::vector<double>& percentiles ) const{
    std::vector<std::pair<int,double> > intensities;
    if ( m_dataSource ){
        intensities = m_dataSource->_getIntensity( frameLow, frameHigh, percentiles );
    }
    return intensities;
}


float LayerData::_getMaskAlpha() const {
    QString key = Carta::State::UtilState::getLookup( MASK, Util::ALPHA );
    float maskInt = m_state.getValue<int>( key );
    float mask = maskInt / Util::MAX_COLOR;
    return mask;
}

quint32 LayerData::_getMaskColor() const {
    QString redKey = Carta::State::UtilState::getLookup( MASK, Util::RED );
    int redColor = m_state.getValue<int>( redKey );
    QString greenKey = Carta::State::UtilState::getLookup( MASK, Util::GREEN );
    int greenColor = m_state.getValue<int>( greenKey );
    QString blueKey = Carta::State::UtilState::getLookup( MASK, Util::BLUE );
    int blueColor = m_state.getValue<int>( blueKey );
    QRgb rgbCol = qRgba( redColor, greenColor, blueColor, 255 );
    return rgbCol;
}

QRectF LayerData::_getOutputRectangle( const QSize& outputSize, bool requestMain,
        bool requestContext) const {
    int leftMargin = 0;
    int rightMargin = 0;
    int topMargin = 0;
    int bottomMargin = 0;
    //Off center image to make room for grid axis and labels on two sides.
    if ( requestMain ){
        leftMargin = m_dataGrid->_getMargin( LabelFormats::EAST );
        rightMargin = m_dataGrid->_getMargin( LabelFormats::WEST );
        topMargin = m_dataGrid->_getMargin( LabelFormats::NORTH );
        bottomMargin = m_dataGrid->_getMargin( LabelFormats::SOUTH );
    }
    //Center the image
    else if ( requestContext ){
        QSize imageSize = _getDisplaySize();
        int extraX = (outputSize.width() - imageSize.width())/2;
        int extraY = (outputSize.height() - imageSize.height())/2;
        if ( extraX > 0 ){
            leftMargin = extraX;
            bottomMargin = outputSize.width() - imageSize.width() - extraX;
        }
        if ( extraY > 0 ){
            topMargin = extraY;
            bottomMargin = outputSize.height() - imageSize.height() - extraY;
        }
    }
    int outWidth = outputSize.width() - leftMargin - rightMargin;
    int outHeight = outputSize.height() - topMargin - bottomMargin;
    QRectF outputRect( leftMargin, topMargin, outWidth, outHeight );
    return outputRect;
}

QPointF LayerData::_getPan() const {
    QString panXKey = Carta::State::UtilState::getLookup( PAN, Util::XCOORD );
    QString panYKey = Carta::State::UtilState::getLookup( PAN, Util::YCOORD );
    double panX = m_state.getValue<double>( panXKey );
    double panY = m_state.getValue<double>( panYKey );
    return QPointF( panX, panY );
}


double LayerData::_getPercentile( int frameLow, int frameHigh, double intensity ) const {
    double percentile = 0;
    if ( m_dataSource ){
        percentile = m_dataSource->_getPercentile( frameLow, frameHigh, intensity );
    }
    return percentile;
}


QPointF LayerData::_getPixelCoordinates( double ra, double dec, bool* valid ) const{
    QPointF result;
    *valid = false;
    if ( m_dataSource ){
        result = m_dataSource->_getPixelCoordinates( ra, dec, valid );
    }
    return result;
}

QPointF LayerData::_getWorldCoordinates( double pixelX, double pixelY,
        Carta::Lib::KnownSkyCS coordSys, bool* valid ) const{
    QPointF result;
    *valid = false;
    if ( m_dataSource ){
        result = m_dataSource->_getWorldCoordinates( pixelX, pixelY, coordSys, valid );
    }
    return result;
}

QString LayerData::_getPixelUnits() const {
    QString units;
    if ( m_dataSource ){
        units = m_dataSource->_getPixelUnits();
    }
    return units;
}

QString LayerData::_getPixelValue( double x, double y, const std::vector<int>& frames ) const {
    QString pixelValue( "" );
    if ( m_dataSource ){
        pixelValue = m_dataSource->_getPixelValue( x, y, frames );
    }
    return pixelValue;
}

Carta::Lib::VectorGraphics::VGList LayerData::_getRegionGraphics() const {
	return m_regionGraphics;
}

std::pair<double,QString> LayerData::_getRestFrequency() const {
	std::pair<double,QString> restFreq(-1,"");
	if ( m_dataSource ){
		restFreq = m_dataSource->_getRestFrequency();
	}
	return restFreq;
}

QSize LayerData::_getSaveSize( const QSize& outputSize,  Qt::AspectRatioMode aspectMode) const {
    QSize saveSize = outputSize;

    //Get the grid margin space.
    int leftMargin = m_dataGrid->_getMargin( LabelFormats::EAST );
    int rightMargin = m_dataGrid->_getMargin( LabelFormats::WEST );
    int topMargin = m_dataGrid->_getMargin( LabelFormats::NORTH );
    int bottomMargin = m_dataGrid->_getMargin( LabelFormats::SOUTH );

    //Get the image dimensions
    Carta::Lib::AxisInfo::KnownType axisXType = m_dataSource->_getAxisXType();
    Carta::Lib::AxisInfo::KnownType axisYType = m_dataSource->_getAxisYType();
    int imageWidth = m_dataSource->_getFrameCount( axisXType );
    int imageHeight = m_dataSource->_getFrameCount( axisYType );


    if ( aspectMode == Qt::KeepAspectRatio || aspectMode == Qt::KeepAspectRatioByExpanding ){
        //Get the width and height after grid margins are subtracted off.
        int outWidth = outputSize.width() - leftMargin - rightMargin;
        int outHeight = outputSize.height() - topMargin - bottomMargin;

        //Get the image dimensions
        if ( m_dataSource ){
            double widthRatio = (outWidth*1.0) / imageWidth;
            double heightRatio = (outHeight*1.0) / imageHeight;
            double ratio = 1;
            if ( aspectMode == Qt::KeepAspectRatio ){
                ratio = qMin( widthRatio, heightRatio );
            }
            else {
                ratio = qMax( widthRatio, heightRatio );
            }
            double saveWidth = ratio * imageWidth + leftMargin + rightMargin;
            double saveHeight = ratio * imageHeight + topMargin + bottomMargin;
            saveSize = QSize( saveWidth, saveHeight );
        }
    }
    else {
        //Just save to the size specified by the user, unless it is too small.
        //If the user specified size is too small, produce an image of minimum size.
        double minWidth = imageWidth +leftMargin + rightMargin;
        double minHeight = imageHeight + topMargin + bottomMargin;
        if ( saveSize.width() < minWidth ){
            saveSize.setWidth( minWidth );
        }
        if ( saveSize.height() < minHeight ){
            saveSize.setHeight( minHeight );
        }
    }
    return saveSize;
}


QString LayerData::_getStateString( bool truncatePaths ) const{
    Carta::State::StateInterface copyState( m_state );
    if ( !truncatePaths ){
        copyState.setValue<QString>(Util::NAME, m_dataSource->_getFileName());
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
        QString colorState( "");
        if ( m_stateColor ){
            colorState =  m_stateColor->getStateString("", SNAPSHOT_PREFERENCES );
        }
        copyState.insertObject( ColorState::CLASS_NAME, colorState );
    }
    QString stateStr = copyState.toString();
    return stateStr;
}

bool LayerData::_getTransform( const QPointF& pan, double zoom, const QSize& size,
		QTransform& tf ) const {
	bool valid = false;
	//where does 0.5, 0.5 map to?
	bool valid1 = false;
	QPointF p1 = m_dataSource->_getScreenPt( { 0.5, 0.5 }, pan, zoom, size,  &valid1 );
	// where does 1.5, 1.5 map to?
	bool valid2 = false;
	QPointF p2 = m_dataSource->_getScreenPt( { 1.5, 1.5 }, pan, zoom, size, &valid2 );
	if ( valid1 && valid2 ){
		valid = true;
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
	}
	return valid;
}


double LayerData::_getZoom() const {
    double zoom = m_state.getValue<double>( Util::ZOOM );
    return zoom;
}

void LayerData::_gridChanged( const Carta::State::StateInterface& state ){
    m_dataGrid->_resetState( state );
}


void LayerData::_initializeState() {
    //Color mix
    m_state.insertObject( MASK );
    QString redKey = Carta::State::UtilState::getLookup( MASK, Util::RED );
    m_state.insertValue<int>( redKey, 255 );
    QString greenKey = Carta::State::UtilState::getLookup( MASK, Util::GREEN );
    m_state.insertValue<int>( greenKey, 255 );
    QString blueKey = Carta::State::UtilState::getLookup( MASK, Util::BLUE );
    m_state.insertValue<int>( blueKey, 255 );
    QString alphaKey = Carta::State::UtilState::getLookup( MASK, Util::ALPHA );
    m_state.insertValue<int>( alphaKey, 255 );
    QString layerColorKey = Carta::State::UtilState::getLookup( MASK, LAYER_COLOR );
    m_state.insertValue<bool>( layerColorKey, false );
    QString layerAlphaKey = Carta::State::UtilState::getLookup( MASK, LAYER_ALPHA );
    m_state.insertValue<bool>( layerAlphaKey, true );

    //Pan and zoom
    m_state.insertValue<double>( Util::ZOOM, DataSource::ZOOM_DEFAULT );
    m_state.insertObject( PAN );
    QString panXKey = Carta::State::UtilState::getLookup( PAN, Util::XCOORD );
    QString panYKey = Carta::State::UtilState::getLookup( PAN, Util::YCOORD );
    m_state.insertValue<double>( panXKey, 0 );
    m_state.insertValue<double>( panYKey, 0 );
}

bool LayerData::_isContourDraw() const {
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

bool LayerData::_isLoadable( const std::vector<int>& frames ) const {
	bool loadable = false;
	if ( m_dataSource ){
		loadable = m_dataSource->_isLoadable( frames );
	}
	return loadable;
}

bool LayerData::_isSpectralAxis() const {
	bool spectralAxis = false;
	if ( m_dataSource ){
		spectralAxis = m_dataSource->_isSpectralAxis();
	}
	return spectralAxis;
}

void LayerData::_load(std::vector<int> frames, bool recomputeClipsOnNewFrame,
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
        if ( m_drawSync ){
        	std::shared_ptr<Carta::Lib::NdArray::RawViewInterface> rawData( m_dataSource->_getRawData( frames ));
        	m_drawSync->setInput( rawData );
        }
    }
}


void LayerData::_removeContourSet( std::shared_ptr<DataContours> contourSet ){
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


void LayerData::_renderingDone(
		QImage image,
		Carta::Lib::VectorGraphics::VGList gridVG,
		Carta::Lib::VectorGraphics::VGList contourVG,
		Carta::Lib::VectorGraphics::VGList regionVG,
		int64_t /*jobId*/){
	/// \todo we should make sure the jobId matches the last submitted job...

	Carta::Lib::VectorGraphics::VGList vectorGraphics;
	QImage qImage;
	if ( !image.isNull()){
		qImage = image;
		Carta::Lib::VectorGraphics::VGComposer comp = Carta::Lib::VectorGraphics::VGComposer( );

		QPointF pan = _getPan();
		double zoom = _getZoom();
		QTransform tf;
		bool valid =  _getTransform( pan, zoom, qImage.size(), tf );
		if ( _isContourDraw() && valid ){
			if ( m_dataSource ){
				comp.append< Carta::Lib::VectorGraphics::Entries::Save >( );
				comp.append< Carta::Lib::VectorGraphics::Entries::SetTransform >( tf );
				comp.appendList( contourVG);
				comp.append< Carta::Lib::VectorGraphics::Entries::Restore >( );
			}
		}
		if ( regionVG.entries().size() > 0 && valid ){
			comp.append< Carta::Lib::VectorGraphics::Entries::Save >( );
			comp.append< Carta::Lib::VectorGraphics::Entries::SetTransform >( tf );
			comp.appendList( regionVG);
			comp.append< Carta::Lib::VectorGraphics::Entries::Restore >( );
		}
		comp.appendList( gridVG);
		vectorGraphics = comp.vgList();
	}

	std::shared_ptr<RenderResponse> response( new RenderResponse(qImage, vectorGraphics, _getLayerId()) );
	emit renderingDone( response );
}




void LayerData::_renderStart(){

    m_renderQueued = true;

    //Get the render parameters from the next request.
    std::shared_ptr<RenderRequest> request = m_renderRequests.pop();
    std::vector<int> frames = request->getFrames();

    Carta::Lib::KnownSkyCS cs = request->getCoordinateSystem();
    bool topOfStack = request->isStackTop();
    QSize outputSize = request->getOutputSize();

    std::shared_ptr<Carta::Lib::IWcsGridRenderService> gridService = m_dataGrid->_getRenderer();
    std::shared_ptr<Carta::Core::ImageRenderService::Service> imageService = m_dataSource->_getRenderer();

    m_dataSource->_viewResize( outputSize );

    //Use the zoom and pan from the render request, if they have been set;
    //otherwise, use the ones from our state.
    double zoom = 0;
    if ( request->isZoomSet() ){
        zoom = request->getZoom();
    }
    else {
        zoom = _getZoom();
    }
    m_dataSource->_setZoom( zoom );

    QPointF center;
    if ( request->isPanSet() ){
        center = request->getPan();
    }
    else {
        center = _getCenterPixel();
    }
    m_dataSource->_setPan( center.x(), center.y());

    gridService-> setOutputSize( outputSize );
    QRectF outputRect = _getOutputRectangle( outputSize, request->isRequestMain(),
            request->isRequestContext() );
    QRectF inputRect = _getInputRectangle( center, zoom, outputRect, outputSize );

    gridService-> setImageRect( inputRect );
    gridService-> setOutputRect( outputRect );


    _load( frames, request->isRecomputeClips(), request->getClipPercentMin(), request->getClipPercentMax());
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

    //Only draw contours and grid for main image.
    if ( request->isRequestMain() ){
        m_drawSync->setContours( m_dataContours );
    }

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

    bool contourDraw = _isContourDraw() && request->isRequestMain();
    bool gridDraw = false;
    if ( topOfStack && request->isRequestMain() ){
    	m_drawSync->setRegionGraphics( m_regionGraphics );
        gridDraw = m_dataGrid->_isGridVisible();
    }
    else {
    	//Empty list
    	Carta::Lib::VectorGraphics::VGList vgList;
    	m_drawSync->setRegionGraphics( vgList );
    }

    m_drawSync-> start( contourDraw, gridDraw );
}


void LayerData::_resetStateContours(const Carta::State::StateInterface& restoreState ){
    int contourCount = restoreState.getArraySize( DataContours::CONTOURS );
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    QStringList supportedContours;

    //Add any contours not there
    for ( int i = 0; i < contourCount; i++ ){
       QString lookup = Carta::State::UtilState::getLookup( DataContours::CONTOURS, i );
       QString nameLookup = Carta::State::UtilState::getLookup( lookup, Util::NAME );
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

void LayerData::_resetState( const Carta::State::StateInterface& restoreState ){
    //Restore the other state variables
    Layer::_resetState( restoreState );

    //Restore the grid
    QString gridStr = restoreState.toString( DataGrid::GRID );
    Carta::State::StateInterface gridState( "" );
    gridState.setState( gridStr );
    _gridChanged( gridState);
    _resetStateContours( restoreState );
    QString colorState = restoreState.toString( ColorState::CLASS_NAME);
    if ( colorState.length() > 0 ){
        //Create a color state if it does not exist.
        if ( ! m_stateColor ){
            Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
            ColorState* cObject = objMan->createObject<ColorState>();

            m_stateColor.reset( cObject);
            connect( m_stateColor.get(), SIGNAL( colorStateChanged()), this, SLOT(_colorChanged()));
        }
        m_stateColor->_resetState( colorState );
    }
    _updateColor();

    //Color mix
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

    m_state.setValue<double>( Util::ZOOM, restoreState.getValue<double>( Util::ZOOM ));
    QString panXKey = Carta::State::UtilState::getLookup( PAN, Util::XCOORD );
    QString panYKey = Carta::State::UtilState::getLookup( PAN, Util::YCOORD );
    m_state.setValue<double>( panXKey, restoreState.getValue<double>( panXKey ));
    m_state.setValue<double>( panYKey, restoreState.getValue<double>( panYKey ));
}


void LayerData::_resetZoom( ){
    _setZoom( DataSource::ZOOM_DEFAULT );
}


void LayerData::_resetPan( ){
    QPointF panValue = m_dataSource->_getCenter();
    _setPan( panValue.x(), panValue.y() );
}


QString LayerData::_setFileName( const QString& fileName, bool * success ){
    QString result = m_dataSource->_setFileName( fileName, success );
    if ( *success){

        //Reset the pan and zoom when the image is loaded.
        _resetPan();
        _resetZoom();

        //Default is to have the layer name match the file name, unless
        //the user has explicitly set it.
        DataLoader* dLoader = Util::findSingletonObject<DataLoader>();
        QString shortName = dLoader->getShortName( fileName );
        QString layerName = m_state.getValue<QString>( Util::NAME );
        if ( layerName.isEmpty() || layerName.length() == 0 ){
            m_state.setValue<QString>( Util::NAME, shortName );
            m_state.flushState();
        }
        result = m_state.getValue<QString>( Util::ID );
    }
    return result;
}

bool LayerData::_setLayersGrouped( bool /*grouped*/, const QSize& /*viewSize*/  ){
    return false;
}


bool LayerData::_setMaskColor( const QString& id, int redAmount,
        int greenAmount, int blueAmount){
    bool changed = false;
    if ( id == _getLayerId() ){
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

void LayerData::_setMaskColorDefault(){
    _setMaskColor( _getLayerId(), 255,255,255);
}


bool LayerData::_setMaskAlpha( const QString& id, int alphaAmount ){
    bool changed = false;
    if ( id == _getLayerId() ){
        QString lookup = Carta::State::UtilState::getLookup( MASK, Util::ALPHA );
        int oldAlpha = m_state.getValue<int>( lookup );
        if ( oldAlpha != alphaAmount ){
            changed = true;
            m_state.setValue<int>( lookup, alphaAmount );
        }
    }
    return changed;
}


void LayerData::_setMaskAlphaDefault(){
    _setMaskAlpha( _getLayerId(), 255);
}


void LayerData::_setPan( double imgX, double imgY ){
    QString panKeyX = Carta::State::UtilState::getLookup( PAN, Util::XCOORD );
    QString panKeyY = Carta::State::UtilState::getLookup( PAN, Util::YCOORD );
    m_state.setValue<double>( panKeyX, imgX );
    m_state.setValue<double>( panKeyY, imgY );
}

void LayerData::_setRegionGraphics( const Carta::Lib::VectorGraphics::VGList& regionVGList ){
	m_regionGraphics = regionVGList;
}

void LayerData::_setSupportAlpha( bool supportAlpha ){
    QString layerAlphaKey = Carta::State::UtilState::getLookup( MASK, LAYER_ALPHA );
    m_state.setValue( layerAlphaKey , supportAlpha );
    if ( !supportAlpha ){
        _setMaskAlphaDefault();
    }
}

void LayerData::_setSupportColor( bool supportColor ){
    QString layerColorKey = Carta::State::UtilState::getLookup( MASK, LAYER_COLOR );
    m_state.setValue( layerColorKey , supportColor );
    if ( !supportColor ){
        _setMaskColorDefault();
    }
}


void LayerData::_setZoom( double zoomAmount){
    double oldZoom = m_state.getValue<double>( Util::ZOOM );
    if ( oldZoom != zoomAmount ){
        m_state.setValue<double>( Util::ZOOM, zoomAmount );
    }
}


void LayerData::_updateClips( std::shared_ptr<Carta::Lib::NdArray::RawViewInterface>& view,
        double minClipPercentile, double maxClipPercentile, const std::vector<int>& frames ){
    if ( m_dataSource ){
        m_dataSource->_updateClips( view,  minClipPercentile, maxClipPercentile, frames );
    }
}

void LayerData::_updateColor(){
    if ( m_dataSource ){
        QString mapName = m_stateColor->_getColorMap();
        m_dataSource->_setColorMap( mapName );
        m_dataSource->_setTransformData( m_stateColor->_getDataTransform() );
        m_dataSource->_setGamma( m_stateColor->_getGamma() );
        bool reversed = m_stateColor->_isReversed();
        m_dataSource->_setColorReversed( reversed );
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

        int redBorder = m_stateColor->_getBorderRed();
        int blueBorder = m_stateColor->_getBorderBlue();
        int greenBorder = m_stateColor->_getBorderGreen();
        int alphaAmount = m_stateColor->_getBorderTransparency();
        m_dataGrid->_setBorderColor( QColor(redBorder, greenBorder, blueBorder, alphaAmount) );
        Layer::_updateColor();
    }
}


LayerData::~LayerData() {
}
}
}
