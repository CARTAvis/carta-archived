#include "Globals.h"
#include "IConnector.h"

#include "State/StateLibrary.h"
#include "Data/DataAnimator.h"
#include "Data/DataSelection.h"
#include "Data/DataController.h"

#include <QDebug>

DataAnimator::DataAnimator(const QString& identifier):
	m_id(identifier),
	m_selectChannel(new DataSelection(identifier, StateKey::FRAME_CHANNEL_START,
			StateKey::FRAME_CHANNEL, StateKey::FRAME_CHANNEL_END)),
	m_selectImage(new DataSelection(identifier, StateKey::FRAME_IMAGE_START,
			StateKey::FRAME_IMAGE, StateKey::FRAME_IMAGE_END))
{
	_initializeStates();
	_initializeCommands();
}


void DataAnimator::addController( const std::shared_ptr<DataController>& controller ){
	m_controllers.push_back( controller );
	connect( &(*controller), SIGNAL(dataChanged()), this, SLOT(_adjustStates()) );
	_adjustStates();
	_saveState();

}

void DataAnimator::_adjustStates(){
	//Channels
	_adjustState( StateKey::FRAME_CHANNEL_START, StateKey::FRAME_CHANNEL_END, m_selectChannel );
	//Frames
	_adjustState( StateKey::FRAME_IMAGE_START, StateKey::FRAME_IMAGE_END, m_selectImage );
}

void DataAnimator::_saveState(){
    auto & globals = *Globals::instance();
    IConnector * connector = globals.connector();
    int controllerCount = m_controllers.size();
    QString controllerCountStr = QString::number(controllerCount);
        //connector->setState(StateKey::CONTROLLER_ID, "", m_winId );
    connector->setState(StateKey::ANIMATOR_LINK_COUNT, m_id, controllerCountStr);
    for (int i = 0; i < controllerCount; i++) {
        QString indexStr( m_id +"-" + QString::number(i) );
        connector->setState(StateKey::ANIMATOR_LINK, indexStr, m_controllers[i]->getId() );
    }

}

void DataAnimator::_adjustState(StateKey lowKey, StateKey highKey, std::shared_ptr<DataSelection>& selection ){
	int maxFrameStart = 0;
	int minFrameEnd = std::numeric_limits<int>::max();
	for ( std::shared_ptr<DataController> controller : m_controllers ){
		QString controlStartStr = controller->getState( lowKey );
		QString controlEndStr = controller->getState( highKey );
		bool validStart = false;
		int controlStart = controlStartStr.toInt( &validStart );
		if ( validStart && controlStart > maxFrameStart ){
			maxFrameStart = controlStart;
		}

		bool validEnd = false;
		int controlEnd = controlEndStr.toInt( &validEnd );
		if ( validEnd && controlEnd < minFrameEnd ){
			minFrameEnd = controlEnd;
		}
	}

	int controllerCount = m_controllers.size();
	if ( controllerCount > 0){
	    selection->setLowerBound( maxFrameStart );
	    selection->setUpperBound( minFrameEnd );
	}

}

void DataAnimator::_initializeStates(){

	auto & globals = * Globals::instance();
	IConnector * connector = globals.connector();
	_initializeStates( StateKey::ANIMATOR_CHANNEL_STEP, StateKey::ANIMATOR_CHANNEL_RATE, StateKey::ANIMATOR_CHANNEL_END_BEHAVIOR, connector );
	_initializeStates( StateKey::ANIMATOR_IMAGE_STEP, StateKey::ANIMATOR_IMAGE_RATE, StateKey::ANIMATOR_IMAGE_END_BEHAVIOR, connector );
}

void DataAnimator::_initializeStates( StateKey keyStep, StateKey keyRate,
        StateKey keyEnd, IConnector* connector ){
    QString stepStr = connector->getState( keyStep, m_id );
    if ( stepStr.length() == 0 ){
        connector->setState( keyStep, m_id, "1");
    }
    QString rateStr = connector->getState( keyRate, m_id );
    if ( rateStr.length() == 0 ){
        connector->setState( keyRate, m_id, "20");
    }
    QString endStr = connector->getState( keyEnd, m_id );
    if ( endStr.length() == 0 ){
        connector->setState( keyEnd, m_id, "Wrap");
    }
}


void DataAnimator::_initializeCommands(){

	auto & globals = * Globals::instance();
	IConnector * connector = globals.connector();

	//A new image is being selected.
	QString setImageKey( StateLibrary::SEPARATOR + StateLibrary::APP_ROOT + StateLibrary::SEPARATOR+
					StateLibrary::ANIMATOR + StateLibrary::SEPARATOR + StateLibrary::IMAGE+StateLibrary::SEPARATOR+
					"setFrame"+StateLibrary::SEPARATOR+ m_id);
	connector->addCommandCallback( setImageKey, [=] (const QString & /*cmd*/,
					 const QString & params, const QString & /*sessionId*/) -> QString {
		//Tell the children about the new image.
		for( std::shared_ptr<DataController> controller : m_controllers ){
			controller->setFrameImage( params );
		}
		//Set our state to reflect the new image.
		int imageIndex = m_selectImage->setIndex( params );
	    return QString("image=%1").arg(imageIndex);
	});

	//A new channel is selected.
	QString setChannelKey( StateLibrary::SEPARATOR + StateLibrary::APP_ROOT + StateLibrary::SEPARATOR+
						StateLibrary::ANIMATOR + StateLibrary::SEPARATOR + StateLibrary::CHANNEL+StateLibrary::SEPARATOR+
						"setFrame"+StateLibrary::SEPARATOR+ m_id);
		connector->addCommandCallback( setChannelKey, [=] (const QString & /*cmd*/,
						 const QString & params, const QString & /*sessionId*/) -> QString {
			//Tell the children about the new channel.
			for( std::shared_ptr<DataController> controller : m_controllers ){
				controller->setFrameChannel( params );
			}
			//Keep a record of the new channel ourselves.
			int channelIndex = m_selectChannel->setIndex( params );
		    return QString("channel=%1").arg(channelIndex);
	});
}

