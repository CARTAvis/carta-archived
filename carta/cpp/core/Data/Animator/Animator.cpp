#include "Animator.h"
#include "Data/Selection.h"
#include "Data/Image/Controller.h"
#include "Data/Image/Grid/AxisMapper.h"
#include "Data/Util.h"
#include "State/UtilState.h"
#include "CartaLib/CartaLib.h"

#include <QDebug>

namespace Carta {

namespace Data {

using Carta::State::StateInterface;
using Carta::State::UtilState;
using Carta::Lib::AxisInfo;

class Animator::Factory : public Carta::State::CartaObjectFactory {

public:

    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new Animator (path, id);
    }
};


const QString Animator::CLASS_NAME = "Animator";
const QString Animator::TYPE = "type";
bool Animator::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass (CLASS_NAME,
                                                   new Animator::Factory());

Animator::Animator(const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id),
    m_linkImpl( new LinkableImpl( path )){
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
            connect( controller, SIGNAL(dataChanged(Controller*)),
                    this, SLOT(_adjustStateController(Controller*)) );
            connect( controller, SIGNAL(axesChanged()),
                    this, SLOT(_axesChanged()));
        }
    }
    else {
        result = "Animator only supports linking to images";
    }

    if ( linkAdded ){
        //_resetAnimationParameters( -1);
        _adjustStateController( controller );
    }
    return result;
}



void Animator::_adjustStateController( Controller* controller){
    int selectImageIndex = controller->getSelectImageIndex();
    _updateSupportedZAxes( controller );
    _resetAnimationParameters(selectImageIndex);
}

void Animator::_adjustStateAnimatorTypes(){
    int animationCount = m_animators.size();
    int animVisibleCount = _getAnimatorTypeVisibleCount();
    m_state.resizeArray( AnimatorType::ANIMATIONS, animVisibleCount, StateInterface::PreserveNone );
    QList<QString> keys = m_animators.keys();
    int j = 0;
    for ( int i = 0; i < animationCount; i++ ){
        if ( !m_animators[keys[i]]->isRemoved()){
            QString arrayPath = UtilState::getLookup(AnimatorType::ANIMATIONS, QString::number(j));
            QString typePath = UtilState::getLookup( arrayPath, TYPE );
            m_state.insertValue<QString>( typePath, keys[i] );
            QString visiblePath = UtilState::getLookup( arrayPath, Util::VISIBLE );
            m_state.insertValue<bool>( visiblePath, m_animators[keys[i]]->isVisible() );
            j++;
        }
    }
    m_state.flushState();
}

bool Animator::_addAnimatorType( const QString& type, QString& animatorTypeId ){
    bool animatorAdded = false;
    animatorTypeId = _initAnimator( type, &animatorAdded );
    if ( animatorAdded ){
        connect( m_animators[type], SIGNAL(indexChanged( int, const QString&)),
                this, SLOT(_frameChanged(int, const QString&)));
    }
    return animatorAdded;
}



QString Animator::addAnimator( const QString& type, QString& animatorTypeId ){
    QString result;
    if ( !m_animators.contains( type )){
        bool animatorAdded = _addAnimatorType( type, animatorTypeId );
        if ( animatorAdded ){
            if ( type == Selection::IMAGE ){
                //Find a controller to use for setting up initial animation
                //parameters.
                Controller* controller = _getControllerSelected();
                if ( controller != nullptr ){
                    int selectImage = controller->getSelectImageIndex();
                    _resetAnimationParameters( selectImage );
                }
            }
            else {
                _updateAnimatorBound( type );
            }
        }
    }
    else {
        m_animators[type]->setVisible( true );
        _adjustStateAnimatorTypes();
        animatorTypeId= m_animators[type]->getPath();
    }
    return result;
}

void Animator::_addRemoveImageAnimator(){
    int maxImages = _getMaxImageCount();
    if ( maxImages > 1 ){
        QString animId;
        addAnimator( Selection::IMAGE, animId );
    }
    else {
        removeAnimator( Selection::IMAGE );
    }
}

