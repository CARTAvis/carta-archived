/***
 * Implementation of the sample application
 *
 */

#pragma once

#include <QObject>

class IPlatform;

class Viewer : public QObject
{

    Q_OBJECT

public:

    // instanciate a viewer with the proper platform
    explicit Viewer(IPlatform * platform);

    /// give up control to the viewer
    /// this does not return until application quits
    int start();

signals:

public slots:

protected:

};
