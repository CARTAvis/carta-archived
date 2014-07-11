#include "DataController.h"
#include "Globals.h"
#include "IConnector.h"
#include "misc.h"
#include "PluginManager.h"
#include "State/StateLibrary.h"
#include "TestView3.h"
#include <iostream>

#include <QDebug>
#include <QCoreApplication>


DataController::DataController( const QString& fileName ) {
	m_fileName = fileName;
}

void DataController::setId( const QString& animId ){
	m_id = animId;

	_initializeStates();
	_initializeCommands();
}

void DataController::createImageView( const QString& winId ){

	std::shared_ptr<TestView3> view(new TestView3( winId, QColor( "pink"), QImage()));
	auto & globals = * Globals::instance();
	IConnector * connector = globals.connector();
	connector-> registerView( view.get());
	m_views.append( view );
	_loadView( 0 );
}

void DataController::_loadView( int frame ){

	auto & globals = * Globals::instance();
	IConnector * connector = globals.connector();
	auto loadImageHookHelper = globals.pluginManager()-> prepare<LoadImage>( m_fileName, frame );
	Nullable<QImage> res = loadImageHookHelper.first();
	if( res.isNull()) {
		qDebug() << "Could not find any plugin to load image";
	}
	else {
	    for ( auto view : m_views ){
	    	view->resetImage( res.val());
	    	connector->refreshView( view.get() );
	    }
	}
}

void DataController::_initializeStates(){
	//Right now we are hard-coding some states.  Plan is to read them from
	//stored settings.
	auto & globals = * Globals::instance();
	IConnector * connector = globals.connector();
	connector->setState( StateKey::ANIMATOR_CHANNEL_FRAME, m_id, "0" );
	connector->setState( StateKey::ANIMATOR_CHANNEL_FRAME_END, m_id,  "41");
	connector->setState( StateKey::ANIMATOR_CHANNEL_FRAME_START, m_id, "0");
	connector->setState( StateKey::ANIMATOR_CHANNEL_FRAME_STEP, m_id, "1");
	connector->setState( StateKey::ANIMATOR_CHANNEL_FRAME_RATE, m_id, "20");
	connector->setState( StateKey::ANIMATOR_CHANNEL_END_BEHAVIOR, m_id, "Wrap");
}



int DataController::_setFrame( const QString& params ){
	int frameValue = -1;
	bool validFrame = false;
	QString basePath = "";
	auto & globals = * Globals::instance();
	IConnector * connector = globals.connector();
	QStringList paramArray = params.split( ",");
	for ( int i = 0; i < paramArray.size(); i++ ){
		QStringList param = paramArray[i].split( ":");
		if ( param[0] == "frame"){
			frameValue = param[1].toInt( &validFrame );
			if ( validFrame ){
				QString frameStartStr = connector->getState( StateKey::ANIMATOR_CHANNEL_FRAME_START, m_id);
				QString frameEndStr = connector->getState( StateKey::ANIMATOR_CHANNEL_FRAME_END, m_id);
				if ( frameValue <  frameStartStr.toInt()){
					validFrame = false;
					frameValue = -1;
				}
				else if ( frameValue > frameEndStr.toInt()){
					validFrame = false;
					frameValue = -1;
				}
			}
		}
		else if ( param[0] == "path"){
			basePath = param[1];
		}
	}
	if ( validFrame ){
		connector-> setState( StateKey::ANIMATOR_CHANNEL_FRAME, m_id, QString::number(frameValue));
	}
	return frameValue;
}

void DataController::_initializeCommands(){
	auto & globals = * Globals::instance();
	IConnector * connector = globals.connector();
	QString frameKey = StateLibrary::instance()->getPath( StateKey::ANIMATOR_CHANNEL_FRAME, m_id );
	connector-> addStateCallback( frameKey, [=] ( const QString & /*path*/, const QString & val) {

		        bool validInt = true;
		        int frame = val.toInt( &validInt);
		        if ( validInt ){
		        	_loadView( frame );
		        }
		        else {
		        	qDebug() << "Value was not a valid int="<<val;
		        }
		    });
	QString setFrameKey( StateLibrary::SEPARATOR + StateLibrary::APP_ROOT + StateLibrary::SEPARATOR+
			StateLibrary::ANIMATOR + StateLibrary::SEPARATOR + StateLibrary::CHANNEL+StateLibrary::SEPARATOR+
			"setFrame"+StateLibrary::SEPARATOR+ m_id);
	connector->addCommandCallback( setFrameKey, [=] (const QString & /*cmd*/,
			 const QString & params, const QString & /*sessionId*/) -> QString {
	      	  int frameValue = _setFrame( params );
	        return QString("frame=%1").arg(frameValue);
	    });

}
