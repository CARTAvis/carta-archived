#include "Data/ViewManager.h"
#include "Data/Animator.h"
#include "Data/Colormap.h"
#include "Data/Colormaps.h"
#include "Data/Controller.h"
#include "Data/DataLoader.h"
#include "Data/ErrorManager.h"
#include "Data/Histogram.h"
#include "Data/Layout.h"
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
bool ViewManager::m_registered =
    ObjectManager::objectManager()->registerClass ( CLASS_NAME,
                                                   new ViewManager::Factory());

ViewManager::ViewManager( const QString& path, const QString& id)
    : CartaObject( CLASS_NAME, path, id ),
      m_layout( nullptr ),
      m_dataLoader( nullptr ),
      m_pluginsLoaded( nullptr ){
    Util::findSingletonObject( Colormaps::CLASS_NAME );
    Util::findSingletonObject( ErrorManager::CLASS_NAME );
    _initCallbacks();

    bool stateRead = this->_readState( "DefaultState" );
    if ( !stateRead ){
        _initializeDefaultState();
    }
    _makeDataLoader();
}

void ViewManager::_clear(){
    int controlCount = m_controllers.size();
    for ( int i = 0; i < controlCount; i++ ){
        m_controllers[i]->clear();
    }
    m_controllers.clear();
    int animCount = m_animators.size();
    for ( int i = 0; i < animCount; i++ ){
        m_animators[i]->clear();
    }
    m_animators.clear();
    int colorCount = m_colormaps.size();
    for ( int i = 0; i < colorCount; i++ ){
        m_colormaps[i]->clear();
    }
    m_colormaps.clear();
    int histCount = m_histograms.size();
    for ( int i = 0; i < histCount; i++ ){
        m_histograms[i]->clear();
    }
    m_histograms.clear();
    if ( m_layout != nullptr ){
        m_layout->clear();
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
    addCommandCallback( "linkAnimator", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        const QString ANIM_ID( "animId");
        const QString WIN_ID( "winId");
        std::set<QString> keys = {ANIM_ID, WIN_ID};
        std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
        //Go through our data animators and find the one that is supposed to
        //be hooked up to.
        for (int i = 0; i < m_animators.size(); i++ ){
            if ( m_animators[i]->getPath()  == dataValues[ANIM_ID] ){
                //Hook up the corresponding controller
                for ( int j = 0; j < m_controllers.size(); j++ ){
                    if ( m_controllers[j]->getPath() == dataValues[WIN_ID] ){
                        m_animators[i]->addController( m_controllers[j]);
                        break;
                    }
                }
                break;
            }
        }
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

QString ViewManager::getObjectId( const QString& plugin, int index ){
    QString viewId("");
    if ( plugin == Controller::PLUGIN_NAME ){
        if ( 0 <= index && index < m_controllers.size()){
            viewId = m_controllers[index]->getPath();
        }
        else {
            viewId = _makeController();
        }
    }
    else if ( plugin == Animator::CLASS_NAME ){
        if ( 0 <= index && index < m_animators.size()){
            viewId = m_animators[index]->getPath();
        }
        else {
            viewId = _makeAnimator();
        }
    }
    else if ( plugin == Colormap::CLASS_NAME ){
        if ( 0 <= index && index < m_colormaps.size()){
            viewId = m_colormaps[index]->getPath();
        }
        else {
            viewId = _makeColorMap();
        }
    }
    else if ( plugin == Histogram::CLASS_NAME ){
        if ( 0 <= index && index < m_histograms.size()){
            viewId = m_histograms[index]->getPath();
        }
        else {
            viewId = _makeHistogram();
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

void ViewManager::_initializeExistingAnimationLinks( int index ){
    int linkCount = m_animators[index]->getLinkCount();
    for ( int i = 0; i < linkCount; i++ ){
        QString controllerId = m_animators[index]->getLinkId( i );
        for ( int j = 0; j < m_controllers.size(); j++ ){
            if ( controllerId == m_controllers[j]->getPath()){
                m_animators[index]->addController( m_controllers[j] );
                break;
            }
            else {
                qDebug() << "Register view could not find controller id="<<controllerId;
            }
        }
    }
}

bool ViewManager::linkColoredView( const QString& colorId, const QString& controlId ){
    int controlIndex = _findController( controlId );
    if ( controlIndex == -1 ){
        qWarning() << "Linker invalid controlId="<<controlId;
        return false;
    }
    int colorIndex = _findColorMap( colorId );
    if ( colorIndex == -1 ){
        qWarning() << "Linker invalid colorId="<<colorId;
        return false;
    }
    bool linked = _linkColoredView(m_colormaps[colorIndex], m_controllers[controlIndex]);
    return linked;
}

bool ViewManager::_linkColoredView( std::shared_ptr<Colormap> colorMap, std::shared_ptr<Controller> controller ){
    return colorMap->addController( controller );
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



QString ViewManager::_makeAnimator(){
    CartaObject* animObj = Util::createObject( Animator::CLASS_NAME );
    std::shared_ptr<Animator> target( dynamic_cast<Animator*>(animObj) );
    m_animators.append(target);
    int lastIndex = m_animators.size() - 1;
    _initializeExistingAnimationLinks( lastIndex );
    return target->getPath();
}

QString ViewManager::_makeColorMap(){
   CartaObject* controlObj = Util::createObject( Colormap::CLASS_NAME );
   std::shared_ptr<Colormap> target( dynamic_cast<Colormap*>(controlObj) );
   m_colormaps.append(target);
   return target->getPath();
}

QString ViewManager::_makeController(){
    CartaObject* controlObj = Util::createObject( Controller::CLASS_NAME );
    std::shared_ptr<Controller> target( dynamic_cast<Controller*>(controlObj) );
    m_controllers.append(target);
    return target->getPath();
}

void ViewManager::_makeDataLoader(){
    if ( m_dataLoader == nullptr ){
        CartaObject* dataLoaderObj = Util::createObject( DataLoader::CLASS_NAME );
        m_dataLoader.reset( dynamic_cast<DataLoader*>( dataLoaderObj ));
    }
}

QString ViewManager::_makeHistogram(){
    CartaObject* controlObj = Util::createObject( Histogram::CLASS_NAME );
    std::shared_ptr<Histogram> target( dynamic_cast<Histogram*>(controlObj) );
    m_histograms.append(target);
    return target->getPath();
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



bool ViewManager::_readState( const QString& saveName ){
    _clear();
    QString fullLocation = getStateLocation( saveName );
    StateReader reader( fullLocation );
    bool successfulRead = reader.restoreState();
    if ( successfulRead ){

        //Make the controllers specified in the state.
        QList<std::pair<QString,QString> > controllerStates = reader.getViews(Controller::CLASS_NAME);
        for ( std::pair<QString,QString> state : controllerStates ){
            _makeController();
            m_controllers[m_controllers.size() - 1]->resetState( state.second );
        }

        //Make the animators specified in the state.
        QList< std::pair<QString,QString> > animatorStates = reader.getViews(Animator::CLASS_NAME);
        for ( std::pair<QString,QString> state : animatorStates ){
            _makeAnimator();
            int animIndex = m_animators.size() - 1;
            m_animators[ animIndex ]->resetState( state.second );

            //Now see if this animator needs to be linked to any of the controllers
            QList<QString> oldLinks = m_animators[ animIndex ]-> getLinks();
            m_animators[animIndex ]->clear();
            for ( int i = 0;  i < controllerStates.size(); i++ ){
                if ( oldLinks.contains( StateInterface::DELIMITER + controllerStates[i].first ) ){
                    m_animators[animIndex]->addController( m_controllers[i]);
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
    _clear();
    if ( m_layout == nullptr ){
        _makeLayout();
    }
    m_layout->setLayoutAnalysis();

    //Create the view objects
    _makeAnimator();
    _makeController();
    _makeHistogram();
    _makeColorMap();

    //Add the links to establish reasonable defaults.
    m_animators[0]->addController( m_controllers[0]);
    m_histograms[0]->addController( m_controllers[0]);
    linkColoredView( m_colormaps[0]->getPath(), m_controllers[0]->getPath());
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

void ViewManager::setImageView(){
    _clear();
    if ( m_layout == nullptr ){
        _makeLayout();
    }
    m_layout->setLayoutImage();
    _makeController();
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
}
}

