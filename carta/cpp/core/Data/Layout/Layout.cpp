#include "Layout.h"
#include "LayoutNodeComposite.h"
#include "LayoutNodeLeaf.h"
#include "NodeFactory.h"
#include "Data/Animator/Animator.h"
#include "Data/Colormap/Colormap.h"
#include "State/UtilState.h"
#include "State/StateInterface.h"
#include "Data/Image/Controller.h"
#include "Data/Image/ImageContext.h"
#include "Data/Image/ImageZoom.h"
#include "Data/Histogram/Histogram.h"
#include "Data/Profile/Profiler.h"
#include "Data/Statistics/Statistics.h"
#include "Data/Util.h"
#include <QtCore/qmath.h>
#include <QDebug>

namespace Carta {

namespace Data {

class Layout::Factory : public Carta::State::CartaObjectFactory {

public:

    Factory():
        CartaObjectFactory( LAYOUT ){};

    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new Layout (path, id);
    }
};

const QString Layout::LAYOUT = "Layout";
const QString Layout::LAYOUT_NODE = "layoutNode";
const QString Layout::LAYOUT_ROWS = "rows";
const QString Layout::LAYOUT_COLS = "cols";
const QString Layout::LAYOUT_PLUGINS = "plugins";
const QString Layout::POSITION = "position";
const QString Layout::CLASS_NAME = "Layout";

const QString Layout::TYPE_DEFAULT = "Default";
const QString Layout::TYPE_ANALYSIS = "Analysis"; //Line Analysis
const QString Layout::TYPE_HISTOGRAMANALYSIS = "HistogramAnalysis";
const QString Layout::TYPE_IMAGECOMPOSITE = "ImageComposite";
const QString Layout::TYPE_IMAGE = "Image";
const QString Layout::TYPE_CUSTOM = "Custom";

const QString Layout::TYPE_SELECTED = "layoutType";

bool Layout::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME,
                                                   new Layout::Factory());

using Carta::State::UtilState;

Layout::Layout( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ),
    m_layoutRoot( nullptr ){
    _initializeDefaultState();
    _initializeCommands();
}


QString Layout::addWindow( const QStringList& windowIds, const QString& position ){
    QString msg;
    //Make sure the position makes sense.
    if ( position != NodeFactory::POSITION_BOTTOM &&
            position != NodeFactory::POSITION_TOP &&
            position != NodeFactory::POSITION_LEFT &&
            position != NodeFactory::POSITION_RIGHT ){
        msg = "Unrecognized position for adding a window: "+position;
    }
    else if ( windowIds.size() == 0 ){
        msg = "Please specify one or more cell locations where a window should be added";
    }
    else {
        //We find the first common parent of all the windows and add
        //the window there.
        QString childId;
        LayoutNode* progenitor = m_layoutRoot->findAncestor( windowIds, childId );
        bool windowAdded = false;
        int emptyCount = _getPluginCount( NodeFactory::EMPTY );
        if ( progenitor != nullptr ){
            windowAdded = progenitor->addWindow( childId, position, emptyCount );
            if ( ! windowAdded ){
                msg = "Unable to add window at "+ position;
            }
        }
        //Otherwise, we need to make a new root
        //node and then set the current one to be a child of the new one.
        else if ( windowIds.size() > 0 ) {
            LayoutNode* oldRoot = m_layoutRoot.get();
            m_layoutRoot.release();
            bool horizontal = true;
            if ( position == NodeFactory::POSITION_TOP || position == NodeFactory::POSITION_BOTTOM ){
                horizontal = false;
            }
            _makeRoot(horizontal);
            LayoutNode* emptyChild = NodeFactory::makeLeaf();
            emptyChild->setIndex( emptyCount );
            if ( position == NodeFactory::POSITION_LEFT || position == NodeFactory::POSITION_TOP ){
                m_layoutRoot->setChildSecond( oldRoot );
                m_layoutRoot->setChildFirst( emptyChild );
            }
            else {
                m_layoutRoot->setChildFirst( oldRoot );
                m_layoutRoot->setChildSecond( emptyChild );
            }
            windowAdded = true;
        }
        else {
            msg = "There was an error finding a location for adding the window(s): "+windowIds.join(",");
        }
        if ( windowAdded ){
            m_state.setValue<QString>( TYPE_SELECTED, TYPE_CUSTOM );
            m_state.flushState();
        }
    }
    return msg;

}



