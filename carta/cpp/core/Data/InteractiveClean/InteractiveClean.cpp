
#include "InteractiveClean.h"
#include "Data/Settings.h"
#include "Data/LinkableImpl.h"
#include "Data/Image/Controller.h"
#include "Data/Image/DataSource.h"
#include "Data/Region/Region.h"
#include "Data/Region/RegionControls.h"
#include "Data/Error/ErrorManager.h"
#include "Data/Util.h"
#include "CartaLib/Regions/IRegion.h"
#include "CartaLib/Hooks/ImageStatisticsHook.h"
#include "State/UtilState.h"
#include "Globals.h"
#include <QDebug>


namespace Carta {

namespace Data {

const QString InteractiveClean::CLASS_NAME = "InteractiveClean";
const QString InteractiveClean::PARAMETERS = "cleanParameters";

class InteractiveClean::Factory : public Carta::State::CartaObjectFactory {
public:

    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new InteractiveClean (path, id);
    }
};

bool InteractiveClean::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new InteractiveClean::Factory());

using Carta::State::UtilState;
using Carta::State::StateInterface;

InteractiveClean::InteractiveClean( const QString& path, const QString& id):
            CartaObject( CLASS_NAME, path, id ),
            m_linkImpl( new LinkableImpl( path )),
            //Store region and image selection
            m_stateData( UtilState::getLookup(path, StateInterface::STATE_DATA )){

    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    Settings* settingsObj = objMan->createObject<Settings>();
    m_settings.reset( settingsObj );

    _initializeDefaultState();
    _initializeCallbacks();
}


QString InteractiveClean::addLink( CartaObject*  target){
    Controller* controller = dynamic_cast<Controller*>(target);
    bool linkAdded = false;
    QString result;
    if ( controller != nullptr ){
        if ( m_controllerLinked ){
            //See if it is the one we are already linked to.
            CartaObject* obj = m_linkImpl->searchLinks( target->getPath());
            if ( obj != nullptr ){
                linkAdded = true;
            }
            else {
                //Clear out any existing link so we can add the new one.
                _clearLinks();
            }
        }
        if ( !linkAdded ){
            linkAdded = m_linkImpl->addLink( controller );
            if ( linkAdded ){
                connect(controller, SIGNAL(dataChanged(Controller*)),
                        this , SLOT(_updateInteractiveClean(Controller*)));
                connect( controller, SIGNAL( frameChanged(Controller*, Carta::Lib::AxisInfo::KnownType)),
                        this, SLOT(_updateInteractiveClean(Controller*, Carta::Lib::AxisInfo::KnownType)));
                connect(controller, SIGNAL(dataChangedRegion(Controller*)),
                        this, SLOT( _updateInteractiveClean( Controller*)));
                m_controllerLinked = true;
                _updateInteractiveClean( controller, Carta::Lib::AxisInfo::KnownType::OTHER );
            }
        }
    }
    else {
        result = "InteractiveClean only supports linking to images";
    }
    return result;
}

void InteractiveClean::_clearLinks(){
    int linkCount = m_linkImpl->getLinkCount();
    for ( int i = linkCount-1; i >= 0; i-- ){
        CartaObject* linkObj = m_linkImpl->getLink(i);
        removeLink( linkObj );
    }
}


QList<QString> InteractiveClean::getLinks() const {
    return m_linkImpl->getLinkIds();
}

QString InteractiveClean::_getPreferencesId() const {
    QString id;
    if ( m_settings.get() != nullptr ){
        id = m_settings->getPath();
    }
    return id;
}

QString InteractiveClean::getInitialParameters( ){
    m_stateData.fetchState();
    QString result = m_stateData.toString("parameters");
    return result;
}

QString InteractiveClean::getStateString( const QString& sessionId, SnapshotType type ) const{
    QString result("");
    if ( type == SNAPSHOT_PREFERENCES ){
        StateInterface prefState( "");
        prefState.setValue<QString>(Carta::State::StateInterface::OBJECT_TYPE, CLASS_NAME );

        prefState.insertValue<QString>(Util::PREFERENCES, m_state.toString());
        prefState.insertValue<QString>( Settings::SETTINGS, m_settings->getStateString(sessionId, type) );
        result = prefState.toString();
    }
    else if ( type == SNAPSHOT_LAYOUT ){
        result = m_linkImpl->getStateString(getIndex(), getSnapType( type ));
    }
    return result;
}


void InteractiveClean::_initializeCallbacks(){

  addCommandCallback( "getInteractiveClean", [=] (const QString & /*cmd*/,
                                                    const QString & params, const QString & /*sessionId*/) -> QString {
                        qDebug() << "get Interactive Clean";
                        _readInitialParameters();
                        QString result = getInitialParameters();
                        return result;
                      });

  addCommandCallback( "performCleanAction", [=] (const QString & /*cmd*/,
                                                    const QString & params, const QString & /*sessionId*/) -> QString {
                        qDebug() << "params: " << params;
                        QString result("clean action is: "+params);
                        return result;
                      });
    addCommandCallback( "performMaskAction", [=] (const QString & /*cmd*/,
                                                    const QString & params, const QString & /*sessionId*/) -> QString {
                          std::set<QString> keys = {"button"};
                          //                          std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                          //                          QString maskStr = dataValues[*keys.begin()];
                          qDebug() << "params: " << params;
                          QString result("mask action is: "+params);
                          return result;
     });

    addCommandCallback( "registerPreferences", [=] (const QString & /*cmd*/,
                                                    const QString & /*params*/, const QString & /*sessionId*/) -> QString {
                          QString result = _getPreferencesId();
                          return result;
     });


}



