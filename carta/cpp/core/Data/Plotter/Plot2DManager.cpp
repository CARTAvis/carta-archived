#include "Plot2DManager.h"
#include "LegendLocations.h"
#include "Data/Error/ErrorManager.h"
#include "Data/Util.h"
#include "Plot2D/Plot2DGenerator.h"
#include "Data/Preferences/PreferencesSave.h"
#include "ImageView.h"
#include "CartaLib/PixelPipeline/IPixelPipeline.h"
#include "State/ObjectManager.h"
#include "State/UtilState.h"
#include <QDir>
#include <QDebug>

namespace Carta {

namespace Data {

const QString Plot2DManager::CLASS_NAME = "Plot2DManager";
const QString Plot2DManager::DATA_PATH = "dataPath";
const QString Plot2DManager::CURSOR_TEXT = "cursorText";


using Carta::State::UtilState;
using Carta::State::StateInterface;
using Carta::State::CartaObject;

Plot2DManager::Plot2DManager( const QString& path, const QString& id ):
        CartaObject( CLASS_NAME, path, id ),
            m_view(nullptr),
            m_plotGenerator( nullptr ),
            m_stateMouse(UtilState::getLookup(path, ImageView::VIEW)){

    m_cursorEnabled = false;

    _initializeDefaultState();
    _initializeCallbacks();

    m_view.reset( new ImageView( path, QColor("yellow"), QImage(), &m_stateMouse));
    connect( m_view.get(), SIGNAL(resize(const QSize&)), this, SLOT(_updateSize(const QSize&)));
    registerView(m_view.get());
    m_selectionEnabled = false;
    m_selectionEnabledColor = false;
}


void Plot2DManager::addData( const Carta::Lib::Hooks::Plot2DResult* data, int index, bool primary ){
    if ( m_plotGenerator ){
        std::vector< std::pair<double,double> > plotData = data->getData();
        const QString& name = data->getName();
        m_plotGenerator->addData( plotData, name, index, primary );
    }
}

void Plot2DManager::addLabels( const std::vector<std::tuple<double,double,QString> >& labels, int index){
    if ( m_plotGenerator ){
        m_plotGenerator->addLabels( labels, index );
    }
}

int Plot2DManager::addPlot(){
    int index = -1;
    if ( m_plotGenerator ){
        index = m_plotGenerator->addPlot();
    }
    updatePlot();
    return index;
}

void Plot2DManager::clearData(){
    if ( m_plotGenerator ){
        m_plotGenerator->clearData();
        updatePlot();
    }
}

void Plot2DManager::clearDataFit(){
    if ( m_plotGenerator ){
        m_plotGenerator->clearDataFit();
        updatePlot();
    }
}

void Plot2DManager::clearLabels( int index ){
    if ( m_plotGenerator ){
        m_plotGenerator->clearLabels( index );
    }
}


void Plot2DManager::clearSelection( int index ){
    if ( m_plotGenerator ){
        m_plotGenerator->clearSelection(index);
        updatePlot();
    }
}


void Plot2DManager::clearSelectionColor( int index ){
    if ( m_plotGenerator ){
        m_plotGenerator->clearSelectionColor( index );
        updatePlot();
    }
}


void Plot2DManager::endSelection(const QString& params ){
    std::set<QString> keys = {Util::XCOORD};
    std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
    QString xstr = dataValues[Util::XCOORD];
    m_selectionEnd = xstr.toDouble();

    if ( m_plotGenerator ){
        m_plotGenerator->setSelectionMode( false );
    }
    m_selectionEnabled = false;
    if ( m_selectionEnd != m_selectionStart ){
        emit userSelection();
    }
}


void Plot2DManager::endSelectionColor(const QString& params ){
    std::set<QString> keys = {Util::XCOORD};
    std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
    QString xstr = dataValues[Util::XCOORD];
    m_selectionEnd = xstr.toDouble();
    if ( m_plotGenerator ){
        m_plotGenerator->setSelectionModeColor( false );
    }
    m_selectionEnabledColor = false;
    if ( m_selectionEnd != m_selectionStart ){
        emit userSelectionColor();
    }
}


QString Plot2DManager::getAxisUnitsY( int index) const {
    QString units="";
    if ( m_plotGenerator ){
        units = m_plotGenerator->getAxisUnitsY( index );
    }
    return units;
}

QPointF Plot2DManager::getImagePoint( const QPointF& screenPoint, bool* valid, int index ) const {
    QPointF imagePt;
    if ( m_plotGenerator ){
        imagePt = m_plotGenerator->getImagePoint( screenPoint, valid, index );
    }
    return imagePt;
}

std::pair<double,double> Plot2DManager::getPlotBoundsY( const QString& id, bool* valid, int index ) const {
    std::pair<double,double> bounds;
    if ( m_plotGenerator ){
        bounds = m_plotGenerator ->getPlotBoundsY( id, valid, index );
    }
    return bounds;
}

QSize Plot2DManager::getPlotSize( int index ) const {
    QSize size;
    if ( m_plotGenerator ){
        size = m_plotGenerator->getPlotSize( index );
    }
    return size;
}

QPointF Plot2DManager::getPlotUpperLeft( int index ) const {
    QPointF pt;
    if ( m_plotGenerator ){
        pt = m_plotGenerator->getPlotUpperLeft( index );
    }
    return pt;
}


QString Plot2DManager::getPlotTitle( int index ) const {
    QString title;
    if ( m_plotGenerator ){
        title = m_plotGenerator->getPlotTitle( index );
    }
    return title;
}


std::pair<double,double> Plot2DManager::getRange( bool* valid, int index ) const {
    std::pair<double,double> range;
    if ( m_plotGenerator ){
        range = m_plotGenerator->getRange( valid, index  );
    }
    return range;
}


std::pair<double,double> Plot2DManager::getRangeColor( bool* valid, int index ) const {
    std::pair<double,double> range;
    if ( m_plotGenerator ){
        range = m_plotGenerator->getRangeColor( valid, index );
    }
    return range;
}

QPointF Plot2DManager::getScreenPoint( const QPointF& dataPoint, bool* valid, int index ) const {
    *valid = false;
    QPointF screenPt;
    if ( m_plotGenerator ){
        *valid = true;
        screenPt = m_plotGenerator->getScreenPoint( dataPoint, index );
    }
    return screenPt;
}


double Plot2DManager::getVLinePosition( bool* valid, int index ) const {
    *valid = false;
    double pos = 0;
    if ( m_plotGenerator ){
        pos = m_plotGenerator->getVLinePosition( valid, index );
    }
    return pos;
}



void Plot2DManager::_initializeDefaultState(){
    m_stateMouse.insertObject( ImageView::MOUSE );
    m_stateMouse.insertValue<QString>(ImageView::MOUSE_X, 0 );
    m_stateMouse.insertValue<QString>(ImageView::MOUSE_Y, 0 );
    m_stateMouse.insertValue<QString>(Util::POINTER_MOVE, "");
    m_stateMouse.insertValue<QString>(CURSOR_TEXT, "");
    m_stateMouse.flushState();
}


void Plot2DManager::_initializeCallbacks(){
    addCommandCallback( "mouseDown", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        startSelection(params);
        return "";
    });

