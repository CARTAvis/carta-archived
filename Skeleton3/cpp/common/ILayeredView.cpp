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