void Animator::_axesChanged(){
    //Go through the list of controllers and get the list of available animators.
    //Add any that are not present.
    int linkCount = m_linkImpl->getLinkCount();
    QSet<QString> existingAnimators;
    for( int i = 0; i < linkCount; i++ ){
        Controller* controller = dynamic_cast<Controller*>( m_linkImpl->getLink(i));
        if ( controller != nullptr ){
            std::set<AxisInfo::KnownType> zAxes = controller->_getAxesHidden();
            for ( std::set<AxisInfo::KnownType>::iterator it = zAxes.begin();
                    it != zAxes.end(); it++ ){
                const Carta::Lib::KnownSkyCS& cs = controller->getCoordinateSystem();
                QString purpose = AxisMapper::getPurpose( *it, cs );
                QString animId;
                addAnimator( purpose, animId );
                existingAnimators.insert( purpose );
            }
        }
    }

    //Remove any existing animators if they are no longer supported.
    QList<QString> keys = m_animators.keys();
    int animCount = keys.size();
    for ( int i = 0; i < animCount; i++ ){
        QString animType = m_animators[keys[i]]->getType();
        bool existing = existingAnimators.contains( animType );
        if ( !existing && animType != Selection::IMAGE ){
            removeAnimator( animType );
        }
    }
}

void Animator::changeFrame( int index, const QString& animName ){
    int linkCount = m_linkImpl->getLinkCount();
    for( int i = 0; i < linkCount; i++ ){
        Controller* controller = dynamic_cast<Controller*>( m_linkImpl->getLink(i));
        if ( controller != nullptr ){
            if ( animName == Selection::IMAGE ){
                controller->setFrameImage( index );
            }
            else {
                AxisInfo::KnownType axisType = AxisMapper::getType( animName );
                if ( axisType != AxisInfo::KnownType::OTHER ){
                    controller->_setFrameAxis( index, axisType );
                }
            }
        }
    }
}

void Animator::clear(){
    m_linkImpl->clear();
}

