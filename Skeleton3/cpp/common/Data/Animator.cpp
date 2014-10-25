#include "Data/Animator.h"
#include "Data/Selection.h"
#include "Data/Controller.h"
#include "Data/Util.h"

#include <QDebug>

const QString Animator::IMAGE = "Image";
const QString Animator::CHANNEL = "Channel";
const QString Animator::LINK = "links";
const QString Animator::ANIMATION_TYPE = "animators";

const QString Animator::CLASS_NAME = "edu.nrao.carta.Animator";
bool Animator::m_registered =
    ObjectManager::objectManager()->registerClass (CLASS_NAME,
                                                   new Animator::Factory());

Animator::Animator(const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id),
    m_state( path ){
    _initializeState();
    _initializeCallbacks();
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

QString Animator::_initializeAnimator( const QString& type ){
    QString animatorTypeId;
    if ( type == IMAGE ){
        animatorTypeId = _initAnimator( type );
        connect( m_animators[IMAGE].get(), SIGNAL(indexChanged( const QString&)), this, SLOT(_imageIndexChanged(const QString&)));
    }
    else if ( type == CHANNEL ){
        animatorTypeId = _initAnimator( type );
        connect( m_animators[CHANNEL].get(), SIGNAL(indexChanged( const QString&)), this, SLOT(_channelIndexChanged( const QString&)));
    }
    else {
        qDebug() << "Unrecognized animation type="<<type;
    }
    return animatorTypeId;
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

void Animator::_initializeState(){
    m_state.insertArray(LINK, 0 );
    m_state.insertArray( ANIMATION_TYPE, 1 );
    m_state.setValue<QString>(ANIMATION_TYPE + StateInterface::DELIMITER + QString::number(0), CHANNEL );
    m_state.flushState();
    _initializeAnimator( CHANNEL );
}

void Animator::_imageIndexChanged( const QString& params ){
    for( std::shared_ptr<Controller> controller : m_controllers ){
        controller->setFrameImage( params );
    }
}

void Animator::_channelIndexChanged( const QString& params ){
    for( std::shared_ptr<Controller> controller : m_controllers ){
        controller->setFrameChannel( params );
    }
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

void Animator::_resetAnimationParameters(){
    if ( m_animators.contains( IMAGE) ){
        m_animators[IMAGE]->setUpperBound(m_controllers.size());
    }
    if ( m_animators.contains( CHANNEL)){
       int maxChannel = 0;
       for ( int i = 0; i < m_controllers.size(); i++ ){
           int highKey = m_controllers[i]->getState( CHANNEL, Selection::HIGH_KEY );
           if ( highKey > maxChannel ){
              maxChannel = highKey;
           }
       }
       m_animators[CHANNEL]->setUpperBound( maxChannel );
   }
}

int Animator::getLinkCount() const {
    int linkCount = m_state.getArraySize( LINK );
    return linkCount;
}

QString Animator::getLinkId( int linkIndex ) const {
    QString idStr( LINK + StateInterface::DELIMITER + QString::number(linkIndex));
    QString linkId = m_state.getValue<QString>( idStr );
    return linkId;
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

void Animator::_adjustStateAnimatorTypes(){
    int animationCount = m_animators.size();
    m_state.resizeArray( ANIMATION_TYPE, animationCount );
    QList<QString> keys = m_animators.keys();
    for ( int i = 0; i < animationCount; i++ ){
        m_state.setValue<QString>(ANIMATION_TYPE + StateInterface::DELIMITER + QString::number(i), keys[i] );
    }
    m_state.flushState();
}