void Layout::clear(){
    QString oldRootId = m_state.getValue<QString>( LAYOUT_NODE );
    QString oldType = m_state.getValue<QString>( TYPE_SELECTED );
    if ( oldRootId.trimmed().length() > 0  || oldType != TYPE_CUSTOM){
        m_state.setValue<QString>(LAYOUT_NODE, "");
        m_state.setValue<QString>(TYPE_SELECTED, TYPE_CUSTOM );
        m_state.flushState();
    }
}


QString Layout::getStateString() const {
    Carta::State::StateInterface layoutState("");
    layoutState.setState( m_state.toString());
    layoutState.insertObject( "nodes", m_layoutRoot->getStateString());
    return layoutState.toString();
}



int Layout::_getIndex( const QString& plugin, const QString& locationId ) const {
    int index = -1;
    bool targetFound  = m_layoutRoot->getIndex( plugin, locationId, &index );
    if ( !targetFound ){
        index = -1;
    }
    return index;
}

QString Layout::_getPlugin( const QString& locationId ) const {
    QString plugin;
    if ( m_layoutRoot.get() != nullptr ){
        plugin = m_layoutRoot->getPlugin( locationId );
    }
    return plugin;
}

int Layout::_getPluginCount( const QString& nodeType ) const {
    int count = 0;
    if ( m_layoutRoot.get() != nullptr ){
        QStringList names = m_layoutRoot->getPluginList();
        for ( int i = 0; i < names.size(); i++ ){
            if ( names[i] == nodeType ){
                count++;
            }
        }
    }
    return count;
}

int Layout::_getPluginIndex( const QString& nodeId, const QString& pluginId ) const{
    int index = -1;
    if ( m_layoutRoot.get() != nullptr ){
        m_layoutRoot->getIndex( pluginId, nodeId, &index );
    }
    return index;
}

QStringList Layout::getPluginList() const {
    QStringList plugins;
    if ( m_layoutRoot.get() != nullptr ){
        plugins = m_layoutRoot->getPluginList();
    }
    return plugins;
}



void Layout::_initializeCommands(){

    addCommandCallback( "setLayoutSize", [=] (const QString & /*cmd*/,
                   const QString & params, const QString & /*sessionId*/) -> QString {
           std::set<QString> keys = {LAYOUT_ROWS, LAYOUT_COLS};
           std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
           QString rowStr = dataValues[LAYOUT_ROWS];
           QString colStr = dataValues[LAYOUT_COLS];
           bool valid = false;
           int rows = rowStr.toInt( &valid );
           QString result;
           if ( valid ){
               int cols = colStr.toInt( &valid );
               if ( valid ){
                   //If we are going to remove plugins with this new layout we will need
                   //to remove them from the model.
                   result = setLayoutSize( rows, cols);
               }
               else {
                   result = "Invalid layout cols: "+params;
               }
           }
           else {
               result =  "Invalid layout rows: " +params;
           }
           return result;
       });

    addCommandCallback( "addWindow", [=] (const QString & /*cmd*/,
                            const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = { Util::ID, POSITION };
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QStringList windowIds = dataValues[Util::ID].split( ' ');
            QString result = addWindow( windowIds, dataValues[POSITION] );
            Util::commandPostProcess( result );
            return result;
        });

    addCommandCallback( "removeWindow", [=] (const QString & /*cmd*/,
                                const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = { Util::ID };
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString result = _removeWindow( dataValues[Util::ID] );
            Util::commandPostProcess( result );
            return result;
        });
}



void Layout::_initializeDefaultState(){
   m_state.insertValue<QString>(LAYOUT_NODE, "");
   m_state.insertValue<QString>(TYPE_SELECTED, TYPE_CUSTOM );
}

