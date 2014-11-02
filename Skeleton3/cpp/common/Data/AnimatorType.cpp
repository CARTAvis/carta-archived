#include "Data/Animator.h"
#include "Data/Selection.h"

#include <QDebug>


const QString AnimatorType::COMMAND_SET_FRAME = "setFrame";
const QString AnimatorType::END_BEHAVIOR = "endBehavior";
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
        _makeSelection();

        _initializeState();
        _initializeCommands();
}

QString AnimatorType::_makeSelection(){
    ObjectManager* objManager = ObjectManager::objectManager();
    QString selId = objManager->createObject( Selection::CLASS_NAME );
    CartaObject* selObj = objManager->getObject( selId );
    m_select.reset( dynamic_cast<Selection*>(selObj) );
    return m_select->getPath();
}

QString AnimatorType::getStateString() const{
    StateInterface writeState( m_state );
    writeState.insertObject(Selection::CLASS_NAME, m_select->getStateString());
    return writeState.toString();
}


void AnimatorType::setUpperBound( int value ){
    m_select->setUpperBound( value );
}


void AnimatorType::_initializeState( ){
    m_state.insertValue<int>( STEP, 1 );
    m_state.insertValue<int>( RATE, 20 );
    m_state.insertValue<QString>( END_BEHAVIOR, "Wrap");
    m_state.flushState();
}


void AnimatorType::_initializeCommands(){

	//A new image frame is being selected.
	addCommandCallback( COMMAND_SET_FRAME, [=] (const QString & /*cmd*/,
					 const QString & params, const QString & /*sessionId*/) -> QString {

        //Set our state to reflect the new image.
        int index = m_select->setIndex( params );

		//Tell the children about the new image.
		emit indexChanged( params );

	    return QString("%1=%2").arg(m_animationType).arg(index);
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

}

