#include "Data/ViewManager.h"
#include "Data/Animator.h"
#include "Data/AnimationTypes.h"
#include "Data/Clips.h"
#include "Data/Colormap.h"
#include "Data/Colormaps.h"
#include "Data/Controller.h"
#include "Data/ChannelUnits.h"
#include "Data/DataLoader.h"
#include "Data/TransformsData.h"
#include "Data/TransformsImage.h"
#include "Data/ErrorManager.h"
#include "Data/Histogram.h"
#include "Data/ILinkable.h"
#include "Data/Layout.h"
#include "Data/Preferences.h"
#include "Data/Snapshots.h"
#include "Data/Statistics.h"
#include "Data/ViewPlugins.h"
#include "Data/Util.h"
#include "State/StateReader.h"

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
    Util::findSingletonObject( AnimationTypes::CLASS_NAME );
    Util::findSingletonObject( Clips::CLASS_NAME );
    Util::findSingletonObject( Colormaps::CLASS_NAME );
    Util::findSingletonObject( TransformsData::CLASS_NAME);
    Util::findSingletonObject( TransformsImage::CLASS_NAME);
    Util::findSingletonObject( ErrorManager::CLASS_NAME );
    Util::findSingletonObject( Preferences::CLASS_NAME );
    Util::findSingletonObject( ChannelUnits::CLASS_NAME );
    Util::findSingletonObject( Snapshots::CLASS_NAME );
    _initCallbacks();
    _initializeDefaultState();
    _makeDataLoader();
}


void ViewManager::_adjustSize( int count, const QString& name, const QVector<int> & insertionIndices ){
    int existingCount = 0;
    if ( name == Colormap::CLASS_NAME ){
        existingCount = m_colormaps.size();
       _clearColormaps(count);
    }
    else if ( name == Histogram::CLASS_NAME ){
        existingCount = m_histograms.size();
        _clearHistograms(count);
    }
    else if ( name == Animator::CLASS_NAME ){
        existingCount = m_animators.size();
        _clearAnimators(count);
    }
    else if ( name == Controller::PLUGIN_NAME ){
        existingCount = m_controllers.size();
        _clearControllers(count);
    }
    else if ( name == Statistics::CLASS_NAME ){
        existingCount = m_statistics.size();
        _clearStatistics(count);
    }

    //Add some at the inserting indices.
    if ( existingCount < count ){
        int index = 0;
        for ( int i = existingCount; i < count; i++ ){
            this->getObjectId( name, insertionIndices[index], true );
            index++;
        }
    }
}

void ViewManager::_clear(){
    _clearHistograms( 0 );
    _clearStatistics( 0 );
    _clearAnimators( 0 );
    _clearColormaps( 0 );
    _clearControllers( 0 );
    if ( m_layout != nullptr ){
        m_layout->clear();
    }
}

