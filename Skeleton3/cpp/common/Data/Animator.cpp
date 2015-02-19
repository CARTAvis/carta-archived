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
    m_linkImpl( new LinkableImpl( &m_state ))
    {
    _initializeState();
    _initializeCallbacks();
}



bool Animator::addLink( const std::shared_ptr<Controller>& controller ){
    bool linkAdded = m_linkImpl->addLink( controller );
    if ( linkAdded ){
        connect( controller.get(), SIGNAL(dataChanged()), this, SLOT(_adjustStateController()) );
        _resetAnimationParameters();
    }
    return linkAdded;
}

void Animator::_adjustStateController(){
    _resetAnimationParameters();
    m_linkImpl->_adjustStateController();
}

void Animator::_adjustStateAnimatorTypes(){
    int animationCount = m_animators.size();
    m_state.setObject( AnimatorType::ANIMATIONS );
    QList<QString> keys = m_animators.keys();
    for ( int i = 0; i < animationCount; i++ ){
        QString objPath = AnimatorType::ANIMATIONS + StateInterface::DELIMITER + keys[i];
        QString val(m_animators[keys[i]]->getStateString());
        m_state.insertObject( objPath, val );
    }
    m_state.flushState();
}

void Animator::changeChannelIndex( const QString& params ){
    _channelIndexChanged( params );   
}

void Animator::_channelIndexChanged( const QString& params ){
    qDebug() << "(JT) Animator::_channelIndexChanged()";
    qDebug() << "(JT) params = " << params;
    for( std::shared_ptr<Controller> controller : m_linkImpl->m_controllers ){
        controller->setFrameChannel( params );
    }
}

void Animator::clear(){
    m_linkImpl->clear();
}


int Animator::getLinkCount() const {
    return m_linkImpl->getLinkCount();
}

QList<QString> Animator::getLinks() const {
    return m_linkImpl->getLinks();
}


QString Animator::getLinkId( int linkIndex ) const {
    return m_linkImpl->getLinkId( linkIndex );
}

void Animator::_imageIndexChanged( const QString& params ){
    qDebug() << "(JT) Animator::_imageIndexChanged()";
    qDebug() << "(JT) params = " << params;
    for( std::shared_ptr<Controller> controller : m_linkImpl->m_controllers ){
        controller->setFrameImage( params );
    }
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
        shared_ptr<AnimatorType> val(dynamic_cast<AnimatorType*>(animObj));
        m_animators.insert(type, val );
        _adjustStateAnimatorTypes();
    }
    else {
        animId = m_animators[type]->getPath();
    }
    return animId;
}

QString Animator::_initializeAnimator( const QString& type ){
    QString animatorTypeId;
    if ( !m_animators.contains( type )){
        if ( type == Selection::IMAGE ){
            animatorTypeId = _initAnimator( type );
            connect( m_animators[Selection::IMAGE].get(), SIGNAL(indexChanged( const QString&)), this, SLOT(_imageIndexChanged(const QString&)));
        }
        else if ( type == Selection::CHANNEL ){
            animatorTypeId = _initAnimator( type );
            connect( m_animators[Selection::CHANNEL].get(), SIGNAL(indexChanged( const QString&)), this, SLOT(_channelIndexChanged( const QString&)));
        }
        else {
            QString errorMsg = "Unrecognized animation initialization type=" +type;
            qWarning() << errorMsg;
            animatorTypeId = Util::commandPostProcess( errorMsg, "-1");
        }
    }
    else {
        animatorTypeId= m_animators[type]->getPath();
    }
    return animatorTypeId;
}

void Animator::_initializeState(){
    m_state.insertObject( AnimatorType::ANIMATIONS);
    m_state.flushState();
    _initializeAnimator( Selection::CHANNEL );
}

QString Animator::_removeAnimator( const QString& type ){
    QString result;
    if ( m_animators.contains( type )){
        m_animators[type] ->getPath();
        disconnect( m_animators[type].get() );
        m_animators.remove( type );
        _adjustStateAnimatorTypes();
    }
    else {
        result= "Error removing animator; unrecognized type="+type;
        qWarning() << result;
        result = Util::commandPostProcess( result, "");
    }
    return result;
}

bool Animator::removeLink( const std::shared_ptr<Controller>& controller ){
    bool linkRemoved = m_linkImpl->removeLink( controller );
    if ( linkRemoved  ){
        disconnect( controller.get(), SIGNAL(dataChanged()), this, SLOT(_adjustStateController()) );
        _resetAnimationParameters();
    }
    return linkRemoved;
}

void Animator::_resetAnimationParameters(){
    if ( m_animators.contains( Selection::IMAGE) ){
        m_animators[Selection::IMAGE]->setUpperBound(m_linkImpl->getLinkCount());
    }
    if ( m_animators.contains( Selection::CHANNEL)){
       int maxChannel = 0;
       for( std::shared_ptr<Controller> controller : m_linkImpl->m_controllers ){
           int highKey = controller->getState( Selection::CHANNEL, Selection::HIGH_KEY );
           if ( highKey > maxChannel ){
              maxChannel = highKey;
           }
       }
       m_animators[Selection::CHANNEL]->setUpperBound( maxChannel );
   }
}
}
}

