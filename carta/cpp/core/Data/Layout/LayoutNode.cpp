#include "LayoutNode.h"
#include "State/UtilState.h"
#include "Data/Util.h"
#include <QDebug>
#include <set>

#include <QDebug>

namespace Carta {

namespace Data {

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
    return m_state.getValue<int>( Util::HEIGHT );
}

QString LayoutNode::getStateString() const {
    return m_state.toString();
}

int LayoutNode::getWidth() const {
    return m_state.getValue<int>( Util::WIDTH );
}


void LayoutNode::_initializeCommands(){
    addCommandCallback( "setSize", [=] (const QString & /*cmd*/,
                      const QString & params, const QString & /*sessionId*/) -> QString {
              std::set<QString> keys = {Util::WIDTH, Util::HEIGHT};
              std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
              QString widthStr = dataValues[Util::WIDTH];
              QString heightStr = dataValues[Util::HEIGHT];
              bool valid = false;
              int width = widthStr.toInt( &valid );
              QString result;
              if ( valid ){
                  int height = heightStr.toInt( &valid );
                  if ( valid ){
                	  result = setSize( width, height );
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
    m_state.insertValue<int>( Util::WIDTH, 1 );
    m_state.insertValue<int>( Util::HEIGHT, 1 );
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
    m_state.setValue<int>( Util::HEIGHT, nodeState.getValue<int>( Util::HEIGHT ) );
    m_state.setValue<int>( Util::WIDTH, nodeState.getValue<int>( Util::WIDTH ) );
}


void LayoutNode::setChildFirst( LayoutNode* /*node*/ ){

}


void LayoutNode::setChildSecond( LayoutNode* /*node*/ ){

}

void LayoutNode::setHorizontal( bool /*horizontal*/ ){

}

QString LayoutNode::setSize( int width, int height ){
	QString result;
	if ( width >= 0 && height >= 0 ){
		m_state.setValue<int>(Util::WIDTH, width);
		m_state.setValue<int>(Util::HEIGHT, height );
	}
	else {
		result="Width/height of layout cell must be nonnegative: ("+QString::number(width)+", "+QString::number(height)+")";
	}
	return result;
}

QString LayoutNode::toString() const {
    return m_state.toString();

}

LayoutNode::~LayoutNode(){
}
}
}
