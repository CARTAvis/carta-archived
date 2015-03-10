#include "Data/ViewManager.h"
#include "Data/Animator.h"
#include "Data/Clips.h"
#include "Data/Colormap.h"
#include "Data/Colormaps.h"
#include "Data/Controller.h"
#include "Data/DataLoader.h"
#include "Data/TransformsData.h"
#include "Data/TransformsImage.h"
#include "Data/ErrorManager.h"
#include "Data/Histogram.h"
#include "Data/ILinkable.h"
#include "Data/Layout.h"
#include "Data/Preferences.h"
#include "Data/Statistics.h"
#include "Data/ViewPlugins.h"
#include "Data/Util.h"
#include "State/StateReader.h"
#include "State/StateWriter.h"

#include <QDir>
#include <QDebug>

namespace Carta {

namespace Data {

class ViewManager::Factory : public CartaObjectFactory {

public:
    Factory():
        CartaObjectFactory( "ViewManager" ){};
    CartaObject * create (const QString & path, const QString & id)
    {
        return new ViewManager (path, id);
    }
};

const QString ViewManager::CLASS_NAME = "ViewManager";
const QString ViewManager::SOURCE_ID = "sourceId";
const QString ViewManager::DEST_ID = "destId";

bool ViewManager::m_registered =
    ObjectManager::objectManager()->registerClass ( CLASS_NAME,
                                                   new ViewManager::Factory());

ViewManager::ViewManager( const QString& path, const QString& id)
    : CartaObject( CLASS_NAME, path, id ),
      m_layout( nullptr ),
      m_dataLoader( nullptr ),
      m_pluginsLoaded( nullptr ){
    Util::findSingletonObject( Clips::CLASS_NAME );
    Util::findSingletonObject( Colormaps::CLASS_NAME );
    Util::findSingletonObject( TransformsData::CLASS_NAME);
    Util::findSingletonObject( TransformsImage::CLASS_NAME);
    Util::findSingletonObject( ErrorManager::CLASS_NAME );
    Util::findSingletonObject( Preferences::CLASS_NAME );
    _initCallbacks();

    bool stateRead = this->_readState( "DefaultState" );
    if ( !stateRead ){
        _initializeDefaultState();
    }
    _makeDataLoader();
}

void ViewManager::_clear(){
    _clearControllers( 0 );
    _clearAnimators( 0 );
    _clearColormaps( 0 );
    _clearHistograms( 0 );
    _clearStatistics( 0 );
    if ( m_layout != nullptr ){
        m_layout->clear();
    }
}

void ViewManager::_clearControllers( int startIndex ){
    ObjectManager* objMan = ObjectManager::objectManager();
    int controlCount = m_controllers.size();
    for ( int i = controlCount-1; i >= startIndex; i-- ){
        objMan->destroyObject( m_controllers[i]->getId());
        m_controllers.pop_back();
    }
}

void ViewManager::_clearAnimators( int startIndex ){
    int animCount = m_animators.size();
    ObjectManager* objMan = ObjectManager::objectManager();
    for ( int i = animCount-1; i >= startIndex; i-- ){
        objMan->destroyObject( m_animators[i]->getId());
        m_animators.pop_back();
    }
}

void ViewManager::_clearColormaps( int startIndex ){
    int colorCount = m_colormaps.size();
    ObjectManager* objMan = ObjectManager::objectManager();
    for ( int i = colorCount-1; i >= startIndex; i-- ){
        objMan->destroyObject( m_colormaps[i]->getId());
        m_colormaps.pop_back();
    }
}

void ViewManager::_clearHistograms( int startIndex ){
    int histCount = m_histograms.size();
    ObjectManager* objMan = ObjectManager::objectManager();
    for ( int i = histCount-1; i >= startIndex; i-- ){
        objMan->destroyObject( m_histograms[i]->getId());
        m_histograms.pop_back();
    }
}

void ViewManager::_clearStatistics( int startIndex ){
    int statCount = m_statistics.size();
    ObjectManager* objMan = ObjectManager::objectManager();
    for ( int i = statCount-1; i >= startIndex; i-- ){
        objMan->destroyObject( m_statistics[i]->getId());
        m_statistics.pop_back();
    }
}

void ViewManager::_initCallbacks(){
    addCommandCallback( "clearLayout", [=] (const QString & /*cmd*/,
                const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        _clear();
        return "";
    });

    addCommandCallback( "setAnalysisLayout", [=] (const QString & /*cmd*/,
                    const QString & /*params*/, const QString & /*sessionId*/) -> QString {
            setAnalysisView();
            return "";
        });

    addCommandCallback( "setImageLayout", [=] (const QString & /*cmd*/,
                        const QString & /*params*/, const QString & /*sessionId*/) -> QString {
                setImageView();
                return "";
            });

    //Callback for adding a data source to a Controller.
    addCommandCallback( "dataLoaded", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        const QString ID( "id");
        const QString DATA( "data");
        std::set<QString> keys = {ID,DATA};
        std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
        loadFile( dataValues[ID], dataValues[DATA]);
        return "";
    });

