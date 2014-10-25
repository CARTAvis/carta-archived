#include "Data/Animator.h"
#include "Data/Selection.h"

#include <QDebug>


const QString AnimatorType::COMMAND_SET_FRAME = "setFrame";
const QString AnimatorType::END_BEHAVIOR = "endBehavior";
const QString AnimatorType::RATE = "frameRate";
const QString AnimatorType::STEP = "frameStep";

const QString AnimatorType::CLASS_NAME = "edu.nrao.carta.AnimatorType";
bool AnimatorType::m_registered =
    ObjectManager::objectManager()->registerClass (CLASS_NAME,
                                                   new AnimatorType::Factory());

AnimatorType::AnimatorType(/*const QString& prefix, const QString& animationType, const QString& id*/
        const QString& path, const QString& id ):
	CartaObject( CLASS_NAME, path, id ),
	m_state( path ){
        ObjectManager* objManager = ObjectManager::objectManager();
        QString selId = objManager->createObject( Selection::CLASS_NAME );
        CartaObject* selObj = objManager->getObject( selId );
        m_select.reset( dynamic_cast<Selection*>(selObj) );

        _initializeState();
        _initializeCommands();
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
	        return QString(m_select->getPath());
	    });

}