void Layout::_initLayout( LayoutNode* root, int rowCount, int colCount ){
    //Create two composite node children
   LayoutNode* leftNode = nullptr;
   LayoutNode* rightNode = nullptr;
   //Split columns in half
   if ( colCount >= 4 ){
       int halfCount = colCount / 2;
       leftNode = _splitNode( rowCount, halfCount, true );
       rightNode = _splitNode( rowCount, colCount - halfCount, true );
   }
   //Split one of the columns in half and make the other into a leaf.
   else if ( colCount == 3){
       //Split the right one more time
       rightNode = NodeFactory::makeComposite( true );
       _initLayout( rightNode, rowCount, 2);
       //Leave the left one as a leaf if there is nothing in the other direction;
       //otherwise, split it in the other direction.
       if ( rowCount <= 1 ){
           leftNode = NodeFactory::makeLeaf();
       }
       else {
           leftNode = _splitNode( rowCount, 1, false );
       }
   }
   //Single column so we change the root to vertical
   else if ( colCount == 1 ){
      //Plenty of rows so we make both the left and right into composites
      if ( rowCount >=4  ){
          int halfCount = rowCount / 2;
          leftNode = _splitNode( halfCount, colCount, false );
          rightNode = _splitNode( rowCount - halfCount, colCount, false );
      }
      //Only split the right one, make the left one into a leaf
      else if ( rowCount == 3 ){
          rightNode = _splitNode( 2, colCount, false);
          leftNode = NodeFactory::makeLeaf();
      }
      //Two leaves
      else if ( rowCount == 2 ){
          //Make leaves
          leftNode = NodeFactory::makeLeaf();
          rightNode = NodeFactory::makeLeaf();
      }
      //Exclude one of the leaves
      else if ( rowCount == 1 ){
          leftNode = NodeFactory::makeLeaf();
          rightNode = NodeFactory::makeLeaf( NodeFactory::HIDDEN );
      }
      else {
          qDebug() << "U-oh shouldn't have a row count of 0";
      }
   }
   else if ( colCount == 2 ){
       //Start splitting horizontally
       if ( rowCount >= 2 ){
           //int halfCount = qCeil(rowCount / 2.0);
           leftNode = _splitNode( rowCount, 1, false );
           rightNode = _splitNode( rowCount, 1, false );
       }
       //We are down to two leaves
       else if ( rowCount == 1 ){
           leftNode = NodeFactory::makeLeaf();
           rightNode = NodeFactory::makeLeaf();
       }
   }
   if ( leftNode != nullptr ){
       root->setChildFirst( leftNode );
   }
   if ( rightNode != nullptr ){
       root->setChildSecond( rightNode );
   }
}

bool Layout::isLayoutDefault() const {
    bool isCurrentLayout = false;
    if ( m_state.getValue<QString>(TYPE_SELECTED) == TYPE_DEFAULT ){
        isCurrentLayout = true;
    }
    return isCurrentLayout;
}

bool Layout::isLayoutAnalysis() const {
    bool layoutAnalysis = false;
    if ( m_state.getValue<QString>(TYPE_SELECTED) == TYPE_ANALYSIS ){
        layoutAnalysis = true;
    }
    return layoutAnalysis;
}

bool Layout::isLayoutHistogramAnalysis() const {
    bool isCurrentLayout = false;
    if ( m_state.getValue<QString>(TYPE_SELECTED) == TYPE_HISTOGRAMANALYSIS ){
        isCurrentLayout = true;
    }
    return isCurrentLayout;
}

bool Layout::isLayoutImageComposite() const {
    if ( m_state.getValue<QString>(TYPE_SELECTED) == TYPE_IMAGECOMPOSITE ){
        return true;
    }
    return false;
}

bool Layout::isLayoutImage() const {
    bool layoutImage = false;
    if ( m_state.getValue<QString>(TYPE_SELECTED) == TYPE_IMAGE ){
        layoutImage = true;
    }
    return layoutImage;
}

void Layout::_makeRoot( bool horizontal ){
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    LayoutNodeComposite* main = objMan->createObject<LayoutNodeComposite>();
    main->setHorizontal( horizontal );
    m_layoutRoot.reset( main );
    m_state.setValue<QString>(LAYOUT_NODE, main->getPath());
}

bool Layout::_replaceRoot( LayoutNode* otherNode, const QString& childReplacement ){
    bool rootReplaced = false;
    if ( otherNode != nullptr ){
        if ( otherNode->isComposite()){
            rootReplaced = true;
            LayoutNode* oldRoot = m_layoutRoot.get();
            //Put in a null child for the one we are going to use as a root
            //so it won't get destroyed when we destroy the root.
            oldRoot->releaseChild( childReplacement );

            //Put the otherNode in as the new root.
            m_layoutRoot.reset( otherNode );
            m_state.setValue<QString>(LAYOUT_NODE, otherNode->getPath());
            m_state.flushState();
        }
    }
    return rootReplaced;
}