    //Callback for registering a view.
    addCommandCallback( "registerView", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        const QString PLUGIN_ID( "pluginId");
        const QString INDEX( "index");
        std::set<QString> keys = {PLUGIN_ID, INDEX};
        std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
        bool validIndex = false;
        int index = dataValues[INDEX].toInt(&validIndex);
        QString viewId( "");
        if ( validIndex ){
            viewId = getObjectId( dataValues[PLUGIN_ID], index );
        }
        else {
            qWarning()<< "Register view: invalid index: "+dataValues[PLUGIN_ID];
        }
        return viewId;
    });

    //Callback for linking an animator with whatever it is going to animate.
    addCommandCallback( "linkAdd", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {SOURCE_ID, DEST_ID};
        std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
        QString result = linkAdd( dataValues[SOURCE_ID], dataValues[DEST_ID]);
        Util::commandPostProcess( result);
        return result;
    });

    //Callback for linking an animator with whatever it is going to animate.
        addCommandCallback( "linkRemove", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = {SOURCE_ID, DEST_ID};
            std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
            QString result = linkRemove( dataValues[SOURCE_ID], dataValues[DEST_ID]);
            Util::commandPostProcess( result );
            return result;
        });


        //Callback for updating links after all objects have been created.
        addCommandCallback( "refreshState", [=] (const QString & /*cmd*/,
                        const QString & /*params*/, const QString & /*sessionId*/) -> QString {
                    qDebug() << "ViewManager refreshing state";
                    _refreshState();
                    return "";
                });


    //Callback for saving state.
    addCommandCallback( "saveState", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
        QStringList paramList = params.split( ":");
        QString saveName="DefaultState";
        if ( paramList.length() == 2 ){
           saveName = paramList[1];
        }
        bool result = _saveState(saveName);
        QString returnVal = "State was successfully saved.";
        if ( !result ){
            returnVal = "There was an error saving state.";
        }
        return returnVal;
    });

    //Callback for restoring state.
    addCommandCallback( "restoreState", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
        QStringList paramList = params.split( ":");
        QString saveName="DefaultState";
        if ( paramList.length() == 2 ){
            saveName = paramList[1];
        }
        bool result = _readState( saveName );
        QString returnVal = "State was successfully restored.";
        if ( !result ){
            returnVal = "There was an error restoring state.";
        }
        return returnVal;
    });

    addCommandCallback( "setPlugin", [=] (const QString & /*cmd*/,
                            const QString & params, const QString & /*sessionId*/) -> QString {
            const QString NAMES( "names");
            std::set<QString> keys = { NAMES };
            std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
            QStringList names = dataValues[NAMES].split(".");
            bool valid = setPlugins( names );
            QString result;
            if ( !valid ){
                result="Could not set plugins";
            }
            return result;
        });
}


void ViewManager::_initializeDefaultState(){
    setAnalysisView();
}

