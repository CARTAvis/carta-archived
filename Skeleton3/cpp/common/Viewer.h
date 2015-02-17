/**
 * The viewer application represented as an object...
 */

#pragma once

#include <QObject>
#include <memory>
#include "ScriptFacade.h"
#include <rapidjson/document.h>

using namespace rapidjson;

class ScriptedCommandListener;
namespace Carta {
namespace Data {
class ViewManager;
}
}

///
/// \brief The Viewer class is the main class of the viewer. It sets up all other
/// components.
///
class Viewer : public QObject
{
    Q_OBJECT

public:

    /// constructor
    /// should be called when platform is initialized, but connector isn't
    explicit Viewer();

    /// this should be called when connector is already initialized (i.e. it's
    /// safe to start setting/getting state)
    void start();

    /**
     * Show areas under active development.
     */
    void setDeveloperView( );

signals:

public slots:

protected slots:

    /// internal callback for scripted commands
    void scriptedCommandCB(QString command);

protected:

    /// pointer to scripted command listener
    /// @todo make it unique ptr for auto-delete niceness
    ScriptedCommandListener * m_scl = nullptr;

    ScriptFacade* m_scriptFacade;

private:

    bool m_devView;

    std::shared_ptr<Carta::Data::ViewManager> m_viewManager;

private:
    /// Recursively parse through a directory structure contained in a json value
    QStringList _parseDirectory( const Value& dir, QString prefix );
};

