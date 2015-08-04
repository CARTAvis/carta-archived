/**
 *
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "IView.h"

#include <QImage>
#include <QPainter>
#include <QSize>
#include <unordered_set>

namespace Carta
{
namespace Lib
{
namespace SceneGraph2D
{
class Node
{
    CLASS_BOILERPLATE( Node );

public:

    virtual const std::vector < Node::SharedPtr > &
    children();

    virtual void
    add( Node::SharedPtr node );

private:

    std::vector < Node::SharedPtr > m_children;
};

class Circle : public Node
{
    CLASS_BOILERPLATE( Circle );

public:

    static SharedPtr
    create()
    {
        return std::make_shared < Circle > ();
    }
};

class Root : public Node
{
    CLASS_BOILERPLATE( Root );

public:

    static Root::SharedPtr
    create()
    {
        return std::make_shared < Root > ();
    }
};
}
}
}

namespace Carta
{
namespace Core
{
class IRasterLayer : QObject
{
    Q_OBJECT
    CLASS_BOILERPLATE( IRasterLayer );

public:

    virtual QSize
    size() = 0;

    virtual void
    setSize( const QSize & size ) = 0;

    virtual const QImage &
    getBuffer() = 0;

signals:

    void
    repaintNeeded();
};

class IVectorLayer
{
    CLASS_BOILERPLATE( IVectorLayer );

public:

    virtual QSize
    size() = 0;

    virtual void
    setSize( const QSize & size ) = 0;

    virtual const QImage &
    getBuffer() = 0;
};

class LayeredView : public QObject, public IView
{
    LayeredView( QString name, QObject * parent = nullptr );

    void
    addLayer( int z, IRasterLayer::SharedPtr layer )
    {
        Q_UNUSED( z );
        Q_UNUSED( layer );
    }

protected:

    virtual void
    registration( IConnector * connector )
    {
        Q_UNUSED( connector );
    }

    virtual const QString &
    name() const
    {
        return m_name;
    }

    virtual QSize
    size()
    {
        return QSize( 1, 1 );
    }

    virtual const QImage &
    getBuffer()
    {
        return m_imageBuffer;
    }

    virtual void
    handleResizeRequest( const QSize & size )
    {
        Q_UNUSED( size );
    }

    virtual void
    handleMouseEvent( const QMouseEvent & event )
    {
        Q_UNUSED( event );
    }

    virtual void
    handleKeyEvent( const QKeyEvent & event )
    {
        Q_UNUSED( event );
    }

    QImage m_imageBuffer;
    QString m_name;
};
}
}

namespace Experimental
{
/// something that can be rendered to QPainter synchronously
class IRenderableQPainterSync
{
public:

    virtual void
    renderSync( QPainter * p ) = 0;
};

/// something that can be rendered to QPainter asynchronously
///
/// part of my brainstorming (Pavol)
class IRenderableQPainterASync
    : public QObject
{
    Q_OBJECT

public slots:

    virtual void
    renderASync( QPainter * p ) = 0;

signals:

    void
    done();
};


/// interface representing the minimal APIs for rendering VGlist
class IRenderer
{
public:

    virtual void
    drawCircle( const QPointF & center, double radius ) = 0;

    virtual void
    drawLine( const QPointF & p1, const QPointF & p2 ) = 0;
};

// forward ref.
class VGList;

/// this class can render a VGList using QPainter APIs
class RendererQPainter : public IRenderer
{
public:

    /// set the destination image
    /// ownership remains with the caller, but the image must exist while calling
    /// methods of this class
    RendererQPainter( QPainter * painter )
    {
        CARTA_ASSERT( painter );
        m_qpainter = painter;
    }

    void
    virtual
    drawCircle( const QPointF & center, double radius ) override
    {
        Q_UNUSED( center );
        Q_UNUSED( radius );
    }

    virtual void
    drawLine( const QPointF & p1, const QPointF & p2 ) override
    {
        Q_UNUSED( p1 );
        Q_UNUSED( p2 );
    }

    void
    rasterize( VGList & vg );

private:

    QPainter * m_qpainter = nullptr;
};

class IVGListEntry
{
public:

    virtual void
    cplusplus( RendererQPainter & painter ) = 0;

    virtual QStringList
    javascript() = 0;

    virtual
    ~IVGListEntry() { }
};

class Circle : public IVGListEntry
{
public:

    Circle( QPointF center, double radius )
    {
        m_center = center;
        m_radius = radius;
    }

    virtual void
    cplusplus( RendererQPainter & painter )
    {
        painter.drawCircle( { 10.0, 20.0 }, 5.0 );
    }

    virtual QStringList
    javascript()
    {
        return QStringList()
               << QString( "p.circle(%1,%2,%3)" )
                   .arg( m_center.x() ).arg( m_center.y() ).arg( m_radius );
    }

private:

    QPointF m_center;
    double m_radius;
};

class Line : public IVGListEntry
{
public:

    Line( const QPointF & p1, const QPointF & p2 )
    {
        m_p1 = p1;
        m_p2 = p2;
    }

    virtual void
    cplusplus( RendererQPainter & painter )
    {
        painter.drawLine( m_p1, m_p2 );
    }

    virtual QStringList
    javascript()
    {
        return QStringList()
               << QString( "p.line(%1,%2,%3,%4)" )
                   .arg( m_p1.x() ).arg( m_p1.y() ).arg( m_p2.x() ).arg( m_p2.y() );
    }

private:

    QPointF m_p1, m_p2;
};

/// class capable of creating / modifying a list of vector graphics commands
/// it is also capable of tracking (simplistically) a list of differences
class VGList
{
public:

    /// reset everything (equivalent of creating brand new instance)
    void
    reset();

    /// start recording changes
    void
    resetChanges();

    /// get the recorded changes
    const std::unordered_set < u_int64_t > &
    getChanges();

    /// get an entry at given index
    IVGListEntry *
    getEntry( int64_t ind );

    /// get current number of entries
    int64_t
    nEntries();

    /// add an entry and we become an owner
    int64_t
    addEntry( IVGListEntry * entry );

    /// templated version of addEntry with parameter forwarding
    template < typename EntryType, typename ... Args >
    int64_t
    add( Args && ... params )
    {
        return addEntry( new EntryType( std::forward < Args > ( params ) ... ) );
    }

    /// set a specific entry
    void
    setEntry( int64_t ind, IVGListEntry * entry );

    /// templated version of setEntry()
    template < typename EntryType, typename ... Args >
    void
    set( int64_t ind, Args && ... params )
    {
        return setEntry( ind, new EntryType( std::forward < Args > ( params ) ... ) );
    }

    ~VGList();

private:

    /// here we keep the actual entries
    std::vector < IVGListEntry * > m_entries;

    /// here we keep the list of changed things
    std::unordered_set < u_int64_t > m_changes;
};
}