int ViewManager::_findColorMap( const QString& id ) const {
    int colorCount = m_colormaps.size();
    int colorIndex = -1;
    for ( int i = 0; i < colorCount; i++ ){
        if ( m_colormaps[i]->getPath() == id ){
            colorIndex = i;
            break;
        }
    }
    return colorIndex;
}

int ViewManager::_findController( const QString& id ) const {
    int controlCount = m_controllers.size();
    int controlIndex = -1;
    for ( int i = 0; i < controlCount; i++ ){
        if ( m_controllers[i]->getPath() == id ){
            controlIndex = i;
            break;
        }
    }
    return controlIndex;
}

int ViewManager::_findAnimator( const QString& id ) const {
    int animCount = m_animators.size();
    int animIndex = -1;
    for ( int i = 0; i < animCount; i++ ){
        if ( m_animators[i]->getPath() == id ){
            animIndex = i;
            break;
        }
    }
    return animIndex;
}



QString ViewManager::linkAdd( const QString& sourceId, const QString& destId ){
    QString result;
    ObjectManager* objManager = ObjectManager::objectManager();
    QString dId = objManager->parseId( destId );
    CartaObject* destObj = objManager->getObject( dId );
    if ( destObj != nullptr ){
        QString id = objManager->parseId( sourceId );
        CartaObject* sourceObj = objManager->getObject( id );
        ILinkable* linkSource = dynamic_cast<ILinkable*>( sourceObj );
        if ( linkSource != nullptr ){
            bool linked = linkSource->addLink( destObj );
            if ( !linked ){
                result = "Could not link source to destination.";
            }
        }
        else {
            result = "Unrecognized add link source: "+sourceId;
        }
    }
    else {
        result = "Unrecognized add link destination: "+dId;
    }
    return result;
}

QString ViewManager::linkRemove( const QString& sourceId, const QString& destId ){
    QString result;
    ObjectManager* objManager = ObjectManager::objectManager();
    QString dId = objManager->parseId( destId );
    CartaObject* destObj = objManager->getObject( dId );
    if ( destObj != nullptr ){
        QString id = objManager->parseId( sourceId );
        CartaObject* sourceObj = objManager->getObject( id );
        ILinkable* linkSource = dynamic_cast<ILinkable*>( sourceObj );
        if ( linkSource != nullptr ){
            bool unlinked = linkSource->removeLink( destObj );
            if ( !unlinked ){
                result = "Could not remove link between source and destination.";
            }
        }
        else {
            result = "Could not remove link, unrecognized source: "+sourceId;
        }
    }
    else {
        result = "Could not remove link, unrecognized destination: "+destId;
    }
    return result;
}

void ViewManager::_removeView( const QString& plugin, int index ){
    ObjectManager* objMan = ObjectManager::objectManager();
    if ( plugin == Controller::PLUGIN_NAME ){
        //First remove all links to the controller.
        QString destId = m_controllers[index]->getPath();
        for ( Animator* animator : m_animators ){
            QString sourceId = animator->getPath();
            linkRemove( sourceId, destId );
        }
        for ( Statistics* stat : m_statistics ){
            QString sourceId = stat->getPath();
            linkRemove( sourceId, destId );
        }
        for ( Colormap* map : m_colormaps ){
            QString sourceId = map->getPath();
            linkRemove( sourceId, destId );
        }
        for ( Histogram* hist : m_histograms ){
            QString sourceId = hist->getPath();
            linkRemove( sourceId, destId );
        }
        objMan->destroyObject( m_controllers[index]->getId());
        m_controllers.removeAt( index );
    }
    else if ( plugin == Animator::CLASS_NAME ){
        objMan->destroyObject( m_animators[index]->getId());
        m_animators.removeAt( index );
    }
    else if ( plugin == Colormap::CLASS_NAME ){
        objMan->destroyObject( m_colormaps[index]->getId());
        m_colormaps.removeAt( index );
    }
    else if ( plugin == Histogram::CLASS_NAME ){
        objMan->destroyObject( m_histograms[index]->getId());
        m_histograms.removeAt( index );
    }
    else if ( plugin == Statistics::CLASS_NAME ){
        objMan->destroyObject( m_statistics[index]->getId());
        m_statistics.removeAt( index );
    }
    else {
        qWarning() << "Unrecognized plugin "<<plugin<<" to remove";
    }
}