void Layout::resetState( const Carta::State::StateInterface& savedState ){
    QString layoutLookup = CLASS_NAME;

    QString typeStr = savedState.getValue<QString>(UtilState::getLookup( layoutLookup, TYPE_SELECTED));
    QString restoreType = m_state.getValue<QString>( TYPE_SELECTED );

    if ( typeStr != restoreType ){
        m_state.setValue<QString>(TYPE_SELECTED, typeStr );
    }
    QStringList oldNames = getPluginList();
    QString nodeLookup = UtilState::getLookup( CLASS_NAME, "nodes");
    QString nodeStr = savedState.toString( nodeLookup );
    QMap<QString,int> usedPlugins;

    m_layoutRoot->resetState( nodeStr, usedPlugins );
    QStringList newNames = getPluginList();
    emit pluginListChanged( newNames, oldNames );
    m_state.flushState();
}

QString Layout::_removeWindow( const QString& locationId ){
    QString result;
    bool windowRemoved = false;
    QStringList oldNames = getPluginList();
    if ( m_layoutRoot != nullptr ){
        LayoutNode* firstChild = m_layoutRoot->getChildFirst();
        LayoutNode* secondChild = m_layoutRoot->getChildSecond();
        if ( firstChild != nullptr &&
                !firstChild->isComposite() &&
                firstChild->getPath() == locationId ){
            windowRemoved = _replaceRoot( secondChild, LayoutNodeComposite::PLUGIN_RIGHT );
        }
        else if ( secondChild!= nullptr &&
                !secondChild->isComposite() &&
                secondChild->getPath() == locationId ){
            windowRemoved = _replaceRoot( firstChild, LayoutNodeComposite::PLUGIN_LEFT );
        }
        else {
            windowRemoved = m_layoutRoot->removeWindow( locationId );
        }
    }
    if ( !windowRemoved ){
        result = "There was a problem removing the window.";
    }
    else {
        QStringList newNames = getPluginList();
        emit pluginListChanged( newNames, oldNames );
    }
    return result;
}

void Layout::setLayoutDefault(bool cleanPluginList) {

    QStringList oldNames;

    if (cleanPluginList) {
        oldNames = getPluginList();
    } else {
        oldNames = QStringList();
    }

    _makeRoot();

    LayoutNode* rightTop = NodeFactory::makeComposite( false );

    LayoutNode* statLeaf = NodeFactory::makeLeaf( Statistics::CLASS_NAME );
    rightTop->setChildFirst( statLeaf );
    LayoutNode* hiddenLeaf = NodeFactory::makeLeaf( NodeFactory::HIDDEN );
    rightTop->setChildSecond( hiddenLeaf );

    LayoutNode* rightBottom = NodeFactory::makeComposite( false );
    LayoutNode* colorLeaf = NodeFactory::makeLeaf( Colormap::CLASS_NAME );
    rightBottom->setChildFirst( colorLeaf );
    LayoutNode* animLeaf = NodeFactory::makeLeaf( Animator::CLASS_NAME );
    rightBottom->setChildSecond( animLeaf );

    LayoutNode* right = NodeFactory::makeComposite( false );

    right->setChildFirst( rightTop );
    right->setChildSecond( rightBottom );

    m_layoutRoot->setHorizontal( true );
    m_layoutRoot->setChildSecond( right );

    LayoutNode* controlLeaf = NodeFactory::makeLeaf( Controller::PLUGIN_NAME );
    m_layoutRoot->setChildFirst( controlLeaf );
    m_state.setValue<QString>( TYPE_SELECTED, TYPE_DEFAULT );
    QStringList names = getPluginList();
    emit pluginListChanged( names, oldNames );
    m_state.flushState();
}

void Layout::setLayoutAnalysis(){
    QStringList oldNames = getPluginList();
    _makeRoot();

    LayoutNode* rightTop = NodeFactory::makeComposite( false );

    LayoutNode* rightBottom = NodeFactory::makeComposite( false );
    LayoutNode* colorLeaf = NodeFactory::makeLeaf( Colormap::CLASS_NAME );
    rightBottom->setChildFirst( colorLeaf );
    LayoutNode* animLeaf = NodeFactory::makeLeaf( Animator::CLASS_NAME );
    rightBottom->setChildSecond( animLeaf );

    LayoutNode* right = NodeFactory::makeComposite( false );

    LayoutNode* profLeaf = NodeFactory::makeLeaf( Profiler::CLASS_NAME );
    right->setChildFirst( profLeaf );
    right->setChildSecond( rightBottom );

    m_layoutRoot->setHorizontal( true );
    m_layoutRoot->setChildSecond( right );

    LayoutNode* controlLeaf = NodeFactory::makeLeaf( Controller::PLUGIN_NAME );
    m_layoutRoot->setChildFirst( controlLeaf );
    m_state.setValue<QString>( TYPE_SELECTED, TYPE_ANALYSIS );
    QStringList names = getPluginList();
    emit pluginListChanged( names, oldNames );
    m_state.flushState();
}

