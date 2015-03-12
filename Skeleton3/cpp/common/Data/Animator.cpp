#include "Data/Animator.h"
#include "Data/Selection.h"
#include "Data/Controller.h"
#include "Data/Util.h"

#include <QDebug>

namespace Carta {

namespace Data {

class Animator::Factory : public CartaObjectFactory {

public:

    CartaObject * create (const QString & path, const QString & id)
    {
        return new Animator (path, id);
    }
};



const QString Animator::CLASS_NAME = "Animator";
bool Animator::m_registered =
    ObjectManager::objectManager()->registerClass (CLASS_NAME,
                                                   new Animator::Factory());

Animator::Animator(const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id),
    m_linkImpl( new LinkableImpl( &m_state))
    {
    _initializeState();
    _initializeCallbacks();
}



bool Animator::addLink( CartaObject* cartaObject ){
    Controller* controller = dynamic_cast<Controller*>(cartaObject);
    bool linkAdded = false;
    if ( controller != nullptr ){
        linkAdded = m_linkImpl->addLink( controller );
        if ( linkAdded ){
            connect( controller, SIGNAL(dataChanged(Controller*)), this, SLOT(_adjustStateController(Controller*)) );
        }
    }

    if ( linkAdded ){
        _resetAnimationParameters( -1);
    }
    return linkAdded;
}

void Animator::_adjustStateController( Controller* controller){
    int selectImageIndex = controller->getSelectImageIndex();
    _resetAnimationParameters(selectImageIndex);
}

void Animator::_adjustStateAnimatorTypes(){
    int animationCount = m_animators.size();
    m_state.setObject( AnimatorType::ANIMATIONS );
    QList<QString> keys = m_animators.keys();
    for ( int i = 0; i < animationCount; i++ ){
        if ( !m_animators[keys[i]]->isRemoved()){
            QString objPath = AnimatorType::ANIMATIONS + StateInterface::DELIMITER + keys[i];
            QString val(m_animators[keys[i]]->getStateString());
            m_state.insertObject( objPath, val );
        }
    }
    m_state.flushState();
}



void Animator::_channelIndexChanged( int index ){
    int linkCount = m_linkImpl->getLinkCount();
    for( int i = 0; i < linkCount; i++ ){
        Controller* controller = dynamic_cast<Controller*>( m_linkImpl->getLink(i));
        if ( controller != nullptr ){
            controller->setFrameChannel( index );
        }
    }
}

void Animator::clear(){
    m_linkImpl->clear();
}


int Animator::getLinkCount() const {
    return m_linkImpl->getLinkCount();
}

QList<QString> Animator::getLinks() const {
    return m_linkImpl->getLinkIds();
}


QString Animator::getLinkId( int linkIndex ) const {
    return m_linkImpl->getLinkId( linkIndex );
}

int Animator::_getMaxImageCount() const {
    int linkCount = m_linkImpl->getLinkCount();
    int maxImages = 0;
    for ( int i = 0; i < linkCount; i++ ){
        Controller* controller = dynamic_cast<Controller*>( m_linkImpl->getLink(i));
        if ( controller != nullptr ){
            int imageCount = controller->getStackedImageCount();
            if ( maxImages < imageCount ){
                maxImages = imageCount;
            }
        }
    }
    return maxImages;
}

void Animator::_imageIndexChanged( int selectedImage){
    int linkCount = m_linkImpl->getLinkCount();
    for( int i = 0; i < linkCount; i++ ){
        Controller* controller = dynamic_cast<Controller*>( m_linkImpl->getLink(i));
        if ( controller != nullptr ){
            controller->setFrameImage( selectedImage );
        }
    }
    _resetAnimationParameters(selectedImage);
}


void Animator::_initializeCallbacks(){
    addCommandCallback( "addAnimator", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {"type"};
        std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
        QString animatorId = _initializeAnimator( dataValues[*keys.begin()] );
        return animatorId;
    });

    addCommandCallback( "removeAnimator", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = {"type"};
            std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
            QString animatorId = _removeAnimator( dataValues[*keys.begin()] );
            return animatorId;
        });
}

QString Animator::_initAnimator( const QString& type ){
    QString animId;
    if ( !m_animators.contains( type ) ){
        CartaObject* animObj = Util::createObject( AnimatorType::CLASS_NAME );
        m_animators.insert(type, dynamic_cast<AnimatorType*>(animObj) );
        _adjustStateAnimatorTypes();
    }
    else {
        animId = m_animators[type]->getPath();
    }
    return animId;
}

