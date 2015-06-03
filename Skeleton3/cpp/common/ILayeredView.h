/**
 *
 **/

#include "CartaLib/CartaLib.h"
#include "IView.h"

#include <QImage>
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
    addLayer( int /*z*/, IRasterLayer::SharedPtr /*layer*/ ) { }

protected:

    virtual void
    registration( IConnector * /*connector*/ )
    { }

    virtual const QString &
    name() const
    {
        static QString removeMe;
        return removeMe;
    }

    virtual QSize
    size()
    {
        return QSize( 1, 1 );
    }

    virtual const QImage &
    getBuffer()
    {
        static QImage removeMe;
        return removeMe;
    }

    virtual void
    handleResizeRequest( const QSize & /*size*/ )
    { }

    virtual void
    handleMouseEvent( const QMouseEvent & /*event*/ )
    { }

    virtual void
    handleKeyEvent( const QKeyEvent & /*event*/ )
    { }
};
}
}
