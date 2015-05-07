#include "Data/Animator/Animator.h"
#include "Data/Selection.h"
#include "Data/Util.h"
#include "State/UtilState.h"

#include <set>

#include <QDebug>

namespace Carta {

namespace Data {

class AnimatorType::Factory : public Carta::State::CartaObjectFactory {

public:

    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new AnimatorType (path, id);
    }
};

const QString AnimatorType::VISIBLE = "visible";
const QString AnimatorType::COMMAND_SET_FRAME = "setFrame";
const QString AnimatorType::END_BEHAVIOR = "endBehavior";
const QString AnimatorType::END_BEHAVIOR_WRAP = "Wrap";
const QString AnimatorType::END_BEHAVIOR_JUMP = "Jump";
const QString AnimatorType::END_BEHAVIOR_REVERSE = "Reverse";
const QString AnimatorType::RATE = "frameRate";
const QString AnimatorType::STEP = "frameStep";

const QString AnimatorType::CLASS_NAME = "AnimatorType";
const QString AnimatorType::ANIMATIONS = "animators";

bool AnimatorType::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass (CLASS_NAME,
                                                   new AnimatorType::Factory());

AnimatorType::AnimatorType(const QString& path, const QString& id ):
	CartaObject( CLASS_NAME, path, id ){
        m_select = nullptr;
        _makeSelection();
        _initializeState();
        _initializeCommands();
}

int AnimatorType::getFrame() const {
    return m_select->getIndex();
}

QString AnimatorType::getStateData() const {
    QString result = m_select->getStateString();
    return result;
}

QString AnimatorType::getStatePreferences() const{
    QString result  = m_state.toString();
    return result;
}

void AnimatorType::_initializeState( ){
    m_state.insertValue<int>( STEP, 1 );
    m_state.insertValue<int>( RATE, 20 );
    m_state.insertValue<QString>( END_BEHAVIOR, "Wrap");
    m_state.insertValue<bool>( VISIBLE, true);
    m_state.flushState();
}

void AnimatorType::_initializeCommands(){

	//A new image frame is being selected.
	addCommandCallback( COMMAND_SET_FRAME, [=] (const QString & /*cmd*/,
					 const QString & params, const QString & /*sessionId*/) -> QString {
	    QString result;
	    bool validInt = false;
	    int index = params.toInt( &validInt );
	    if ( validInt ){
	        result = setFrame( index );
	    }
	    else {
	        result = "Animator index must be a valid integer "+params;
	    }
	    Util::commandPostProcess( result );
	    return result;
	});

	addCommandCallback( "getSelection", [=] (const QString & /*cmd*/,
	                     const QString & /*params*/, const QString & /*sessionId*/) -> QString {
	    QString selectId;
	    if ( m_select != nullptr){
	        selectId = m_select->getPath();
	    }
	    else {
	        selectId = _makeSelection();
	    }
	    return selectId;
	});

	addCommandCallback( "setEndBehavior", [=] (const QString & /*cmd*/,
	                                    const QString & params, const QString & /*sessionId*/) -> QString {
	    QString result;
	    std::set<QString> keys = {END_BEHAVIOR};
	    std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
	    QString endStr = dataValues[*keys.begin()];
	    result = setEndBehavior( endStr );
	    Util::commandPostProcess( result );
	    return result;
	});


    addCommandCallback( "setFrameRate", [=] (const QString & /*cmd*/,
                                        const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {RATE};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString rateStr = dataValues[*keys.begin()];
        bool validRate = false;
        int rate = rateStr.toInt( &validRate );
        if ( validRate  ){
            result = setFrameRate( rate );
        }
        else {
            result = "Frame rate must be an integer: "+params;
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setFrameStep", [=] (const QString & /*cmd*/,
                                            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {STEP};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString stepStr = dataValues[*keys.begin()];
        bool validStep = false;
        int step = stepStr.toInt( &validStep );
        if ( validStep  ){
            result = setFrameStep( step );
        }
        else {
            result = "Frame step must be a positive integer: "+params;
        }
        Util::commandPostProcess( result );
        return result;
    });
}

bool AnimatorType::isRemoved() const {
    bool visible = m_state.getValue<bool>( VISIBLE );
    return !visible;
}


QString AnimatorType::_makeSelection(){
    Carta::State::ObjectManager* objManager = Carta::State::ObjectManager::objectManager();
    if ( m_select != nullptr ){
        objManager->destroyObject( m_select->getId());
    }
    QString selId = objManager->createObject( Selection::CLASS_NAME );
    m_select = dynamic_cast<Selection*>(objManager->getObject( selId ));
    return m_select->getPath();
}

void AnimatorType::resetStateData( const QString& state ){
    if ( m_select != nullptr ){
        m_select->resetState( state );
    }
}

QString AnimatorType::setEndBehavior( const QString& endStr ){
    QString result;
    if ( endStr == END_BEHAVIOR_WRAP || endStr == END_BEHAVIOR_REVERSE || endStr == END_BEHAVIOR_JUMP ){
        if ( endStr != m_state.getValue<QString>(END_BEHAVIOR)){
            m_state.setValue<QString>(END_BEHAVIOR, endStr );
            m_state.flushState();
        }
    }
    else {
        result = "Invalid animator end behavior: "+ endStr;
    }
    return result;
}

QString AnimatorType::setFrameRate( int rate ){
    QString result;
    if ( rate > 0 ){
        if ( rate != m_state.getValue<int>(RATE)){
            m_state.setValue<int>(RATE, rate );
            m_state.flushState();
        }
    }
    else {
        result = "Animator frame rate must be positive: "+QString::number(rate);
    }
    return result;
}

QString AnimatorType::setFrameStep( int step ){
    QString result;
    if ( step > 0 ){
        if ( step != m_state.getValue<int>(STEP)){
            m_state.setValue<int>(STEP, step );
            m_state.flushState();
        }
    }
    else {
        result = "Animator frame step size must be positive: "+QString::number(step);
    }
    return result;
}

QString AnimatorType::setFrame( int frameIndex ){
    QString result;
    //Set our state to reflect the new image.
    if ( frameIndex >= 0 ){
        int oldIndex = m_select->getIndex();
        if ( frameIndex != oldIndex ){
            result = m_select->setIndex( frameIndex );
             if ( result.isEmpty()){
                 //Tell the children about the new image.
                 emit indexChanged( frameIndex );
             }
        }
    }
    else {
        result="Frame index must be nonnegative: "+QString::number(frameIndex);
    }
    return result;
}

void AnimatorType::setVisible( bool visible ){
    bool oldVisible = m_state.getValue<bool>(VISIBLE);
    if ( oldVisible != visible ){
        m_state.setValue<bool>(VISIBLE, visible );
        m_state.flushState();
    }
}

void AnimatorType::setUpperBound( int value ){
    if ( m_select->getUpperBound() != value ){
        m_select->setUpperBound( value );
    }
}


AnimatorType::~AnimatorType(){
    if ( m_select != nullptr ){
        Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
        objMan->destroyObject( m_select->getId());
    }
}
}
}