void Layout::setLayoutHistogramAnalysis(){
    QStringList oldNames = getPluginList();
    _makeRoot();

    LayoutNode* rightTop = NodeFactory::makeComposite( false );
    LayoutNode* histLeaf = NodeFactory::makeLeaf( Histogram::CLASS_NAME );
    rightTop->setChildFirst( histLeaf );
    LayoutNode* hiddenLeaf = NodeFactory::makeLeaf( NodeFactory::HIDDEN );
    rightTop->setChildSecond( hiddenLeaf );

    LayoutNode* rightBottom = NodeFactory::makeComposite( false );
    LayoutNode* colorLeaf = NodeFactory::makeLeaf( Colormap::CLASS_NAME );
    rightBottom->setChildFirst( colorLeaf );
    LayoutNode* animLeaf = NodeFactory::makeLeaf( Animator::CLASS_NAME );
    rightBottom->setChildSecond( animLeaf );

    LayoutNode* right = NodeFactory::makeComposite( false );

    right->setChildFirst( rightTop );
    right->setChildSecond( rightBottom );

    m_layoutRoot->setHorizontal( true );
    m_layoutRoot->setChildSecond( right );

    LayoutNode* controlLeaf = NodeFactory::makeLeaf( Controller::PLUGIN_NAME );
    m_layoutRoot->setChildFirst( controlLeaf );
    m_state.setValue<QString>( TYPE_SELECTED, TYPE_HISTOGRAMANALYSIS );
    QStringList names = getPluginList();
    emit pluginListChanged( names, oldNames );
    m_state.flushState();
}

void Layout::setLayoutDeveloper(){
    _makeRoot();
    QStringList oldNames = getPluginList();
    LayoutNode* rightBottom = NodeFactory::makeComposite( false );

    LayoutNode* colorLeaf = NodeFactory::makeLeaf( Colormap::CLASS_NAME );
    rightBottom->setChildFirst( colorLeaf );
    //LayoutNode* histLeaf = NodeFactory::makeLeaf( Histogram::CLASS_NAME );
    //rightBottom->setChildFirst( histLeaf );
    //LayoutNode* contextLeaf = NodeFactory::makeLeaf( ImageContext::CLASS_NAME );
    //rightBottom->setChildFirst( contextLeaf );

    LayoutNode* animLeaf = NodeFactory::makeLeaf( Animator::CLASS_NAME );
    rightBottom->setChildSecond( animLeaf );

    LayoutNode* right = NodeFactory::makeComposite( false );

    //LayoutNode* histLeaf = NodeFactory::makeLeaf( Histogram::CLASS_NAME );

    //right->setChildFirst( histLeaf );
    //LayoutNode* statLeaf = NodeFactory::makeLeaf( Statistics::CLASS_NAME );
    //right->setChildFirst( statLeaf );
    LayoutNode* profLeaf = NodeFactory::makeLeaf( Profiler::CLASS_NAME );
    right->setChildFirst( profLeaf );
    //LayoutNode* imageZoomLeaf = NodeFactory::makeLeaf( ImageZoom::CLASS_NAME );
    //right->setChildFirst( imageZoomLeaf );
    right->setChildSecond( rightBottom );

    m_layoutRoot->setHorizontal( true );
    m_layoutRoot->setChildSecond( right );

    LayoutNode* controlLeaf = NodeFactory::makeLeaf( Controller::PLUGIN_NAME );
    m_layoutRoot->setChildFirst( controlLeaf );

    QStringList names = getPluginList();
    emit pluginListChanged( names, oldNames );
    m_state.flushState();
}


