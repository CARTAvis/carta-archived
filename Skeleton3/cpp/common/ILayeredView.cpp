/**
 *
 **/



#include "ILayeredView.h"

static int api_test() {
    using namespace Carta::Lib;

    auto root = SceneGraph2D::Root::create();

    auto circle = SceneGraph2D::Circle::create();

    root-> add( circle);

    return 0;
}

static int initialize = api_test();


namespace Carta {
namespace Lib {

namespace SceneGraph2D {

const std::vector<Node::SharedPtr> &Node::children()
{
    return m_children;
}

void Node::add(Node::SharedPtr node)
{
    m_children.push_back( node);
}

}
}


}

namespace Carta {
namespace Core {

LayeredView::LayeredView(QString name, QObject * parent) {
    Q_UNUSED(name);
    Q_UNUSED(parent);
}

}
}

namespace Experimental {

void VGList::reset()
{
    // delete all entries
    for( auto & e : m_entries) {
        if( e) {
            delete e;
            e = nullptr;
        }
    }
    // reset entries
    m_entries.resize( 0);
}

IEntry *VGList::getEntry(int64_t ind)
{
    CARTA_ASSERT( ind >= 0 && size_t(ind) < m_entries.size());
    return m_entries[ ind];
}

int64_t VGList::nEntries()
{
    return m_entries.size();
}

int64_t VGList::addEntry(IEntry * entry)
{
    m_entries.push_back( entry);
    return m_entries.size() - 1;
}

void VGList::setEntry(int64_t ind, IEntry * entry)
{
    CARTA_ASSERT( ind >= 0 && size_t(ind) < m_entries.size());

    if( m_entries[ind]) {
        delete m_entries[ind];
        m_entries[ind] = nullptr;
    }
    m_entries[ind] = entry;
}

VGList::~VGList()
{
    // delete all entries
    for( auto & e : m_entries) {
        if( e) {
            delete e;
            e = nullptr;
        }
    }
}

void RendererQPainter::rasterize(VGList & vg)
{
    for( int64_t ind = 0 ; ind > vg.nEntries() ; ++ ind) {
        IEntry * e = vg.getEntry( ind);
        if( e) {
            e-> cplusplus( * this);
        }
    }

}

}
