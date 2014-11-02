#include "Data/Animator.h"
#include "Data/Selection.h"
#include "Data/Controller.h"
#include "Data/Util.h"

#include <QDebug>


const QString Animator::LINK = "links";
//const QString Animator::ANIMATION_TYPE = "animators";

const QString Animator::CLASS_NAME = "Animator";
bool Animator::m_registered =
    ObjectManager::objectManager()->registerClass (CLASS_NAME,
                                                   new Animator::Factory());

Animator::Animator(const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id)
    {
    _initializeState();
    _initializeCallbacks();
}

void Animator::addController( const std::shared_ptr<Controller>& controller ){
    if ( controller ){
        int index = _getIndex( controller );
        if ( index == -1  ){
            m_controllers.push_back( controller );
            connect( controller.get(), SIGNAL(dataChanged()), this, SLOT(_adjustStateController()) );
            _adjustStateController();
            _resetAnimationParameters();
        }
    }
}

void Animator::_adjustStateAnimatorTypes(){
    int animationCount = m_animators.size();
    m_state.setObject( AnimatorType::ANIMATIONS );
    m_state.flushState();
    QList<QString> keys = m_animators.keys();
    for ( int i = 0; i < animationCount; i++ ){
        QString objPath = AnimatorType::ANIMATIONS + StateInterface::DELIMITER + keys[i];
        QString val(m_animators[keys[i]]->getStateString());
        m_state.insertObject( objPath, val );
    }
    m_state.flushState();
}

void Animator::_adjustStateController(){
    _resetAnimationParameters();
    int controllerCount = m_controllers.size();
    m_state.resizeArray( LINK, controllerCount );
    for ( int i = 0; i < controllerCount; i++ ){
        QString idStr( LINK + StateInterface::DELIMITER + QString::number(i));
        m_state.setValue<QString>(idStr, m_controllers[i]->getPath());
    }
    m_state.flushState();
}

void Animator::_channelIndexChanged( const QString& params ){
    for( std::shared_ptr<Controller> controller : m_controllers ){
        controller->setFrameChannel( params );
    }
}

void Animator::clearLinks(){
    m_state.resizeArray( LINK, 0 );
}

int Animator::_getIndex( const std::shared_ptr<Controller>& controller ){
    int index = -1;
    int controllerCount = m_controllers.size();
    QString targetPath = controller->getPath();
    for ( int i = 0; i < controllerCount; i++ ){
        if ( targetPath == m_controllers[i]->getPath()){
           index = i;
           break;
        }
     }
     return index;
}

int Animator::getLinkCount() const {
    int linkCount = m_state.getArraySize( LINK );
    return linkCount;
}

QList<QString> Animator::getLinks() const {
    int linkCount = getLinkCount();
    QList<QString> linkList;
    for ( int i = 0; i < linkCount; i++ ){
        linkList.append(getLinkId( i ));
    }
    return linkList;
}


QString Animator::getLinkId( int linkIndex ) const {
    QString idStr( LINK + StateInterface::DELIMITER + QString::number(linkIndex));
    QString linkId = m_state.getValue<QString>( idStr );
    return linkId;
}

void Animator::_imageIndexChanged( const QString& params ){
    for( std::shared_ptr<Controller> controller : m_controllers ){
        controller->setFrameImage( params );
    }
}


void Animator::_initializeCallbacks(){
    addCommandCallback( "addAnimator", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
        QList<QString> keys = {"type"};
        QVector<QString> dataValues = Util::parseParamMap( params, keys );
        QString animatorId;
        if ( dataValues.size() == keys.size()){
            animatorId = _initializeAnimator( dataValues[0] );
        }
        return animatorId;
    });

    addCommandCallback( "removeAnimator", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
            QList<QString> keys = {"type"};
            QVector<QString> dataValues = Util::parseParamMap( params, keys );
            QString animatorId;
            if ( dataValues.size() == keys.size()){
               animatorId = _removeAnimator( dataValues[0] );
            }
            return animatorId;
        });
}

QString Animator::_initAnimator( const QString& type ){
    QString animId;
    if ( !m_animators.contains( type ) ){
        ObjectManager* objManager = ObjectManager::objectManager();
        animId = objManager->createObject( AnimatorType::CLASS_NAME );
        CartaObject* animObj = objManager->getObject( animId );
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
    if ( type == Selection::IMAGE ){
        animatorTypeId = _initAnimator( type );
        connect( m_animators[Selection::IMAGE].get(), SIGNAL(indexChanged( const QString&)), this, SLOT(_imageIndexChanged(const QString&)));
    }
    else if ( type == Selection::CHANNEL ){
        animatorTypeId = _initAnimator( type );
        connect( m_animators[Selection::CHANNEL].get(), SIGNAL(indexChanged( const QString&)), this, SLOT(_channelIndexChanged( const QString&)));
    }
    else {
        qDebug() << "Unrecognized animation type="<<type;
    }
    return animatorTypeId;
}

void Animator::_initializeState(){
    m_state.insertArray(LINK, 0 );
    m_state.insertObject( AnimatorType::ANIMATIONS);
    m_state.flushState();
    _initializeAnimator( Selection::CHANNEL );
}

QString Animator::_removeAnimator( const QString& type ){
    QString removeId;
    if ( m_animators.contains( type )){
        removeId = m_animators[type] ->getPath();
        disconnect( m_animators[type].get() );
        m_animators.remove( type );
        _adjustStateAnimatorTypes();
    }
    return removeId;
}



void Animator::_resetAnimationParameters(){
    if ( m_animators.contains( Selection::IMAGE) ){
        m_animators[Selection::IMAGE]->setUpperBound(m_controllers.size());
    }
    if ( m_animators.contains( Selection::CHANNEL)){
       int maxChannel = 0;
       for ( int i = 0; i < m_controllers.size(); i++ ){
           int highKey = m_controllers[i]->getState( Selection::CHANNEL, Selection::HIGH_KEY );
           if ( highKey > maxChannel ){
              maxChannel = highKey;
           }
       }
       m_animators[Selection::CHANNEL]->setUpperBound( maxChannel );
   }
}


