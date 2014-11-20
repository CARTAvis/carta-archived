#include "Data/ViewManager.h"
#include "Data/Animator.h"
#include "Data/Colormap.h"
#include "Data/Controller.h"
#include "Data/DataLoader.h"
#include "Data/Layout.h"
#include "Data/ViewPlugins.h"
#include "Data/Util.h"
#include "State/StateReader.h"
#include "State/StateWriter.h"

#include <QDir>
#include <QDebug>

const QString ViewManager::CLASS_NAME = "ViewManager";
bool ViewManager::m_registered =
    ObjectManager::objectManager()->registerClass ( CLASS_NAME,
                                                   new ViewManager::Factory());

ViewManager::ViewManager( const QString& path, const QString& id)
    : CartaObject( CLASS_NAME, path, id ),
      m_layout( nullptr ),
      m_dataLoader( nullptr ),
      m_pluginsLoaded( nullptr ){
    _initCallbacks();
    _initializeDataLoader();

    bool stateRead = this->_readState( "DefaultState" );
    if ( !stateRead ){
        _initializeDefaultState();
    }
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
        QList<QString> keys = {"id", "data"};
        QVector<QString> dataValues = Util::parseParamMap( params, keys );
        if ( dataValues.size() == keys.size()){
            for ( int i = 0; i < m_controllers.size(); i++ ){
                if ( dataValues[0]  == m_controllers[i]->getPath() ){
                    //Add the data to it.
                    QString path = dataValues[1];
                    _initializeDataLoader();
                    path = m_dataLoader->getFile( path, sessionId );
                    m_controllers[i]->addData( path );
                    break;
                }
            }
        }
        return "";
    });

    //Callback for registering a view.
    addCommandCallback( "registerView", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QList<QString> keys = {"pluginId", "index"};
        QVector<QString> dataValues = Util::parseParamMap( params, keys );
        QString viewId( "" );
        if ( dataValues.size() == keys.size()){
            viewId = _makeWindow( dataValues );
        }
        return viewId;
    });

    //Callback for linking an animator with whatever it is going to animate.
    addCommandCallback( "linkAnimator", [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) -> QString {
        QList<QString> keys = {"animId", "winId"};
        QVector<QString> dataValues = Util::parseParamMap( params, keys );
        if ( dataValues.size() == keys.size()){
            //Go through our data animators and find the one that is supposed to
            //be hooked up to.
            for (int i = 0; i < m_animators.size(); i++ ){
                if ( m_animators[i]->getPath()  == dataValues[0] ){
                    //Hook up the corresponding controller
                    for ( int j = 0; j < m_controllers.size(); j++ ){
                        if ( m_controllers[j]->getPath() == dataValues[1] ){
                            m_animators[i]->addController( m_controllers[j]);
                            break;
                        }
                    }
                    break;
                }
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


void ViewManager::_initializeDataLoader(){
    if ( !m_dataLoader ){
        ObjectManager* objManager = ObjectManager::objectManager();
        QString dataLoaderId = objManager->createObject( DataLoader::CLASS_NAME );
        CartaObject* dataLoaderObj = objManager->getObject( dataLoaderId );
        m_dataLoader.reset( dynamic_cast<DataLoader*>( dataLoaderObj ));
    }
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
    ObjectManager* objManager = ObjectManager::objectManager();
    QString viewId = objManager->createObject( Animator::CLASS_NAME );
    CartaObject* animObj = objManager->getObject( viewId );
    std::shared_ptr<Animator> target( dynamic_cast<Animator*>(animObj) );
    m_animators.append(target);
    int lastIndex = m_animators.size() - 1;
    _initializeExistingAnimationLinks( lastIndex );
    return target->getPath();
}


QString ViewManager::_makeController(){
    ObjectManager* objManager = ObjectManager::objectManager();
    QString viewId = objManager->createObject( Controller::CLASS_NAME );
    CartaObject* controlObj = objManager->getObject( viewId );
    std::shared_ptr<Controller> target( dynamic_cast<Controller*>(controlObj) );
    m_controllers.append(target);
    return target->getPath();
}

QString ViewManager::_makeLayout(){
    if ( !m_layout ){
        ObjectManager* objManager = ObjectManager::objectManager();
        QString layoutId = objManager->createObject( Layout::CLASS_NAME );
        CartaObject* layoutObj = objManager->getObject( layoutId );
        m_layout.reset( dynamic_cast<Layout*>(layoutObj ));
    }
    QString layoutPath = m_layout->getPath();
    return layoutPath;
}

QString ViewManager::_makePluginList(){
    if ( !m_pluginsLoaded ){
        //Initialize a view showing the plugins that have been loaded
        ObjectManager* objManager = ObjectManager::objectManager();
        QString pluginsId = objManager->createObject( ViewPlugins::CLASS_NAME );
        CartaObject* pluginsObj = objManager->getObject( pluginsId );
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
    else if ( dataValues[0] == ViewPlugins::CLASS_NAME ){
        viewId = _makePluginList();
    }
    /*else if ( dataValues[0] == Colormap::CLASS_NAME ){
        ObjectManager* objManager = ObjectManager::objectManager();
        QString id = objManager->createObject( Colormap::CLASS_NAME );
        CartaObject* controlObj = objManager->getObject( id );
        viewId = controlObj->getPath();
        qDebug() << "made colormap path="<<viewId;
    }*/
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