QString Animator::_initializeAnimator( const QString& type ){
    QString animatorTypeId = "-1";
    if ( !m_animators.contains( type )){
        if ( type == Selection::IMAGE ){
            animatorTypeId = _initAnimator( type );
            connect( m_animators[Selection::IMAGE], SIGNAL(indexChanged( int)), this, SLOT(_imageIndexChanged(int)));
            //Find a controller to use for setting up initial animation
            //parameters.
            int linkCount = m_linkImpl->getLinkCount();
            for ( int i = 0; i < linkCount; i++ ){
                CartaObject* obj = m_linkImpl->getLink( i );
                Controller* controller = dynamic_cast<Controller*>(obj);
                if ( controller != nullptr ){
                    int selectImage = controller->getSelectImageIndex();
                    _resetAnimationParameters( selectImage );
                    break;
                }
            }
        }
        else if ( type == Selection::CHANNEL ){
            animatorTypeId = _initAnimator( type );
            connect( m_animators[Selection::CHANNEL], SIGNAL(indexChanged(int)), this, SLOT(_channelIndexChanged( int)));
        }
        else {
            QString errorMsg = "Unrecognized animation initialization type=" +type;
            Util::commandPostProcess( errorMsg);
        }
    }
    else {
        m_animators[type]->setRemoved( false );
        _adjustStateAnimatorTypes();
        animatorTypeId= m_animators[type]->getPath();
    }
    return animatorTypeId;
}

void Animator::_initializeState(){
    m_state.insertObject( AnimatorType::ANIMATIONS);
    m_state.insertValue<bool>( Util::STATE_FLUSH, false );
    m_state.flushState();
    _initializeAnimator( Selection::CHANNEL );
}

void Animator::refreshState(){
    m_state.setValue<bool>(Util::STATE_FLUSH, true );
    m_state.flushState();
    m_state.setValue<bool>(Util::STATE_FLUSH, false );
}

QString Animator::_removeAnimator( const QString& type ){
    QString result;
    if ( m_animators.contains( type )){
        //m_animators[type] ->getPath();
        //disconnect( m_animators[type].get() );
        //m_animators.remove( type );
        m_animators[type]->setRemoved( true );
        _adjustStateAnimatorTypes();
    }
    else {
        result= "Error removing animator; unrecognized type="+type;
        Util::commandPostProcess( result);
    }
    return result;
}

bool Animator::removeLink( CartaObject* cartaObject ){
    Controller* controller = dynamic_cast<Controller*>(cartaObject);
    bool linkRemoved = false;
    if ( controller != nullptr ){
        linkRemoved = m_linkImpl->removeLink( controller );
        if ( linkRemoved  ){
            disconnect( controller);
            _resetAnimationParameters(-1);
        }
    }
    return linkRemoved;
}



void Animator::_resetAnimationParameters( int selectedImage ){
    if ( m_animators.contains( Selection::IMAGE) ){
        int maxImages = _getMaxImageCount();
        m_animators[Selection::IMAGE]->setUpperBound(maxImages);
        if ( selectedImage >= 0 ){
            m_animators[Selection::IMAGE]->setIndex( selectedImage );
        }
    }
    if ( m_animators.contains( Selection::CHANNEL)){
       int maxChannel = 0;
       int linkCount = m_linkImpl->getLinkCount();
       for ( int i = 0; i < linkCount; i++ ){
           Controller* controller = dynamic_cast<Controller*>( m_linkImpl->getLink(i));
           if ( controller != nullptr ){
               int highKey = controller->getState( Selection::CHANNEL, Selection::HIGH_KEY );
               if ( highKey > maxChannel ){
                  maxChannel = highKey;
               }
           }
       }
       m_animators[Selection::CHANNEL]->setUpperBound( maxChannel );

   }
}

Animator::~Animator(){
    ObjectManager* objMan = ObjectManager::objectManager();
    int animationCount = m_animators.size();
    QList<QString> keys = m_animators.keys();
    for ( int i = 0; i < animationCount; i++ ){
        QString id = m_animators[keys[i]]->getId();
        if ( id.size() > 0 ){
            objMan->destroyObject( id );
        }
    }
    m_animators.clear();
}

}
}

