#include "Data/Animator.h"
#include "Data/Selection.h"
#include "Data/Util.h"

#include <set>

#include <QDebug>

namespace Carta {

namespace Data {

class AnimatorType::Factory : public CartaObjectFactory {

public:

    CartaObject * create (const QString & path, const QString & id)
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
const QString AnimatorType::STEP = "frameStep";

const QString AnimatorType::CLASS_NAME = "AnimatorType";
const QString AnimatorType::ANIMATIONS = "animators";

bool AnimatorType::m_registered =
    ObjectManager::objectManager()->registerClass (CLASS_NAME,
                                                   new AnimatorType::Factory());

AnimatorType::AnimatorType(/*const QString& prefix, const QString& animationType, const QString& id*/
        const QString& path, const QString& id ):
	CartaObject( CLASS_NAME, path, id ){
        m_select = nullptr;
        _makeSelection();
        m_removed = false;
        _initializeState();
        _initializeCommands();
}

QString AnimatorType::getStateString() const{
    StateInterface writeState( m_state );
    writeState.insertObject(Selection::CLASS_NAME, m_select->getStateString());
    return writeState.toString();
}


int AnimatorType::getIndex() const {
    return m_select->getIndex();
}

void AnimatorType::_initializeState( ){
    m_state.insertValue<int>( STEP, 1 );
    m_state.insertValue<int>( RATE, 20 );
    m_state.insertValue<QString>( END_BEHAVIOR, "Wrap");
    m_state.flushState();
}

bool AnimatorType::isRemoved() const {
    return m_removed;
}


QString AnimatorType::_makeSelection(){
    ObjectManager* objManager = ObjectManager::objectManager();
    if ( m_select != nullptr ){
        objManager->destroyObject( m_select->getId());
    }
    QString selId = objManager->createObject( Selection::CLASS_NAME );
    m_select = dynamic_cast<Selection*>(objManager->getObject( selId ));
    return m_select->getPath();
}

QString AnimatorType::_setEndBehavior( const QString& params ){
    QString result;
    std::set<QString> keys = {END_BEHAVIOR};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString endStr = dataValues[*keys.begin()];
    if ( endStr == END_BEHAVIOR_WRAP || endStr == END_BEHAVIOR_REVERSE || endStr == END_BEHAVIOR_JUMP ){
        if ( endStr != m_state.getValue<QString>(END_BEHAVIOR)){
            m_state.setValue<QString>(END_BEHAVIOR, endStr );
            m_state.flushState();
        }
    }
    else {
        result = "Invalid animator end behavior: "+ params;
        qWarning() << result;
    }
    return result;
}

QString AnimatorType::_setFrameRate( const QString& params ){
    QString result;
    std::set<QString> keys = {RATE};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString rateStr = dataValues[*keys.begin()];
    bool validRate = false;
    int rate = rateStr.toInt( &validRate );
    if ( validRate  ){
        if ( rate > 0 ){
            if ( rate != m_state.getValue<int>(RATE)){
                m_state.setValue<int>(RATE, rate );
                m_state.flushState();
            }
        }
        else {
            result = "Animator frame rate must be positive: "+params;
            qWarning() << result;
        }
    }
    else {
        result = "Invalid animator frame rate: "+ params;
        qWarning() << result;
    }
    return result;
}

QString AnimatorType::_setFrameStep( const QString& params ){
    QString result;
    std::set<QString> keys = {STEP};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString stepStr = dataValues[*keys.begin()];
    bool validStep = false;
    int step = stepStr.toInt( &validStep );
    if ( validStep  ){
        if ( step > 0 ){
            if ( step != m_state.getValue<int>(STEP)){
                m_state.setValue<int>(STEP, step );
                m_state.flushState();
            }
        }
        else {
            result = "Animator frame step size must be positive: "+params;
            qWarning() << result;
        }
    }
    else {
        result = "Invalid animator frame step size: "+ params;
        qWarning() << result;
    }
    return result;
}

void AnimatorType::setRemoved( bool removed ){
    m_removed = removed;
}

QString AnimatorType::changeIndex( const QString & params )
{
    QString result;
    bool validInt = false;
    int index = params.toInt( &validInt );
    if ( validInt ){
    //Set our state to reflect the new image.
        result = m_select->setIndex( index );
        if ( result.isEmpty()){
            //Tell the children about the new image.
            emit indexChanged( index );
        }
    }
    else {
        result = "Animator index must be a valid integer "+params;
    }
    return result;
}

void AnimatorType::_initializeCommands(){

	//A new image frame is being selected.
	addCommandCallback( COMMAND_SET_FRAME, [=] (const QString & /*cmd*/,
					 const QString & params, const QString & /*sessionId*/) -> QString {

        QString result = changeIndex( params );
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
	                            QString result = _setEndBehavior( params );
	                            return result;
	                        });


    addCommandCallback( "setFrameRate", [=] (const QString & /*cmd*/,
                                        const QString & params, const QString & /*sessionId*/) -> QString {
                                QString result = _setFrameRate( params );
                                return result;
                            });

    addCommandCallback( "setFrameStep", [=] (const QString & /*cmd*/,
                                            const QString & params, const QString & /*sessionId*/) -> QString {
                                    QString result = _setFrameStep( params );
                                    return result;
                                });
}

void AnimatorType::setUpperBound( int value ){
    m_select->setUpperBound( value );
}

void AnimatorType::setIndex( int value ){
    m_select->setIndex( value );
}

AnimatorType::~AnimatorType(){
    if ( m_select != nullptr ){
        ObjectManager* objMan = ObjectManager::objectManager();
        objMan->destroyObject( m_select->getId());
    }
}
}
}
