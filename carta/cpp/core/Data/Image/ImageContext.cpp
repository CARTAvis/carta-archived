#include "ImageContext.h"
#include "Controller.h"
#include "Data/Image/Draw/DrawStackSynchronizer.h"
#include "Data/Image/ImageContext.h"
#include "Data/LinkableImpl.h"
#include "Data/Settings.h"
#include "Data/Util.h"
#include "ImageView.h"

#include "State/ObjectManager.h"
#include "State/UtilState.h"

#include <QDebug>

namespace Carta {

namespace Data {

class ImageContext::Factory : public Carta::State::CartaObjectFactory {

public:

    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new ImageContext(path, id);
    }
};

const QString ImageContext::BOX = "box";
const QString ImageContext::BOX_VISIBLE = "boxVisible";
const QString ImageContext::CLASS_NAME = "ImageContext";
const QString ImageContext::COMPASS_VISIBLE_XY = "compassVisibleXY";
const QString ImageContext::COMPASS_VISIBLE_NE = "compassVisibleNE";
const QString ImageContext::COMPASS_XY = "compassXY";
const QString ImageContext::COMPASS_NE = "compassNE";
const QString ImageContext::CURSOR_TEXT = "cursorText";
const QString ImageContext::CORNER_0 = "corner0";
const QString ImageContext::CORNER_1 = "corner1";
const QString ImageContext::MODE = "mode";
const int ImageContext::PEN_FACTOR = 5;



using Carta::State::UtilState;
using Carta::State::StateInterface;
using Carta::State::CartaObject;

bool ImageContext::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass (CLASS_NAME,
                                                   new ImageContext::Factory());

ImageContext::ImageContext( const QString& path, const QString& id ):
                CartaObject( CLASS_NAME, path, id ),
                m_linkImpl( new LinkableImpl( path )),
                m_stateMouse(UtilState::getLookup(path, ImageView::VIEW)),
                m_stateData(UtilState::getLookup(path, StateInterface::STATE_DATA)),
                m_contextDraw( nullptr ){
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    Settings* settingsObj = objMan->createObject<Settings>();
    m_settings.reset( settingsObj );


    _initializeDefaultState();
    _initializeCallbacks();
}


QString ImageContext::addLink( CartaObject* cartaObject ){
    Controller* controller = dynamic_cast<Controller*>(cartaObject);
    bool linkAdded = false;
    QString result;
    if ( controller != nullptr ){
        linkAdded = m_linkImpl->addLink( controller );
        if ( linkAdded ){
            QString viewName = Carta::State::UtilState::getLookup( getPath(), Util::VIEW);
            m_contextDraw.reset( new DrawStackSynchronizer(makeRemoteView( viewName)));
            controller->_setViewDrawContext( m_contextDraw );
            connect( m_contextDraw.get(), SIGNAL(viewResize()), this, SLOT(_contextChanged()));
            connect( controller, SIGNAL(dataChanged(Controller*)), this, SLOT(_contextChanged()));
            connect( controller, SIGNAL(contextChanged()), this, SLOT(_contextChanged()));
        }
    }
    else {
        result = "Image context only supports linking to images";
    }
    return result;
}

void ImageContext::_contextChanged(){
    int linkCount = m_linkImpl->getLinkCount();
    if ( linkCount > 0 ){
        Controller* alt = dynamic_cast<Controller*>( m_linkImpl->getLink(0) );
        if ( alt != nullptr ){
            //Get the size of the image.
            QSize imageSize = alt->_getDisplaySize();
            double outputHeight = imageSize.height();
            double outputWidth = imageSize.width();
            if ( outputHeight > 1 && outputWidth > 1 ){
                //Get the rectangle inside the image that is visible in the
                //main view.
                QRectF inputRect = alt->_getInputRectangle();
                QPointF topLeft = inputRect.topLeft();
                QPointF bottomRight = inputRect.bottomRight();

                //Size of the context window.
                QSize contextSize = m_contextDraw->getClientSize();
                double contextCenterX = contextSize.width();
                double contextCenterY = contextSize.height();

                //Account for margins between the view window and the actual image.
                double translateX = (contextCenterX - outputWidth) / 2;
                double translateY = (contextCenterY - outputHeight) / 2;
                double topY = topLeft.y() + translateY;
                double leftX = topLeft.x() + translateX;
                QPointF topLeftTranslate( leftX, topY );
                double bottomY = bottomRight.y() + translateY;
                double rightX = bottomRight.x() + translateX;
                QPointF bottomRightTranslate( rightX, bottomY );

                //Store the location of the image rectangle.
                setImageRectangle(  bottomRightTranslate, topLeftTranslate );

                //Redraw it.
                alt->_renderContext();
            }
        }
    }
}


Controller* ImageContext::_getControllerSelected() const {
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

QList<QString> ImageContext::getLinks() const {
    return m_linkImpl->getLinkIds();
}

QString ImageContext::_getPreferencesId() const {
    QString id;
    if ( m_settings.get() != nullptr ){
        id = m_settings->getPath();
    }
    return id;
}

QString ImageContext::getStateString( const QString& sessionId, SnapshotType type ) const{
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
    return result;
}

void ImageContext::_initializeDefaultPen( const QString& key, int red, int green, int blue,
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
        QString widthLookup = Carta::State::UtilState::getLookup( key, Util::PEN_WIDTH );
        m_state.insertValue<int>( widthLookup, width );
    }
}


void ImageContext::_initializeDefaultState(){
    m_stateMouse.insertObject( ImageView::MOUSE );
    m_stateMouse.insertValue<QString>(ImageView::MOUSE_X, 0 );
    m_stateMouse.insertValue<QString>(ImageView::MOUSE_Y, 0 );
    m_stateMouse.insertValue<QString>(Util::POINTER_MOVE, "");
    m_stateMouse.flushState();

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
    m_stateData.flushState();

    m_state.insertValue<bool>( BOX_VISIBLE, true );
    m_state.insertValue<bool>( COMPASS_VISIBLE_XY, true );
    m_state.insertValue<bool>( COMPASS_VISIBLE_NE, true );
    m_state.insertValue<int>( Util::TAB_INDEX, 0 );
    m_state.insertValue<int>( "penWidthMax", PEN_FACTOR );
    _initializeDefaultPen( BOX, Util::MAX_COLOR, Util::MAX_COLOR, Util::MAX_COLOR, Util::MAX_COLOR, 1 );
    _initializeDefaultPen( COMPASS_XY, Util::MAX_COLOR, Util::MAX_COLOR, Util::MAX_COLOR, Util::MAX_COLOR, 1 );
    _initializeDefaultPen( COMPASS_NE, Util::MAX_COLOR, Util::MAX_COLOR, Util::MAX_COLOR, Util::MAX_COLOR, 1 );
    m_state.flushState();
}


void ImageContext::_initializeCallbacks(){
    addCommandCallback( "mouseDown", [=] (const QString & /*cmd*/,
            const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        return "";
    });

    addCommandCallback( "mouseDownShift", [=] (const QString & /*cmd*/,
                const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        return "";
    });

    addCommandCallback( "mouseUp", [=] (const QString & /*cmd*/,
            const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        return "";
    });

    addCommandCallback( "mouseUpShift", [=] (const QString & /*cmd*/,
                const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        return "";
    });

    addCommandCallback( "registerPreferences", [=] (const QString & /*cmd*/,
                                const QString & /*params*/, const QString & /*sessionId*/) -> QString {
            QString result = _getPreferencesId();
            return result;
       });

    addCommandCallback( "setColor", [=] (const QString & /*cmd*/,
                               const QString & params, const QString & /*sessionId*/) -> QString {
           QString result;
           qDebug() << "Got box color params="<<params;
           std::set<QString> keys = {Util::RED, Util::GREEN, Util::BLUE, MODE};
           std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );

           bool validRed = false;
           double redValue = dataValues[Util::RED].toInt(&validRed );

           bool validBlue = false;
           double blueValue = dataValues[Util::BLUE].toInt(&validBlue );

           bool validGreen = false;
           double greenValue = dataValues[Util::GREEN].toInt(&validGreen);

           if ( validRed && validBlue && validGreen ){
               bool validMode = false;
               int mode = dataValues[MODE].toInt( &validMode );
               if ( validMode ){
                   if ( mode == IMAGE_BOX ){
                       result = setBoxColor( redValue, greenValue, blueValue );
                   }
               }
               else {
                   result = "Unrecognized context mode: "+params;
               }
           }
           else {
               result = "Context view box color values must be in [0,255]: "+params;
           }
           Util::commandPostProcess( result );
           return result;
       });

