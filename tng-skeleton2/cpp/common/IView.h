/**
 *
 **/

#ifndef COMMON_IVIEW_H
#define COMMON_IVIEW_H

class IConnector;
class QImage;
class QMouseEvent;
class QKeyEvent;
class QString;
class QSize;

/**
 * @brief The IView interface defines all functionality that a server side view
 * has to implement to create an object that can interact with the client side view.
 */
class IView {

public:
    /// accept a connector during registration
    /// called once during IConnector::registerView()
    /// store the value if you would like to request repaint inside the implementation
    virtual void registration( IConnector * connector) = 0;

    /// get the name of the view
    virtual const QString & name() const = 0;

    /// handle request for obtaining size
    virtual QSize size() = 0;

    /// handle request for getting the draw buffer that will be drawn
    /// strongly suggested format: QImage::Format_RGB888, to avoid conversion
    virtual const QImage & getBuffer() = 0;

    /// handle client resize request
    virtual void handleResizeRequest( const QSize & size) = 0;

    /// handle mouse events
    virtual void handleMouseEvent( const QMouseEvent & event) = 0;

    /// handle keyboard events
    virtual void handleKeyEvent( const QKeyEvent & event) = 0;


    virtual ~IView() {}
};

#endif // COMMON_IVIEW_H
