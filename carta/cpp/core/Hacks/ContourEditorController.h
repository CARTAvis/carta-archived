/**
 * Helper class to manage as many contour related activities as possible. It acts
 * both as a controller and a contour vector graphics generator...
 *
 * - monitors the shared state to see when user changed the UI related to contours
 * - it can start a rendering request (asynchronously)
 * - it reports rendered vector graphics via signal
 *
 * It is essentially a combination of service & controller in one class.
 **/

#pragma once

#include "CartaLib/IWcsGridRenderService.h"
#include "CartaLib/IContourGeneratorService.h"
#include "CartaLib/CartaLib.h"
#include "core/IConnector.h"
#include "core/Hacks/SharedState.h"
#include "core/DefaultContourGeneratorService.h"
#include <QObject>

namespace Carta
{
namespace Hacks
{
namespace SS = Carta::Lib::SharedState;

class ContourEditorController : public QObject
{
    Q_OBJECT
    CLASS_BOILERPLATE( ContourEditorController );

public:

    /// job IDs
    typedef int64_t JobId;

    explicit
    ContourEditorController( QObject * parent,
                             QString pathPrefix
                             );

    /// start rendering with current parameters
    /// if possible, previously scheduled rendering will be canceled
    /// or you can think of it as this: multiple calls to this will normally only
    /// result in a single result reported
    ///
    /// \param jobId id assigned to the rendering request, which will be reported back
    /// witht the done() signal, which can be used to make sure the arrived done() signal
    /// corresponds to the latest request. It should be a positive number. If unspecified
    /// (or negative number is supplied, a new id will be generated, which will
    /// the previous one + 1)
    /// \return the jobId to expect when the rendering is done (useful for unspecified
    /// jobId)
    JobId
    startRendering( JobId jobId = - 1 );

    /// set the input data
    void
    setInput( Carta::Lib::NdArray::RawViewInterface::SharedPtr rawView );

signals:

    /// emitted when user changed some parameters, and a recompute/re-render is required
    /// \note it is up to the owner of this instance to listen for this and manually
    /// triggering startRendering(). We don't do this automatically because we may want
    /// to synchronize rendering with other layers...
    void
    updated();

    /// gets emitted when rendering is done
    /// \param vg the rendered vector graphics
    /// \param id the jobId this result corresponds to
    void
    done( Carta::Lib::VectorGraphics::VGList vg, JobId id );

public slots:

private slots:

    void
    contourServiceCB( const Carta::Core::DefaultContourGeneratorService::Result & result,
                      JobId jobId );

private:

    /// keep track of the last requested job
    JobId m_lastJobId = - 1;

    /// shared callback for all state variables
    void
    stdVarCB();

    IConnector * m_connector = nullptr;
    QString m_statePrefix = "/";

    SS::StringVar::UniquePtr m_stringVar;

    Carta::Core::DefaultContourGeneratorService::UniquePtr m_contourSvc = nullptr;

    std::vector<QPen> m_pens;
};
}
}