    addCommandCallback( "setVisible", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) -> QString {
                QString result;
                std::set<QString> keys = {Util::VISIBLE, MODE};
                std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                QString boxVisibleStr = dataValues[Util::VISIBLE];
                bool validBool = false;
                int boxVisible = Util::toBool( boxVisibleStr, &validBool );
                if ( validBool ){
                    bool validMode = false;
                    int mode = dataValues[MODE].toInt( &validMode );
                    if ( validMode ){
                        if ( mode == IMAGE_BOX ){
                            setBoxVisible( boxVisible );
                        }
                    }
                    else {
                        result = "Unrecognized context mode: "+params;
                    }
                }
                else {
                    result = "Visibility of the view box must be true/false: " + params;
                }
                Util::commandPostProcess( result );
                return result;
            });

    addCommandCallback( "setLineWidth", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        qDebug() << "Got line width command params="<<params;
        std::set<QString> keys = {Util::PEN_WIDTH, MODE};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString widthStr = dataValues[Util::PEN_WIDTH];
        bool validInt = false;
        int width = widthStr.toInt( &validInt );
        if ( validInt ){
            bool validMode = false;
            int mode = dataValues[MODE].toInt( &validMode );
            if ( validMode ){
                if ( mode == IMAGE_BOX ){
                    result = setBoxLineWidth( width );
                }
            }
            else {
                result = "Unrecognized context mode: " + params;
            }
        }
        else {
            result = "View box line width must be a nonnegative integer : " + params;
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


    QString pointerPath= UtilState::getLookup(getPath(), UtilState::getLookup(Util::VIEW, Util::POINTER_MOVE));
    addStateCallback( pointerPath, [=] ( const QString& /*path*/, const QString& value ) {
        QStringList mouseList = value.split( " ");
        if ( mouseList.size() == 4 ){
            /*bool validX = false;
            int mouseX = mouseList[0].toInt( &validX );
            bool validY = false;
            int mouseY = mouseList[1].toInt( &validY );
            bool validWidth = false;
            int width = mouseList[2].toInt( &validWidth );
            bool validHeight = false;
            int height = mouseList[3].toInt( &validHeight );
            if ( validX && validY && validWidth && validHeight ){
                updateSelection( mouseX, mouseY, width, height);
            }*/
        }
    });
}




bool ImageContext::isLinked( const QString& linkId ) const {
    bool linked = false;
    CartaObject* obj = m_linkImpl->searchLinks( linkId );
    if ( obj != nullptr ){
        linked = true;
    }
    return linked;
}


QString ImageContext::removeLink( CartaObject* cartaObject ){
    Controller* controller = dynamic_cast<Controller*>(cartaObject);
    bool linkRemoved = false;
    QString result;
    if ( controller != nullptr ){
        linkRemoved = m_linkImpl->removeLink( controller );
        if ( linkRemoved  ){
            controller->disconnect( this );
            controller->_setViewDrawContext( std::shared_ptr<DrawStackSynchronizer>( nullptr) );
        }
    }
    else {
        result = "Image context only supports links to images; link could not be removed.";
    }
    return result;
}


void ImageContext::resetState( const QString& state ){
    Carta::State::StateInterface restoredState( "");
    restoredState.setState( state );

    QString settingStr = restoredState.getValue<QString>(Settings::SETTINGS);
    m_settings->resetStateString( settingStr );

    QString prefStr = restoredState.getValue<QString>(Util::PREFERENCES);
    m_state.setState( prefStr );
    m_state.flushState();
}



QString ImageContext::setBoxColor( int redValue, int greenValue, int blueValue){
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

QString ImageContext::setBoxLineWidth( int width ){
    QString result;
    if ( width < 0 || width > PEN_FACTOR ){
        result = "Image box line width must be in [0,"+QString::number(PEN_FACTOR)+
                "]: "+QString::number(width);
    }
    else {
        QString lookup = Carta::State::UtilState::getLookup( BOX, Util::PEN_WIDTH );
        int oldWidth = m_state.getValue<int>(lookup);
        if ( oldWidth != width ){
            m_state.setValue<int>( lookup, width);
            m_state.flushState();
        }
    }
    return result;
}

void ImageContext::setBoxVisible( bool visible){
    bool oldVisible = m_state.getValue<bool>( BOX_VISIBLE );
    if ( visible != oldVisible ){
        m_state.setValue<bool>( BOX_VISIBLE, visible );
        m_state.flushState();
    }
}

bool ImageContext::_setColor( const QString& key, const QString& majorKey,
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
void ImageContext::setImageRectangle( QPointF br, QPointF tl ){
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

QString ImageContext::setTabIndex( int index ){
    QString result;
    qDebug() << "Setting tab index="<<index;
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



ImageContext::~ImageContext(){
    unregisterView();
}
}
}
