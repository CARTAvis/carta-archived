#include "LayoutNodeComposite.h"
#include "LayoutNodeLeaf.h"
#include "State/UtilState.h"
#include "Data/Util.h"
#include "NodeFactory.h"

#include <QDebug>

namespace Carta {

namespace Data {


const QString LayoutNodeComposite::CLASS_NAME = "LayoutNodeComposite";
const QString LayoutNodeComposite::HORIZONTAL = "horizontal";
const QString LayoutNodeComposite::PLUGIN_LEFT = "layoutLeft";
const QString LayoutNodeComposite::PLUGIN_RIGHT = "layoutRight";
const QString LayoutNodeComposite::COMPOSITE = "composite";

class LayoutNodeComposite::Factory : public Carta::State::CartaObjectFactory {
    public:
        Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new LayoutNodeComposite (path, id);
        }
};


bool LayoutNodeComposite::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME,
                                                   new LayoutNodeComposite::Factory());


using Carta::State::UtilState;
using Carta::State::ObjectManager;

LayoutNodeComposite::LayoutNodeComposite( const QString& path, const QString& id):
    LayoutNode( CLASS_NAME, path, id ),
    m_firstChild( nullptr ),
    m_secondChild( nullptr ){
    _initializeDefaultState();
}




bool LayoutNodeComposite::_addWindow( const QString& nodeId, const QString& position,
        const QString& childKey, std::unique_ptr<LayoutNode>& child, int index ){
    bool windowAdded = false;
    if ( child.get() != nullptr ){
        QString childId = child->getPath();
        if ( childId == nodeId ){
            //Replace the child with a composite of the appropriate type.
            LayoutNode* oldComp = child.get();
            LayoutNode* newComp = nullptr;
            if ( position == NodeFactory::POSITION_TOP || position == NodeFactory::POSITION_BOTTOM  ){
                newComp = NodeFactory::makeComposite( false );
            }
            else {
                newComp = NodeFactory::makeComposite( true );
            }

            if ( newComp != nullptr ){
                windowAdded = true;
                LayoutNode* emptyChild = NodeFactory::makeLeaf();
                emptyChild->setIndex( index );
                int oldWidth = oldComp->getWidth();
                int oldHeight = oldComp->getHeight();
                if ( position == NodeFactory::POSITION_TOP || position==NodeFactory::POSITION_BOTTOM ){
                	emptyChild->setSize( oldWidth, oldHeight / 2 );
                	oldComp->setSize( oldWidth, oldHeight / 2 );
                }
                else {
                	emptyChild->setSize( oldWidth / 2, oldHeight );
                	oldComp->setSize( oldWidth / 2, oldHeight );
                }
                //Make the old child a child of the composite as well as the empty leaf.
                if ( position == NodeFactory::POSITION_TOP || position == NodeFactory::POSITION_LEFT ){
                    newComp->setChildSecond( oldComp );
                    newComp->setChildFirst( emptyChild );
                }
                else {
                    newComp->setChildSecond( emptyChild );
                    newComp->setChildFirst( oldComp );
                }

                //Make the child into a composite.
                _setChild( childKey, child, newComp, false );
            }
        }
    }
    return windowAdded;
}


bool LayoutNodeComposite::addWindow( const QString& nodeId, const QString& position, int index ){
    bool windowAdded = _addWindow( nodeId, position, PLUGIN_LEFT, m_firstChild, index );
    if ( !windowAdded ){
        windowAdded = _addWindow( nodeId, position, PLUGIN_RIGHT, m_secondChild, index );
    }
    return windowAdded;
}

QStringList LayoutNodeComposite::_checkChild( LayoutNode* child, const QStringList & nodeIds ) const {
    QStringList foundChildren;
    if ( child != nullptr ){
        bool ancestorNode = true;
        for ( int i = 0; i < nodeIds.size(); i++ ){
            ancestorNode = child->containsNode( nodeIds[i] );
            if ( ancestorNode ){
                foundChildren.append( nodeIds[i] );
            }
        }
    }
    return foundChildren;
}

bool LayoutNodeComposite::containsNode( const QString& nodeId ) const {
    bool commonAncestor = false;
    if ( m_firstChild.get() != nullptr ){
        commonAncestor = m_firstChild->containsNode( nodeId );
    }
    if ( !commonAncestor ){
        if ( m_secondChild.get() != nullptr ){
           commonAncestor = m_secondChild->containsNode( nodeId );
        }
    }
    return commonAncestor;
}

