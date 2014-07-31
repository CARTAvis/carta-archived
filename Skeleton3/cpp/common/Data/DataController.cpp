
#include "DataController.h"
#include "Globals.h"
#include "IConnector.h"

#include "State/StateLibrary.h"
#include "Data/DataSource.h"
#include "Data/DataSelection.h"
#include "TestView3.h"

#include <QDebug>

DataController::DataController():
	m_selectChannel( nullptr ),
	m_selectImage( nullptr ),
	m_view( nullptr ){
}



void DataController::addData( const QString& fileName ){

	//Find the location of the data, if it already exists.
	int targetIndex = -1;
	for ( int i = 0; i < m_datas.size(); i++ ){
		if ( m_datas[i]->contains( fileName) ){
			targetIndex = i;
			break;
		}
	}

	//Add the data if it is not already there.
	if ( targetIndex == -1 ){
		std::shared_ptr<DataSource> targetSource( new DataSource( fileName ) );

		targetIndex = m_datas.size();
		m_datas.push_back( targetSource);
	}

	//Update the data selectors upper bound and index based on the data.
	m_selectImage->setUpperBound( m_datas.size() - 1 );
	m_selectImage->setIndex( targetIndex );

	//Refresh the view of the data.
	_loadView();

	//Notify others there has been a change to the data.
	emit dataChanged();
}



void DataController::setId( const QString& winId ){

	m_winId = winId;

	//Reset the view
	m_view.reset(new TestView3( m_winId, QColor( "pink"), QImage()));
	auto & globals = * Globals::instance();
	IConnector * connector = globals.connector();
	connector-> registerView( m_view.get());

	//Reset the selections based on the new id.
	m_selectChannel.reset( new DataSelection( m_winId, StateKey::FRAME_CHANNEL_START,
			StateKey::FRAME_CHANNEL, StateKey::FRAME_CHANNEL_END ));
	m_selectImage.reset( new DataSelection( m_winId, StateKey::FRAME_IMAGE_START,
				StateKey::FRAME_IMAGE, StateKey::FRAME_IMAGE_END ));

	//Add
	_initializeCommands();
	_loadView();

}


void DataController::setFrameChannel( const QString& val ){
	if ( m_selectChannel != nullptr ){
		m_selectChannel->setIndex( val );
	}
}

void DataController::setFrameImage( const QString& val ){
	if ( m_selectImage != nullptr ){
		m_selectImage->setIndex( val );
	}
}


QString DataController::getState( StateKey key ) const {
	auto & globals = * Globals::instance();
	IConnector * connector = globals.connector();
	return connector->getState( key, m_winId );
}


void DataController::_loadView( ){

	//Determine the index of the data to load.
	int imageIndex = 0;
	if ( m_selectImage != nullptr ){
		imageIndex = m_selectImage->getIndex();
	}

	if ( imageIndex >= 0  && imageIndex < m_datas.size()){
		if ( m_datas[imageIndex] != nullptr ){

			//Determine the index of the channel to load.
			int frameIndex = 0;
			if ( m_selectChannel != nullptr ){
				frameIndex = m_selectChannel->getIndex();
			}

			//Load the image.
			Nullable<QImage> res = m_datas[imageIndex]->load( frameIndex );
			if( res.isNull()) {
				qDebug() << "Could not find any plugin to load image";
			}
			else {
				//Refresh the view.
				m_view->resetImage( res.val());
				auto & globals = * Globals::instance();
				IConnector * connector = globals.connector();
				connector->refreshView( m_view.get() );
			}
		}
	}
	else {
		qDebug() << "Image index="<<imageIndex <<" is out of range";
	}
}




void DataController::_initializeCommands(){
	auto & globals = * Globals::instance();
	IConnector * connector = globals.connector();

	//Listen for changes to channel selection.
	QString channelFrameKey = StateLibrary::instance()->getPath( StateKey::FRAME_CHANNEL, m_winId );
	connector-> addStateCallback( channelFrameKey, [=] ( const QString & /*path*/, const QString & /*val*/) {
		_loadView();
	});

	//Listen for changes to data selection.
	QString imageFrameKey = StateLibrary::instance()->getPath( StateKey::FRAME_IMAGE, m_winId );
	connector-> addStateCallback( imageFrameKey, [=] ( const QString & /*path*/, const QString & /*val*/) {
		_loadView();
	});

}

