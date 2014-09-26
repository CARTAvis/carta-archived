/**
 * Restores the state from XML written to a file.
 */
#ifndef STATE_STATEXMLRESTORER_H
#define STATE_STATEXMLRESTORER_H

#include <QXmlDefaultHandler>
#include <QString>
#include <QStack>

class IConnector;

class StateXmlRestorer: public QXmlDefaultHandler {

public:

    StateXmlRestorer(IConnector* connector);

    virtual bool characters(const QString& ch);
    virtual bool startElement(const QString & namespaceURI,
            const QString & localName, const QString & qName,
            const QXmlAttributes & atts);
    virtual bool endElement(const QString & namespaceURI,
            const QString & localName, const QString & qName);

private:
    bool m_foundRoot;
    int m_tagWriteIndex;
    IConnector* m_connector;
    QStack<QString> m_pathNames;

};

#endif // STATE_STATEXMLRESTORER_H

