/***
 * Implementation of the sample application.
 *
 */

#pragma once

#include "CartaLib/IImage.h"
#include <QObject>
//#include <QList>
#include <memory>


class ScriptedCommandListener;
class ViewManager;

//class RawView2QImageConverter;
//class IConnector;

class Viewer : public QObject
{

    Q_OBJECT

public:

    /// constructor - does not need to do anything
    /// it gets called/constructed by skeleton when connector is not yet connected, but platform is
    /// functioning
    explicit Viewer();

    /// called by skeleton when connector is already initialized (i.e. it's
    /// safe to start setting/getting state)
    void start();

signals:

public slots:


protected slots:

    /// internal callback for scripted commands
    void scriptedCommandCB(QString command);

    //void reloadFrame(bool forceClipRecompute = false);

    //void mouseCB( const QString & path, const QString & val);

protected:

    /// pointer to scripted command listener
    ScriptedCommandListener * m_scl = nullptr;

    /// pointer to the loaded image
    Image::ImageInterface::SharedPtr m_image = nullptr;


private:
    std::shared_ptr<ViewManager> m_viewManager;
};