QString ViewManager::getObjectId( const QString& plugin, int index ){
    QString viewId("");
    if ( plugin == Controller::PLUGIN_NAME ){
        if ( 0 <= index && index < m_controllers.size()){
            viewId = m_controllers[index]->getPath();
        }
        else {
            if ( index == -1 ){
                index = m_controllers.size() - 1;
            }
            viewId = _makeController(index+1);
        }
    }
    else if ( plugin == Animator::CLASS_NAME ){
        if ( 0 <= index && index < m_animators.size()){
            viewId = m_animators[index]->getPath();
        }
        else {
            if ( index == -1 ){
                index = m_animators.size() - 1;
            }
            viewId = _makeAnimator(index+1);
        }
    }
    else if ( plugin == Colormap::CLASS_NAME ){
        if ( 0 <= index && index < m_colormaps.size()){
            viewId = m_colormaps[index]->getPath();
        }
        else {
            if ( index == -1 ){
                index = m_colormaps.size() - 1;
            }
            int newIndex = index + 1;
            viewId = _makeColorMap(newIndex);
        }
    }
    else if ( plugin == Histogram::CLASS_NAME ){
        if ( 0 <= index && index < m_histograms.size()){
            viewId = m_histograms[index]->getPath();
        }
        else {
            if ( index == -1 ){
                index = m_histograms.size() - 1;
            }
            viewId = _makeHistogram(index+1);
        }
    }
    else if ( plugin == Statistics::CLASS_NAME ){
        if ( 0 <= index && index < m_statistics.size()){
            viewId = m_statistics[index]->getPath();
        }
        else {
            if ( index == -1 ){
                index = m_statistics.size() - 1;
            }
            viewId = _makeStatistics(index+1);
        }
    }
    else if ( plugin == ViewPlugins::CLASS_NAME ){
        viewId = _makePluginList();
    }
    else {
        qDebug() << "Unrecognized top level window type: "<<plugin;
    }
    return viewId;
}

void ViewManager::loadFile( const QString& controlId, const QString& fileName){
    int controlCount = m_controllers.size();
    for ( int i = 0; i < controlCount; i++ ){
        const QString controlPath= m_controllers[i]->getPath();
        if ( controlId  == controlPath ){
           //Add the data to it
            _makeDataLoader();
           QString path = m_dataLoader->getFile( fileName, "" );
           m_controllers[i]->addData( path );
           break;
        }
    }
}

QString ViewManager::_makeAnimator( int maxCount ){
    int currentCount = m_animators.size();
    if ( currentCount < maxCount ){
        for ( int i = currentCount; i < maxCount; i++ ){
            CartaObject* animObj = Util::createObject( Animator::CLASS_NAME );
            m_animators.append( dynamic_cast<Animator*>(animObj));
        }
    }
    QString path = m_animators[maxCount-1] ->getPath();
    return path;
}

QString ViewManager::_makeColorMap( int maxCount ){
    int currentCount = m_colormaps.size();
    if ( currentCount < maxCount ){
        for ( int i = currentCount; i < maxCount; i++  ){
            CartaObject* controlObj = Util::createObject( Colormap::CLASS_NAME );
            m_colormaps.append( dynamic_cast<Colormap*>(controlObj) );
        }
    }
    QString path = m_colormaps[maxCount-1]->getPath();
   return path;
}

QString ViewManager::_makeController( int maxCount ){
    int currentCount = m_controllers.size();
    if ( currentCount < maxCount ){
        for ( int i = currentCount; i < maxCount; i++ ){
            CartaObject* controlObj = Util::createObject( Controller::CLASS_NAME );
            m_controllers.append( dynamic_cast<Controller*>(controlObj) );
        }
    }
    return m_controllers[maxCount-1]->getPath();
}