void Animator::_frameChanged( int index, const QString& axisName ){
    changeFrame( index, axisName );
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

int Animator::_getAnimatorTypeVisibleCount() const {
    int count = 0;
    QList<QString> keys = m_animators.keys();
    int animationCount = keys.size();
    for ( int i = 0; i < animationCount; i++ ){
        if ( !m_animators[keys[i]]->isRemoved() ){
            count++;
        }
    }
    return count;
}

Controller* Animator::_getControllerSelected() const {
    Controller* controller = nullptr;
    if ( m_animators.contains( Selection::IMAGE ) ){
        int imageIndex = m_animators[Selection::IMAGE]->getIndex();
        controller = dynamic_cast<Controller*>(m_linkImpl->getLink( imageIndex));
    }
    else {
        //Find the first controller that has an image.
        int linkCount = m_linkImpl->getLinkCount();
        for ( int i = 0; i < linkCount; i++ ){
            Controller* control = dynamic_cast<Controller*>( m_linkImpl->getLink(i) );
            if ( control != nullptr ){
                int imageCount = control->getStackedImageCount();
                if ( imageCount > 0 ){
                    controller = control;
                    break;
                }
            }
        }
    }

    return controller;
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


int Animator::getMaxImageCount() const {
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

int Animator::_getMaxImageCount() const {
    int maxImages = getMaxImageCount();
    return maxImages;
}

QString Animator::getStateString( const QString& /*sessionId*/, SnapshotType type ) const{
    QString result;
    if ( type == SNAPSHOT_PREFERENCES ){
        //User preferences should include animators visible (m_state)
        //and individual animator preferences.
        StateInterface prefState("");
        prefState.setState( /*getPath(),*/ m_state.toString());
        QMap<QString, AnimatorType*>::const_iterator animIter;
        for (animIter = m_animators.begin(); animIter != m_animators.end(); ++animIter){
            prefState.insertObject( animIter.key(), animIter.value()->getStatePreferences());
        }
        result = prefState.toString();
    }
    else if ( type == SNAPSHOT_LAYOUT ){
        result = m_linkImpl->getStateString(getIndex(), getSnapType( type ) );
    }
    else if ( type == SNAPSHOT_DATA ){
        //Data State is selections on individual animators.
        StateInterface dataState("");
        QMap<QString, AnimatorType*>::const_iterator animIter;
        int animCount = m_animators.size();
        dataState.setValue<int>(StateInterface::INDEX, getIndex() );
        dataState.insertArray(AnimatorType::ANIMATIONS, animCount);
        dataState.setValue<QString>( StateInterface::OBJECT_TYPE,
                CLASS_NAME + StateInterface::STATE_DATA );
        int i = 0;
        for (animIter = m_animators.begin(); animIter != m_animators.end(); ++animIter){
            QString animKey = Carta::State::UtilState::getLookup( AnimatorType::ANIMATIONS, i );
            QString animState = animIter.value()->getStateData();
            QString animKeyName = UtilState::getLookup(animKey, "name");
            QString animKeyValue = UtilState::getLookup(animKey, "value");
            dataState.insertValue<QString>( animKeyName, animIter.key() );
            dataState.insertValue<QString>( animKeyValue, animState );

            i++;
        }
        result = dataState.toString();
    }
    return result;
}

QString Animator::getSnapType(CartaObject::SnapshotType snapType) const {
    QString objType = CartaObject::getSnapType( snapType );
    if ( snapType == SNAPSHOT_DATA ){
        objType = objType + StateInterface::STATE_DATA;
    }
    return objType;
}


QString Animator::_initAnimator( const QString& type, bool* newAnimator ){
    QString animId;
    if ( !m_animators.contains( type ) ){
        Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
        AnimatorType* animObj = objMan->createObject<AnimatorType>();
        animObj->_setType( type );
        m_animators.insert(type, animObj );
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
        std::map<QString,QString> dataValues = UtilState::parseParamMap( params, keys );
        QString animId = "-1";
        QString result = addAnimator( dataValues[*keys.begin()], animId );
        Util::commandPostProcess( result );
        return animId;
    });

    addCommandCallback( "removeAnimator", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = {"type"};
            std::map<QString,QString> dataValues = UtilState::parseParamMap( params, keys );
            QString animatorId = removeAnimator( dataValues[*keys.begin()] );
            return animatorId;
        });
}


void Animator::_initializeState(){
    m_state.insertArray( AnimatorType::ANIMATIONS, 0);
}

bool Animator::isLinked( const QString& linkId ) const {
    bool linked = false;
    CartaObject* obj = m_linkImpl->searchLinks( linkId );
    if ( obj != nullptr ){
        linked = true;
    }
    return linked;
}


void Animator::refreshState(){
    CartaObject::refreshState();
    QList<QString> animKeys = m_animators.keys();
    for ( int i = 0; i < animKeys.size(); i++ ){
        m_animators[animKeys[i]]->refreshState();
    }
    m_linkImpl->refreshState();
}

QString Animator::removeAnimator( const QString& type ){
    QString result;
    if ( m_animators.contains( type )){
        m_animators[type]->setVisible( false );
        _adjustStateAnimatorTypes();
    }
    else if ( type != Selection::IMAGE ){
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
            controller->disconnect( this );
            _resetAnimationParameters(-1);
        }
    }
    else {
        result = "Animator only supports links to images; link could not be removed.";
    }
    return result;
}


void Animator::_resetAnimationParameters( int selectedImage ){
    _addRemoveImageAnimator();
    if ( m_animators.contains( Selection::IMAGE) ){
        int maxImages = _getMaxImageCount();
        if ( maxImages == 0 ){
            m_animators[Selection::IMAGE]->setUpperBound( 1 );
        }
        else {
            m_animators[Selection::IMAGE]->setUpperBound(maxImages);
        }
        if ( selectedImage >= 0 ){
            m_animators[Selection::IMAGE]->setFrame( selectedImage );
        }
        else {
            int index = m_animators[Selection::IMAGE]->getFrame();
            if ( index > maxImages ){
                m_animators[Selection::IMAGE]->setIndex( 0 );
            }
        }
    }
    _updateAnimatorBounds();
}


void Animator::_resetStateAnimator( const Carta::State::StateInterface& state, const QString& key ){
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
    Carta::State::StateInterface prefState("");
    prefState.setState( state );
    _resetStateAnimator( prefState, Selection::CHANNEL);
    _resetStateAnimator( prefState, Selection::IMAGE );
    _adjustStateAnimatorTypes();
}

