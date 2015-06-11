#include "NodeFactory.h"
#include "LayoutNodeComposite.h"
#include "LayoutNodeLeaf.h"
#include "State/ObjectManager.h"
#include "Data/Util.h"


#include <QDebug>

namespace Carta {

namespace Data {

const QString NodeFactory::EMPTY = "Empty";
const QString NodeFactory::HIDDEN = "Hidden";
const QString NodeFactory::POSITION_TOP = "top";
const QString NodeFactory::POSITION_BOTTOM = "bottom";
const QString NodeFactory::POSITION_LEFT = "left";
const QString NodeFactory::POSITION_RIGHT = "right";

NodeFactory::NodeFactory(){
}


LayoutNode* NodeFactory::makeComposite( bool horizontal ){
    Carta::State::CartaObject* nodeObj = Util::createObject( LayoutNodeComposite::CLASS_NAME );
    LayoutNodeComposite* node = dynamic_cast<LayoutNodeComposite*>( nodeObj);
    node->setHorizontal( horizontal );
    return node;
}

LayoutNode* NodeFactory::makeLeaf( const QString& plugin ){
    Carta::State::CartaObject* nodeObj = Util::createObject( LayoutNodeLeaf::CLASS_NAME );
    LayoutNodeLeaf* node = dynamic_cast<LayoutNodeLeaf*>( nodeObj);
    node->setPluginType( plugin );
    return node;
}


NodeFactory::~NodeFactory(){

}
}
}
