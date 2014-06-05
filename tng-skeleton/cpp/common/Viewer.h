/***
 * Implementation of the sample application
 *
 */

#ifndef VIEWER_H
#define VIEWER_H

#include "IPlatform.h"
#include <QObject>

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

};

#endif // VIEWER_H
