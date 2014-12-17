#include "Data/ViewManager.h"
#include "Data/Animator.h"
#include "Data/Colormap.h"
#include "Data/Colormaps.h"
#include "Data/Controller.h"
#include "Data/DataLoader.h"
#include "Data/Histogram.h"
#include "Data/Layout.h"
#include "Data/ViewPlugins.h"
#include "Data/Util.h"
#include "State/StateReader.h"
#include "State/StateWriter.h"

#include <QDir>
#include <QDebug>

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
    _initCallbacks();

    bool stateRead = this->_readState( "DefaultState" );
    if ( !stateRead ){
        _initializeDefaultState();
    }
    _makeDataLoader();
}

void ViewManager::_clearLayout(){
    int controlCount = m_controllers.size();
    for ( int i = 0; i < controlCount; i++ ){
        m_controllers[i]->clear();
    }
    m_controllers.clear();
    m_animators.clear();
    if ( m_layout != nullptr ){
        m_layout->clear();
    }
}

void ViewManager::_initCallbacks(){
    addCommandCallback( "clearLayout", [=] (const QString & /*cmd*/,
                const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        _clearLayout();
        return "";
    });

    //Callback for adding a data source to a Controller.
    addCommandCallback( "dataLoaded", [=] (const QString & /*cmd*/,
            const QString & params, const QString & sessionId) -> QString {
        const QString ID( "id");
        const QString DATA( "data");
        std::set<QString> keys = {ID,DATA};
        std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
        for ( int i = 0; i < m_controllers.size(); i++ ){
            if ( dataValues[ID]  == m_controllers[i]->getPath() ){
                //Add the data to it.
                QString path = dataValues[DATA];
                path = m_dataLoader->getFile( path, sessionId );
                m_controllers[i]->addData( path );
                break;
            }
        }
        return "";
    });

    //Callback for registering a view.
    addCommandCallback( "registerView", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        const QString PLUGIN_ID( "pluginId");
        const QString INDEX( "index");
        std::set<QString> keys = {PLUGIN_ID, INDEX};
        std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
        QVector<QString> dataVector( keys.size() );
        dataVector[0] = dataValues[PLUGIN_ID];
        dataVector[1] = dataValues[INDEX];
        QString viewId = _makeWindow( dataVector );
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

    _makeAnimator();
    _makeController();
    m_animators[0]->addController( m_controllers[0]);
    _makeLayout();
    _makePluginList();

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

void ViewManager::loadFile( QString fileName ){
    if ( m_controllers.size() == 0 ){
        _makeController();
    }
    m_controllers[0]->addData( fileName );
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

QString ViewManager::_makeWindow( QVector<QString>& dataValues ){
    QString viewId("");
    if ( dataValues[0] == "CasaImageLoader"){
        bool validIndex = false;
        int dataIndex = dataValues[1].toInt( &validIndex );
        if ( validIndex  && 0 <= dataIndex && dataIndex < m_controllers.size()){
            viewId = m_controllers[dataIndex]->getPath();
        }
        else {
            viewId = _makeController();
        }
    }
    else if ( dataValues[0] == Animator::CLASS_NAME ){
        bool validIndex = false;
        int animIndex = dataValues[1].toInt( &validIndex );
        if ( validIndex && 0 <= animIndex && animIndex < m_animators.size()){
            viewId = m_animators[animIndex]->getPath();
        }
        else {
            viewId = _makeAnimator();
        }
    }
    else if ( dataValues[0] == Colormap::CLASS_NAME ){
        bool validIndex = false;
        int colorIndex = dataValues[1].toInt( &validIndex );
        if ( validIndex && 0 <= colorIndex && colorIndex < m_colormaps.size()){
            viewId = m_colormaps[colorIndex]->getPath();
        }
        else {
            viewId = _makeColorMap();
        }
    }
    else if ( dataValues[0] == Histogram::CLASS_NAME ){
        bool validIndex = false;
        int histIndex = dataValues[1].toInt( &validIndex );
        if ( validIndex && 0 <= histIndex && histIndex < m_histograms.size()){
            viewId = m_histograms[histIndex]->getPath();
        }
        else {
            viewId = _makeHistogram();
        }
    }
    else if ( dataValues[0] == ViewPlugins::CLASS_NAME ){
        viewId = _makePluginList();
    }
    else {
        qDebug() << "Unrecognized top level window type: "<<dataValues[0];
    }
    return viewId;
}

bool ViewManager::_readState( const QString& saveName ){
    _clearLayout();
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
            m_animators[animIndex ]->clearLinks();
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

