#include "Statistics.h"
#include "Data/Settings.h"
#include "Data/LinkableImpl.h"
#include "Data/Image/Controller.h"
#include "Data/Image/DataSource.h"
#include "Data/Error/ErrorManager.h"
#include "Data/Util.h"

#include "CartaLib/Hooks/ImageStatisticsHook.h"
#include "CartaLib/RegionInfo.h"

#include "State/UtilState.h"

#include <QDebug>

#include "Globals.h"

namespace Carta {

namespace Data {

const QString Statistics::CLASS_NAME = "Statistics";
const QString Statistics::STATS = "stats";
const QString Statistics::SHOW_STATS_IMAGE = "showStatsImage";
const QString Statistics::SHOW_STATS_REGION = "showStatsRegion";
const QString Statistics::SELECTED_INDEX = "selectedIndex";

class Statistics::Factory : public Carta::State::CartaObjectFactory {
public:

    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new Statistics (path, id);
    }
};

bool Statistics::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new Statistics::Factory());

using Carta::State::UtilState;
using Carta::State::StateInterface;

Statistics::Statistics( const QString& path, const QString& id):
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


QString Statistics::addLink( CartaObject*  target){
    Controller* controller = dynamic_cast<Controller*>(target);
    bool linkAdded = false;
    QString result;
    if ( controller != nullptr ){
        if ( !m_controllerLinked ){
            linkAdded = m_linkImpl->addLink( controller );
            if ( linkAdded ){
                connect(controller, SIGNAL(dataChanged(Controller*)),
                        this , SLOT(_updateStatistics(Controller*)));
                connect(controller, SIGNAL(dataChangedRegion(Controller*)),
                        this, SLOT( _updateStatistics( Controller*)));
                m_controllerLinked = true;
                _updateStatistics( controller );
            }
        }
        else {
            CartaObject* obj = m_linkImpl->searchLinks( target->getPath());
            if ( obj != nullptr ){
                linkAdded = true;
            }
            else {
                result = "Statistics only supports linking to a single image source.";
            }
        }
    }
    else {
        result = "Statistics only supports linking to images";
    }
    return result;
}


QList<QString> Statistics::getLinks() const {
    return m_linkImpl->getLinkIds();
}

QString Statistics::_getPreferencesId() const {
    QString id;
    if ( m_settings.get() != nullptr ){
        id = m_settings->getPath();
    }
    return id;
}

void Statistics::_initializeCallbacks(){
    addCommandCallback( "registerPreferences", [=] (const QString & /*cmd*/,
                                const QString & /*params*/, const QString & /*sessionId*/) -> QString {
            QString result = _getPreferencesId();
            return result;
       });

    addCommandCallback( "setShowStatsImage", [=] (const QString & /*cmd*/,
                                    const QString & params, const QString & /*sessionId*/) -> QString {
                std::set<QString> keys = {Util::VISIBLE};
                std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                QString showStatsStr = dataValues[*keys.begin()];
                bool validBool = false;
                bool showStats = Util::toBool( showStatsStr, &validBool );
                QString result;
                if ( validBool ){
                    setShowStatsImage( showStats );
                }
                else {
                    result = "Show statistics image must be true/false: "+params;
                }
                Util::commandPostProcess( result );
                return result;
            });

    addCommandCallback( "setShowStatsRegion", [=] (const QString & /*cmd*/,
                                const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = {Util::VISIBLE};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString showStatsStr = dataValues[*keys.begin()];
            bool validBool = false;
            bool showStats = Util::toBool( showStatsStr, &validBool );
            QString result;
            if ( validBool ){
                setShowStatsRegion( showStats );
            }
            else {
                result = "Show statistics region must be true/false: "+params;
            }
            Util::commandPostProcess( result );
            return result;
        });
}