void Animator::resetStateData( const QString& state ){
    Carta::State::StateInterface dataState("");
    dataState.setState( state );
    int animationCount = dataState.getArraySize( AnimatorType::ANIMATIONS );
    for (int i = 0; i < animationCount; i++ ){
        QString lookup = UtilState::getLookup( AnimatorType::ANIMATIONS, i );
        QString keyName = UtilState::getLookup(lookup, "name");
        QString animName = dataState.getValue<QString>( keyName );
        QString keyValue = UtilState::getLookup(lookup, "value");
        QString stateValue = dataState.getValue<QString>( keyValue );
        if ( m_animators.contains( animName )){
            m_animators[animName]->resetStateData( stateValue );
        }
        else {
            qDebug() << "Unrecognized animator state: "<<stateValue;
        }
    }
}

bool Animator::_setAnimatorVisibility( const QString& key, bool visible ){
    bool visibilityChanged = false;
    if ( m_animators.contains( key ) ){
        bool animVisible = m_animators[key]->isVisible();
        if ( animVisible != visible ){
           m_animators[key]->setVisible( visible );
           visibilityChanged = true;
        }
    }
    return visibilityChanged;
}

bool Animator::_updateAnimatorBound( const QString& key ){
    int maxFrame = 0;
    int currentFrame = 0;
    bool visibilityChanged = false;
    AxisInfo::KnownType axisType = AxisMapper::getType( key );
    std::vector<AxisInfo::KnownType> animationAxes;
    Controller* controller = _getControllerSelected();
    if ( controller != nullptr ){
        maxFrame = controller->getFrameUpperBound( axisType );
        currentFrame = controller->getFrame( axisType );
        animationAxes = controller->_getAxisZTypes();
    }
    m_animators[key]->setUpperBound( maxFrame );
    m_animators[key]->setFrame( currentFrame );

    //Decide the visibility of the animator based on whether it is an animation axis
    int animAxisCount = animationAxes.size();
    if ( animAxisCount > 0 ){
        bool axisFound = false;
        if ( controller != nullptr ){
            for ( int i = 0; i < animAxisCount; i++ ){
                const Carta::Lib::KnownSkyCS& cs = controller->getCoordinateSystem();
                QString animPurpose = AxisMapper::getPurpose( animationAxes[i], cs );
                if ( animPurpose == key ){
                    axisFound = true;
                    break;
                }
            }
        }

        //Okay we will set the animator visible if it has at least one
        //frame.
        if ( axisFound ){
            if ( maxFrame > 1 ){
                visibilityChanged = _setAnimatorVisibility( key, true );
            }
            else {
                visibilityChanged = _setAnimatorVisibility( key, false );
            }
        }
        else {
            visibilityChanged = _setAnimatorVisibility( key, false );
        }
    }
    return visibilityChanged;
}

void Animator::_updateAnimatorBounds(){
    QList<QString> animKeys =m_animators.keys();
    bool visibilityChanged = false;
    for ( QString key : animKeys  ){
        if ( key != Selection::IMAGE ){
           bool animVisibility = _updateAnimatorBound( key );
           if ( animVisibility ){
               visibilityChanged = true;
           }
        }
    }
    if ( visibilityChanged ){
        _adjustStateAnimatorTypes();
    }
}


void Animator::_updateSupportedZAxes( Controller* controller ){
    std::set<AxisInfo::KnownType> animAxes = controller->_getAxesHidden();
    for ( std::set<AxisInfo::KnownType>::iterator it = animAxes.begin();
        it != animAxes.end(); it++ ){
        const Carta::Lib::KnownSkyCS& cs = controller->getCoordinateSystem();
        QString animName = AxisMapper::getPurpose( *it, cs );
        if ( !m_animators.contains( animName )){
            QString animId;
            addAnimator( animName , animId );
        }
    }
}

Animator::~Animator(){
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    int animationCount = m_animators.size();
    QList<QString> keys = m_animators.keys();
    for ( int i = 0; i < animationCount; i++ ){
        if ( m_animators[keys[i]] != nullptr ){
            objMan->destroyObject( m_animators[keys[i]]->getId() );
            m_animators[keys[i]] = nullptr;
        }
    }
    m_animators.clear();
}

}
}

