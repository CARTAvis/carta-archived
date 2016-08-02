#include "LayoutNodeLeaf.h"
#include "NodeFactory.h"

#include <QDebug>

namespace Carta {

namespace Data {

const QString LayoutNodeLeaf::CLASS_NAME = "LayoutNodeLeaf";
const QString LayoutNodeLeaf::PLUGIN = "plugin";


class LayoutNodeLeaf::Factory : public Carta::State::CartaObjectFactory {

public:

    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new LayoutNodeLeaf (path, id);
    }
};



bool LayoutNodeLeaf::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME,
                                                   new LayoutNodeLeaf::Factory());

LayoutNodeLeaf::LayoutNodeLeaf( const QString& path, const QString& id):
    LayoutNode( CLASS_NAME, path, id ){
    _initializeDefaultState();
    _initializeCommands();
}

bool LayoutNodeLeaf::getIndex( const QString& plugin, const QString& locationId, int* index ) const {
    bool targetFound = false;
    QString leafPlugin= m_state.getValue<QString>(PLUGIN);
    if ( leafPlugin == plugin ){
        if ( locationId == getPath()){
            *index = m_state.getValue<int>( Carta::State::StateInterface::INDEX );
            targetFound = true;
        }
    }
    return targetFound;
}

QString LayoutNodeLeaf::getPlugin( const QString& locationId ) const {
    QString plugin;
    if ( locationId == this->getPath()){
        plugin = m_state.getValue<QString>( PLUGIN );
    }
    return plugin;
}


QStringList LayoutNodeLeaf::getPluginList() const {
    QStringList plugins;
    plugins.append( m_state.getValue<QString>( PLUGIN));
    return plugins;
}


void LayoutNodeLeaf::_initializeDefaultState(){
    m_state.insertValue<QString>( PLUGIN, "" );
    m_state.flushState();
}

void LayoutNodeLeaf::_initializeCommands(){


}

void LayoutNodeLeaf::resetState( const QString& stateStr, QMap<QString,int>& usedPlugins ){
    LayoutNode::resetState( stateStr, usedPlugins );
    Carta::State::StateInterface leafState( "" );
    leafState.setState( stateStr );
    QString newPlugin = leafState.getValue<QString>(PLUGIN);
    QStringList newPluginList(newPlugin);
    setPlugins( newPluginList, usedPlugins, false );
    m_state.flushState();
}

bool LayoutNodeLeaf::setPlugin( const QString& nodeId, const QString& nodeType, int index ){
    bool nodeChanged = false;
    if ( nodeId == getPath() ){
        nodeChanged = true;
        bool stateChanged = false;
        QString oldNodeType = m_state.getValue<QString>(PLUGIN);
        if ( oldNodeType != nodeType ){
            m_state.setValue<QString>(PLUGIN, nodeType );
            stateChanged = true;
        }
        int oldIndex = m_state.getValue<int>( Carta::State::StateInterface::INDEX );
        if ( oldIndex != index ){
            m_state.setValue<int>(Carta::State::StateInterface::INDEX, index );
            stateChanged = true;
        }
        if ( stateChanged ){
            m_state.flushState();
        }
    }
    return nodeChanged;
}

bool LayoutNodeLeaf::setPlugins( QStringList& names, QMap<QString,int>& usedPlugins, bool useFirst ){
    QString oldPlugin = m_state.getValue<QString>(PLUGIN);
    bool pluginSet = true;
    //if ( oldPlugin != NodeFactory::HIDDEN ){
        //First see if there is a plugin in the list that matches the old one.
        int pluginIndex = 0;
        if ( !useFirst ){
            pluginIndex = names.indexOf( oldPlugin );
            if ( pluginIndex < 0 ){
                pluginIndex = 0;
            }
        }
        if ( names.size() > 0 ){
            bool stateChanged = false;
            QString newPlugin = names[pluginIndex ];
            if ( oldPlugin != newPlugin ){
                m_state.setValue<QString>(PLUGIN, newPlugin );
                stateChanged = true;
            }
            int usedCount = 0;
            if ( usedPlugins.contains( newPlugin ) ){
                usedCount = usedPlugins[newPlugin];
                usedCount++;
            }
            int plugIndex = m_state.getValue<int>( Carta::State::StateInterface::INDEX );
            if ( usedCount != plugIndex){
                m_state.setValue<int>(Carta::State::StateInterface::INDEX, usedCount );
                stateChanged = true;
            }
            if ( stateChanged ){
                m_state.flushState();
            }
            usedPlugins[newPlugin] = usedCount;
            names.removeOne( newPlugin );
        }
        else {
            pluginSet = false;
        }
    //}
    return pluginSet;
}

void LayoutNodeLeaf::setPluginType( const QString& name ){
    if ( m_state.getValue<QString>( PLUGIN) != name ){
        m_state.setValue<QString>(PLUGIN, name );
        m_state.flushState();
    }
}

QString LayoutNodeLeaf::toString() const {
    return "id:"+getPath()+" plugin="+m_state.getValue<QString>(PLUGIN)+
            " index=" + QString::number(m_state.getValue<int>(Carta::State::StateInterface::INDEX));
}

LayoutNodeLeaf::~LayoutNodeLeaf(){

}
}
}
