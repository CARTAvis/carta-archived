/**
 * Experimental code below. None of it is actually being used anywhere (yet).
 **/

#ifdef DONT_COMPILE

#include "ILayeredView.h"

static int
api_test()
{
    using namespace Carta::Lib;

    auto root = SceneGraph2D::Root::create();

    auto circle = SceneGraph2D::Circle::create();

    root-> add( circle );

    return 0;
}

static int initialize = api_test();

namespace Carta
{
namespace Lib
{
namespace SceneGraph2D
{
const std::vector < Node::SharedPtr > &
Node::children()
{
    return m_children;
}

void
Node::add( Node::SharedPtr node )
{
    m_children.push_back( node );
}
}
}
}

namespace Carta
{
namespace Core
{
LayeredView::LayeredView( QString name, QObject * parent )
{
    Q_UNUSED( name );
    Q_UNUSED( parent );
}
}
}

namespace Experimental
{
void
VGList::reset()
{
    // delete all entries
    for ( auto & e : m_entries ) {
        if ( e ) {
            delete e;
            e = nullptr;
        }
    }

    // reset entries
    m_entries.resize( 0 );
}

void
VGList::resetChanges()
{
    m_changes.clear();
}

const std::unordered_set < u_int64_t > &
VGList::getChanges()
{
    return m_changes;
}

IVGListEntry *
VGList::getEntry( int64_t ind )
{
    CARTA_ASSERT( ind >= 0 && size_t( ind ) < m_entries.size() );
    return m_entries[ind];
}

int64_t
VGList::nEntries()
{
    return m_entries.size();
}

int64_t
VGList::addEntry( IVGListEntry * entry )
{
    auto currentIndex = m_entries.size();
    m_entries.push_back( entry );
    m_changes.insert( currentIndex );
    return currentIndex;
}

void
VGList::setEntry( int64_t ind, IVGListEntry * entry )
{
    CARTA_ASSERT( ind >= 0 && size_t( ind ) < m_entries.size() );

    if ( m_entries[ind] ) {
        delete m_entries[ind];
        m_entries[ind] = nullptr;
    }
    m_entries[ind] = entry;
    m_changes.insert( ind );
}

VGList::~VGList()
{
    // delete all entries
    for ( auto & e : m_entries ) {
        if ( e ) {
            delete e;
            e = nullptr;
        }
    }
}

void
RendererQPainter::rasterize( VGList & vg )
{
    for ( int64_t ind = 0 ; ind > vg.nEntries() ; ++ind ) {
        IVGListEntry * e = vg.getEntry( ind );
        if ( e ) {
            e-> cplusplus( * this );
        }
    }
}
}

using namespace Experimental;

// something drawing a full scene
static
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
    QImage img( 100, 100, QImage::Format_ARGB32_Premultiplied );
    QPainter painter( & img );
    RendererQPainter renderer( & painter );
    renderer.rasterize( vg );

//    Rasterizer().rasterize( vg, img );

    vg.add < Circle > ( QPointF( 1, 2 ), 10 );
    vg.addEntry( new Circle( { 10, 20}, 5));
    vg.addEntry( new Line( {0,0}, {1,1}));

    return 0;
}

static int activator = apitest();


#endif
