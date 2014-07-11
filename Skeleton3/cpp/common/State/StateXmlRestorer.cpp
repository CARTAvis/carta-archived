/**
  * ServerPlatform does not really do much at the moment, other than creating
  * a server connector.
  **/

#include "StateXmlRestorer.h"
#include "common/IConnector.h"
#include "StateLibrary.h"
#include <QDebug>

StateXmlRestorer::StateXmlRestorer(IConnector* connector)
	:m_connector( connector ){
	m_tagWriteIndex = 0;
	m_foundRoot = false;
}


bool StateXmlRestorer::startElement(const QString & /*namespaceURI*/, const QString & localName,
					const QString & /*qName*/, const QXmlAttributes & /*atts*/ ){

		if ( localName == StateLibrary::APP_ROOT){
			m_foundRoot = true;
		}
		if ( m_foundRoot ){
			m_pathNames.append( localName );
			m_tagWriteIndex = qMin( m_tagWriteIndex, m_pathNames.size() - 1 );
		}
		return true;
}

bool StateXmlRestorer::endElement ( const QString & /*namespaceURI*/, const QString & localName,
		const QString & /*qName*/ ){
	int pathCount = m_pathNames.size();
	if ( pathCount > 0 ){
		if ( m_foundRoot && m_pathNames[pathCount-1] == localName ){
			m_pathNames.pop();
		}

	}
	return true;
}


bool StateXmlRestorer::characters( const QString& data ){
	if ( m_foundRoot && data.trimmed().length() > 0 ){
		QString lookup("");
		int pathCount = m_pathNames.size();
		for ( int i = 0; i < pathCount - 1; i++ ){
			lookup.append( "/"+m_pathNames[i]);
		}
		StateKey key = StateLibrary::instance()->findKey( lookup );
		if ( key != StateKey::END_KEY ){
			m_connector->setState( key, m_pathNames[pathCount-1], data );
		}
		else {
			qDebug() << "Could not find key for path="<<lookup;
		}
		m_pathNames.pop();
	}
	return true;
}
