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
#include <math.h>

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
const QString ImageContext::BOX_SELECTED = "boxSelected";
const QString ImageContext::CLASS_NAME = "ImageContext";
const QString ImageContext::COMPASS_VISIBLE_XY = "compassVisibleXY";
const QString ImageContext::COMPASS_VISIBLE_NE = "compassVisibleNE";
const QString ImageContext::COMPASS_XY = "compassXY";
const QString ImageContext::COMPASS_NE = "compassNE";
const QString ImageContext::COORD_ROTATION = "rotate";
const QString ImageContext::CURSOR_TEXT = "cursorText";
const QString ImageContext::CORNER_0 = "corner0";
const QString ImageContext::CORNER_1 = "corner1";
const QString ImageContext::IMAGE_WIDTH = "imageWidth";
const QString ImageContext::IMAGE_HEIGHT = "imageHeight";
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
    QString viewName = Carta::State::UtilState::getLookup( getPath(), Util::VIEW);
    m_contextDraw.reset( new DrawStackSynchronizer(makeRemoteView( viewName)));
    _initializeCallbacks();
}



QString ImageContext::addLink( CartaObject* cartaObject ){
    Controller* controller = dynamic_cast<Controller*>(cartaObject);
    bool linkAdded = false;
    QString result;
    if ( controller != nullptr ){
        linkAdded = m_linkImpl->addLink( controller );
        if ( linkAdded ){
            controller->_setViewDrawContext( m_contextDraw );
            connect( m_contextDraw.get(), SIGNAL(viewResize()), this, SLOT(_contextChanged()));
            //connect( controller, SIGNAL(dataChanged(Controller*)), this, SLOT(_contextChanged()));
            connect( controller, SIGNAL(contextChanged()), this, SLOT(_contextChanged()));
            _contextChanged();
        }
    }
    else {
        result = "Image context only supports linking to images";
    }
    return result;
}

void ImageContext::_clearDraw(){
    //Reset the context draw parameters so they will not be shown.
    setImageRectangle( QPointF(0, 0), QPointF(0, 0) );
    m_stateData.setValue<QString>( StateInterface::OBJECT_TYPE, CLASS_NAME + StateInterface::STATE_DATA );
    m_stateData.setValue<int>( IMAGE_WIDTH, 0 );
    m_stateData.setValue<int>( IMAGE_HEIGHT, 0 );
    m_stateData.flushState();
}

void ImageContext::_clearView(){
    m_contextDraw->_clear();
}

