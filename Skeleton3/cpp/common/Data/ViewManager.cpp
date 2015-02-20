#include "Data/ViewManager.h"
#include "Data/Animator.h"
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
    int controlCount = m_controllers.size();
    for ( int i = startIndex; i < controlCount; i++ ){
        m_controllers.pop_back();
    }
}

void ViewManager::_clearAnimators( int startIndex ){
    int animCount = m_animators.size();
    for ( int i = startIndex; i < animCount; i++ ){
        m_animators.pop_back();
    }
}

void ViewManager::_clearColormaps( int startIndex ){
    int colorCount = m_colormaps.size();
    for ( int i = startIndex; i < colorCount; i++ ){
        m_colormaps.pop_back();
    }
}

void ViewManager::_clearHistograms( int startIndex ){
    int histCount = m_histograms.size();
    for ( int i = startIndex; i < histCount; i++ ){
        m_histograms.pop_back();
    }
}

void ViewManager::_clearStatistics( int startIndex ){
    int statCount = m_statistics.size();
    for ( int i = startIndex; i < statCount; i++ ){
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
        result = Util::commandPostProcess( result, "");
        return result;
    });

    //Callback for linking an animator with whatever it is going to animate.
        addCommandCallback( "linkRemove", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = {SOURCE_ID, DEST_ID};
            std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
            QString result = linkRemove( dataValues[SOURCE_ID], dataValues[DEST_ID]);
            result = Util::commandPostProcess( result, "");
            return result;
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

int ViewManager::_findController( const QString& id ) const {
    int controlCount = getControllerCount();
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
    qDebug() << "(JT) ViewManager::linkAdd";
    qDebug() << "sourceId = " << sourceId;
    qDebug() << "destId = " << destId;

    int controlIndex = _findController( destId );
    QString result;
    if ( controlIndex == -1 ){
        result = "Unsupported destination link: "+destId;
    }
    else {
        ObjectManager* objManager = ObjectManager::objectManager();
        QString id = objManager->parseId( sourceId );
        CartaObject* sourceObj = objManager->getObject( id );
        ILinkable* linkSource = dynamic_cast<ILinkable*>( sourceObj );
        if ( linkSource != nullptr ){
            bool linked = linkSource->addLink( m_controllers[controlIndex]);
            if ( !linked ){
                result = "Could not link source to destination.";
            }
        }
        else {
            result = "Unrecognized link source: "+sourceId;
        }
    }
    qDebug() << "(JT) ViewManager::linkAdd result: " << result;
    return result;
}

QString ViewManager::linkRemove( const QString& sourceId, const QString& destId ){
    int controlIndex = _findController( destId );
    QString result;
    if ( controlIndex == -1 ){
        result = "Unsupported destination link: "+destId;
    }
    else {
        ObjectManager* objManager = ObjectManager::objectManager();
        QString id = objManager->parseId( sourceId );
        CartaObject* sourceObj = objManager->getObject( id );
        ILinkable* linkSource = dynamic_cast<ILinkable*>( sourceObj );
        if ( linkSource != nullptr ){
            bool unlinked = linkSource->removeLink( m_controllers[controlIndex]);
            if ( !unlinked ){
                result = "Could not remove link between source and destination.";
            }
            else {
                result = "Unrecognized link source: "+sourceId;
            }
        }
    }
    return result;
}

QString ViewManager::getObjectId( const QString& plugin, int index ){
    QString viewId("");
    if ( plugin == Controller::PLUGIN_NAME ){
        if ( 0 <= index && index < getControllerCount()){
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
        if ( 0 <= index && index < getAnimatorCount()){
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
        if ( 0 <= index && index < getColorMapCount()){
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
        if ( 0 <= index && index < getHistogramCount()){
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
        if ( 0 <= index && index < getStatisticsCount()){
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

QString ViewManager::_makeAnimator( int maxCount ){
    int currentCount = m_animators.size();
    if ( currentCount < maxCount ){
        for ( int i = currentCount; i < maxCount; i++ ){
            CartaObject* animObj = Util::createObject( Animator::CLASS_NAME );
            std::shared_ptr<Animator> animator( dynamic_cast<Animator*>(animObj) );
            m_animators.append(animator);
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
            std::shared_ptr<Colormap> target( dynamic_cast<Colormap*>(controlObj) );
            m_colormaps.append(target);
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
            std::shared_ptr<Controller> target( dynamic_cast<Controller*>(controlObj) );
            m_controllers.append(target);
        }
    }
    return m_controllers[maxCount-1]->getPath();
}

void ViewManager::_makeDataLoader(){
    if ( m_dataLoader == nullptr ){
        CartaObject* dataLoaderObj = Util::createObject( DataLoader::CLASS_NAME );
        m_dataLoader.reset( dynamic_cast<DataLoader*>( dataLoaderObj ));
    }
}

QString ViewManager::_makeHistogram( int maxCount ){
    int currentCount = m_histograms.size();
    if ( currentCount < maxCount ){
        for ( int i = currentCount; i < maxCount; i++ ){
            CartaObject* controlObj = Util::createObject( Histogram::CLASS_NAME );
            std::shared_ptr<Histogram> target( dynamic_cast<Histogram*>(controlObj) );
            m_histograms.append(target);
        }
    }
    return m_histograms[maxCount - 1]->getPath();
}

QString ViewManager::_makeLayout(){
    if ( !m_layout ){
        CartaObject* layoutObj = Util::createObject( Layout::CLASS_NAME );
        m_layout.reset( dynamic_cast<Layout*>(layoutObj ));
    }
    QString layoutPath = m_layout->getPath();
    return layoutPath;
}

QString ViewManager::_makePluginList(){
    if ( !m_pluginsLoaded ){
        //Initialize a view showing the plugins that have been loaded
        CartaObject* pluginsObj = Util::createObject( ViewPlugins::CLASS_NAME );
        m_pluginsLoaded.reset( dynamic_cast<ViewPlugins*>(pluginsObj ));
    }
    QString pluginsPath = m_pluginsLoaded->getPath();
    return pluginsPath;
}

QString ViewManager::_makeStatistics( int maxCount ){
    int currentCount = m_statistics.size();
    if (currentCount < maxCount ){
        for ( int i = currentCount; i < maxCount; i++ ){
            CartaObject* controlObj = Util::createObject( Statistics::CLASS_NAME );
            std::shared_ptr<Statistics> target( dynamic_cast<Statistics*>(controlObj) );
            m_statistics.append(target);
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

bool ViewManager::reverseColorMap( const QString& colormapId, const QString& trueOrFalse ){
    int colormapIndex = _findColorMap( colormapId );
    bool colorMapFound = false;
    if ( colormapIndex >= 0 ){
        m_colormaps[colormapIndex]->reverseColorMap( trueOrFalse );
        colorMapFound = true;
    }
    return colorMapFound;
}

bool ViewManager::invertColorMap( const QString& colormapId, const QString& trueOrFalse ){
    int colormapIndex = _findColorMap( colormapId );
    bool colorMapFound = false;
    if ( colormapIndex >= 0 ){
        m_colormaps[colormapIndex]->invertColorMap( trueOrFalse );
        colorMapFound = true;
    }
    return colorMapFound;
}

bool ViewManager::setFrame( const QString& animatorId, const QString& index ){
    int animatorIndex = _findAnimator( animatorId );
    bool animatorFound = false;
    if ( animatorIndex >= 0 ){
        m_animators[animatorIndex]->changeChannelIndex( index );
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
    qDebug() << "(JT) ViewManager::setCustomView() result = " << result;
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

void ViewManager::setPlugins( const QStringList& names ){
    if ( m_layout == nullptr ){
        _makeLayout();
    }
    m_layout->setPlugins( names );
}

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

bool ViewManager::saveState( const QString& saveName ){
    bool result = _saveState( saveName );
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

bool ViewManager::_saveState( const QString& saveName ){
    QString filePath = getStateLocation( saveName );
    StateWriter writer( filePath );
    writer.addPathData( m_layout->getPath(), m_layout->getStateString());
    for ( int i = 0; i < getControllerCount(); i++ ){
        writer.addPathData( m_controllers[i]->getPath(), m_controllers[i]->getStateString() );
    }
    for ( int i = 0; i < getAnimatorCount(); i++ ){
        writer.addPathData( m_animators[i]->getPath(), m_animators[i]->getStateString() );
    }
    bool stateSaved = writer.saveState();
    return stateSaved;
}
}
}

