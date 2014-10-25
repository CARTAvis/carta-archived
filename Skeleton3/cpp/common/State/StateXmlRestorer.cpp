/**
 * ServerPlatform does not really do much at the moment, other than creating
 * a server connector.
 **/

#include "StateXmlRestorer.h"
#include "common/IConnector.h"
#include "StateInterface.h"
#include "ObjectManager.h"
#include <QDebug>

StateXmlRestorer::StateXmlRestorer(IConnector* connector) :
        m_connector(connector) {
    m_tagWriteIndex = 0;
    m_foundRoot = false;
}

bool StateXmlRestorer::startElement(const QString & /*namespaceURI*/,
        const QString & localName, const QString & /*qName*/,
        const QXmlAttributes & /*atts*/) {
    ObjectManager* objManager = ObjectManager::objectManager();
    if (localName == objManager->getRootPath()) {
        m_foundRoot = true;
    }
    if (m_foundRoot) {
        m_pathNames.append(localName);
        m_tagWriteIndex = qMin(m_tagWriteIndex, m_pathNames.size() - 1);
    }
    return true;
}

bool StateXmlRestorer::endElement(const QString & /*namespaceURI*/,
        const QString & localName, const QString & /*qName*/) {
    int pathCount = m_pathNames.size();
    if (pathCount > 1) {
        if (m_foundRoot && m_pathNames[pathCount - 1] == localName) {
            m_pathNames.pop();
        }

    }
    return true;
}

bool StateXmlRestorer::characters(const QString& data) {
    if (m_foundRoot && data.trimmed().length() > 0) {
        QString lookup("");
        int pathCount = m_pathNames.size();
        for (int i = 0; i < pathCount; i++) {
            lookup.append(StateInterface::DELIMITER + m_pathNames[i]);
        }
        /*QString lastComponent = m_pathNames[pathCount-1];
        if ( lastComponent.at(lastComponent.length() -1).isDigit()){
            indexStr = lastComponent;
        }
        else {
            lookup.append( "/"+lastComponent);
        }*/
        m_connector->setState(lookup, data );
        m_pathNames.pop();
    }
    return true;
}
