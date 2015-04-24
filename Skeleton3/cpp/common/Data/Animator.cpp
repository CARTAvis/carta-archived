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
    m_linkImpl( new LinkableImpl( path ))
    {
    _initializeState();
    _initializeCallbacks();
}



QString Animator::addLink( CartaObject* cartaObject ){
    Controller* controller = dynamic_cast<Controller*>(cartaObject);
    bool linkAdded = false;
    QString result;
    if ( controller != nullptr ){
        linkAdded = m_linkImpl->addLink( controller );
        if ( linkAdded ){
            connect( controller, SIGNAL(dataChanged(Controller*)), this, SLOT(_adjustStateController(Controller*)) );
        }
    }
    else {
        result = "Animator only supports linking to images";
    }

    if ( linkAdded ){
        _resetAnimationParameters( -1);
    }
    return result;
}

void Animator::_adjustStateController( Controller* controller){
    int selectImageIndex = controller->getSelectImageIndex();
    _resetAnimationParameters(selectImageIndex);
}

void Animator::_adjustStateAnimatorTypes(){
    int animationCount = m_animators.size();
    m_state.resizeArray( AnimatorType::ANIMATIONS, animationCount );
    QList<QString> keys = m_animators.keys();
    for ( int i = 0; i < animationCount; i++ ){
        if ( !m_animators[keys[i]]->isRemoved()){
            QString objPath = AnimatorType::ANIMATIONS + StateInterface::DELIMITER + QString::number(i);
            m_state.setValue<QString>( objPath, keys[i] );
        }
    }
    m_state.flushState();
}

void Animator::_channelIndexChanged( int index ){
    changeChannelIndex( index );
}

void Animator::clear(){
    m_linkImpl->clear();
}

