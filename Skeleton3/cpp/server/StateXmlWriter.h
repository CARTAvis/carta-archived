/**
 * Writes the state as XML to a file.
 */
#ifndef SERVER_STATEXMLWRITER_H
#define SERVER_STATEXMLWRITER_H

#include <QXmlDefaultHandler>
#include <QXmlStreamWriter>
#include <QString>
#include <QStack>


class StateXmlWriter : public QXmlDefaultHandler
{

public:

    StateXmlWriter( const QString& filePath);

    virtual bool characters( const QString& ch );
    virtual bool startElement(const QString & namespaceURI, const QString & localName,
                              const QString & qName, const QXmlAttributes & atts );
    virtual bool endElement ( const QString & namespaceURI, const QString & localName,
                              const QString & qName );
    virtual bool startDocument();
    virtual bool endDocument();
protected:

private:
    bool m_characterWritten;
    bool m_foundRoot;
    bool m_persistentElement;
    QXmlStreamWriter m_xmlWriter;
    QFile m_file;
    int m_tagWriteIndex;
    QStack<QString> m_pathNames;

};

#endif // SERVER_STATEXMLWRITER_H