LayoutNode* LayoutNodeComposite::_findAncestorChild( const QStringList& nodeIds, QString& childId,
        LayoutNode* child ){
    LayoutNode* ancestor = nullptr;
    if ( child != nullptr ){
        if ( child->getType() == CLASS_NAME ){
            ancestor = child -> findAncestor( nodeIds, childId );
        }
        if ( ancestor == nullptr ){
            QStringList childNodeIds = _checkChild( child, nodeIds );
            bool allnodesFound = Util::isListMatch( childNodeIds, nodeIds );
            if ( allnodesFound ){
                ancestor = this;
                childId = child->getPath();
            }
        }
    }
    return ancestor;
}

LayoutNode* LayoutNodeComposite::findAncestor( const QStringList& nodeIds, QString& childId){
    //Try to delegate the ancestor job to the children if we can.
    LayoutNode* ancestor = nullptr;
    if ( nodeIds.size() == 0 ){
        return ancestor;
    }
    ancestor = _findAncestorChild( nodeIds, childId, m_firstChild.get());
    if ( ancestor == nullptr ){
        ancestor = _findAncestorChild( nodeIds, childId, m_secondChild.get());
    }
    //Okay, the children aren't ancestors, see if we can to the job.
    /*if ( ancestor == nullptr && nodeIds.size() > 1 ){
        QStringList childFirstNodeIds = _checkChild( m_firstChild.get(), nodeIds );
        QStringList childSecondNodeIds = _checkChild( m_secondChild.get(), nodeIds );
        QStringList childNodeIds;
        childNodeIds.append( childFirstNodeIds );
        childNodeIds.append( childSecondNodeIds );
        bool allNodesFound = Util::isListMatch( childNodeIds, nodeIds );
        if ( allNodesFound ){
            ancestor = this;
            childId = "";
        }
    }*/
    return ancestor;
}


LayoutNode* LayoutNodeComposite::getChildFirst() const {
    return m_firstChild.get();
}

LayoutNode* LayoutNodeComposite::getChildSecond() const {
    return m_secondChild.get();
}


bool LayoutNodeComposite::getIndex( const QString& plugin, const QString& locationId, int* index ) const {
    bool targetFound = false;
    if ( m_firstChild.get() != nullptr ){
        targetFound = m_firstChild->getIndex( plugin, locationId, index );
    }
    if ( !targetFound ){
        if ( m_secondChild.get() != nullptr ){
            targetFound = m_secondChild->getIndex( plugin, locationId, index );
        }
    }
    return targetFound;
}

QString LayoutNodeComposite::getPlugin( const QString& locationId ) const {
    QString plugin;
    if ( m_firstChild.get() != nullptr ){
        plugin = m_firstChild->getPlugin( locationId );
    }
    if ( plugin.isEmpty() && m_secondChild.get() != nullptr ){
        plugin = m_secondChild->getPlugin( locationId );
    }
    return plugin;
}


QStringList LayoutNodeComposite::getPluginList() const {
    QStringList plugins;
    if ( m_firstChild.get() != nullptr ){
        plugins.append( m_firstChild->getPluginList() );
    }
    if ( m_secondChild.get() != nullptr ){
        plugins.append( m_secondChild->getPluginList() );
    }
    return plugins;
}

QString LayoutNodeComposite::getStateString() const {
    Carta::State::StateInterface compState( "");
    compState.setState( m_state.toString());
    if ( m_firstChild.get() != nullptr ){
        compState.setObject( PLUGIN_LEFT, m_firstChild->getStateString());
    }
    if ( m_secondChild.get() != nullptr ){
        compState.setObject( PLUGIN_RIGHT, m_secondChild->getStateString());
    }
    return compState.toString();
}

bool LayoutNodeComposite::isComposite() const {
    return true;
}


void LayoutNodeComposite::_initializeDefaultState(){
    m_state.insertValue<bool>( HORIZONTAL, true );
    m_state.insertObject( PLUGIN_LEFT);
    m_state.insertObject( PLUGIN_RIGHT);

    QString idLookupLeft = UtilState::getLookup( PLUGIN_LEFT, Util::ID );
    QString typeLookupLeft = UtilState::getLookup( PLUGIN_LEFT, COMPOSITE );
    QString widthLookupLeft = UtilState::getLookup( PLUGIN_LEFT, Util::WIDTH );
    QString heightLookupLeft = UtilState::getLookup( PLUGIN_LEFT, Util::HEIGHT );
    m_state.insertValue<QString>( idLookupLeft, "");
    m_state.insertValue<bool>( typeLookupLeft, false);
    m_state.insertValue<int>(widthLookupLeft, 1 );
    m_state.insertValue<int>(heightLookupLeft, 1 );
    QString idLookupRight = UtilState::getLookup( PLUGIN_RIGHT, Util::ID );
    QString typeLookupRight = UtilState::getLookup( PLUGIN_RIGHT, COMPOSITE );
    QString widthLookupRight = UtilState::getLookup( PLUGIN_RIGHT, Util::WIDTH );
    QString heightLookupRight = UtilState::getLookup( PLUGIN_RIGHT, Util::HEIGHT );
    m_state.insertValue<QString>( idLookupRight, "");
    m_state.insertValue<bool>( typeLookupRight, false);
    m_state.insertValue<int>( widthLookupRight, 1 );
    m_state.insertValue<int>( heightLookupRight, 1 );
    m_state.flushState();
}

