/**
  * ServerPlatform does not really do much at the moment, other than creating
  * a server connector.
  **/

#include "StateXmlWriter.h"
#include "common/State/StateInterface.h"
#include "common/State/ObjectManager.h"
#include <QDebug>

StateXmlWriter::StateXmlWriter(const QString& filePath){
	m_file.setFileName( filePath );
	m_tagWriteIndex = 0;
	m_foundRoot = false;
	m_persistentElement = false;
	m_characterWritten = false;
}


bool StateXmlWriter::startElement(const QString & /*namespaceURI*/, const QString & localName,
					const QString & /*qName*/, const QXmlAttributes & /*atts*/ ){
        ObjectManager* objManager = ObjectManager::objectManager();
		if ( localName == objManager->getRootPath() ){
			m_foundRoot = true;
		}
		if ( m_foundRoot ){
		    m_characterWritten = false;
			m_pathNames.append( localName );
			m_tagWriteIndex = qMin( m_tagWriteIndex, m_pathNames.size() - 1 );
		}
		return true;
}

bool StateXmlWriter::endElement ( const QString & /*namespaceURI*/, const QString & localName,
		const QString & /*qName*/ ){
	int pathCount = m_pathNames.size();
	if ( pathCount > 1 ){
		if ( m_foundRoot && m_pathNames[pathCount-1] == localName ){
		    if ( m_characterWritten ){
		        m_xmlWriter.writeEndElement();
		    }
		    m_pathNames.pop();
		}
	}
	else {
	    m_characterWritten = false;
	}
	return true;
}


bool StateXmlWriter::characters( const QString& data ){
	if ( m_foundRoot && data.trimmed().length() > 0 ){

		QString lookup("");
		int pathCount = m_pathNames.size();
		for ( int i = 0; i < pathCount; i++ ){
			lookup.append( "/"+m_pathNames[i]);
		}

		for ( int i = m_tagWriteIndex; i < pathCount-1; i++ ){
			m_xmlWriter.writeStartElement( m_pathNames[i] );
		}
		m_tagWriteIndex= pathCount - 1;
		m_xmlWriter.writeTextElement( m_pathNames[pathCount-1], data.trimmed());
		m_characterWritten = true;

		if ( m_pathNames.size() > 1 ){
		    m_pathNames.pop();
		}
	}
	return true;
}


bool StateXmlWriter::startDocument(){
	bool successfulStart = true;

	if ( !m_file.open( QIODevice::WriteOnly | QIODevice::Truncate ) ){
		successfulStart = false;
	}
	else {
		m_xmlWriter.setDevice( &m_file );
		m_xmlWriter.setAutoFormatting( true );
		m_xmlWriter.writeStartDocument();
	}
	return successfulStart;
}


bool StateXmlWriter::endDocument(){
    m_xmlWriter.writeEndElement();
	m_xmlWriter.writeEndDocument();
	m_file.flush();
	m_file.close();
	return true;
}