void ViewManager::_makeDataLoader(){
    if ( m_dataLoader == nullptr ){
        CartaObject* dataLoaderObj = Util::createObject( DataLoader::CLASS_NAME );
        m_dataLoader =dynamic_cast<DataLoader*>( dataLoaderObj );
    }
}

QString ViewManager::_makeHistogram( int maxCount ){
    int currentCount = m_histograms.size();
    if ( currentCount < maxCount ){
        for ( int i = currentCount; i < maxCount; i++ ){
            CartaObject* controlObj = Util::createObject( Histogram::CLASS_NAME );
            m_histograms.append( dynamic_cast<Histogram*>(controlObj) );
        }
    }
    return m_histograms[maxCount - 1]->getPath();
}

QString ViewManager::_makeLayout(){
    if ( !m_layout ){
        CartaObject* layoutObj = Util::createObject( Layout::CLASS_NAME );
        m_layout = dynamic_cast<Layout*>(layoutObj );
    }
    QString layoutPath = m_layout->getPath();
    return layoutPath;
}

QString ViewManager::_makePluginList(){
    if ( !m_pluginsLoaded ){
        //Initialize a view showing the plugins that have been loaded
        CartaObject* pluginsObj = Util::createObject( ViewPlugins::CLASS_NAME );
        m_pluginsLoaded = dynamic_cast<ViewPlugins*>(pluginsObj);
    }
    QString pluginsPath = m_pluginsLoaded->getPath();
    return pluginsPath;
}

QString ViewManager::_makeStatistics( int maxCount ){
    int currentCount = m_statistics.size();
    if (currentCount < maxCount ){
        for ( int i = currentCount; i < maxCount; i++ ){
            CartaObject* controlObj = Util::createObject( Statistics::CLASS_NAME );
            m_statistics.append( dynamic_cast<Statistics*>(controlObj) );
        }
    }
    return m_statistics[maxCount-1]->getPath();
}

bool ViewManager::_readState( const QString& saveName ){
    _clear();
    QString fullLocation = getStateLocation( saveName );
    StateReader reader( fullLocation );
    bool successfulRead = reader.restoreState();
    if ( successfulRead ){

        //Make the controllers specified in the state.
        QList<std::pair<QString,QString> > controllerStates = reader.getViews(Controller::CLASS_NAME);
        int count = 0;
        for ( std::pair<QString,QString> state : controllerStates ){
            count++;
            _makeController(count);
            m_controllers[count - 1]->resetState( state.second );
        }

        //Make the animators specified in the state.
        count = 0;
        QList< std::pair<QString,QString> > animatorStates = reader.getViews(Animator::CLASS_NAME);
        for ( std::pair<QString,QString> state : animatorStates ){
            count++;
            _makeAnimator( count );
            int animIndex = count - 1;
            m_animators[ animIndex ]->resetState( state.second );

            //Now see if this animator needs to be linked to any of the controllers
            QList<QString> oldLinks = m_animators[ animIndex ]-> getLinks();
            m_animators[animIndex ]->clear();
            for ( int i = 0;  i < controllerStates.size(); i++ ){
                if ( oldLinks.contains( StateInterface::DELIMITER + controllerStates[i].first ) ){
                    m_animators[animIndex]->addLink( m_controllers[i]);
                }
            }
         }

        //Reset the layout
        QString layoutState = reader.getState(Layout::CLASS_NAME);
        _makeLayout();
        m_layout->resetState( layoutState );
    }
    return successfulRead;
}

void ViewManager::_refreshState(){
    for ( Animator* anim : m_animators ){
        anim->refreshState();
    }
    for ( Histogram* hist : m_histograms ){
        hist->refreshState();
    }
    for ( Colormap* map : m_colormaps ){
        map->refreshState();
    }
}

void ViewManager::setAnalysisView(){
    _clearControllers( 1 );
    _clearAnimators( 1 );
     _clearColormaps( 1 );
    _clearHistograms( 0 );
    _clearStatistics( 1 );

    if ( m_layout == nullptr ){
        _makeLayout();
    }
    m_layout->setLayoutAnalysis();

    //Create the view objects
    _makeAnimator(1);
    _makeController(1);
    _makeColorMap(1);
    _makeStatistics(1);

    //Add the links to establish reasonable defaults.
    m_animators[0]->addLink( m_controllers[0]);
    m_colormaps[0]->addLink( m_controllers[0]);
    m_statistics[0]->addLink( m_controllers[0]);
}

