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
const QString Animator::NAME = "name";
const QString Animator::VALUE = "value";
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
            connect( controller, SIGNAL(dataChangedRegion(Controller*)),
                                this, SLOT(_regionsChanged(Controller*)) );
            connect( controller, SIGNAL(axesChanged()),
                    this, SLOT(_axesChanged()));
            connect( controller, SIGNAL(frameChanged(Controller*, Carta::Lib::AxisInfo::KnownType)),
                    this, SLOT(_updateFrame(Controller*, Carta::Lib::AxisInfo::KnownType)));
        }
    }
    else {
        result = "Animator only supports linking to images";
    }

    if ( linkAdded ){
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
            QString typePath = UtilState::getLookup( arrayPath, Util::TYPE );
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
        //Restore the stored preferences.
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
            else if ( type == Selection::REGION ){
            	//Find a controller to use for setting up initial animation
            	//parameters.
            	Controller* controller = _getControllerSelected();
            	if ( controller != nullptr ){
            		int selectRegion = controller->getRegionIndexCurrent();
            		_resetAnimationRegion( selectRegion );
            	}
            }
            else {
                _updateAnimatorBound( type );
            }
        }
    }
    else {
        m_animators[type]->setVisible( true );
        m_animators[type]->setRemoved( false );
        _adjustStateAnimatorTypes();
        animatorTypeId= m_animators[type]->getPath();
    }
    //Reset the preference state.
    _resetStateAnimator( /*m_animPrefs[type],*/ type);
    return result;
}

void Animator::_addRemoveImageAnimator(){
    int maxImages = getMaxImageCount();
    if ( maxImages > 1 ){
        QString animId;
        addAnimator( Selection::IMAGE, animId );
    }
    else {
        removeAnimator( Selection::IMAGE );
    }
}

void Animator::_addRemoveRegionAnimator(){
    int maxRegions = getMaxRegionCount();
    if ( maxRegions > 1 ){
        QString animId;
        addAnimator( Selection::REGION, animId );
    }
    else {
        removeAnimator( Selection::REGION );
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
                if ( purpose.length() > 0 ){
                    addAnimator( purpose, animId );
                    existingAnimators.insert( purpose );
                }
            }
        }
    }

    //Remove any existing animators if they are no longer supported.
    QList<QString> keys = m_animators.keys();
    int animCount = keys.size();
    for ( int i = 0; i < animCount; i++ ){
        QString animType = m_animators[keys[i]]->getType();
        bool existing = existingAnimators.contains( animType );
        if ( !existing && animType != Selection::IMAGE && animType != Selection::REGION ){
            m_animators[keys[i]]->setRemoved( true );
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
            else if ( animName == Selection::REGION ){
            	controller->setFrameRegion( index );
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
        Controller* alt = nullptr;
        for ( int i = 0; i < linkCount; i++ ){
            Controller* control = dynamic_cast<Controller*>( m_linkImpl->getLink(i) );
            if ( control != nullptr ){
                int imageCount = control->getStackedImageCount();
                if ( imageCount > 0 ){
                    controller = control;
                    break;
                }
                alt = control;
            }
        }
        //Otherwise, just choose one.
        if ( controller == nullptr ){
            controller = alt;
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
            int imageCount = controller->getStackedImageCountVisible();
            if ( maxImages < imageCount ){
                maxImages = imageCount;
            }
        }
    }
    return maxImages;
}

int Animator::getMaxRegionCount() const {
    int linkCount = m_linkImpl->getLinkCount();
    int maxImages = 0;
    for ( int i = 0; i < linkCount; i++ ){
        Controller* controller = dynamic_cast<Controller*>( m_linkImpl->getLink(i));
        if ( controller != nullptr ){
            int regionCount = controller->getRegionCount();
            if ( maxImages < regionCount ){
                maxImages = regionCount;
            }
        }
    }
    return maxImages;
}

QString Animator::getStateString( const QString& /*sessionId*/, SnapshotType type ) const {
    QString result;
    if ( type == SNAPSHOT_PREFERENCES ){
        StateInterface prefState("");
        prefState.setState(m_state.toString() );

        //Update the preference state strings using existing animators
        QMap<QString, AnimatorType*>::const_iterator animIter;
        for (animIter = m_animators.begin(); animIter != m_animators.end(); ++animIter){
            QString key = animIter.key();
            QString prefState = animIter.value()->getStatePreferences();
            m_animPrefs[key] = prefState;
        }

        //Now save the preference states to the state.
        int animCount = m_animPrefs.size();
        prefState.insertArray(AnimatorType::CLASS_NAME, animCount);
        int i = 0;
        QMap<QString,QString>::const_iterator prefIter;
        for (prefIter = m_animPrefs.begin(); prefIter != m_animPrefs.end(); ++prefIter){
            QString animKey = Carta::State::UtilState::getLookup( AnimatorType::CLASS_NAME, i );
            QString animKeyName = UtilState::getLookup(animKey, NAME );
            QString animKeyValue = UtilState::getLookup(animKey, VALUE );
            prefState.insertValue<QString>( animKeyName, prefIter.key() );
            prefState.insertValue<QString>( animKeyValue, prefIter.value() );
            i++;
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
            QString animKeyName = UtilState::getLookup(animKey, NAME );
            QString animKeyValue = UtilState::getLookup(animKey, VALUE );
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
        if ( m_animPrefs.contains(type) ){
            m_animators[type]->resetState( m_animPrefs[type], SnapshotType::SNAPSHOT_PREFERENCES);
        }
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
        std::set<QString> keys = {Util::TYPE};
        std::map<QString,QString> dataValues = UtilState::parseParamMap( params, keys );
        QString animId = "-1";
        QString result = addAnimator( dataValues[*keys.begin()], animId );
        Util::commandPostProcess( result );
        return animId;
    });

    addCommandCallback( "registerAnimator", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = {Util::TYPE};
            std::map<QString,QString> dataValues = UtilState::parseParamMap( params, keys );
            QString targetKey = dataValues[*keys.begin()];
            QString animId = "-1";
            QMap<QString, AnimatorType*>::const_iterator animIter;
            for (animIter = m_animators.begin(); animIter != m_animators.end(); ++animIter){
                QString animatorKey = animIter.key();
                if ( animatorKey == targetKey ){
                    animId = animIter.value()->getPath();
                }
            }
            return animId;
        });

    addCommandCallback( "removeAnimator", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = { Util::TYPE };
            std::map<QString,QString> dataValues = UtilState::parseParamMap( params, keys );
            QString animatorId = removeAnimator( dataValues[*keys.begin()] );
            return animatorId;
        });
}


