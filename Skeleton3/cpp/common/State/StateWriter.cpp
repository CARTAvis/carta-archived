/**
 *
 **/

#include "StateWriter.h"
#include "common/State/ObjectManager.h"
#include <QDebug>
#include <QFile>


StateWriter::StateWriter(const QString& filePath)
	:
	 m_state( ObjectManager::objectManager()->getRoot(), "" ),
	 m_fileLocation( filePath ){
     m_state.insertObject( ObjectManager::objectManager()->getRoot());
     m_state.flushState();
}

void StateWriter::addPathData( const QString& path, const QString& value ){
	QStringList pathParts = path.split( StateInterface::DELIMITER );
	int pathCount = pathParts.size();
	if ( pathCount > 1 ){
	    //path starts with a / so part 0 is blank.
	    QString basePath( pathParts[1]);
	    for ( int i = 2; i < pathCount - 1; i++ ){
	        basePath = basePath + StateInterface::DELIMITER + pathParts[i];
	        m_state.insertObject( basePath );
	    }
	    QString key = basePath + StateInterface::DELIMITER + pathParts[pathCount - 1];
	    m_state.insertObject( key, value );
	    m_state.flushState();
	}
}

bool StateWriter::saveState(){
	QFile file( m_fileLocation );
	bool fileSaved = true;
	if ( !file.open( QIODevice::WriteOnly) ){
		fileSaved = false;
	}
	else {
		QTextStream stream( &file );
		stream << m_state.toString();
		stream.flush();
	}
	return fileSaved;
}