void Layout::setLayoutImageComposite(){
    QStringList oldNames = getPluginList();
    _makeRoot();

    LayoutNode* rightTop = NodeFactory::makeComposite( false );
    rightTop->setHorizontal( true );
    //Image Contxt
    LayoutNode* contextLeaf = NodeFactory::makeLeaf( ImageContext::CLASS_NAME );
    rightTop->setChildFirst( contextLeaf  );
    LayoutNode* zoomLeaf = NodeFactory::makeLeaf( ImageZoom::CLASS_NAME );
    rightTop->setChildSecond( zoomLeaf );

    LayoutNode* rightBottom = NodeFactory::makeComposite( false );
    LayoutNode* colorLeaf = NodeFactory::makeLeaf( Colormap::CLASS_NAME );
    rightBottom->setChildFirst( colorLeaf );
    LayoutNode* animLeaf = NodeFactory::makeLeaf( Animator::CLASS_NAME );
    rightBottom->setChildSecond( animLeaf );

    LayoutNode* right = NodeFactory::makeComposite( false );

    right->setChildFirst( rightTop );
    right->setChildSecond( rightBottom );

    m_layoutRoot->setHorizontal( true );
    m_layoutRoot->setChildSecond( right );

    //Image Loader
    LayoutNode* controlLeaf = NodeFactory::makeLeaf( Controller::PLUGIN_NAME );
    m_layoutRoot->setChildFirst( controlLeaf );

    QStringList names = getPluginList();
    emit pluginListChanged( names, oldNames );
    m_state.setValue<QString>( TYPE_SELECTED, TYPE_IMAGECOMPOSITE );
    m_state.flushState();
}

void Layout::setLayoutImage(){
    QStringList oldNames = getPluginList();
    _makeRoot();
    m_layoutRoot->setHorizontal( true );


    //Image Loader
    LayoutNode* controlLeaf = NodeFactory::makeLeaf( Controller::PLUGIN_NAME );
    m_layoutRoot->setChildFirst( controlLeaf );

    //Hidden Window
    LayoutNode* emptyLeaf = NodeFactory::makeLeaf(NodeFactory::HIDDEN);
    m_layoutRoot->setChildSecond( emptyLeaf );

    QStringList names = getPluginList();
    emit pluginListChanged( names, oldNames );
    m_state.setValue<QString>( TYPE_SELECTED, TYPE_IMAGE );
    m_state.flushState();
}

QString Layout::setPlugins( const QStringList& names, bool useFirst) {
    QString msg;
    bool validList = _setPlugin( names, useFirst );
    if ( !validList ){
        msg = "There was an error setting the plugins.";
    }
    return msg;
}


LayoutNode* Layout::_splitNode( int rowCount, int colCount,  bool horizontal ){
    LayoutNode* node = NodeFactory::makeComposite( horizontal );
    _initLayout( node, rowCount, colCount );
    return node;
}



QString Layout::setLayoutSize( int rows, int cols, const QString& layoutType ){
    QString errorMsg;
    if ( rows >= 0 && cols >= 0 ){
        QStringList oldNames = getPluginList();
        //Replace any hidden plugins with empty ones.
        int oldNameCount = oldNames.size();
        for ( int i = 0; i < oldNameCount; i++ ){
            if ( oldNames[i] == NodeFactory::HIDDEN ){
                oldNames[i] = NodeFactory::EMPTY;
            }
        }
        bool horizontal = true;
        if ( cols == 1 ){
            horizontal = false;
        }
        _makeRoot( horizontal );
        LayoutNode* currNode = m_layoutRoot.get();
        _initLayout( currNode, rows, cols );
        _setPlugin( oldNames, false );
        m_state.setValue<QString>(TYPE_SELECTED, layoutType );
        m_state.flushState();
        QStringList newNames = getPluginList();
        emit pluginListChanged( newNames, oldNames );
    }
    else {
        errorMsg = "Invalid layout rows ="+QString::number(rows)+" and/or cols="+QString::number(cols);
    }
    Util::commandPostProcess( errorMsg);
    return errorMsg;
}



bool Layout::_setPlugin( const QString& nodeId, const QString& nodeType ){
    int destCount = _getPluginCount( nodeType );
    bool pluginSet = m_layoutRoot->setPlugin( nodeId, nodeType, destCount );
    return pluginSet;
}

bool Layout::_setPlugin( const QStringList& names, bool useFirst ){
    QStringList plugins( names );
    QStringList oldPlugins = getPluginList();
    bool pluginsChanged = !Util::isListMatch( names, oldPlugins );
    bool validList = true;
    if ( pluginsChanged ){
        QMap<QString,int> usedPluginCounts;
        validList = m_layoutRoot->setPlugins( plugins, usedPluginCounts, useFirst );
    }
    return validList;
}

Layout::~Layout(){

}
}
}