void Statistics::_initializeDefaultState(){
    //Data state is the selected image and image/region statistics
    m_stateData.insertValue<int>(SELECTED_INDEX, 0 );
    m_stateData.insertArray( STATS, 0 );
    m_stateData.flushState();

    //Preference state
    m_state.insertValue<bool>( SHOW_STATS_IMAGE, true );
    m_state.insertValue<bool>( SHOW_STATS_REGION, true );
    m_state.flushState();
}


bool Statistics::isLinked( const QString& linkId ) const {
    bool linked = false;
    CartaObject* obj = m_linkImpl->searchLinks( linkId );
    if ( obj != nullptr ){
        linked = true;
    }
    return linked;
}


QString Statistics::removeLink( CartaObject* cartaObject){
    bool removed = false;
    QString result;
    Controller* controller = dynamic_cast<Controller*>( cartaObject );
    if ( controller != nullptr ){
        removed = m_linkImpl->removeLink( controller );
        if ( removed ){
            controller->disconnect(this);
            m_controllerLinked = false;
            m_stateData.resizeArray( STATS, 0 );
        }
    }
    else {
       result = "Statistics was unable to remove link only image links are supported";
    }
    return result;
}

void Statistics::setShowStatsImage( bool showStats ){
    bool oldStats = m_state.getValue<bool>( SHOW_STATS_IMAGE );
    if ( oldStats != showStats ){
        m_state.setValue<bool>( SHOW_STATS_IMAGE, showStats );
        m_state.flushState();
    }
}

void Statistics::setShowStatsRegion( bool showStats ){
    bool oldStats = m_state.getValue<bool>( SHOW_STATS_REGION );
    if ( oldStats != showStats ){
        m_state.setValue<bool>( SHOW_STATS_REGION, showStats );
        m_state.flushState();
    }
}


void Statistics::_updateStatistics( Controller* controller ){
    if ( controller != nullptr ){
        /**
         * TODO!!!! Eventually need to have a different callback for updating
         * the data state, which most likely will happen more frequently than
         * statistics need to be updated.
         */
        int selectedIndex = controller->getSelectImageIndex();
        m_stateData.setValue<int>(SELECTED_INDEX, selectedIndex );
        m_stateData.flushState();


        std::vector< std::shared_ptr<Carta::Lib::Image::ImageInterface> > dataSources = controller->getDataSources();
        std::vector<Carta::Lib::RegionInfo> regions = controller->getRegions();
        int sourceCount = dataSources.size();
        if ( sourceCount > 0 ){
            auto result = Globals::instance()-> pluginManager()
                         -> prepare <Carta::Lib::Hooks::ImageStatisticsHook>(dataSources, regions);
            auto lam = [=] ( const Carta::Lib::Hooks::ImageStatisticsHook::ResultType &data ) {

                //An array for each image
                int dataCount = data.size();
                m_stateData.resizeArray( STATS, dataCount );
                for ( int i = 0; i < dataCount; i++ ){

                    //Each element of the image array contains an array of statistics.
                    QString arrayLookup = UtilState::getLookup( STATS, i );
                    int statCount = data[i].size();
                    m_stateData.setArray( arrayLookup, statCount );

                    //Go through each set of statistics for the image.
                    for ( int k = 0; k < statCount; k++ ){

                        QList<QString> keys = data[ i ][k].keys();
                        QString objLookup = UtilState::getLookup( arrayLookup, k );
                        QList<QString> existingKeys = m_stateData.getMemberNames( objLookup );
                        int keyCount = keys.size();
                        for ( int j = 0; j < keyCount; j++ ){
                            QString lookup = UtilState::getLookup( objLookup, keys[j] );
                            m_stateData.insertValue<QString>( lookup, data[i][k][keys[j]] );
                        }
                    }
                }
                m_stateData.flushState();
            };
            try {
                result.forEach( lam );
            }
            catch( char*& error ){
                QString errorStr( error );
                ErrorManager* hr = Util::findSingletonObject<ErrorManager>();
                hr->registerError( errorStr );
            }
        }
    }
}


Statistics::~Statistics(){

}
}
}