bool LayoutNodeComposite::removeWindow( const QString& nodeId ){
    //See if one of the children has a child with matching id,
    //in which case we need to replace the
    //child with the grand child that does not have a matching id.
    bool windowRemoved = _removeWindow( nodeId, PLUGIN_LEFT, m_firstChild );

    if ( !windowRemoved ){
        windowRemoved = _removeWindow( nodeId, PLUGIN_RIGHT, m_secondChild );
    }

    //We failed at the removal so see if the kids can do it.
    if ( !windowRemoved ){
        if ( m_firstChild.get() != nullptr ){
            windowRemoved = m_firstChild->removeWindow( nodeId );

        }
        if ( ! windowRemoved ){
            windowRemoved = m_secondChild->removeWindow( nodeId );
        }
    }
    return windowRemoved;
}

bool LayoutNodeComposite::_removeWindow( const QString& nodeId,
        const QString& childKey, std::unique_ptr<LayoutNode>& child ){
    //See if the child has a child (our grand child) with a matching id.
    //If we find one with matching id we replace the parent (our child)
    //with the other grand kid.
    bool windowRemoved = false;
    LayoutNode* replacementNode = nullptr;
    if ( child.get() != nullptr ){
        LayoutNode* firstGrandKid = child->getChildFirst();
        if ( firstGrandKid != nullptr ){
            if ( firstGrandKid->getPath() == nodeId ){
                replacementNode = child->getChildSecond();
            }
        }
        if ( replacementNode == nullptr ){
            LayoutNode* secondGrandKid = child->getChildSecond();
            if ( secondGrandKid != nullptr ){
                if ( secondGrandKid->getPath() == nodeId ){
                    replacementNode = child->getChildFirst();
                }
            }
        }
        if ( replacementNode != nullptr ){
            windowRemoved = true;
            _setChild( childKey, child, replacementNode, false );
        }
    }
    return windowRemoved;
}
void LayoutNodeComposite::resetState( const QString& state, QMap<QString,int>& usedPlugins ){
    LayoutNode::resetState( state, usedPlugins );
    Carta::State::StateInterface newState( "" );
    newState.setState( state );
    bool newHorizontal = newState.getValue<bool>( HORIZONTAL );
    setHorizontal( newHorizontal );
    _resetStateChild( PLUGIN_LEFT, m_firstChild, newState, usedPlugins );
    _resetStateChild( PLUGIN_RIGHT, m_secondChild, newState, usedPlugins );

    int width = m_firstChild->getWidth();
    if ( newHorizontal ){
        width = width + m_secondChild->getWidth();
    }
    int height = m_firstChild->getHeight();
    if ( !newHorizontal ){
        height = height + m_secondChild->getHeight();
    }
    m_state.setValue<int>( Util::WIDTH, width );
    m_state.setValue<int>( Util::HEIGHT, height );
    m_state.flushState();
}

void LayoutNodeComposite::_resetStateChild( const QString& childKey, std::unique_ptr<LayoutNode>& child,
        const Carta::State::StateInterface& newState, QMap<QString,int>& usedPlugins ){
    QString childState = newState.toString( childKey );
    QString typeLookup = UtilState::getLookup( childKey, Carta::State::StateInterface::OBJECT_TYPE);
    QString childComp = newState.getValue<QString>( typeLookup );
    bool childComposite = false;
    if ( childComp == CLASS_NAME ){
        childComposite = true;
    }
    if ( child.get() == nullptr || childComposite != child->isComposite() ){
        LayoutNode* childNode = nullptr;
        if ( childComposite ){
            childNode = NodeFactory::makeComposite( true );
        }
        else {
            childNode = NodeFactory::makeLeaf();
        }
        child.reset( childNode );
    }

    //Reset the state of the child.
    child->resetState( childState, usedPlugins );
    _updateChildState(  childKey, child.get() );

}


