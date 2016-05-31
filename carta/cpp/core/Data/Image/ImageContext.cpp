#include "ImageContext.h"
#include "Controller.h"
#include "Data/Image/Draw/DrawStackSynchronizer.h"
#include "Data/LinkableImpl.h"
#include "Data/Util.h"
#include "ImageView.h"

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

const QString ImageContext::CLASS_NAME = "ImageContext";
const QString ImageContext::CURSOR_TEXT = "cursorText";


using Carta::State::UtilState;
using Carta::State::StateInterface;
using Carta::State::CartaObject;

bool ImageContext::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass (CLASS_NAME,
                                                   new ImageContext::Factory());

ImageContext::ImageContext( const QString& path, const QString& id ):
                CartaObject( CLASS_NAME, path, id ),
                m_linkImpl( new LinkableImpl( path )),
                m_stateMouse(UtilState::getLookup(path, ImageView::VIEW)){

    _initializeDefaultState();
    _initializeCallbacks();

    //m_view.reset( new ImageView( path, QColor("yellow"), QImage(), &m_stateMouse));
    //connect( m_view.get(), SIGNAL(resize(const QSize&)), this, SLOT(_updateSize(const QSize&)));
    //registerView(m_view.get());
}


QString ImageContext::addLink( CartaObject* cartaObject ){
    Controller* controller = dynamic_cast<Controller*>(cartaObject);
    bool linkAdded = false;
    QString result;
    if ( controller != nullptr ){
        linkAdded = m_linkImpl->addLink( controller );
        if ( linkAdded ){
            QString viewName = Carta::State::UtilState::getLookup( getPath(), Util::VIEW);
            m_contextDraw = new DrawStackSynchronizer(makeRemoteView( viewName));
            controller->_setViewDrawContext( contextDraw );
            connect( m_contextDraw.get(), SIGNAL(viewResize()), this, SLOT(_viewResize()));
        }
    }
    else {
        result = "Image context only supports linking to images";
    }
    return result;
}

QList<QString> ImageContext::getLinks() const {
    return m_linkImpl->getLinkIds();
}

void ImageContext::_initializeDefaultState(){
    m_stateMouse.insertObject( ImageView::MOUSE );
    m_stateMouse.insertValue<QString>(ImageView::MOUSE_X, 0 );
    m_stateMouse.insertValue<QString>(ImageView::MOUSE_Y, 0 );
    m_stateMouse.insertValue<QString>(Util::POINTER_MOVE, "");

    m_stateMouse.flushState();
}


void ImageContext::_initializeCallbacks(){
    addCommandCallback( "mouseDown", [=] (const QString & /*cmd*/,
            const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        //startSelection(params);
        return "";
    });

    addCommandCallback( "mouseDownShift", [=] (const QString & /*cmd*/,
                const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        //startSelectionColor(params);
        return "";
    });

    addCommandCallback( "mouseUp", [=] (const QString & /*cmd*/,
            const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        //endSelection( params );
        return "";
    });

    addCommandCallback( "mouseUpShift", [=] (const QString & /*cmd*/,
                const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        //endSelectionColor( params );
        return "";
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
           // _resetAnimationParameters(-1);
        }
    }
    else {
        result = "Image context only supports links to images; link could not be removed.";
    }
    return result;
}


void ImageContext::_viewResize(){
    int linkCount = m_linkImpl->getLinkCount();
    if ( linkCount > 0 ){
        Controller* alt = dynamic_cast<Controller*>( m_linkImpl->getLink(0) );
        qDebug() << "Imagecontext renderZoom";
        alt->_renderZoom();
    }
}


ImageContext::~ImageContext(){
    unregisterView();
}
}
}
