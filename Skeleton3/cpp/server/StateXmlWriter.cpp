/**
  * ServerPlatform does not really do much at the moment, other than creating
  * a server connector.
  **/

#include "StateXmlWriter.h"
#include "common/State/StateLibrary.h"
#include <QDebug>
#include <QStringBuilder>

StateXmlWriter::StateXmlWriter(const QString& filePath){
	m_file.setFileName( filePath );
	m_tagWriteIndex = 0;
	m_foundRoot = false;
	m_persistentElement = false;
}


bool StateXmlWriter::startElement(const QString & /*namespaceURI*/, const QString & localName,
					const QString & /*qName*/, const QXmlAttributes & /*atts*/ ){

		if ( localName == StateLibrary::APP_ROOT ){
			m_foundRoot = true;
		}
		if ( m_foundRoot ){
			m_pathNames.append( localName );
			m_tagWriteIndex = qMin( m_tagWriteIndex, m_pathNames.size() - 1 );
		}
		return true;
}

bool StateXmlWriter::endElement ( const QString & /*namespaceURI*/, const QString & localName,
		const QString & /*qName*/ ){
	int pathCount = m_pathNames.size();
	if ( pathCount > 0 ){
		if ( m_foundRoot && m_pathNames[pathCount-1] == localName ){

			m_pathNames.pop();
			if ( m_persistentElement ){
				m_xmlWriter.writeEndElement();
			}
		}

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


		m_persistentElement = StateLibrary::instance()->isPersistent( lookup );
		if ( m_persistentElement ){
			for ( int i = m_tagWriteIndex; i < pathCount-1; i++ ){
				m_xmlWriter.writeStartElement( m_pathNames[i] );
			}
			m_tagWriteIndex= pathCount - 1;
			m_xmlWriter.writeTextElement( m_pathNames[pathCount-1], data.trimmed());
		}
		m_pathNames.pop();
	}
	return true;
}


bool StateXmlWriter::startDocument(){
	bool successfulStart = true;
	if ( !m_file.open( QIODevice::WriteOnly) ){
		qDebug() << "StateXmlContentHandler could not open the file for writing";
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
	m_xmlWriter.writeEndDocument();
	m_file.close();
	return true;
}
