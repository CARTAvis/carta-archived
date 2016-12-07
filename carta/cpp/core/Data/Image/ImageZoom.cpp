#include "ImageZoom.h"
#include "Controller.h"
#include "Data/Image/Draw/DrawStackSynchronizer.h"
#include "Data/LinkableImpl.h"
#include "Data/Settings.h"
#include "Data/Util.h"
#include "ImageView.h"
#include "State/UtilState.h"
#include <QDebug>

namespace Carta {

namespace Data {

class ImageZoom::Factory : public Carta::State::CartaObjectFactory {

public:

    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new ImageZoom (path, id);
    }
};

const QString ImageZoom::CLASS_NAME = "ImageZoom";

const QString ImageZoom::BOX = "box";
const QString ImageZoom::BOX_VISIBLE = "boxVisible";
const QString ImageZoom::CORNER_0 = "corner0";
const QString ImageZoom::CORNER_1 = "corner1";
const QString ImageZoom::ZOOM_FACTOR = "zoom";
const int ImageZoom::ENLARGE = 20;
const int ImageZoom::PEN_FACTOR = 5;

using Carta::State::UtilState;
using Carta::State::StateInterface;
using Carta::State::CartaObject;

bool ImageZoom::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass (CLASS_NAME,
                                                   new ImageZoom::Factory());

ImageZoom::ImageZoom( const QString& path, const QString& id ):
                CartaObject( CLASS_NAME, path, id ),
                m_linkImpl( new LinkableImpl( path )),
                m_stateData(UtilState::getLookup(path, StateInterface::STATE_DATA)),
                m_zoomDraw( nullptr ){


    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    Settings* settingsObj = objMan->createObject<Settings>();
    m_settings.reset( settingsObj );

    QString viewName = Carta::State::UtilState::getLookup( getPath(), Util::VIEW);
    m_zoomDraw.reset( new DrawStackSynchronizer(makeRemoteView( viewName)));

    _initializeDefaultState();
    _initializeCallbacks();
}

QString ImageZoom::addLink( CartaObject* cartaObject ){
    Controller* controller = dynamic_cast<Controller*>(cartaObject);
    bool linkAdded = false;
    QString result;
    if ( controller != nullptr ){
        linkAdded = m_linkImpl->addLink( controller );
        if ( linkAdded ){

            controller->_setViewDrawZoom( m_zoomDraw );

            connect( m_zoomDraw.get(), SIGNAL(viewResize()), this, SLOT(_zoomChanged()));
            connect( controller, SIGNAL(zoomChanged()), this, SLOT(_zoomChanged()));
        }
    }
    else {
        result = "ImageZoom only supports linking to images";
    }
    return result;
}

void ImageZoom::_clearDraw(){
    setPixelRectangle( QPointF(0,0), QPointF(0,0));
}

void ImageZoom::_clearView(){
    m_zoomDraw->_clear();
}

Controller* ImageZoom::_getControllerSelected() const {
    //We are only supporting one linked controller.
    Controller* controller = nullptr;
    int linkCount = m_linkImpl->getLinkCount();
    for ( int i = 0; i < linkCount; i++ ){
        CartaObject* obj = m_linkImpl->getLink(i );
        Controller* control = dynamic_cast<Controller*>( obj);
        if ( control != nullptr){
            controller = control;
            break;
        }
    }
    return controller;
}

QString ImageZoom::getStateString( const QString& sessionId, SnapshotType type ) const{
	QString result("");
	if ( type == SNAPSHOT_PREFERENCES ){
		StateInterface prefState( "");
		prefState.setValue<QString>(Carta::State::StateInterface::OBJECT_TYPE, CLASS_NAME );
		prefState.insertValue<QString>(Util::PREFERENCES, m_state.toString());
		prefState.insertValue<QString>(Settings::SETTINGS, m_settings->getStateString(sessionId, type) );
		result = prefState.toString();
	}
	else if ( type == SNAPSHOT_DATA ){
		result = m_stateData.toString();
	}
	else if ( type == SNAPSHOT_LAYOUT ){
		result = m_linkImpl->getStateString(getIndex(), getSnapType( type ));
	}
	return result;
}


QList<QString> ImageZoom::getLinks() const {
    return m_linkImpl->getLinkIds();
}

QString ImageZoom::_getPreferencesId() const {
    QString id;
    if ( m_settings.get() != nullptr ){
        id = m_settings->getPath();
    }
    return id;
}


int ImageZoom::getZoomFactor() const {
    return m_state.getValue<int>( ZOOM_FACTOR );
}


void ImageZoom::_initializeDefaultState(){
    m_stateData.insertObject(CORNER_0);
    QString c0X = Carta::State::UtilState::getLookup( CORNER_0, Util::XCOORD );
    QString c0Y = Carta::State::UtilState::getLookup( CORNER_0, Util::YCOORD );
    m_stateData.insertValue<double>( c0X, 0 );
    m_stateData.insertValue<double>( c0Y, 0 );
    m_stateData.insertObject( CORNER_1);
    QString c1X = Carta::State::UtilState::getLookup( CORNER_1, Util::XCOORD );
    QString c1Y = Carta::State::UtilState::getLookup( CORNER_1, Util::YCOORD );
    m_stateData.insertValue<double>( c1X, 0 );
    m_stateData.insertValue<double>( c1Y, 0 );
    m_stateData.setValue<QString>( StateInterface::OBJECT_TYPE, CLASS_NAME + StateInterface::STATE_DATA );
    m_stateData.flushState();

    m_state.insertValue<bool>( BOX_VISIBLE, true );
    m_state.insertValue<int>( ZOOM_FACTOR, ENLARGE );
    m_state.insertValue<int>( Util::TAB_INDEX, 0 );
    m_state.insertValue<int>( "penWidthMax", PEN_FACTOR );
    _initializeDefaultPen( BOX, Util::MAX_COLOR, Util::MAX_COLOR, Util::MAX_COLOR, Util::MAX_COLOR, 1 );
    m_state.flushState();
}


void ImageZoom::_initializeCallbacks(){
    addCommandCallback( "registerPreferences", [=] (const QString & /*cmd*/,
            const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        QString result = _getPreferencesId();
        return result;
    });

    addCommandCallback( "setColor", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {Util::RED, Util::GREEN, Util::BLUE};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );

        bool validRed = false;
        double redValue = dataValues[Util::RED].toInt(&validRed );

        bool validBlue = false;
        double blueValue = dataValues[Util::BLUE].toInt(&validBlue );

        bool validGreen = false;
        double greenValue = dataValues[Util::GREEN].toInt(&validGreen);

        if ( validRed && validBlue && validGreen ){
            result = setBoxColor( redValue, greenValue, blueValue );
        }
        else {
            result = "Invalid RGB values for zoom box: "+params;
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setVisible", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {Util::VISIBLE};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString boxVisibleStr = dataValues[Util::VISIBLE];
        bool validBool = false;
        int boxVisible = Util::toBool( boxVisibleStr, &validBool );
        if ( validBool ){
            setBoxVisible( boxVisible );
        }
        else {
            result = "Visibility of the zoom box must be true/false: " + params;
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setLineWidth", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {Util::WIDTH};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString widthStr = dataValues[Util::WIDTH];
        bool validInt = false;
        int width = widthStr.toInt( &validInt );
        if ( validInt ){
            result = setBoxLineWidth( width );
        }
        else {
            result = "Zoom box line width must be a nonnegative integer : " + params;
        }
        Util::commandPostProcess( result );
        return result;
    });


    addCommandCallback( "setTabIndex", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {Util::TAB_INDEX};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString tabIndexStr = dataValues[Util::TAB_INDEX];
        bool validIndex = false;
        int tabIndex = tabIndexStr.toInt( &validIndex );
        if ( validIndex ){
            result = setTabIndex( tabIndex );
        }
        else {
            result = "Please check that the tab index is a number: " + params;
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setZoomFactor", [=] (const QString & /*cmd*/,
               const QString & params, const QString & /*sessionId*/) -> QString {
           QString result;
           std::set<QString> keys = {ZOOM_FACTOR};
           std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
           QString zoomFactorStr = dataValues[ZOOM_FACTOR];
           bool validIndex = false;
           int zoomFactor = zoomFactorStr.toInt( &validIndex );
           if ( validIndex ){
               result = setZoomFactor( zoomFactor );
           }
           else {
               result = "Please check that the zoom factor is a number: " + params;
           }
           Util::commandPostProcess( result );
           return result;
       });
}


void ImageZoom::_initializeDefaultPen( const QString& key, int red, int green, int blue,
        int alpha, int width ){
    m_state.insertObject( key );
    QString redLookup = Carta::State::UtilState::getLookup( key, Util::RED );
    m_state.insertValue<int>( redLookup, red );
    QString blueLookup = Carta::State::UtilState::getLookup( key, Util::BLUE );
    m_state.insertValue<int>( blueLookup, blue );
    QString greenLookup = Carta::State::UtilState::getLookup( key, Util::GREEN );
    m_state.insertValue<int>( greenLookup, green );
    QString alphaLookup = Carta::State::UtilState::getLookup( key, Util::ALPHA );
    m_state.insertValue<int>( alphaLookup, alpha );
    if ( width >= 0 ){
        QString widthLookup = Carta::State::UtilState::getLookup( key, Util::WIDTH );
        m_state.insertValue<int>( widthLookup, width );
    }
}




bool ImageZoom::isLinked( const QString& linkId ) const {
    bool linked = false;
    CartaObject* obj = m_linkImpl->searchLinks( linkId );
    if ( obj != nullptr ){
        linked = true;
    }
    return linked;
}


QString ImageZoom::removeLink( CartaObject* cartaObject ){
    Controller* controller = dynamic_cast<Controller*>(cartaObject);
    bool linkRemoved = false;
    QString result;
    if ( controller != nullptr ){
        linkRemoved = m_linkImpl->removeLink( controller );
        if ( linkRemoved  ){
            controller->disconnect( this );
            controller->_setViewDrawZoom( std::shared_ptr<DrawStackSynchronizer>( nullptr) );
            _zoomChanged();
        }
    }
    else {
        result = "Image zoom only supports links to images; link could not be removed.";
    }
    return result;
}


void ImageZoom::resetState( const QString& state ){
    Carta::State::StateInterface restoredState( "");
    restoredState.setState( state );

    QString settingStr = restoredState.getValue<QString>(Settings::SETTINGS);
    m_settings->resetStateString( settingStr );

    QString prefStr = restoredState.getValue<QString>(Util::PREFERENCES);
    m_state.setState( prefStr );
    m_state.flushState();
}

QString ImageZoom::setBoxColor( int redValue, int greenValue, int blueValue){
    QString result;
    const QString USER_ID = "View box color ";
       bool greenChanged = _setColor( Util::GREEN, BOX, USER_ID, greenValue, result );
       bool redChanged = _setColor( Util::RED, BOX, USER_ID, redValue, result );
       bool blueChanged = _setColor( Util::BLUE, BOX, USER_ID, blueValue, result );
       if ( redChanged || blueChanged || greenChanged ){
           m_state.flushState();
       }
    return result;
}

QString ImageZoom::setBoxLineWidth( int width ){
    QString result;
    if ( width < 0 || width > PEN_FACTOR ){
        result = "Zoom box line width must be in [0,"+QString::number(PEN_FACTOR)+
                "]: "+QString::number(width);
    }
    else {
        QString lookup = Carta::State::UtilState::getLookup( BOX, Util::WIDTH );
        int oldWidth = m_state.getValue<int>(lookup);
        if ( oldWidth != width ){
            m_state.setValue<int>( lookup, width);
            m_state.flushState();
        }
    }
    return result;
}

void ImageZoom::setBoxVisible( bool visible){
    bool oldVisible = m_state.getValue<bool>( BOX_VISIBLE );
    if ( visible != oldVisible ){
        m_state.setValue<bool>( BOX_VISIBLE, visible );
        m_state.flushState();
    }
}

bool ImageZoom::_setColor( const QString& key, const QString& majorKey,
        const QString& userId, int colorAmount, QString& errorMsg ){
    bool colorChanged = false;
    if ( colorAmount<0 || colorAmount > 255 ){
        errorMsg = errorMsg + userId + " "+key + " must be in [0,255]. ";
    }
    else {
        QString valueKey = Carta::State::UtilState::getLookup( majorKey, key );
        double oldColorAmount = m_state.getValue<int>( valueKey );
        if ( colorAmount != oldColorAmount ){
            m_state.setValue<int>(valueKey, colorAmount );
            colorChanged = true;
        }
    }
    return colorChanged;
}


void ImageZoom::setPixelRectangle( QPointF br, QPointF tl ){
    QString c0X = Carta::State::UtilState::getLookup( CORNER_0, Util::XCOORD );
    QString c0Y = Carta::State::UtilState::getLookup( CORNER_0, Util::YCOORD );
    m_stateData.setValue<double>( c0X, tl.x() );
    m_stateData.setValue<double>( c0Y, tl.y() );
    QString c1X = Carta::State::UtilState::getLookup( CORNER_1, Util::XCOORD );
    QString c1Y = Carta::State::UtilState::getLookup( CORNER_1, Util::YCOORD );
    m_stateData.setValue<double>( c1X, br.x() );
    m_stateData.setValue<double>( c1Y, br.y() );
    m_stateData.flushState();
}

QString ImageZoom::setTabIndex( int index ){
    QString result;
    if ( index >= 0 ){
        int oldIndex = m_state.getValue<int>( Util::TAB_INDEX );
        if ( index != oldIndex ){
            m_state.setValue<int>( Util::TAB_INDEX, index );
            m_state.flushState();
        }
    }
    else {
        result = "Image context settings tab index must be nonnegative: "+ QString::number(index);
    }
    return result;
}


QString ImageZoom::setZoomFactor( int zoomFactor ){
    QString result;
    if ( zoomFactor >= 1 ){
        int oldFactor = m_state.getValue<int>( ZOOM_FACTOR );
        if ( oldFactor != zoomFactor ){
            m_state.setValue<int>( ZOOM_FACTOR, zoomFactor );
            m_state.flushState();
            _zoomChanged();
        }
    }
    return result;
}


void ImageZoom::_zoomChanged(){
    int linkCount = m_linkImpl->getLinkCount();
    if ( linkCount > 0 ){
        Controller* alt = dynamic_cast<Controller*>( m_linkImpl->getLink(0) );
        if ( alt != nullptr ){
            bool valid = false;
            QPointF imgPt = alt->getImagePt( &valid );
            if ( valid ){
                QSize displaySize = alt->_getDisplaySize();

                //Do a zoom if the image pt is inside the image
                if ( 0 <= imgPt.x() && imgPt.x()< displaySize.width() &&
                        0 <= imgPt.y() && imgPt.y()<displaySize.height() ){
                    double zoomFactor = alt->getZoomLevel();
                    zoomFactor = zoomFactor * getZoomFactor();
                    QSize outputSize = m_zoomDraw->getClientSize();
                    double centerX = outputSize.width() / 2;
                    double centerY = outputSize.height() / 2;
                    QPointF topLeft( centerX - zoomFactor / 2, centerY - zoomFactor / 2);
                    QPointF bottomRight( centerX + zoomFactor / 2, centerY + zoomFactor / 2 );

                    //Store the location of the image rectangle.
                    setPixelRectangle(  topLeft, bottomRight );

                    //Redraw it.
                    alt->_renderZoom( zoomFactor );
                }
                //Cursor is off the image so clear.
                else {
                    _clearDraw();
                    _clearView();
                }
            }
            else {
                _clearDraw();
                _clearView();
            }
        }
    }
    //Clear, no linked controller.
    else {
        _clearDraw();
        _clearView();
    }
}


ImageZoom::~ImageZoom(){
    Controller* cont = _getControllerSelected();
    if ( cont ){
        cont->_setViewDrawZoom( std::shared_ptr<DrawStackSynchronizer>( nullptr) );
    }
    unregisterView();
}
}
}
