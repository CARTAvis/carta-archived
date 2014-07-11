/**
 *
 **/


#ifndef DESKTOP_DESKTOPSTATEWRITER_H
#define DESKTOP_DESKTOPSTATEWRITER_H

#include <QDomElement>


class DesktopStateWriter {

public:

    /// constructor
    DesktopStateWriter( const QString& filePath );

    void addPathData( const QString& path, const QString& data );
    bool saveState();
private:
    void addChildren( QDomNode& node, QStringList& parts, const QString& value );
    QDomDocument document;
    QString fileLocation;
};


#endif // DESKTOP_DESKTOPCONNECTOR_H