void ImageContext::_contextChanged(){
	if ( !m_mouseDown ){
		int linkCount = m_linkImpl->getLinkCount();
		if ( linkCount > 0 ){
			Controller* alt = dynamic_cast<Controller*>( m_linkImpl->getLink(0) );
			if ( alt != nullptr ){
				//Get the size of the image.
				QSize imageSize = alt->_getDisplaySize();
				double outputHeight = imageSize.height();
				double outputWidth = imageSize.width();
				if ( outputHeight > 1 && outputWidth > 1 ){

					//Size of the context window.
					QSize contextSize = m_contextDraw->getClientSize();
					double contextWidth = contextSize.width();
					double contextHeight = contextSize.height();

					//Actual output size should be the minimum of the image and the
					//context rectangle - we don't want an image larger that the view
					//that can hold it.  However, we do want to preserve the image dimensions.
					double actualHeight = outputHeight;
					double actualWidth = outputWidth;
					double zoomFactor = 1;
					//Determine whether width or height is the most constrained factor.
					double widthRatio = contextWidth / outputWidth;
					double heightRatio = contextHeight / outputHeight;
					//Height is the more constrained resource.
					if ( heightRatio < widthRatio ){
						//Choose height to be the smallest of image and window height.
						actualHeight = qMin( contextHeight, outputHeight );
						//Determine how much it has been constrained
						zoomFactor = actualHeight / outputHeight;
						//Use the ratio to get width
						actualWidth = outputWidth * zoomFactor;
					}
					else {
						actualWidth = qMin( contextWidth, outputWidth );
						zoomFactor = actualWidth / outputWidth;
						actualHeight = outputHeight * zoomFactor;
					}

					if ( zoomFactor > 0 ){
						bool contextChanged = false;
						int oldWidth = m_stateData.getValue<int>( IMAGE_WIDTH );
						int oldHeight = m_stateData.getValue<int>( IMAGE_HEIGHT );
						if ( oldWidth != actualWidth || oldHeight != actualHeight ){
							m_stateData.setValue<int>( IMAGE_WIDTH, actualWidth );
							m_stateData.setValue<int>( IMAGE_HEIGHT, actualHeight );
							contextChanged = true;
						}

						//Get the rectangle inside the image that is visible in the
						//main view.
						QRectF inputRect = alt->_getInputRectangle();
						QPointF topLeft = inputRect.topLeft();
						QPointF bottomRight = inputRect.bottomRight();

						//Scale the input rectangle to the context zoom.
						QPointF topLeftZoomed (topLeft.x() * zoomFactor, topLeft.y() * zoomFactor );
						QPointF bottomRightZoomed( bottomRight.x() * zoomFactor,bottomRight.y() * zoomFactor );

						//Account for margins between the view window and the actual image.
						double translateX = (contextWidth - actualWidth) / 2;
						double translateY = (contextHeight - actualHeight) / 2;
						double topY = topLeftZoomed.y() + translateY;
						double leftX = topLeftZoomed.x() + translateX;
						QPointF topLeftTranslate( leftX, topY );
						double bottomY = bottomRightZoomed.y() + translateY;
						double rightX = bottomRightZoomed.x() + translateX;
						QPointF bottomRightTranslate( rightX, bottomY );

						//Store the location of the image rectangle.
						QRectF oldRect = getImageRectangle();
						if ( oldRect.bottomRight() != bottomRightTranslate || oldRect.topLeft() != topLeftTranslate ){
							setImageRectangle(  bottomRightTranslate, topLeftTranslate );
							contextChanged = true;
						}

						//Get the native value at the center
						bool validCenter = false;
						QPointF imageCenterWorld = alt->getWorldCoordinates( outputWidth/2, outputHeight/2, &validCenter );

						//Get the native value to the left of center.
						bool validLeft = false;
						QPointF imageLeftWorld = alt->getWorldCoordinates( 0, outputHeight/2, &validLeft );
						if ( validLeft && validCenter ){
							//Calculate the angle.
							double ratio = ( imageCenterWorld.y() - imageLeftWorld.y() ) /
									( imageCenterWorld.x() - imageLeftWorld.x() );
							double angle = atan ( ratio );
							double oldAngle = m_stateData.getValue<double>(COORD_ROTATION);
							if ( angle != oldAngle ){
								m_stateData.setValue<double>( COORD_ROTATION, angle );
								contextChanged = true;
							}
						}
						if ( contextChanged ){
							m_stateData.flushState();
							//Redraw it.
							alt->_renderContext( zoomFactor );
						}
					}
				}
				else {
					_clearDraw();
					_clearView();
				}
			}
		}
		//No controllers so just clear the view.
		else {
			_clearDraw();
			_clearView();
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

QRectF ImageContext::getImageRectangle() const {
    QString c0X = Carta::State::UtilState::getLookup( CORNER_0, Util::XCOORD );
    QString c0Y = Carta::State::UtilState::getLookup( CORNER_0, Util::YCOORD );
    double x0 = m_stateData.getValue<double>( c0X );
    double y0 = m_stateData.getValue<double>( c0Y );
    QString c1X = Carta::State::UtilState::getLookup( CORNER_1, Util::XCOORD );
    QString c1Y = Carta::State::UtilState::getLookup( CORNER_1, Util::YCOORD );
    double x1 = m_stateData.getValue<double>( c1X );
    double y1 = m_stateData.getValue<double>( c1Y );
    QRectF rect( QPointF( x0, y0), QPointF( x1, y1 ));
    return rect;
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
    else if ( type == SNAPSHOT_LAYOUT ){
           result = m_linkImpl->getStateString(getIndex(), getSnapType( type ));
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
        QString widthLookup = Carta::State::UtilState::getLookup( key, Util::WIDTH );
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

    m_stateData.insertValue<double>(COORD_ROTATION, 0);
    m_stateData.insertValue<bool>( BOX_SELECTED, false );
    m_stateData.insertValue<int>( IMAGE_WIDTH, 0 );
    m_stateData.insertValue<int>( IMAGE_HEIGHT, 0 );
    m_stateData.flushState();

    m_state.insertValue<bool>( BOX_VISIBLE, true );
    m_state.insertValue<bool>( COMPASS_VISIBLE_XY, true );
    m_state.insertValue<bool>( COMPASS_VISIBLE_NE, true );
    m_state.insertValue<int>( Util::TAB_INDEX, 0 );
    m_state.insertValue<int>( "penWidthMax", PEN_FACTOR );
    _initializeDefaultPen( BOX, 0, 0, 255, Util::MAX_COLOR, 1 );
    _initializeDefaultPen( COMPASS_XY, 255, 255, 0, Util::MAX_COLOR, 1 );
    _initializeDefaultPen( COMPASS_NE, 0, 255, 0, Util::MAX_COLOR, 1 );
    m_state.flushState();
}


void ImageContext::_initializeCallbacks(){
    addCommandCallback( "mouseDown", [=] (const QString & /*cmd*/,
            const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        m_mouseDown = true;
        return "";
    });

    addCommandCallback( "mouseDownShift", [=] (const QString & /*cmd*/,
                const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        m_mouseDown = true;
        return "";
    });

    addCommandCallback( "mouseUp", [=] (const QString & /*cmd*/,
            const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        m_mouseDown = false;
        return "";
    });

    addCommandCallback( "mouseUpShift", [=] (const QString & /*cmd*/,
                const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        m_mouseDown = false;
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
                   else if ( mode == ARROW_XY ){
                       result = setCompassXYColor( redValue, greenValue, blueValue );
                   }
                   else if ( mode == ARROW_NE ){
                       result = setCompassNEColor( redValue, greenValue, blueValue );
                   }
                   else {
                       qWarning()<<"Unrecognized context draw element: "<<mode;
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
                QString visibleStr = dataValues[Util::VISIBLE];
                bool validBool = false;
                int visible = Util::toBool( visibleStr, &validBool );
                if ( validBool ){
                    bool validMode = false;
                    int mode = dataValues[MODE].toInt( &validMode );
                    if ( validMode ){
                        if ( mode == IMAGE_BOX ){
                            setBoxVisible( visible );
                        }
                        else if ( mode == ARROW_XY ){
                            setCompassXYVisible( visible );
                        }
                        else if ( mode == ARROW_NE ){
                            setCompassNEVisible( visible );
                        }
                        else {
                            qWarning()<<"Unrecognized context draw element: "<<mode;
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
        std::set<QString> keys = {Util::WIDTH, MODE};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString widthStr = dataValues[Util::WIDTH];
        bool validInt = false;
        int width = widthStr.toInt( &validInt );
        if ( validInt ){
            bool validMode = false;
            int mode = dataValues[MODE].toInt( &validMode );
            if ( validMode ){
                if ( mode == IMAGE_BOX ){
                    result = setBoxLineWidth( width );
                }
                else if ( mode == ARROW_XY ){
                    result = setCompassXYLineWidth( width );
                }
                else if ( mode == ARROW_NE ){
                    result = setCompassNELineWidth( width );
                }
                else {
                    qWarning()<<"Unrecognized context draw element: "<<mode;
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
        if ( mouseList.size() >= 2 ){
            bool validX = false;
            int mouseX = mouseList[0].toInt( &validX );
            bool validY = false;
            int mouseY = mouseList[1].toInt( &validY );
            if ( validX && validY ){
                _updateSelection( mouseX, mouseY);
            }
        }
    });
}

void ImageContext::_updateSelection( int mouseX, int mouseY ){
    //Update whether or not the image rectangle is selected.
    QRectF imageRect = getImageRectangle();
    bool inside = false;
    if ( imageRect.contains( QPointF( mouseX, mouseY) ) ){
        inside = true;
    }
    bool oldSelected = m_stateData.getValue<bool>( BOX_SELECTED );
    if ( oldSelected != inside ){
        m_stateData.setValue<bool>( BOX_SELECTED, inside );
        m_stateData.flushState();
    }

    //Decide if we should move the context rectangle.
    if ( m_mouseDown && inside ){
        double oldCenterX = imageRect.x() + imageRect.width() / 2;
        double oldCenterY = imageRect.y() + imageRect.height() / 2;
        double moveX = mouseX - oldCenterX;
        double moveY = mouseY - oldCenterY;
        QPointF topLeft = imageRect.topLeft();
        QPointF bottomRight = imageRect.bottomRight();
        QPointF newTopLeft( topLeft.x() + moveX, topLeft.y() + moveY );
        QPointF newBottomRight( bottomRight.x() + moveX, bottomRight.y() + moveY );
        setImageRectangle(newBottomRight, newTopLeft );
        _updateImageView( newTopLeft, newBottomRight );
    }
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

            _clearDraw();
            _contextChanged();
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
    QString result = _setDrawColor( redValue, greenValue, blueValue, BOX, "View box color ");
    return result;
}

QString ImageContext::setCompassXYColor( int redValue, int greenValue, int blueValue){
    QString result = _setDrawColor( redValue, greenValue, blueValue, COMPASS_XY, "Compass xy color ");
    return result;
}

QString ImageContext::setCompassNEColor( int redValue, int greenValue, int blueValue){
    QString result = _setDrawColor( redValue, greenValue, blueValue, COMPASS_NE, "Compass NE color ");
    return result;
}

QString ImageContext::_setDrawColor( int redValue, int greenValue, int blueValue,
        const QString& key, const QString& userID ){
    QString result;
    bool greenChanged = _setColor( Util::GREEN, key, userID, greenValue, result );
    bool redChanged = _setColor( Util::RED, key, userID, redValue, result );
    bool blueChanged = _setColor( Util::BLUE, key, userID, blueValue, result );
    if ( redChanged || blueChanged || greenChanged ){
        m_state.flushState();
    }
    return result;
}

QString ImageContext::setCompassNELineWidth( int width ){
    QString result = _setLineWidth( COMPASS_NE, "Compass NE", width );
    return result;
}

QString ImageContext::setCompassXYLineWidth( int width ){
    QString result = _setLineWidth( COMPASS_XY, "Compass XY", width );
    return result;
}

QString ImageContext::setBoxLineWidth( int width ){
    QString result = _setLineWidth( BOX, "Image box", width );
    return result;
}

QString ImageContext::_setLineWidth( const QString& key, const QString& userName, int width ){
    QString result;
    if ( width < 0 || width > PEN_FACTOR ){
        result = userName + " line width must be in [0,"+QString::number(PEN_FACTOR)+
                "]: "+QString::number(width);
    }
    else {
        QString lookup = Carta::State::UtilState::getLookup( key, Util::WIDTH );
        int oldWidth = m_state.getValue<int>(lookup);
        if ( oldWidth != width ){
            m_state.setValue<int>( lookup, width);
            m_state.flushState();
        }
    }
    return result;
}

void ImageContext::setBoxVisible( bool visible){
    _setVisible( visible, BOX_VISIBLE );
}

void ImageContext::setCompassNEVisible( bool visible){
    _setVisible( visible, COMPASS_VISIBLE_NE );
}

void ImageContext::setCompassXYVisible( bool visible){
    _setVisible( visible, COMPASS_VISIBLE_XY );
}

void ImageContext::_setVisible( bool visible, const QString& key){
    bool oldVisible = m_state.getValue<bool>( key );
    if ( visible != oldVisible ){
        m_state.setValue<bool>( key, visible );
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
	//Ignore if nans
	if ( br.x() == br.x() && br.y() == br.y() && tl.x() == tl.x() && tl.y() == tl.y() ){
		QString c0X = Carta::State::UtilState::getLookup( CORNER_0, Util::XCOORD );
		QString c0Y = Carta::State::UtilState::getLookup( CORNER_0, Util::YCOORD );
		double oldC0X = m_stateData.getValue<double>( c0X );
		double oldC0Y = m_stateData.getValue<double>( c0Y );
		bool rectChanged = false;
		if ( oldC0X != tl.x() || oldC0Y != tl.y() ){
			m_stateData.setValue<double>( c0X, tl.x() );
			m_stateData.setValue<double>( c0Y, tl.y() );
			rectChanged = true;
		}
		QString c1X = Carta::State::UtilState::getLookup( CORNER_1, Util::XCOORD );
		QString c1Y = Carta::State::UtilState::getLookup( CORNER_1, Util::YCOORD );
		double oldC1X = m_stateData.getValue<double>( c1X );
		double oldC1Y = m_stateData.getValue<double>( c1Y );
		if ( oldC1X != br.x() || oldC1Y != br.y() ){
			m_stateData.setValue<double>( c1X, br.x() );
			m_stateData.setValue<double>( c1Y, br.y() );
			rectChanged = true;
		}
		if ( rectChanged ){
			m_stateData.flushState();
		}
	}
}

void ImageContext::_updateImageView( const QPointF& topLeft, const QPointF& bottomRight ){
	//Change pixels to image coordinates.
	Controller* controller = _getControllerSelected();
	if ( controller ){
		QSize clientSize = m_contextDraw->getClientSize();
		bool tlValid = false;
		QPointF imageTL = controller->_getContextPt( topLeft, clientSize, &tlValid );
		bool brValid = false;
		QPointF imageBR = controller->_getContextPt( bottomRight, clientSize, &brValid );
		if ( tlValid && brValid ){
			double centerX = ( imageTL.x() + imageBR.x() ) / 2;
			double centerY = ( imageTL.y() + imageBR.y() ) / 2;
			controller->centerOnPixel( centerX, centerY);
		}
	}
}

QString ImageContext::setTabIndex( int index ){
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



ImageContext::~ImageContext(){
    unregisterView();
}
}
}
