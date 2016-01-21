// ==================================================================================
// WARNING * WARNING * WARNING * WARNING * WARNING * WARNING * WARNING * WARNING
//
// highly experimental stuff below
// not ready for consumption
//
// WARNING * WARNING * WARNING * WARNING * WARNING * WARNING * WARNING * WARNING
// ==================================================================================

#ifdef DONT_COMPILE


#pragma once


#include <bitset>
#include <unordered_set>
#include <memory>
#include <QtGlobal>
#include <QPointF>
#include <vector>

class QSize;

namespace Carta {
namespace Lib {

}
}

namespace Hacks {

// placeholder
class IVGListEntry
{ };

/// placeholder for API for acessing vector graphics list
/// it needs to support
///   - rasterization
///   - getting a complete list of vector graphics commands
///   - getting a change list from last invocation
class IVectorGraphicsListWithChangeset
{
    typedef std::unordered_set < bool > ChangeSet;

public:

    /// return all entries
    virtual std::vector < IVGListEntry * > &
    entries() = 0;

    /// add a bit set to track changes
    /// ownership remains with caller
    virtual void
    addChangeSet( ChangeSet * vbs ) = 0;

    /// remove a bit set
    /// the set is not deleted by the method, it's up to the caller to do that
    virtual void
    removeChangeSet( ChangeSet * vbs ) = 0;
};

class VGInteractionEvent
{
public:

    enum Type { CLICK, HOVER, DRAG, KEY };
    QPointF mouse();
    int buttons();
    int key();
    QString id();
};

/**
 * minimal API to implement remote vector graphics view
 */
class IVectorGraphicsView
{
public:

    /// should return a unique name of the view
    virtual QString
    name() = 0;

    /// will be called when the client resizes the view
    virtual void
    handleResizeRequest( const QSize & size ) = 0;

    /// will be called to retrieve the vector graphics to be rendered
    virtual IVectorGraphicsListWithChangeset &
    getVectorGraphics() = 0;

    /// will be called with information regarding interaction from client
    virtual void
    interactionCB( const VGInteractionEvent & ev ) = 0;

    virtual
    ~IVectorGraphicsView() { }
};

}

#endif
