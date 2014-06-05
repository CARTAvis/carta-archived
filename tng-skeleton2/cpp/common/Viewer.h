/***
 * Implementation of the sample application
 *
 */

#ifndef VIEWER_H
#define VIEWER_H

#include <QObject>

class IPlatform;
class PluginManager;

class Viewer : public QObject
{
    Q_OBJECT
public:
    explicit Viewer(IPlatform *platform);

    void start();

signals:

public slots:

protected:

    IPlatform * m_platform;
    PluginManager * m_pluginManager;

};

#endif // VIEWER_H