bool ViewManager::setColorMap( const QString& colormapId, const QString& colormapName ){
    int colormapIndex = _findColorMap( colormapId );
    bool colorMapFound = false;
    if ( colormapIndex >= 0 ){
        m_colormaps[colormapIndex]->setColorMap( colormapName );
        colorMapFound = true;
    }
    return colorMapFound;
}

void ViewManager::setDeveloperView(){

    _clearControllers( 1 );
    _clearAnimators( 1 );
    _clearColormaps( 1 );
    _clearHistograms( 1 );
    _clearStatistics( 1 );

    //Create the view objects
    _makeAnimator(1);
    _makeController(1);
    _makeColorMap(1);
    _makeHistogram(1);
    _makeStatistics(1);

    if ( m_layout == nullptr ){
        _makeLayout();
    }
    m_layout->setLayoutDeveloper();

    //Add the links to establish reasonable defaults.
    m_animators[0]->addLink( m_controllers[0]);
    m_histograms[0]->addLink( m_controllers[0]);
    m_colormaps[0]->addLink( m_controllers[0]);
    m_statistics[0]->addLink( m_controllers[0]);
    m_histograms[0]->addLink( m_colormaps[0]);
    m_animators[0]->addLink( m_histograms[0]);
}

void ViewManager::setImageView(){
    _clearControllers( 1 );
     _clearAnimators( 0 );
     _clearColormaps( 0 );
     _clearHistograms( 0 );
    _clearStatistics( 0 );
    if ( m_layout == nullptr ){
        _makeLayout();
    }
    m_layout->setLayoutImage();
    _makeController(1);
}


bool ViewManager::_saveState( const QString& saveName ){
    QString filePath = getStateLocation( saveName );
    StateWriter writer( filePath );
    writer.addPathData( m_layout->getPath(), m_layout->getStateString());
    for ( int i = 0; i < m_controllers.size(); i++ ){
        writer.addPathData( m_controllers[i]->getPath(), m_controllers[i]->getStateString() );
    }
    for ( int i = 0; i < m_animators.size(); i++ ){
        writer.addPathData( m_animators[i]->getPath(), m_animators[i]->getStateString() );
    }
    bool stateSaved = writer.saveState();
    return stateSaved;
}

bool ViewManager::setPlugins( const QStringList& names ){
    bool pluginsSet = false;
    if ( m_layout ){
        QStringList oldNames = m_layout->getPluginList();
        bool valid = m_layout->_setPlugin( names );
        if ( !valid ){
            qDebug() << "Invalid plugins for layout size";
        }
        else {
            QMap<QString, int> pluginMap;
            for ( int i = 0; i < oldNames.size(); i++ ){
                pluginMap[oldNames[i]] = -1;
            }
            for ( int i = 0; i < names.size(); i++ ){
                pluginMap[oldNames[i]] = pluginMap[oldNames[i]] + 1;
                if ( names[i] != oldNames[i]){
                    _removeView( oldNames[i], pluginMap[oldNames[i]]);
                }
            }
            pluginsSet = true;
        }
    }
    else {
        qWarning() << "A layout for the plugins is missing";
    }
    return pluginsSet;
}

ViewManager::~ViewManager(){
    ObjectManager* objMan = ObjectManager::objectManager();
    if ( m_layout != nullptr){
        objMan->destroyObject(m_layout->getId());
        m_layout = nullptr;
    }
    if ( m_dataLoader != nullptr ){
        objMan->destroyObject( m_dataLoader->getId());
        m_dataLoader = nullptr;
    }
    if ( m_pluginsLoaded != nullptr ){
        objMan->destroyObject(m_pluginsLoaded->getId());
        m_pluginsLoaded = nullptr;
    }
}
}
}

