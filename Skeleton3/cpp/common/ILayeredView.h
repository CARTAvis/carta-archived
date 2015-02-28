/**
 *
 **/

#include "CartaLib/CartaLib.h"
#include "IView.h"

#include <QImage>
#include <QPainter>
#include <QSize>

#pragma once

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

    static Circle::SharedPtr
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
class IRasterLayer
{
    CLASS_BOILERPLATE( IRasterLayer );

public:

    virtual QSize
    size() = 0;

    virtual void
    setSize( const QSize & size ) = 0;

    virtual const QImage &
    getBuffer() = 0;
};

class IVectorLayer
{
    CLASS_BOILERPLATE( IRasterLayer );

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

class VGList;

/// interface representing the minimal APIs for rendering VGlist
class IRenderer
{
public:

    virtual void
    drawCircle( QPointF center, double radius ) = 0;
};


/// this class can render a VGList using QPainter APIs
class RendererQPainter : public IRenderer
{
public:

    /// set the destination image
    /// ownership remains with the caller, but the image must exist while calling
    /// methods of this class
    RendererQPainter( QPainter * painter) {
        CARTA_ASSERT( painter);
        m_qpainter = painter;
    }

    void
    virtual drawCircle( QPointF center, double radius ) override
    {
        Q_UNUSED( center );
        Q_UNUSED( radius );
    }

    void
    rasterize( VGList & vg);

private:

    QPainter * m_qpainter = nullptr;
};

class IEntry
{
public:

    virtual void
    cplusplus( RendererQPainter & painter ) = 0;

    virtual QStringList
    javascript() = 0;

    virtual
    ~IEntry() { }
};

class Circle : public IEntry
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
               << QString("p.circle(%1,%2,%3)")
                  .arg(m_center.x()).arg(m_center.y()).arg(m_radius);
    }

private:

    QPointF m_center;
    double m_radius;
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
    startChanges();

    /// get the recorded changes
    const std::vector < int64_t > &
    getChanges();

    /// get an entry from position
    IEntry *
    getEntry( int64_t ind );

    /// get current number of entries
    int64_t
    nEntries();

    /// add an entry and we become an owner
    int64_t
    addEntry( IEntry * entry );

    /// templated version of addEntry with parameter forwarding
    template < typename EntryType, typename ... Args >
    int64_t
    add( Args && ... params )
    {
        return addEntry( new EntryType( std::forward < Args > ( params ) ... ) );
    }

    /// set a specific entry
    void
    setEntry( int64_t ind, IEntry * entry );

    /// templated version of setEntry()
    template < typename EntryType, typename ... Args >
    void
    set( int64_t ind, Args && ... params )
    {
        return setEntry( ind, new EntryType( std::forward < Args > ( params ) ... ) );
    }

    ~VGList();

private:

    std::vector < IEntry * > m_entries;
};

// something drawing a full scene
VGList
drawScene()
{
    VGList vg;
    vg.reset();
    return vg;
}

// the way we call it
int
apitest()
{
    // get the vector graphics
    VGList vg = drawScene();

    // rasterize it into image
    QImage img( 100, 100, QImage::Format_ARGB32 );
    QPainter painter( & img);
    RendererQPainter renderer( & painter);
    renderer.rasterize( vg);
//    Rasterizer().rasterize( vg, img );

    vg.add < Circle > ( QPointF( 1, 2 ), 10 );

    return 0;
}

static int activator = apitest();

// object that will report differences using vector graphics
}