    addCommandCallback( "mouseDownShift", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
        startSelectionColor(params);
        return "";
    });

    addCommandCallback( "mouseUp", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        endSelection( params );
        return "";
    });

    addCommandCallback( "mouseUpShift", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
        endSelectionColor( params );
        return "";
    });

    addCommandCallback( "saveImage", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {DATA_PATH};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString result = savePlot( dataValues[DATA_PATH]);
        if ( !result.isEmpty() ){
            Util::commandPostProcess( result );
        }
        else {
            ErrorManager* hr = Util::findSingletonObject<ErrorManager>();
            hr->registerInformation( "Plot2DManager was successfully saved.");
        }
        return result;
    });

    QString pointerPath= UtilState::getLookup(getPath(), UtilState::getLookup(Util::VIEW, Util::POINTER_MOVE));
    addStateCallback( pointerPath, [=] ( const QString& /*path*/, const QString& value ) {
        QStringList mouseList = value.split( " ");
        if ( mouseList.size() == 4 ){
            bool validX = false;
            int mouseX = mouseList[0].toInt( &validX );
            bool validY = false;
            int mouseY = mouseList[1].toInt( &validY );
            if ( validX && validY ){
                updateSelection( mouseX, mouseY);
            }
        }
    });
}


void Plot2DManager::_refreshView(){
    QImage image = m_plotGenerator->toImage();
    m_view->resetImage( image );
    m_view->scheduleRedraw();
}


void Plot2DManager::removeData( const QString& dataName, int index ){
    if ( m_plotGenerator ){
        m_plotGenerator->removeData( dataName, index );
    }
}