void InteractiveClean::_initializeDefaultState(){
  /**
   * List of clean parameters is static and so size of array can be set to 8 at initialization:
   *
   */
    //m_stateData.insertValue<int>(SELECTED_INDEX, 0 );
    m_stateData.insertArray( PARAMETERS, 8 );
    m_stateData.insertObject( "parameters" );
    m_stateData.flushState();

    m_state.flushState();
}


bool InteractiveClean::isLinked( const QString& linkId ) const {
    bool linked = false;
    CartaObject* obj = m_linkImpl->searchLinks( linkId );
    if ( obj != nullptr ){
        linked = true;
    }
    return linked;
}


QString InteractiveClean::removeLink( CartaObject* cartaObject){
    bool removed = false;
    QString result;
    Controller* controller = dynamic_cast<Controller*>( cartaObject );
    if ( controller != nullptr ){
        removed = m_linkImpl->removeLink( controller );
        if ( removed ){
            controller->disconnect(this);
            m_controllerLinked = false;
        }
    }
    else {
       result = "InteractiveClean was unable to remove link only image links are supported";
    }
    return result;
}

void InteractiveClean::resetState( const QString& state ){
    StateInterface restoredState( "");
    restoredState.setState( state );

    QString settingStr = restoredState.getValue<QString>(Settings::SETTINGS);
    m_settings->resetStateString( settingStr );

    QString prefStr = restoredState.getValue<QString>(Util::PREFERENCES);
    m_state.setState( prefStr );
    m_state.setValue<bool>( "flush", true );
    m_state.flushState();
    m_state.setValue<bool>("flush", false );
}


void InteractiveClean::_updateInteractiveClean( Controller* controller, Carta::Lib::AxisInfo::KnownType /*type*/  ){
    if ( controller != nullptr ){

        // int selectedIndex = controller->getSelectImageIndex();
        // m_stateData.setValue<int>(SELECTED_INDEX, selectedIndex );

        // std::vector< std::shared_ptr<Carta::Lib::Image::ImageInterface> > dataSources =
        //         controller->getImages();


        // std::vector<std::shared_ptr<Region> > coreRegions;
        // std::shared_ptr<RegionControls> regionControls = controller->getRegionControls();
        // if ( regionControls ){
        // 	coreRegions = regionControls->getRegions();
        // }
        // int regionCount = coreRegions.size();
        // std::vector<std::shared_ptr<Carta::Lib::Regions::RegionBase> > regions;
        // for ( int i = 0; i < regionCount; i++ ){
        //     regions.push_back( coreRegions[i]->getModel() );
        // }

        // std::vector<int> frameIndices = controller->getImageSlice();

        // int sourceCount = dataSources.size();
        // if ( sourceCount > 0 ){
        //     auto result = Globals::instance()-> pluginManager()
        //                  -> prepare <Carta::Lib::Hooks::ImageInteractiveCleanHook>(dataSources, regions, frameIndices);
        //     auto lam = [=] ( const Carta::Lib::Hooks::ImageInteractiveCleanHook::ResultType &data ) {

        //         //An array for each image
        //         int dataCount = data.size();
        //         m_stateData.resizeArray( STATS, dataCount );
        //         for ( int i = 0; i < dataCount; i++ ){
        //             //Each element of the image array contains an array of interactive clean.
        //             QString arrayLookup = UtilState::getLookup( STATS, i );
        //             int statCount = data[i].size();
        //             m_stateData.setArray( arrayLookup, statCount );

        //             //Go through each set of interactive clean for the image.
        //             for ( int k = 0; k < statCount; k++ ){
        //                 QString objLookup = UtilState::getLookup( arrayLookup, k );
        //                 QList<QString> existingKeys = m_stateData.getMemberNames( objLookup );
        //                 int keyCount = data[i][k].size();
        //                 for ( int j = 0; j < keyCount; j++ ){
        //                     QString label = data[i][k][j].getLabel();
        //                     QString lookup = UtilState::getLookup( objLookup, label );
        //                     m_stateData.insertValue<QString>( lookup, data[i][k][j].getValue() );
        //                 }
        //             }
        //         }
        //         m_stateData.flushState();
        //     };
        //     try {
        //         result.forEach( lam );
        //     }
        //     catch( char*& error ){
        //         QString errorStr( error );
        //         ErrorManager* hr = Util::findSingletonObject<ErrorManager>();
        //         hr->registerError( errorStr );
        //     }
        // }
        // //No interactive clean
        // else {
        //     m_stateData.resizeArray( STATS, 0 );
        //     m_stateData.flushState();
        // }

    }
}

void InteractiveClean::_readInitialParameters(){

  /**
    TO-DO: Connect to interactive clean session, read parameters and store them in the state.
  */

  int niter = 80;
  QString threshold = "0.12mJy";
  int cycleNiter = 30;
  int interactiveNiter = 5;
  QString cycleThreshold = "0.05";
  QString interactiveThreshold = "0.01";
  int cycleFactor = 3;
  double loopGain = 1;

  QString pars("{\"niter\":"+QString::number(niter)+
               ",\"threshold\":\""+threshold+"\""+
               ",\"cycleNiter\":"+QString::number(cycleNiter)+
               ",\"interactiveNiter\":"+QString::number(interactiveNiter)+
               ",\"cycleThreshold\":"+cycleThreshold+
               ",\"interactiveThreshold\":"+interactiveThreshold+
               ",\"cycleFactor\":"+QString::number(cycleFactor)+
               ",\"loopGain\":"+QString::number(loopGain)+"}");

  m_stateData.setObject("parameters",pars);
  m_stateData.flushState();

}


InteractiveClean::~InteractiveClean(){

}
}
}
