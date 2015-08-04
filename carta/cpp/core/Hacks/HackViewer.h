/**
 * Pavol's experiments
 */

#pragma once

#include "../IConnector.h"
#include "../PluginManager.h"
#include "../IPlatform.h"
#include "CartaLib/IImage.h"
#include "ImageViewController.h"

#include <QPixmap>
#include <QObject>

namespace Hacks
{

///
/// \brief The HackView class contains Pavol's experiments.
///
/// The purpose of this class is to keep the Viewer class code as clean as possible.
///
class HackViewer : public QObject
{
    Q_OBJECT

    CLASS_BOILERPLATE( HackViewer);

public:

    /// constructor
    /// should be called when platform is initialized, but connector isn't
    explicit
    HackViewer( QString prefix = "/hacks" );

    /// this should be called when connector is already initialized (i.e. it's
    /// safe to start setting/getting state)
    void
    start();

protected slots:

protected:

    // prefixed version of set state
    void
    prefixedSetState( const QString & path,  const QString & value );

    // prefixed version of getState
    QString
    prefixedGetState( const QString & path );

    // prefixed version of addStateCallback
    IConnector::CallbackID
    prefixedAddStateCallback( QString path, IConnector::StateChangedCallback cb );

    /// pointer to connector
    IConnector * m_connector;

    /// our own part of the state
    QString m_statePrefix;

    // view controller with the new render service
    Hacks::ImageViewController::UniquePtr m_imageViewController;
    std::vector < Carta::Lib::PixelPipeline::IColormapNamed::SharedPtr > m_allColormaps;

};
}