void Plot2DManager::removePlot( int index ){
    if ( m_plotGenerator ){
        m_plotGenerator->removePlot( index );
        updatePlot();
    }
}


QString Plot2DManager::savePlot( const QString& fileName ){
    QString result = "";
    //Check and make sure the directory exists.
    int dirIndex = fileName.lastIndexOf( QDir::separator() );
    QString dirName = fileName;
    if ( dirIndex >= 0 ){
        dirName = fileName.left( dirIndex );
    }
    QDir dir( dirName );
    if ( ! dir.exists() ){
        result = "Please make sure the save path is valid: "+fileName;
    }
    else {
        PreferencesSave* prefSave = Util::findSingletonObject<PreferencesSave>();
        int width = prefSave->getWidth();
        int height = prefSave->getHeight();
        Qt::AspectRatioMode aspectRatioMode = prefSave->getAspectRatioMode();
        QImage imgScaled;
        QImage image;
        if ( aspectRatioMode == Qt::IgnoreAspectRatio ){
            image = m_plotGenerator->toImage( width, height );
            imgScaled = image;
        }
        else {
            image = m_plotGenerator->toImage();
            QSize outputSize( width, height );
            imgScaled = image.scaled( outputSize, aspectRatioMode, Qt::SmoothTransformation );
        }
        bool saveSuccessful = imgScaled.save( fileName, 0, 100 );
        if ( !saveSuccessful ){
            result = "The image could not be saved; please check the path: "+fileName+" is valid.";
        }
        //delete image;
    }
    return result;
}


void Plot2DManager::setAxisXRange( double min, double max ){
    if ( m_plotGenerator ){
        m_plotGenerator->setAxisXRange( min, max);
        updatePlot();
    }
}


void Plot2DManager::setColor( QColor curveColor, const QString& id ){
    if ( m_plotGenerator ){
        m_plotGenerator->setColor( curveColor, id);
        updatePlot();
    }
}


void Plot2DManager::setColored( bool colored, const QString& id, int index ){
    if ( m_plotGenerator ){
        m_plotGenerator->setColored( colored, id, index );
        updatePlot();
    }
}

void Plot2DManager::setCursorEnabled( bool enabled ){
    m_cursorEnabled = enabled;
}

void Plot2DManager::setCursorText( const QString& cursorText ){
    if ( m_stateMouse.getValue<QString>(CURSOR_TEXT) != cursorText ){
        m_stateMouse.setValue<QString>(CURSOR_TEXT, cursorText );
        m_stateMouse.flushState();
    }
}

void Plot2DManager::setCurveName( const QString& oldName, const QString& newName, int index ){
    if ( m_plotGenerator ){
        m_plotGenerator->setCurveName( oldName, newName, index );
        _refreshView();
    }
}


void Plot2DManager::setGridLines( bool showLines ){
    if ( m_plotGenerator ){
        m_plotGenerator->setGridLines( showLines );
        updatePlot();
    }
}

void Plot2DManager::setHLinePosition( double position, int index ){
    if ( m_plotGenerator ){
        m_plotGenerator->setHLinePosition( position, index );
    }
}

void Plot2DManager::setHLineVisible( bool visible, int index ){
    if ( m_plotGenerator ){
        m_plotGenerator->setHLineVisible( visible, index );
    }
}


void Plot2DManager::setLegendLocation( const QString& location ){
    if ( m_plotGenerator ){
        m_plotGenerator->setLegendLocation( location );
        updatePlot();
    }
}


void Plot2DManager::setLegendExternal( bool externalLegend ){
    if ( m_plotGenerator ){
        m_plotGenerator->setLegendExternal( externalLegend );
        updatePlot();
    }
}


void Plot2DManager::setLegendShow( bool showLegend ){
    if ( m_plotGenerator ){
        m_plotGenerator->setLegendVisible( showLegend );
        updatePlot();
    }
}

void Plot2DManager::setLegendLine( bool showLegendLine ){
    if ( m_plotGenerator ){
        m_plotGenerator->setLegendLine( showLegendLine );
        updatePlot();
    }
}


void Plot2DManager::setLineStyle( const QString& style, const QString& id, int index, bool primary ){
    if ( m_plotGenerator ){
        m_plotGenerator->setLineStyle( style, id, index, primary );
        updatePlot();
    }
}


void Plot2DManager::setLogScale( bool logScale, int index ){
    if ( m_plotGenerator ){
        m_plotGenerator->setLogScale( logScale, index );
        updatePlot();
    }
}


