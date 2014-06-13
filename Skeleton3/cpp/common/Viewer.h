/***
 * Implementation of the sample application.
 *
 */

#pragma once

#include <QObject>


class Viewer : public QObject
{

    Q_OBJECT

public:

    // constructor - does not need to do anything
    // it gets called/constructed by skeleton when connector is not yet connected, but platform is
    // functioning
    explicit Viewer();

    /// called by skeleton when connector is already initialized (i.e. it's
    /// safe to start setting/getting state)
    void start();

signals:

public slots:

protected:

};
