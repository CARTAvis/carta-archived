/***
 * Implementation of the sample application
 *
 */

#pragma once

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

};
