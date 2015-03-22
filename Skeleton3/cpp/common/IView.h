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

/**
 * @brief The IView interface defines the necessary functionality that a server side view
 * has to implement to create an object that can interact with the client side view.
 *
 * This is an API for views that render themselves as raster images only.
 */
class IView
{

public:

    /// This is a silly mechanism for letting the view know who is the connector :)
    /// store the value if you would like to interact with the connector, for example to
    /// request repaint inside the implementation (connector-> refreshView())
    ///
    /// \note this method is called exactly once, \underline directly from
    /// IConnector::registerView()
    /// \todo we could call this 'setConnector', but that might hide the fact that it
    /// is called as part of the registration process...
    /// I am not sure I like this API... I feel it's kind of confusing. The connector
    /// could simply be passed into the instance via constructor, or through some other
    /// manual way. Maybe it's completely unnecssary for this method to even exist.
    virtual void registration( IConnector * connector) = 0;

    /// get the name of the view
    /// \note must be unique
    virtual const QString & name() const = 0;

    /// handle request for obtaining size
    /// \note this must match the size returned by getBuffer()
    virtual QSize size() = 0;

    /// handle request for getting the draw buffer that will be drawn
    /// \note strongly suggested format: QImage::Format_RGB888, to avoid conversion
    virtual const QImage & getBuffer() = 0;

    /// handle client resize request
    virtual void handleResizeRequest( const QSize & size) = 0;

    /// handle mouse events
    /// \deprecated this is going away in the future, or at least will become optional
    virtual void handleMouseEvent( const QMouseEvent & event) = 0;

    /// handle keyboard events
    /// \deprecated this may be removed altogether, or become optional
    virtual void handleKeyEvent( const QKeyEvent & event) = 0;

    virtual ~IView() {}
};

// ==================================================================================
// WARNING * WARNING * WARNING * WARNING * WARNING * WARNING * WARNING * WARNING
//
// highly experimental stuff below
// not ready for consumption
//
// WARNING * WARNING * WARNING * WARNING * WARNING * WARNING * WARNING * WARNING
// ==================================================================================

#include <QtGlobal>
#include <bitset>
#include <memory>
/// placeholder for API for acessing vector graphics list
/// it needs to support
///   - rasterization
///   - getting a complete list of vector graphics commands
///   - getting a change list from last invocation
class IVectorGraphicsWithChangeset {
    typedef int IVGListEntry; // placeholder

    typedef std::vector<bool> VarBitSet;
public:
    /// return all entries
    virtual std::vector<IVGListEntry *> & entries() = 0;
    /// add a bit set to track changes
    /// ownership remains with caller
    virtual void addChangeSet( VarBitSet * vbs) = 0;
    /// remove a bit set
    /// the bitset is not deleted, it's up to the caller to do that
    virtual void removeChangeSet( VarBitSet * vbs) = 0;

    /// returns number of entries
//    virtual quint64 nEntries() = 0;
    /// get an actual entry at specified index
//    virtual IVGListEntry & entry( quint64 ind) = 0;

    typedef int CheckPoint;
    virtual CheckPoint makeNewCheckpoint() = 0;
    /// get the list of changes since check point (a list of indices)
    /// a list with just one negative entry has a special meaning: everything changed...
    virtual VarBitSetPtr changes( CheckPoint checkPoint) = 0;

    /// get the list of changes since check point (a list of indices)
    /// a list with just one negative entry has a special meaning: everything changed...
    virtual std::vector<bool> changes( qint64 checkPoint) = 0;
    /// mark any checkpoint below this one as un-needed
    virtual void deleteCheckpoint( qint64 checkPoint) = 0;
};

/**
 * minimal API to implement remote vector graphics view
 */
class IVectorGraphicsView
{
public:

    /// called to get the unique name of the view
    virtual const QString & name() const = 0;

    /// called when client resizes the view
    virtual void handleResizeRequest( const QSize & size) = 0;

    /// called to retrieve the vector graphics to be rendered
    virtual IVectorGraphicsWithChangeset & getVectorGraphics() = 0;

    virtual ~IVectorGraphicsView() {}

};
