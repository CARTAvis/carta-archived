/**
 *
 **/

#pragma once

class IConnector;
class QImage;
class QMouseEvent;
class QKeyEvent;
class QString;
class QSize;

#include <QtGlobal>

/**
 * @brief The IView interface defines the necessary functionality that a server side view
 * has to implement to create an object that can interact with the client side view.
 *
 * This is an API for views that render themselves as raster images only.
 *
 * \todo rename this to IRasterView
 */
class IView
{
public:

    /// This is a (silly?) mechanism for letting the view know who is the connector...
    /// You can store the connector if you would like to interact with it, for example to
    /// request repaint inside the implementation (connector-> refreshView())
    ///
    /// This method is called exactly once, \underline directly from
    /// IConnector::registerView()
    ///
    /// We could call this 'setConnector', but that might hide the fact that it
    /// is called as part of the registration process...
    ///
    /// I don't like this API... I feel it's kind of confusing. The connector
    /// could simply be passed into the instance via constructor, or through some other
    /// manual way. Maybe it's completely unnecssary for this method to even exist.
    /// Another point to consider is that there can only be one connector, which is already
    /// available via the global singleton Globals...
    ///
    /// \warning possibly deprecated
    virtual void
    registration( IConnector * connector ) = 0;

    /// get the name of the view
    /// \note must be unique
    virtual const QString &
    name() const = 0;

    /// handle request for obtaining size
    /// \note this must match the size returned by getBuffer()
    virtual QSize
    size() = 0;

    /// handle request for getting the draw buffer that will be drawn
    /// \note strongly suggested format: QImage::Format_ARGB32_Premultiplied, as that is
    /// the most optimized version
    ///
    virtual const QImage &
    getBuffer() = 0;

    /// handle client resize request
    virtual void
    handleResizeRequest( const QSize & size ) = 0;

    /// handle mouse events
    /// \deprecated this is going away in the future, or at least will become optional
    virtual void
    handleMouseEvent( const QMouseEvent & event ) = 0;

    /// handle keyboard events
    /// \deprecated this may be removed altogether, or become optional
    virtual void
    handleKeyEvent( const QKeyEvent & event ) = 0;

    /// this is called when a view is refreshed in the GUI
    virtual void viewRefreshed( qint64 id) = 0;

    virtual
    ~IView() { }
};
