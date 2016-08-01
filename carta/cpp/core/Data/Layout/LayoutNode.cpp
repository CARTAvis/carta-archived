#include "LayoutNode.h"
#include "State/UtilState.h"
#include <QDebug>
#include <set>

#include <QDebug>

namespace Carta {

namespace Data {

const QString LayoutNode::WIDTH = "width";
const QString LayoutNode::HEIGHT = "height";

LayoutNode::LayoutNode( const QString& className, const QString& path, const QString& id):
    CartaObject( className, path, id ){
    _initializeDefaultState();
    _initializeCommands();
}


bool LayoutNode::addWindow( const QString& /*nodeId*/, const QString& /*position*/, int /*index*/ ){
    return false;
}


bool LayoutNode::containsNode( const QString& nodeId ) const {
    bool nodeMatch = false;
    if ( this->getPath() == nodeId ){
        nodeMatch = true;
    }
    return nodeMatch;
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

int LayoutNode::getHeight() const {
    return m_state.getValue<int>( HEIGHT );
}

QString LayoutNode::getStateString() const {
    return m_state.toString();
}

int LayoutNode::getWidth() const {
    return m_state.getValue<int>( WIDTH );
}

void LayoutNode::_initializeCommands(){
    addCommandCallback( "setSize", [=] (const QString & /*cmd*/,
                      const QString & params, const QString & /*sessionId*/) -> QString {
              std::set<QString> keys = {WIDTH, HEIGHT};
              std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
              QString widthStr = dataValues[WIDTH];
              QString heightStr = dataValues[HEIGHT];
              bool valid = false;
              int width = widthStr.toInt( &valid );
              QString result;
              if ( valid ){
                  int height = heightStr.toInt( &valid );
                  if ( valid ){
                      if ( width >= 0 && height >= 0 ){
                          m_state.setValue<int>(WIDTH, width);
                          m_state.setValue<int>(HEIGHT, height );
                      }
                      else {
                          result="Width/height of layout cell must be nonnegative: "+params;
                      }
                  }
                  else {
                      result = "Invalid layout height: "+heightStr;
                  }
              }
              else {
                  result =  "Invalid layout width: " +widthStr;
              }
              return result;
          });
}


void LayoutNode::_initializeDefaultState(){
    m_state.insertValue<int>( WIDTH, 1 );
    m_state.insertValue<int>( HEIGHT, 1 );
}


bool LayoutNode::isComposite() const {
    return false;
}

void LayoutNode::releaseChild( const QString& /*key*/ ){

}

bool LayoutNode::removeWindow( const QString& /*nodeId*/ ){
    return false;
}


void LayoutNode::resetState( const QString& stateStr, QMap<QString,int>& /*usedPlugins*/ ){
    Carta::State::StateInterface nodeState( "" );
    nodeState.setState( stateStr );
    m_state.setValue<int>( HEIGHT, nodeState.getValue<int>( HEIGHT ) );
    m_state.setValue<int>( WIDTH, nodeState.getValue<int>( WIDTH ) );
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
}
}
}