void Plot2DManager::setMarkedRange( double minY, double maxY, int index ){
    if ( m_plotGenerator ){
        m_plotGenerator->setMarkedRange( minY, maxY, index);
    }
}


void Plot2DManager::setPipeline( std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> pipeline, int index) {
    if ( m_plotGenerator ){
        m_plotGenerator->setPipeline( pipeline, index );
        updatePlot();
    }
}


void Plot2DManager::setPlotGenerator( Carta::Plot2D::Plot2DGenerator* gen ){
    delete m_plotGenerator;
    m_plotGenerator = gen;
}


void Plot2DManager::setRange( double min, double max, int index ){
    if ( m_plotGenerator ){
        m_plotGenerator->setRange( min, max, index );
    }
}


void Plot2DManager::setRangeColor( double min, double max){
    if ( m_plotGenerator ){
        m_plotGenerator->setRangeColor( min, max );
    }
}


void Plot2DManager::setRangeMarkerVisible( bool visible, int index ){
    if ( m_plotGenerator ){
        m_plotGenerator->setRangeMarkerVisible( visible, index);
    }
}


void Plot2DManager::setStyle( const QString& styleName, const QString& id, int index ){
    if ( m_plotGenerator ){
        m_plotGenerator->setStyle( styleName, id, index );
        updatePlot();
    }
}


void Plot2DManager::setTitleAxisX( const QString& title){
    if ( m_plotGenerator ){
        m_plotGenerator->setTitleAxisX( title);
        updatePlot();
    }
}


void Plot2DManager::setTitleAxisY( const QString& title ){
    if ( m_plotGenerator ){
        m_plotGenerator->setTitleAxisY( title );
        updatePlot();
    }
}


void Plot2DManager::setVLinePosition( double xPos){
    if ( m_plotGenerator ){
        m_plotGenerator->setMarkerLine( xPos );
        updatePlot();
    }
}

void Plot2DManager::setVLineVisible( bool visible ){
    if ( m_plotGenerator ){
        m_plotGenerator->setMarkerLineVisible( visible );
        updatePlot();
    }
}



void Plot2DManager::startSelection(const QString& params ){
    std::set<QString> keys = {Util::XCOORD};
    std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
    QString xstr = dataValues[Util::XCOORD];
    double proposedStart = xstr.toDouble();
    bool onTarget = m_plotGenerator->isSelectionOnCanvas( proposedStart );
    if ( onTarget ){
        m_selectionEnabled = true;
        m_selectionStart = proposedStart;
        if ( m_plotGenerator ){
            m_plotGenerator->setSelectionMode( true );
        }
    }
}


void Plot2DManager::startSelectionColor(const QString& params ){
    std::set<QString> keys = {Util::XCOORD};
    std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
    QString xstr = dataValues[Util::XCOORD];
    m_selectionEnabledColor = true;
    m_selectionStart = xstr.toDouble();
    if ( m_plotGenerator ){
        m_plotGenerator->setSelectionModeColor( true );
    }
}


void Plot2DManager::updatePlot( ){
    if ( m_plotGenerator ){
        //User is selecting a range.
        if ( m_selectionEnabled ){
            m_plotGenerator->setRangePixels( m_selectionStart, m_selectionEnd);
        }
        else if ( m_selectionEnabledColor ){
            m_plotGenerator->setRangePixelsColor( m_selectionStart, m_selectionEnd );
        }

        //Refresh the view
        _refreshView();
    }
}


void Plot2DManager::updateSelection(int x, int y, int index){
    m_selectionEnd = x;
    if ( m_selectionEnabled || m_selectionEnabledColor ){
       updatePlot();
    }
    else {
       if ( m_cursorEnabled ){
           bool valid = false;
           QPointF imageValue = m_plotGenerator->getImagePoint( QPointF(x, y), &valid, index );
           if ( valid ){
               emit cursorMove( imageValue.x(), imageValue.y() );
           }
           else {
               setCursorText( "");
           }
       }
    }
}


void Plot2DManager::_updateSize( const QSize& size ){
    if ( m_plotGenerator ){
        bool newSize = m_plotGenerator->setSize( size.width(), size.height());
        if ( newSize ){
            updatePlot();
            emit plotSizeChanged();
        }
    }
}


Plot2DManager::~Plot2DManager(){
    unregisterView();
    delete m_plotGenerator;
}
}
}
