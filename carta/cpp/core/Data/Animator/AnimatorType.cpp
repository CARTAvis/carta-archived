#include "core/Data/Animator/Animator.h"
#include "core/Data/Selection.h"
#include "core/Data/Util.h"
#include "core/State/UtilState.h"

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


const QString AnimatorType::COMMAND_SET_FRAME = "setFrame";
const QString AnimatorType::END_BEHAVIOR = "endBehavior";
const QString AnimatorType::END_BEHAVIOR_WRAP = "Wrap";
const QString AnimatorType::END_BEHAVIOR_JUMP = "Jump";
const QString AnimatorType::END_BEHAVIOR_REVERSE = "Reverse";
const QString AnimatorType::RATE = "frameRate";
const QString AnimatorType::SETTINGS_VISIBLE = "showSettings";
const QString AnimatorType::STEP = "frameStep";

const QString AnimatorType::CLASS_NAME = "AnimatorType";
const QString AnimatorType::ANIMATIONS = "animators";

bool AnimatorType::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass (CLASS_NAME,
                                                   new AnimatorType::Factory());

AnimatorType::AnimatorType(const QString& path, const QString& id ):
	CartaObject( CLASS_NAME, path, id ){
        m_select = nullptr;
        m_removed = false;
        m_visible = true;
        _initializeState();
        _makeSelection();

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

QString AnimatorType::getType() const {
    return m_type;
}

void AnimatorType::_initializeState( ){
    m_state.insertValue<int>( STEP, 1 );
    m_state.insertValue<int>( RATE, 100 );
    m_state.insertValue<bool>(SETTINGS_VISIBLE, false );
    m_state.insertValue<QString>( END_BEHAVIOR, "Wrap");
    //m_state.insertValue<bool>( VISIBLE, true);
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

	addCommandCallback( "setUpperBoundUser", [=] (const QString & /*cmd*/,
	                     const QString & params, const QString & /*sessionId*/) -> QString {
	        QString result;
	        bool validInt = false;
	        int index = params.toInt( &validInt );
	        if ( validInt ){
	            result = setUpperBoundUser( index );
	        }
	        else {
	            result = "Animator upper bound must be a valid integer "+params;
	        }
	        Util::commandPostProcess( result );
	        return result;
	    });

	addCommandCallback( "setLowerBoundUser", [=] (const QString & /*cmd*/,
	                         const QString & params, const QString & /*sessionId*/) -> QString {
	            QString result;
	            bool validInt = false;
	            int index = params.toInt( &validInt );
	            if ( validInt ){
	                result = setLowerBoundUser( index );
	            }
	            else {
	                result = "Animator lower bound must be a valid integer "+params;
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

    addCommandCallback( "setSettingsVisible", [=] (const QString & /*cmd*/,
                                                const QString & params, const QString & /*sessionId*/) -> QString {
            QString result;
            std::set<QString> keys = {SETTINGS_VISIBLE};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString visibleStr = dataValues[*keys.begin()];
            bool validBool = false;
            bool settingsVisible = Util::toBool( visibleStr, &validBool );
            if ( validBool  ){
                setSettingsVisible( settingsVisible );
            }
            else {
                result = "Animator setting visibility must be true/false: "+params;
            }
            Util::commandPostProcess( result );
            return result;
        });
}

bool AnimatorType::isRemoved() const {
    return m_removed;
}

bool AnimatorType::isVisible() const {
    return m_visible;
}


QString AnimatorType::_makeSelection(){
    Carta::State::ObjectManager* objManager = Carta::State::ObjectManager::objectManager();
    if ( m_select != nullptr ){
        objManager->destroyObject( m_select->getId());
    }
    m_select = objManager->createObject<Selection>();
    connect( m_select, SIGNAL(indexChanged()), this, SLOT(_selectionChanged()));
    QString path = m_select->getPath();
    return path;
}

void AnimatorType::resetStateData( const QString& state ){
    if ( m_select != nullptr ){
        m_select->resetState( state );
    }
}

void AnimatorType::_selectionChanged(){
    emit indexChanged( m_select->getIndex(), m_type );
}

void AnimatorType::_setType( const QString& type ){
    m_type = type;
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
        }
    }
    else {
        result="Frame index must be nonnegative: "+QString::number(frameIndex);
    }
    return result;
}

QString AnimatorType::setLowerBoundUser( int lowerBound ){
    QString result = m_select->setLowerBoundUser( lowerBound );
    return result;
}

void AnimatorType::setSettingsVisible( bool visible ){
    bool oldVisible = m_state.getValue<bool>(SETTINGS_VISIBLE);
    if ( oldVisible != visible ){
        m_state.setValue<bool>(SETTINGS_VISIBLE, visible );
        m_state.flushState();
    }
}


QString AnimatorType::setUpperBoundUser( int upperBound ){
    QString result = m_select->setUpperBoundUser( upperBound);
    return result;
}

void AnimatorType::setRemoved( bool removed ){
    m_removed = removed;
}

void AnimatorType::setVisible( bool visible ){
    m_visible = visible;
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