void ViewManager::_clearControllers( int startIndex ){
    ObjectManager* objMan = ObjectManager::objectManager();
    int controlCount = m_controllers.size();
    for ( int i = controlCount-1; i >= startIndex; i-- ){
        for ( Animator* anim : m_animators ){
            anim->removeLink( m_controllers[i]);
        }
        for ( Histogram* hist : m_histograms ){
            hist->removeLink( m_controllers[i]);
        }
        for ( Colormap* map : m_colormaps ){
            map->removeLink( m_controllers[i]);
        }
        for ( Statistics* stat : m_statistics ){
            stat->removeLink( m_controllers[i]);
        }
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
        for (Histogram* hist : m_histograms ){
            hist->removeLink( m_colormaps[i]);
        }
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
                    _refreshState();
                    return "";
                });


    //Callback for saving state.
    addCommandCallback( "saveState", [=] (const QString & /*cmd*/,
                const QString & params, const QString & sessionId) -> QString {
        std::set<QString> keys = {Snapshots::FILE_NAME,Snapshots::SAVE_LAYOUT,
                Snapshots::SAVE_PREFERENCES,Snapshots::SAVE_DATA};
        std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
        QString result;
        bool validLayout = false;
        bool saveLayout = Util::toBool( dataValues[Snapshots::SAVE_LAYOUT], &validLayout );
        if ( !validLayout ){
            result = "Error saving state; save layout should be true/false:"+params;
        }
        else {
            bool validPrefs = false;
            bool savePrefs = Util::toBool( dataValues[Snapshots::SAVE_PREFERENCES], & validPrefs );
            if ( !validPrefs ){
                result = "Error saving state; preferences should be true/false:"+params;
            }
            else {
                bool validData = false;
                bool saveData = Util::toBool( dataValues[Snapshots::SAVE_DATA], &validData );
                if ( !validData ){
                    result = "Error saving state: save data should be true/false:"+params;
                }
                else {
                    result = saveState(sessionId, dataValues[Snapshots::FILE_NAME],
                           saveLayout, savePrefs, saveData );
                }
            }
        }
        Util::commandPostProcess(result);
        return result;
    });

    //Callback for restoring state.
    addCommandCallback( "restoreState", [=] (const QString & /*cmd*/,
                const QString & params, const QString & sessionId) -> QString {
        std::set<QString> keys = {Snapshots::FILE_NAME};
        std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
        QString saveName=dataValues[Snapshots::FILE_NAME];
        QString result;
        bool stateRestored = _readState( sessionId, saveName );
        if ( !stateRestored ){
            result = "There was an error restoring state: "+saveName;
        }
        Util::commandPostProcess( result );
        return result;
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
    int colorCount = getColorMapCount();
    int colorIndex = -1;
    for ( int i = 0; i < colorCount; i++ ){
        if ( m_colormaps[i]->getPath() == id ){
            colorIndex = i;
            break;
        }
    }
    return colorIndex;
}

int ViewManager::_findAnimator( const QString& id ) const {
    int animCount = getAnimatorCount();
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
            result = linkSource->addLink( destObj );
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
            result = linkSource->removeLink( destObj );
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
    if ( plugin == Layout::HIDDEN ){
        return;
    }
    else if ( plugin == Controller::PLUGIN_NAME ){
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

QString ViewManager::getObjectId( const QString& plugin, int index, bool forceCreate ){
    QString viewId("");
    if ( plugin == Controller::PLUGIN_NAME ){
        if ( 0 <= index && index < m_controllers.size()&&!forceCreate){
            viewId = m_controllers[index]->getPath();
        }
        else {
            if ( index == -1 ){
                index = m_controllers.size();
            }
            viewId = _makeController(index);
        }
    }
    else if ( plugin == Animator::CLASS_NAME ){
        if ( 0 <= index && index < m_animators.size() && !forceCreate ){
            viewId = m_animators[index]->getPath();
        }
        else {
            if ( index == -1 ){
                index = m_animators.size();
            }
            viewId = _makeAnimator(index);
        }
    }
    else if ( plugin == Colormap::CLASS_NAME ){
        if ( 0 <= index && index < m_colormaps.size() && !forceCreate){
            viewId = m_colormaps[index]->getPath();
        }
        else {
            if ( index == -1 ){
                index = m_colormaps.size();
            }
            viewId = _makeColorMap( index );
        }
    }
    else if ( plugin == Histogram::CLASS_NAME ){
        if ( 0 <= index && index < m_histograms.size() && !forceCreate){
            viewId = m_histograms[index]->getPath();
        }
        else {
            if ( index == -1 ){
                index = m_histograms.size();
            }
            viewId = _makeHistogram(index);
        }
    }
    else if ( plugin == Statistics::CLASS_NAME ){
        if ( 0 <= index && index < m_statistics.size() && !forceCreate ){
            viewId = m_statistics[index]->getPath();
        }
        else {
            if ( index == -1 ){
                index = m_statistics.size();
            }
            viewId = _makeStatistics(index);
        }
    }
    else if ( plugin == ViewPlugins::CLASS_NAME ){
        viewId = _makePluginList();
    }
    else if ( plugin == Layout::EMPTY || plugin == Layout::HIDDEN ){
        //Do nothing
    }
    else {
        qDebug() << "Unrecognized top level window type: "<<plugin;
    }
    return viewId;
}

void ViewManager::loadFile( const QString& controlId, const QString& fileName){
    int controlCount = getControllerCount();
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

void ViewManager::loadLocalFile( const QString& controlId, const QString& fileName){
    int controlCount = getControllerCount();
    for ( int i = 0; i < controlCount; i++ ){
        const QString controlPath= m_controllers[i]->getPath();
        if ( controlId  == controlPath ){
           //Add the data to it
            _makeDataLoader();
           m_controllers[i]->addData( fileName );
           break;
        }
    }
}

QString ViewManager::getFileList() {
    QString fileList = m_dataLoader->getData("", "");
    return fileList;
}

int ViewManager::getControllerCount() const {
    int controllerCount = m_controllers.size();
    return controllerCount;
}

int ViewManager::getColorMapCount() const {
    int colorMapCount = m_colormaps.size();
    return colorMapCount;
}

int ViewManager::getAnimatorCount() const {
    int animatorCount = m_animators.size();
    return animatorCount;
}

int ViewManager::getHistogramCount() const {
    int histogramCount = m_histograms.size();
    return histogramCount;
}

int ViewManager::getStatisticsCount() const {
    int statisticsCount = m_statistics.size();
    return statisticsCount;
}

QString ViewManager::_makeAnimator( int index ){
    int currentCount = m_animators.size();
    CARTA_ASSERT( 0 <= index && index <= currentCount );
    CartaObject* animObj = Util::createObject( Animator::CLASS_NAME );
    m_animators.insert( index, dynamic_cast<Animator*>(animObj));
    QString path = m_animators[index] ->getPath();
    return path;
}

QString ViewManager::_makeColorMap( int index ){
    int currentCount = m_colormaps.size();
    CARTA_ASSERT( 0 <= index && index <= currentCount );
    CartaObject* controlObj = Util::createObject( Colormap::CLASS_NAME );
    m_colormaps.insert( index, dynamic_cast<Colormap*>(controlObj) );
    QString path = m_colormaps[index]->getPath();
   return path;
}

QString ViewManager::_makeController( int index ){
    int currentCount = m_controllers.size();
    CARTA_ASSERT( 0 <= index && index <= currentCount );
    CartaObject* controlObj = Util::createObject( Controller::CLASS_NAME );
    m_controllers.insert( index, dynamic_cast<Controller*>(controlObj) );
    QString path = m_controllers[index]->getPath();
    return path;
}

void ViewManager::_makeDataLoader(){
    if ( m_dataLoader == nullptr ){
        CartaObject* dataLoaderObj = Util::createObject( DataLoader::CLASS_NAME );
        m_dataLoader =dynamic_cast<DataLoader*>( dataLoaderObj );
    }
}

QString ViewManager::_makeHistogram( int index ){
    int currentCount = m_histograms.size();
    CARTA_ASSERT( 0 <= index && index <= currentCount );
    CartaObject* controlObj = Util::createObject( Histogram::CLASS_NAME );
    m_histograms.insert( index, dynamic_cast<Histogram*>(controlObj) );
    return m_histograms[index]->getPath();
}

QString ViewManager::_makeLayout(){
    if ( !m_layout ){
        CartaObject* layoutObj = Util::createObject( Layout::CLASS_NAME );
        m_layout = dynamic_cast<Layout*>(layoutObj );
        connect( m_layout, SIGNAL(pluginListChanged(const QStringList&, const QStringList&)),
                this, SLOT( _pluginsChanged( const QStringList&, const QStringList&)));
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

QString ViewManager::_makeStatistics( int index ){
    int currentCount = m_statistics.size();
    CARTA_ASSERT( 0 <= index && index <= currentCount );
    CartaObject* controlObj = Util::createObject( Statistics::CLASS_NAME );
    m_statistics.append( dynamic_cast<Statistics*>(controlObj) );
    return m_statistics[index]->getPath();
}

void ViewManager::_pluginsChanged( const QStringList& names, const QStringList& oldNames ){
    QMap<QString, int> pluginMap;
    QMap<QString, QVector<int>> insertionIndices;
    int oldNameCount = oldNames.size();
    for ( int i = 0; i < names.size(); i++ ){
        if ( !insertionIndices.contains( names[i]) ){
            insertionIndices[names[i]] = QVector<int>();
        }
        //Store the accumulated count for this plugin.
        if ( pluginMap.contains( names[i]) ){
            pluginMap[names[i]] = pluginMap[names[i]]+1;
        }
        else {
            pluginMap[names[i]] = 1;
        }

        //If there is an existing plugin of this type at i,
        //then we don't need to do an insertion, otherwise, we may need to
        //if the new plugin count ends up being more than the old one.
        if ( i >= oldNameCount || oldNames[i] != names[i] ){
            QVector<int> & indices = insertionIndices[names[i]];
            indices.append( pluginMap[names[i]] - 1);
        }
    }

    //Add counts of zero for old ones that no longer exist.
    for ( int i = 0; i < oldNameCount; i++ ){
        if ( !names.contains( oldNames[i] ) ){
            pluginMap[oldNames[i]] = 0;
            insertionIndices[oldNames[i]] = QVector<int>(0);
        }
    }

    //Remove any views that are no longer needed
    QList<QString> keys = pluginMap.keys();
    for ( QString key : keys ){
        _adjustSize( pluginMap[key], key, insertionIndices[key]);
    }

}

bool ViewManager::_readStateLayout( const QString& sessionId, const QString& saveName ){
    Snapshots* snapshot = dynamic_cast<Snapshots*>(Util::findSingletonObject( Snapshots::CLASS_NAME ));
    QString layoutState = snapshot->readLayout( sessionId, saveName );
    bool layoutRead = false;
    if ( ! layoutState.isEmpty() ){
        StateInterface state("");
        state.setState( layoutState );
        QString layState = state.toString(Layout::CLASS_NAME);
        m_layout->resetState( layState );
        int linkCount = state.getArraySize( LinkableImpl::LINK );
        ObjectManager* objManager = ObjectManager::objectManager();
        for ( int i = 0; i < linkCount; i++ ){
            QString sourceLookup = Util::getLookup( LinkableImpl::LINK, i);
            QString sourceLink = state.getValue<QString>( sourceLookup );
            StateInterface sourceState( "");
            sourceState.setState( sourceLink );
            QString sourcePath = sourceState.getValue<QString>( LinkableImpl::PARENT_ID );
            QString sourceId = objManager->parseId( sourcePath );
            CartaObject* obj = objManager->getObject( sourceId );
            ILinkable* linkable = dynamic_cast<ILinkable*>(obj);
            if ( linkable != nullptr ){
                int destCount = sourceState.getArraySize( LinkableImpl::LINK );
                for ( int j = 0; j < destCount; j++ ){
                    QString destLookup = Util::getLookup( LinkableImpl::LINK, j );
                    QString destPath = sourceState.getValue<QString>(destLookup);
                    QString destId = objManager->parseId( destPath );
                    CartaObject* destObj = objManager->getObject( destId );
                    if ( destObj != nullptr ){
                        linkable->addLink( destObj );
                    }
                    else {
                        qDebug() << "Null link destination";
                    }
                }
            }
            else {
                qDebug() << "Null link source!";
            }
        }
        layoutRead = true;
    }
    return layoutRead;
}


bool ViewManager::_readState( const QString& stateStr, SnapshotType type ){
    bool prefsRead = false;
    if ( ! stateStr.isEmpty() ){
        StateInterface state("");
        state.setState( stateStr );
        int stateCount = state.getArraySize( ObjectManager::STATE_ARRAY );
        ObjectManager* objManager = ObjectManager::objectManager();
        for ( int i = 0; i < stateCount; i++ ){
            QString stateLookup = Util::getLookup( ObjectManager::STATE_ARRAY, i);
            QString idLookup = stateLookup + StateInterface::DELIMITER + ObjectManager::STATE_ID;
            QString objId = state.getValue<QString>( idLookup );
            CartaObject* cartaObj = objManager->getObject( objId );
            if ( cartaObj != NULL ){
                QString valLookup = stateLookup + StateInterface::DELIMITER + ObjectManager::STATE_VALUE;
                QString stateVal = state.getValue<QString>(valLookup);
                cartaObj ->resetState( stateVal, type );
            }
        }
        prefsRead = true;
    }
    return prefsRead;
}


bool ViewManager::_readState( const QString& sessionId, const QString& saveName ){
    //Order is important: layout -> preferences -> data
    bool layoutRead = _readStateLayout( sessionId, saveName);
    Snapshots* snapshot = dynamic_cast<Snapshots*>(Util::findSingletonObject( Snapshots::CLASS_NAME ));
    QString prefStateStr = snapshot->readPreferences( sessionId, saveName );
    bool prefRead = _readState( prefStateStr, SNAPSHOT_PREFERENCES);
    QString dataStateStr = snapshot->readData( sessionId, saveName );
    bool dataRead = _readState( dataStateStr, SNAPSHOT_DATA );
    bool stateRead = layoutRead || prefRead || dataRead;
    return stateRead;
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
    if ( m_layout == nullptr ){
        _makeLayout();
    }
    else {
        _clearHistograms( 1 );
        _clearAnimators( 1 );
        _clearColormaps( 1 );
        _clearStatistics( 1 );
        _clearControllers( 1 );
    }
    m_layout->setLayoutAnalysis();

    //Add the links to establish reasonable defaults.
    m_animators[0]->addLink( m_controllers[0]);
    m_colormaps[0]->addLink( m_controllers[0]);
    m_statistics[0]->addLink( m_controllers[0]);
    m_histograms[0]->addLink( m_controllers[0]);
    m_histograms[0]->addLink( m_colormaps[0]);
    _refreshState();
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

bool ViewManager::reverseColorMap( const QString& colormapId, const QString& reverseStr ){
    int colormapIndex = _findColorMap( colormapId );
    bool colorMapFound = false;
    if ( colormapIndex >= 0 ){
        m_colormaps[colormapIndex]->reverseColormap( reverseStr );
        colorMapFound = true;
    }
    return colorMapFound;
}

QString ViewManager::setCacheColormap( const QString& colormapId, const QString& cacheStr ){
    QString output = "";
    int colormapIndex = _findColorMap( colormapId );
    if ( colormapIndex >= 0 ){
        output = m_colormaps[colormapIndex]->setCacheColormap( cacheStr );
    }
    return output;
}

QString ViewManager::setCacheSize( const QString& colormapId, const QString& cacheSize ){
    QString output = "";
    int colormapIndex = _findColorMap( colormapId );
    if ( colormapIndex >= 0 ){
        output = m_colormaps[colormapIndex]->setCacheSize( cacheSize );
    }
    return output;
}

QString ViewManager::setInterpolatedColorMap( const QString& colormapId, const QString& interpolateStr ){
    QString output = "";
    int colormapIndex = _findColorMap( colormapId );
    if ( colormapIndex >= 0 ){
        output = m_colormaps[colormapIndex]->setInterpolatedColorMap( interpolateStr );
    }
    return output;
}

bool ViewManager::invertColorMap( const QString& colormapId, const QString& invertStr ){
    int colormapIndex = _findColorMap( colormapId );
    bool colorMapFound = false;
    if ( colormapIndex >= 0 ){
        m_colormaps[colormapIndex]->invertColorMap( invertStr );
        colorMapFound = true;
    }
    return colorMapFound;
}

bool ViewManager::setColorMix( const QString& colormapId, const QString& percentString ){
    int colormapIndex = _findColorMap( colormapId );
    bool colorMapFound = false;
    if ( colormapIndex >= 0 ){
        QString output = m_colormaps[colormapIndex]->setColorMix( percentString );
        colorMapFound = true;
    }
    return colorMapFound;
}

QString ViewManager::setGamma( const QString& colormapId, double gamma ){
    QString output = "";
    int colormapIndex = _findColorMap( colormapId );
    if ( colormapIndex >= 0 ){
        output = m_colormaps[colormapIndex]->setGamma( gamma );
    }
    return output;
}

QString ViewManager::setDataTransform( const QString& colormapId, const QString& transformString ){
    QString output = "";
    int colormapIndex = _findColorMap( colormapId );
    if ( colormapIndex >= 0 ){
        output = m_colormaps[colormapIndex]->setDataTransform( transformString );
    }
    return output;
}

bool ViewManager::setChannel( const QString& animatorId, int index ){
    int animatorIndex = _findAnimator( animatorId );
    bool animatorFound = false;
    if ( animatorIndex >= 0 ){
        m_animators[animatorIndex]->changeChannelIndex( index );
        animatorFound = true;
    }
    return animatorFound;
}

bool ViewManager::setImage( const QString& animatorId, int index ){
    int animatorIndex = _findAnimator( animatorId );
    bool animatorFound = false;
    if ( animatorIndex >= 0 ){
        m_animators[animatorIndex]->changeImageIndex( index );
        animatorFound = true;
    }
    return animatorFound;
}

void ViewManager::setCustomView( int rows, int cols ){
    _clear();
    if ( m_layout == nullptr ){
        _makeLayout();
    }
    QString result = m_layout->setLayoutSize( rows, cols );
}

void ViewManager::setDeveloperView(){
    if ( m_layout == nullptr ){
        _makeLayout();
    }

    m_layout->setLayoutDeveloper();

    //Add the links to establish reasonable defaults.
    m_animators[0]->addLink( m_controllers[0]);
    m_histograms[0]->addLink( m_controllers[0]);
    m_colormaps[0]->addLink( m_controllers[0]);
    m_histograms[0]->addLink( m_colormaps[0]);
    _refreshState();
}

void ViewManager::setImageView(){
    if ( m_layout == nullptr ){
        _makeLayout();
    }
    else {
        _clearHistograms( 0 );
        _clearAnimators( 0 );
        _clearColormaps( 0 );
        _clearStatistics( 0 );
        _clearControllers( 1 );
    }
    m_layout->setLayoutImage();
}

//void ViewManager::setPlugins( const QStringList& names ){
//    if ( m_layout == nullptr ){
//        _makeLayout();
//    }
//    m_layout->setPlugins( names );
//}

void ViewManager::setClipValue( const QString& controlId, const QString& param ){
    int controlCount = getControllerCount();
    for ( int i = 0; i < controlCount; i++ ){
        const QString controlPath= m_controllers[i]->getPath();
        if ( controlId  == controlPath ){
           m_controllers[i]->setClipValue( param );
           break;
        }
    }
}

bool ViewManager::saveImage( const QString& controlId, const QString& filename ){
    int controlCount = getControllerCount();
    bool result = false;
    for ( int i = 0; i < controlCount; i++ ){
        const QString controlPath= m_controllers[i]->getPath();
        if ( controlId  == controlPath ){
           result = m_controllers[i]->saveImage( filename );
           break;
        }
    }
    return result;
}

bool ViewManager::saveFullImage( const QString& controlId, const QString& filename, double scale ){
    int controlCount = getControllerCount();
    bool result = false;
    for ( int i = 0; i < controlCount; i++ ){
        const QString controlPath= m_controllers[i]->getPath();
        if ( controlId  == controlPath ){
           result = m_controllers[i]->saveFullImage( filename, scale );
           break;
        }
    }
    return result;
}

QStringList ViewManager::getLinkedColorMaps( const QString& controlId ) {
    QStringList linkedColorMaps;
    int colormapCount = m_colormaps.size();
    for ( int i = 0; i < colormapCount; i++ ){
        QList<QString> oldLinks = m_colormaps[ i ]-> getLinks();
        QString colormapId = getObjectId(Colormap::CLASS_NAME, i);
        if (oldLinks.contains( controlId )) {
            linkedColorMaps.append(colormapId);
        }
    }
    return linkedColorMaps;
}

QStringList ViewManager::getLinkedAnimators( const QString& controlId ) {
    QStringList linkedAnimators;
    int animatorCount = m_animators.size();
    for ( int i = 0; i < animatorCount; i++ ){
        QList<QString> oldLinks = m_animators[ i ]-> getLinks();
        QString animatorId = getObjectId(Animator::CLASS_NAME, i);
        if (oldLinks.contains( controlId )) {
            linkedAnimators.append(animatorId);
        }
    }
    return linkedAnimators;
}

QStringList ViewManager::getLinkedHistograms( const QString& controlId ) {
    QStringList linkedHistograms;
    int histogramCount = m_histograms.size();
    for ( int i = 0; i < histogramCount; i++ ){
        QList<QString> oldLinks = m_histograms[ i ]-> getLinks();
        QString histogramId = getObjectId(Histogram::CLASS_NAME, i);
        if (oldLinks.contains( controlId )) {
            linkedHistograms.append(histogramId);
        }
    }
    return linkedHistograms;
}

QStringList ViewManager::getLinkedStatistics( const QString& controlId ) {
    QStringList linkedStatistics;
    int statisticsCount = m_statistics.size();
    for ( int i = 0; i < statisticsCount; i++ ){
        QList<QString> oldLinks = m_statistics[ i ]-> getLinks();
        QString statisticsId = getObjectId(Statistics::CLASS_NAME, i);
        if (oldLinks.contains( controlId )) {
            linkedStatistics.append(statisticsId);
        }
    }
    return linkedStatistics;
}

QString ViewManager::centerOnPixel( const QString& controlId, double x, double y ) {
    QString result = "";
    int controlCount = getControllerCount();
    for ( int i = 0; i < controlCount; i++ ){
        const QString controlPath= m_controllers[i]->getPath();
        if ( controlId  == controlPath ){
            m_controllers[i]->centerOnPixel( x, y );
            break;
        }
    }
    return result;
}

QString ViewManager::setZoomLevel( const QString& controlId, double zoomLevel ) {
    QString result = "";
    int controlCount = getControllerCount();
    for ( int i = 0; i < controlCount; i++ ){
        const QString controlPath= m_controllers[i]->getPath();
        if ( controlId  == controlPath ){
            m_controllers[i]->setZoomLevel( zoomLevel );
            break;
        }
    }
    return result;
}

double ViewManager::getZoomLevel( const QString& controlId ) {
    double zoom;
    int controlCount = getControllerCount();
    for ( int i = 0; i < controlCount; i++ ){
        const QString controlPath= m_controllers[i]->getPath();
        if ( controlId  == controlPath ){
            zoom = m_controllers[i]->getZoomLevel();
            break;
        }
    }
    return zoom;
}

QStringList ViewManager::getImageDimensions( const QString& controlId ) {
    QStringList result;
    int controlCount = getControllerCount();
    for ( int i = 0; i < controlCount; i++ ){
        const QString controlPath= m_controllers[i]->getPath();
        if ( controlId  == controlPath ){
            result = m_controllers[i]->getImageDimensions();
            break;
        }
    }
    return result;
}

QStringList ViewManager::getOutputSize( const QString& controlId ) {
    QStringList result;
    int controlCount = getControllerCount();
    for ( int i = 0; i < controlCount; i++ ){
        const QString controlPath= m_controllers[i]->getPath();
        if ( controlId  == controlPath ){
            result = m_controllers[i]->getOutputSize();
            break;
        }
    }
    return result;
}

QString ViewManager::getIntensity( const QString& controlId, int frameLow, int frameHigh, double percentile ) {
    QString result;
    double intensity;
    bool valid;
    int controlCount = getControllerCount();
    for ( int i = 0; i < controlCount; i++ ){
        const QString controlPath= m_controllers[i]->getPath();
        if ( controlId  == controlPath ){
            valid = m_controllers[i]->getIntensity( frameLow, frameHigh, percentile, &intensity );
            break;
        }
    }
    if ( valid ) {
        result = QString::number( intensity );
    }
    else {
        result = Util::toString( valid );
    }
    return result;
}

QString ViewManager::setClipBuffer( const QString& histogramId, int bufferAmount ){
    QString result = "";
    int histogramCount = getHistogramCount();
    for ( int i = 0; i < histogramCount; i++ ){
        QString histogramPath = getObjectId(Histogram::CLASS_NAME, i);
        if ( histogramId == histogramPath ){
            result = m_histograms[i]->setClipBuffer( bufferAmount );
            break;
        }
    }
    return result;
}

QString ViewManager::setUseClipBuffer( const QString& histogramId, const QString& useBufferStr ){
    QString result = "";
    bool validBool = false;
    bool useBuffer = Util::toBool( useBufferStr, &validBool );
    if ( validBool || useBufferStr.toLower() == "toggle" ) {
        int histogramCount = getHistogramCount();
        for ( int i = 0; i < histogramCount; i++ ){
            QString histogramPath = getObjectId(Histogram::CLASS_NAME, i);
            if ( histogramId == histogramPath ){
                if ( useBufferStr.toLower() == "toggle" ) {
                    bool currentUseBuffer = m_histograms[i]->getUseClipBuffer();
                    useBuffer = !currentUseBuffer;
                }
                result = m_histograms[i]->setUseClipBuffer( useBuffer );
                break;
            }
        }
    }
    else {
        result = "Set clip buffer parameter must be true/false: " + useBufferStr;
    }
    return result;
}

QString ViewManager::setClipRange( const QString& histogramId, double minRange, double maxRange ){
    QString result = "";
    int histogramCount = getHistogramCount();
    for ( int i = 0; i < histogramCount; i++ ){
        QString histogramPath = getObjectId(Histogram::CLASS_NAME, i);
        if ( histogramId == histogramPath ){
            result = m_histograms[i]->setClipRange( minRange, maxRange );
            break;
        }
    }
    return result;
}

QString ViewManager::applyClips( const QString& histogramId, double clipMinValue, double clipMaxValue, QString mode ){
    QString result = "";
    int histogramCount = getHistogramCount();
    for ( int i = 0; i < histogramCount; i++ ){
        QString histogramPath = getObjectId(Histogram::CLASS_NAME, i);
        if ( histogramId == histogramPath ){
            if ( mode == "percent" ) {
                result += m_histograms[i]->setClipMinPercent( clipMinValue );
                result += m_histograms[i]->setClipMaxPercent( clipMaxValue );
            }
            else if ( mode == "intensity" ) {
                result += m_histograms[i]->setClipMin( clipMinValue );
                result += m_histograms[i]->setClipMax( clipMaxValue );
            }
            else {
                result = "invalid mode: " + mode;
                break;
            }
            m_histograms[i]->applyClips();
            break;
        }
    }
    return result;
}

QString ViewManager::setBinCount( const QString& histogramId, int binCount ){
    QString result = "";
    int histogramCount = getHistogramCount();
    for ( int i = 0; i < histogramCount; i++ ){
        QString histogramPath = getObjectId(Histogram::CLASS_NAME, i);
        if ( histogramId == histogramPath ){
            result = m_histograms[i]->setBinCount( binCount );
            break;
        }
    }
    return result;
}

QString ViewManager::setBinWidth( const QString& histogramId, double binWidth ){
    QString result = "";
    int histogramCount = getHistogramCount();
    for ( int i = 0; i < histogramCount; i++ ){
        QString histogramPath = getObjectId(Histogram::CLASS_NAME, i);
        if ( histogramId == histogramPath ){
            result = m_histograms[i]->setBinWidth( binWidth );
            break;
        }
    }
    return result;
}

QString ViewManager::setPlaneMode( const QString& histogramId, const QString& planeMode ){
    QString result = "";
    int histogramCount = getHistogramCount();
    for ( int i = 0; i < histogramCount; i++ ){
        QString histogramPath = getObjectId(Histogram::CLASS_NAME, i);
        if ( histogramId == histogramPath ){
            result = m_histograms[i]->setPlaneMode( planeMode );
            break;
        }
    }
    return result;
}

QString ViewManager::setPlaneRange( const QString& histogramId, double minPlane, double maxPlane ){
    QString result = "";
    int histogramCount = getHistogramCount();
    for ( int i = 0; i < histogramCount; i++ ){
        QString histogramPath = getObjectId(Histogram::CLASS_NAME, i);
        if ( histogramId == histogramPath ){
            result = m_histograms[i]->setPlaneRange( minPlane, maxPlane );
            break;
        }
    }
    return result;
}

QString ViewManager::setChannelUnit( const QString& histogramId, const QString& units ){
    QString result = "";
    int histogramCount = getHistogramCount();
    for ( int i = 0; i < histogramCount; i++ ){
        QString histogramPath = getObjectId(Histogram::CLASS_NAME, i);
        if ( histogramId == histogramPath ){
            result = m_histograms[i]->setChannelUnit( units );
            break;
        }
    }
    return result;
}

QString ViewManager::setGraphStyle( const QString& histogramId, const QString& style ){
    QString result = "";
    int histogramCount = getHistogramCount();
    for ( int i = 0; i < histogramCount; i++ ){
        QString histogramPath = getObjectId(Histogram::CLASS_NAME, i);
        if ( histogramId == histogramPath ){
            result = m_histograms[i]->setGraphStyle( style );
            break;
        }
    }
    return result;
}

QString ViewManager::setLogCount( const QString& histogramId, const QString& logCountStr ){
    QString result = "";
    bool validBool = false;
    bool logCount = Util::toBool( logCountStr, &validBool );
    if ( validBool || logCountStr.toLower() == "toggle" ) {
        int histogramCount = getHistogramCount();
        for ( int i = 0; i < histogramCount; i++ ){
            QString histogramPath = getObjectId(Histogram::CLASS_NAME, i);
            if ( histogramId == histogramPath ){
                if ( logCountStr.toLower() == "toggle" ) {
                    bool currentLogCount = m_histograms[i]->getLogCount();
                    logCount = !currentLogCount;
                }
                result = m_histograms[i]->setLogCount( logCount );
                break;
            }
        }
    }
    else {
        result = "Set log count parameter must be true/false: " + logCountStr;
    }
    return result;
}

QString ViewManager::setColored( const QString& histogramId, const QString& coloredStr ){
    QString result = "";
    bool validBool = false;
    bool colored = Util::toBool( coloredStr, &validBool );
    if ( validBool || coloredStr.toLower() == "toggle" ) {
        int histogramCount = getHistogramCount();
        for ( int i = 0; i < histogramCount; i++ ){
            QString histogramPath = getObjectId(Histogram::CLASS_NAME, i);
            if ( histogramId == histogramPath ){
                if ( coloredStr.toLower() == "toggle" ) {
                    bool currentColored = m_histograms[i]->getColored();
                    colored = !currentColored;
                }
                result = m_histograms[i]->setColored( colored );
                break;
            }
        }
    }
    else {
        result = "Set colored parameter must be true/false: " + coloredStr;
    }
    return result;
}

QString ViewManager::saveHistogram( const QString& histogramId, const QString& filename, int width, int height ){
    QString result = "";
    int histogramCount = getHistogramCount();
    for ( int i = 0; i < histogramCount; i++ ){
        QString histogramPath = getObjectId(Histogram::CLASS_NAME, i);
        if ( histogramId == histogramPath ){
            result = m_histograms[i]->saveHistogram( filename, width, height );
            break;
        }
    }
    return result;
}

QString ViewManager::saveState( const QString& sessionId, const QString& saveName, bool saveLayout, bool savePreferences, bool saveData ){
    QString result;
    ObjectManager* objMan = ObjectManager::objectManager();
    Snapshots* snapshot = dynamic_cast<Snapshots*>(Util::findSingletonObject( Snapshots::CLASS_NAME ));
    if ( saveLayout ){
        //Layout state consists of both the layout and the links.
        QString layoutState = m_layout->getStateString();
        QString linkState =objMan->getStateString( sessionId, LinkableImpl::LINK, SNAPSHOT_LAYOUT);
        StateInterface state( "");
        state.insertObject( Layout::CLASS_NAME, layoutState);
        state.insertObject( LinkableImpl::LINK, linkState );
        QString layoutStr = state.toString();
        result = snapshot->saveLayout( sessionId, saveName, layoutStr);
    }
    if ( savePreferences ){
        QString prefStr = objMan->getStateString( sessionId, Preferences::CLASS_NAME,SNAPSHOT_PREFERENCES );
        result = snapshot->savePreferences( sessionId, saveName, prefStr );
    }
    if ( saveData ){
        QString dataStr = objMan->getStateString( sessionId, "Data", SNAPSHOT_DATA );
        result = snapshot->saveData( sessionId, saveName, dataStr);
    }
    return result;
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
            _pluginsChanged( names, oldNames );
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

