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
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    LayoutNodeComposite* node = objMan->createObject<LayoutNodeComposite>();
    node->setHorizontal( horizontal );
    return node;
}

LayoutNode* NodeFactory::makeLeaf( const QString& plugin ){
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    LayoutNodeLeaf* node = objMan->createObject<LayoutNodeLeaf>();
    node->setPluginType( plugin );
    return node;
}


NodeFactory::~NodeFactory(){

}
}
}