QString Animator::addAnimator( const QString& type, QString& animatorTypeId ){
    QString result;
    if ( !m_animators.contains( type )){
        if ( type == Selection::IMAGE ){
            bool animatorAdded = false;
            animatorTypeId = _initAnimator( type, &animatorAdded );
            if ( animatorAdded ){
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
        }
        else if ( type == Selection::CHANNEL ){
            bool animatorAdded = false;
            animatorTypeId = _initAnimator( type, &animatorAdded );
            if ( animatorAdded ){
                connect( m_animators[Selection::CHANNEL], SIGNAL(indexChanged(int)), this, SLOT(_channelIndexChanged( int)));
            }
        }
        else {
            result = "Unrecognized animation initialization type=" +type;
        }
    }
    else {
        m_animators[type]->setVisible( true );
        _adjustStateAnimatorTypes();
        animatorTypeId= m_animators[type]->getPath();
    }
    return result;
}

AnimatorType* Animator::getAnimator( const QString& type ){
    AnimatorType* animator = nullptr;
    if ( m_animators.contains(type ) ){
        animator = m_animators[type];
    }
    else {
        qWarning() << "Unrecognized or non-constructed animation type: "+type;
    }
    return animator;
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

void Animator::changeChannelIndex( int index ){
    int linkCount = m_linkImpl->getLinkCount();
    for( int i = 0; i < linkCount; i++ ){
        Controller* controller = dynamic_cast<Controller*>( m_linkImpl->getLink(i));
        if ( controller != nullptr ){
            controller->setFrameChannel( index );
        }
    }
}

void Animator::changeImageIndex( int selectedImage ){
    int linkCount = m_linkImpl->getLinkCount();
    for( int i = 0; i < linkCount; i++ ){
        Controller* controller = dynamic_cast<Controller*>( m_linkImpl->getLink(i));
        if ( controller != nullptr ){
            controller->setFrameImage( selectedImage );
        }
    }
    _resetAnimationParameters(selectedImage);
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

QString Animator::getStateString( const QString& /*sessionId*/, SnapshotType type ) const{
    QString result;
    if ( type == SNAPSHOT_PREFERENCES ){
        //User preferences should include animators visible (m_state)
        //and individual animator preferences.
        StateInterface prefState("");
        prefState.insertObject( getPath(), m_state.toString());
        QMap<QString, AnimatorType*>::const_iterator animIter;
        for (animIter = m_animators.begin(); animIter != m_animators.end(); ++animIter){
            prefState.insertObject( animIter.key(), animIter.value()->getStatePreferences());
        }
        result = prefState.toString();
    }
    else if ( type == SNAPSHOT_LAYOUT ){
        result = m_linkImpl->getStateString();
    }
    else if ( type == SNAPSHOT_DATA ){
        //Data State is selections on individual animators.
        StateInterface dataState("");
        QMap<QString, AnimatorType*>::const_iterator animIter;
        int animCount = m_animators.size();
        dataState.insertArray(AnimatorType::ANIMATIONS, animCount);
        int i = 0;
        for (animIter = m_animators.begin(); animIter != m_animators.end(); ++animIter){
            QString animKey = Util::getLookup( AnimatorType::ANIMATIONS, i );
            QString animState = animIter.value()->getStateData();
            QString animKeyName = animKey + StateInterface::DELIMITER + "name";
            QString animKeyValue = animKey + StateInterface::DELIMITER + "value";
            dataState.insertValue<QString>( animKeyName, animIter.key() );
            dataState.insertValue<QString>( animKeyValue, animState );
            i++;
        }
        result = dataState.toString();
    }
    return result;
}

void Animator::_imageIndexChanged( int selectedImage){
    changeImageIndex( selectedImage );
}

QString Animator::_initAnimator( const QString& type, bool* newAnimator ){
    QString animId;
    if ( !m_animators.contains( type ) ){
        CartaObject* animObj = Util::createObject( AnimatorType::CLASS_NAME );
        m_animators.insert(type, dynamic_cast<AnimatorType*>(animObj) );
        _adjustStateAnimatorTypes();
        *newAnimator = true;
    }
    else {
        animId = m_animators[type]->getPath();
        *newAnimator = false;
    }
    return animId;
}

void Animator::_initializeCallbacks(){
    addCommandCallback( "addAnimator", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {"type"};
        std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
        QString animId = "-1";
        QString result = addAnimator( dataValues[*keys.begin()], animId );
        Util::commandPostProcess( result );
        return animId;
    });

    addCommandCallback( "removeAnimator", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = {"type"};
            std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
            QString animatorId = removeAnimator( dataValues[*keys.begin()] );
            return animatorId;
        });
}


void Animator::_initializeState(){
    m_state.insertArray( AnimatorType::ANIMATIONS, 0);
    m_state.insertValue<bool>( Util::STATE_FLUSH, false );
    QString animId;
    addAnimator( Selection::CHANNEL, animId);
}

void Animator::refreshState(){
    m_state.setValue<bool>(Util::STATE_FLUSH, true );
    m_state.flushState();
    m_state.setValue<bool>(Util::STATE_FLUSH, false );
}

QString Animator::removeAnimator( const QString& type ){
    QString result;
    if ( m_animators.contains( type )){
        m_animators[type]->setVisible( false );
        _adjustStateAnimatorTypes();
    }
    else {
        result= "Error removing animator; unrecognized type="+type;
        Util::commandPostProcess( result);
    }
    return result;
}

QString Animator::removeLink( CartaObject* cartaObject ){
    Controller* controller = dynamic_cast<Controller*>(cartaObject);
    bool linkRemoved = false;
    QString result;
    if ( controller != nullptr ){
        linkRemoved = m_linkImpl->removeLink( controller );
        if ( linkRemoved  ){
            disconnect( controller);
            _resetAnimationParameters(-1);
        }
    }
    else {
        result = "Animator only supports links to images; link could not be removed.";
    }
    return result;
}



void Animator::_resetAnimationParameters( int selectedImage ){
    if ( m_animators.contains( Selection::IMAGE) ){
        int maxImages = _getMaxImageCount();
        m_animators[Selection::IMAGE]->setUpperBound(maxImages);
        if ( selectedImage >= 0 ){
            m_animators[Selection::IMAGE]->setFrame( selectedImage );
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

void Animator::_resetStateAnimator( const StateInterface& state, const QString& key ){
    try {
        QString animPrefs = state.toString( key );
        if ( animPrefs.length() > 0 ){
            if ( ! m_animators.contains( key )){
                QString animId;
                addAnimator( key , animId );
            }
            m_animators[key]->resetState( animPrefs, SNAPSHOT_PREFERENCES);
        }
        else {
            removeAnimator( key );
        }
    }
    catch( std::invalid_argument& ex ){
        //State did not contain this animator so we remove it.
        removeAnimator( key );
    }
}

void Animator::resetState( const QString& state ){
    StateInterface prefState("");
    prefState.setState( state );
    _resetStateAnimator( prefState, Selection::CHANNEL);
    _resetStateAnimator( prefState, Selection::IMAGE );
    _adjustStateAnimatorTypes();
}

void Animator::resetStateData( const QString& state ){
    StateInterface dataState("");
    dataState.setState( state );
    int animationCount = dataState.getArraySize( AnimatorType::ANIMATIONS );
    for (int i = 0; i < animationCount; i++ ){
        QString lookup = Util::getLookup( AnimatorType::ANIMATIONS, i );
        QString keyName = lookup + StateInterface::DELIMITER + "name";
        QString animName = dataState.getValue<QString>( keyName );
        QString keyValue = lookup + StateInterface::DELIMITER + "value";
        QString stateValue = dataState.getValue<QString>( keyValue );
        if ( m_animators.contains( animName )){
            m_animators[animName]->resetStateData( stateValue );
        }
        else {
            qDebug() << "Unrecognized animator state: "<<stateValue;
        }
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