void LayoutNodeComposite::_setChild( const QString& key,
        std::unique_ptr<LayoutNode>& child, LayoutNode* node, bool destroy ){
    if ( node != nullptr ){
        QString lookup = Carta::State::UtilState::getLookup( key, Util::ID);
        QString oldLookup = m_state.getValue<QString>( lookup );
        if ( node->getPath() !=  oldLookup ){
            if ( child ){
                if ( !destroy ){
                    child.release();
                }
            }
            child.reset( node );
            _updateChildState( key, node );
            m_state.flushState();
        }
    }
}
void LayoutNodeComposite::releaseChild( const QString& key ){
    if ( key == PLUGIN_LEFT ){
        m_firstChild.release();
        m_firstChild.reset( nullptr);
    }
    else if ( key == PLUGIN_RIGHT ){
        m_secondChild.release();
        m_secondChild.reset( nullptr );
    }
}



void LayoutNodeComposite::setChildFirst( LayoutNode* node ){
   _setChild( PLUGIN_LEFT, m_firstChild, node, true );
}


void LayoutNodeComposite::setChildSecond( LayoutNode* node ){
    _setChild( PLUGIN_RIGHT, m_secondChild, node, true  );
}

void LayoutNodeComposite::setHorizontal( bool horizontal ){
    if ( m_state.getValue<bool>(HORIZONTAL) != horizontal ){
        m_state.setValue<bool>(HORIZONTAL, horizontal );
        m_state.flushState();
    }
}



bool LayoutNodeComposite::setPlugin( const QString& nodeId, const QString& nodeType, int index ){
    bool pluginSet = false;
    if ( m_firstChild.get() != nullptr ){
        pluginSet = m_firstChild->setPlugin( nodeId, nodeType, index );
        if ( !pluginSet ){
            if ( m_secondChild != nullptr ){
                pluginSet = m_secondChild->setPlugin( nodeId, nodeType, index );
            }
        }
    }
    return pluginSet;
}

bool LayoutNodeComposite::setPlugins( QStringList& names, QMap<QString,int>& usedPlugins, bool useFirst ){
    bool pluginSetOne = true;
    if ( m_firstChild.get() != nullptr ){
        pluginSetOne = m_firstChild->setPlugins( names, usedPlugins, useFirst );
    }
    bool pluginSetTwo = true;
    if ( m_secondChild.get() != nullptr ){
        pluginSetTwo = m_secondChild->setPlugins( names, usedPlugins, useFirst );
    }
    return pluginSetOne && pluginSetTwo;
}

QString LayoutNodeComposite::setSize( int width, int height ){
	QString result;
	if ( width >= 0 && height >= 0 ){
		m_state.setValue<int>(Util::WIDTH, width);
		m_state.setValue<int>(Util::HEIGHT, height );
		//If we have children, we have to divide up the space among them.
		int childHeight = height / 2;
		int childWidth = width;
		if ( m_state.getValue<bool>( HORIZONTAL ) ){
			childHeight = height;
			childWidth = width / 2;
		}
		if ( m_firstChild ){
			m_firstChild->setSize( childWidth, childHeight );
		}
		if ( m_secondChild ){
			m_secondChild->setSize( childWidth, childHeight );
		}
	}
	else {
		result="Width/height of layout cell must be nonnegative: ("+
				QString::number(width)+","+QString::number(height)+")";
	}
	return result;
}


QString LayoutNodeComposite::toString() const {
    QString result = "Composite: "+getPath();
    if ( m_firstChild.get() != nullptr ){
        result.append( "\nLeft: "+m_firstChild->toString() );
    }
    if ( m_secondChild.get() != nullptr ){
        result.append( "\nRight: "+m_secondChild->toString() );
    }
    return result;
}

void LayoutNodeComposite::_updateChildState( const QString& childKey,
        LayoutNode* child ){
    QString id =  child->getPath();
    bool composite =  child->isComposite();
    QString idLookup = Carta::State::UtilState::getLookup( childKey, Util::ID);
    QString oldId = m_state.getValue<QString>( idLookup );
    if ( oldId != id ){
        m_state.setValue<QString>( idLookup, id );
    }
    QString compLookup = Carta::State::UtilState::getLookup( childKey, COMPOSITE );
    bool oldComposite = m_state.getValue<bool>( compLookup );
    if ( oldComposite != composite ){
        m_state.setValue<bool>( compLookup, composite);
    }

    int width = child->getWidth();
    int height = child->getHeight();
    QString widthLookup = Carta::State::UtilState::getLookup( childKey, Util::WIDTH );
    QString heightLookup = Carta::State::UtilState::getLookup( childKey, Util::HEIGHT );
    m_state.setValue<int>( widthLookup, width );
    m_state.setValue<int>( heightLookup, height );
}

LayoutNodeComposite::~LayoutNodeComposite(){
}
}
}
