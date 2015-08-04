#include "LayoutNode.h"

#include <QDebug>

namespace Carta {

namespace Data {

LayoutNode::LayoutNode( const QString& className, const QString& path, const QString& id):
    CartaObject( className, path, id ){
    _initializeDefaultState();
    _initializeCommands();
}


bool LayoutNode::addWindow( const QString& /*nodeId*/, const QString& /*position*/ ){
    return false;
}


bool LayoutNode::containsNode( const QString& nodeId ) const {
    bool nodeMatch = false;
    if ( this->getPath() == nodeId ){
        nodeMatch = true;
    }
    return nodeMatch;
}


void LayoutNode::clear(){
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    QString id = getId();
    if ( id.trimmed().length() > 0 ){
        objMan->removeObject( id );
    }
}



LayoutNode* LayoutNode::findAncestor( const QStringList& /*nodeIds*/, QString& /*childId*/ ){
    return nullptr;
}


LayoutNode* LayoutNode::getChildFirst() const {
    return nullptr;
}


LayoutNode* LayoutNode::getChildSecond() const {
    return nullptr;
}

QString LayoutNode::getStateString() const {
    return m_state.toString();
}

void LayoutNode::_initializeCommands(){
}


void LayoutNode::_initializeDefaultState(){
}


bool LayoutNode::isComposite() const {
    return false;
}

void LayoutNode::releaseChild( const QString& /*key*/ ){

}

bool LayoutNode::removeWindow( const QString& /*nodeId*/ ){
    return false;
}


void LayoutNode::setChildFirst( LayoutNode* /*node*/ ){

}


void LayoutNode::setChildSecond( LayoutNode* /*node*/ ){

}

void LayoutNode::setHorizontal( bool /*horizontal*/ ){

}

QString LayoutNode::toString() const {
    return m_state.toString();
}

LayoutNode::~LayoutNode(){
    clear();
}
}
}