void Animator::_initializeState(){
    m_state.insertArray( AnimatorType::ANIMATIONS, 0);
    m_state.flushState();
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

void Animator::_regionsChanged( Controller* controller ){
	int selectRegionIndex = controller->getRegionIndexCurrent();
	_resetAnimationRegion( selectRegionIndex );
}


QString Animator::removeAnimator( const QString& type ){
    QString result;
    if ( m_animators.contains( type )){
        //Store the prefs
        m_animPrefs[type]= m_animators[type]->getStatePreferences();
        m_animators[type]->setVisible( false );
        _adjustStateAnimatorTypes();
    }
    else if ( type != Selection::IMAGE && type != Selection::REGION ){
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

void Animator::_resetAnimationRegion( int selectedIndex ){
    _addRemoveRegionAnimator();
    if ( m_animators.contains( Selection::REGION) ){
        int maxRegions = getMaxRegionCount();
        if ( maxRegions == 0 ){
            m_animators[Selection::REGION]->setUpperBound( 1 );
        }
        else {
            m_animators[Selection::REGION]->setUpperBound(maxRegions);
        }
        if ( selectedIndex >= 0 ){
            m_animators[Selection::REGION]->setFrame( selectedIndex );
        }

    }
    _updateAnimatorBounds();
}


void Animator::_resetAnimationParameters( int selectedImage ){
    _addRemoveImageAnimator();
    if ( m_animators.contains( Selection::IMAGE) ){
        int maxImages = getMaxImageCount();
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
            if ( index > maxImages  ){
                m_animators[Selection::IMAGE]->setIndex( 0 );
            }
        }
    }
    _updateAnimatorBounds();
}


void Animator::_resetStateAnimator( const QString& key ){
    if ( m_animPrefs.contains( key ) ){
        if ( m_animators.contains( key )){
            m_animators[key]->resetState( m_animPrefs[key], SNAPSHOT_PREFERENCES);
        }
    }
}

void Animator::resetState( const QString& state ){
    StateInterface stateInterface( "");
    stateInterface.setState( state );

    //Now store the preference setting for each animator.
    int prefCount = stateInterface.getArraySize( AnimatorType::CLASS_NAME );
    for (int i = 0; i < prefCount; i++ ){
        QString lookup = UtilState::getLookup( AnimatorType::CLASS_NAME, i );
        QString keyName = UtilState::getLookup(lookup, NAME );
        QString animName = stateInterface.getValue<QString>( keyName );
        QString keyValue = UtilState::getLookup(lookup, VALUE );

        QString prefValue = stateInterface.getValue<QString>( keyValue );
        m_animPrefs[animName] = prefValue;
        if ( m_animators.contains( animName ) ){
            this->_resetStateAnimator( animName );
        }
    }
}

void Animator::resetStateData( const QString& state ){
    Carta::State::StateInterface dataState("");
    dataState.setState( state );

    //Add the necessary animators based on the data available.
    int animationCount = dataState.getArraySize( AnimatorType::ANIMATIONS );
    QStringList supportedAnim;
    for (int i = 0; i < animationCount; i++ ){
        QString lookup = UtilState::getLookup( AnimatorType::ANIMATIONS, i );
        QString keyName = UtilState::getLookup(lookup, NAME );
        QString animName = dataState.getValue<QString>( keyName );
        QString keyValue = UtilState::getLookup(lookup, VALUE );
        QString stateValue = dataState.getValue<QString>( keyValue );
        if ( !m_animators.contains( animName) ){
            QString animId;
            addAnimator( animName, animId );
        }
        supportedAnim.append( animName );
        m_animators[animName]->resetStateData( stateValue );
    }

    //Remove any animators not supported.
    QList<QString> keys = m_animators.keys();
    for ( QString key : keys ){
        if ( !supportedAnim.contains(key) ){
            removeAnimator( key );
        }
    }
}

bool Animator::_setAnimatorAvailability( const QString& key, bool available ){
    bool availableChanged = false;
    if ( m_animators.contains( key ) ){
        if ( !available ){
           m_animators[key]->setVisible( false );
        }
        m_animators[key]->setRemoved( !available );
        availableChanged = true;
    }
    return availableChanged;
}

bool Animator::_updateAnimatorBound( const QString& key ){
    int maxFrame = 0;
    int currentFrame = 0;
    bool availableChanged = false;
    AxisInfo::KnownType axisType = AxisMapper::getType( key );
    if ( axisType == AxisInfo::KnownType::OTHER ){
        return availableChanged;
    }
    std::vector<AxisInfo::KnownType> animationAxes;
    Controller* controller = _getControllerSelected();
    if ( controller != nullptr ){
        maxFrame = controller->getFrameUpperBound( axisType );
        currentFrame = controller->getFrame( axisType );
        animationAxes = controller->_getAxisZTypes();
    }
    m_animators[key]->setUpperBound( maxFrame );
    m_animators[key]->setFrame( currentFrame );

    //Decide the availability of the animator based on whether it is an animation axis
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

        //Okay we will set the animator available if it has at least one
        //frame.
        if ( axisFound ){
            if ( maxFrame > 1 ){
                availableChanged = _setAnimatorAvailability( key, true );
            }
            else {
                availableChanged = _setAnimatorAvailability( key, false );
            }
        }
        else {
            availableChanged = _setAnimatorAvailability( key, false );
        }
    }
    return availableChanged;
}

void Animator::_updateAnimatorBounds(){
    QList<QString> animKeys =m_animators.keys();
    bool availableChanged = false;
    for ( QString key : animKeys  ){
        if ( key != Selection::IMAGE && key != Selection::REGION ){
           bool animAvailable = _updateAnimatorBound( key );
           if ( animAvailable ){
               availableChanged = true;
           }
        }
    }
    if ( availableChanged ){
        _adjustStateAnimatorTypes();
    }
}


void Animator::_updateSupportedZAxes( Controller* controller ){
    std::set<AxisInfo::KnownType> animAxes = controller->_getAxesHidden();
    for ( std::set<AxisInfo::KnownType>::iterator it = animAxes.begin();
        it != animAxes.end(); it++ ){
        const Carta::Lib::KnownSkyCS& cs = controller->getCoordinateSystem();
        QString animName = AxisMapper::getPurpose( *it, cs );
        if ( !m_animators.contains( animName ) && animName.length() > 0 ){
            QString animId;
            addAnimator( animName , animId );
        }
        else {
            if ( m_animators.contains( animName ) ){
                if ( m_animators[animName]->isRemoved( ) ){
                    m_animators[animName]->setRemoved( false );
                    if ( controller->getFrameUpperBound(*it) > 0 ){
                        m_animators[animName]->setVisible( true );
                    }
                    _adjustStateAnimatorTypes();
                }
            }
        }
    }
}

void Animator::_updateFrame( Controller* controller, Carta::Lib::AxisInfo::KnownType type ){
    if ( controller ){
        int frameIndex = controller->getFrame( type );
        const Carta::Lib::KnownSkyCS& cs = controller->getCoordinateSystem();
        QString animName = AxisMapper::getPurpose( type, cs );
        if ( m_animators.contains( animName) ){
            int currentIndex = m_animators[animName]->getFrame();
            if ( currentIndex != frameIndex ){
                m_animators[animName]->setFrame( frameIndex );
                _adjustStateAnimatorTypes();
            }
        }
    }
}

Animator::~Animator(){
    int animationCount = m_animators.size();
    QList<QString> keys = m_animators.keys();
    for ( int i = 0; i < animationCount; i++ ){
        if ( m_animators[keys[i]] != nullptr ){
            delete m_animators[keys[i]];
            m_animators[keys[i]] = nullptr;
        }
    }
    m_animators.clear();
}

}
}

