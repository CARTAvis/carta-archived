/**
 * Pavol's experiments
 */


#pragma once

#include "CartaLib/IImage.h"
#include "Algorithms/RawView2QImageConverter.h"
#include "IConnector.h"
#include "PluginManager.h"
#include "IPlatform.h"

#include <QObject>

///
/// \brief The HackView class contains Pavol's experiments.
///
/// The purpose of this class is to keep the Viewer class code as clean as possible.
///
class HackViewer : public QObject
{
    Q_OBJECT
public:
    /// constructor
    /// should be called when platform is initialized, but connector isn't
    explicit HackViewer( QString prefix = "/hacks");

    /// this should be called when connector is already initialized (i.e. it's
    /// safe to start setting/getting state)
    void start();

signals:

public slots:

protected:

    // prefixed version of set state
    void setState(const QString & path,  const QString & value);

    // prefixed version of getState
    QString getState(const QString & path);

    // prefixed version of addStateCallback
    IConnector::CallbackID addStateCallback( QString path, IConnector::StateChangedCallback cb);

    /// pointer to the loaded image
    Image::ImageInterface::SharedPtr m_image = nullptr;
    /// are we recomputing clip on frame change?
    bool m_clipRecompute = true;
    /// pointer to the rendering algorithm
    Carta::Core::RawView2QImageConverter3::UniquePtr m_rawView2QImageConverter;
    /// current frame
    int m_currentFrame = -1;

    /// pointer to connector
    IConnector * m_connector;

    /// coordinate formatter
    CoordinateFormatterInterface::SharedPtr m_coordinateFormatter;

    /// our own part of the state
    QString m_statePrefix;

    bool m_cmapUseCaching = true;
    bool m_cmapUseInterpolatedCaching = true;
    int m_cmapCacheSize = 1000;

    void reloadFrame();
};
