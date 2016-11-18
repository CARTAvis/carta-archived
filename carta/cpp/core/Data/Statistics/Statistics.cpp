
#include "Statistics.h"
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

const QString Statistics::CLASS_NAME = "Statistics";
const QString Statistics::FROM = "from";
const QString Statistics::LABEL = "label";
const QString Statistics::SHOW_STATS_IMAGE = "showStatsImage";
const QString Statistics::SHOW_STATS_REGION = "showStatsRegion";
const QString Statistics::SELECTED_INDEX = "selectedIndex";
const QString Statistics::STATS = "stats";
const QString Statistics::STATS_IMAGE = "image";
const QString Statistics::STATS_REGION = "region";
const QString Statistics::TO = "to";

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
                        this , SLOT(_updateStatistics(Controller*)));
                connect( controller, SIGNAL( frameChanged(Controller*, Carta::Lib::AxisInfo::KnownType)),
                        this, SLOT(_updateStatistics(Controller*, Carta::Lib::AxisInfo::KnownType)));
                connect(controller, SIGNAL(dataChangedRegion(Controller*)),
                        this, SLOT( _updateStatistics( Controller*)));
                m_controllerLinked = true;
                _updateStatistics( controller, Carta::Lib::AxisInfo::KnownType::OTHER );
            }
        }
    }
    else {
        result = "Statistics only supports linking to images";
    }
    return result;
}

void Statistics::_clearLinks(){
    int linkCount = m_linkImpl->getLinkCount();
    for ( int i = linkCount-1; i >= 0; i-- ){
        CartaObject* linkObj = m_linkImpl->getLink(i);
        removeLink( linkObj );
    }
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

QString Statistics::getStateString( const QString& sessionId, SnapshotType type ) const{
    QString result("");
    if ( type == SNAPSHOT_PREFERENCES ){
        StateInterface prefState( "");
        prefState.setValue<QString>(Carta::State::StateInterface::OBJECT_TYPE, CLASS_NAME );
        prefState.insertValue<bool>( SHOW_STATS_IMAGE, m_state.getValue<bool>(SHOW_STATS_IMAGE) );
        prefState.insertValue<bool>( SHOW_STATS_REGION, m_state.getValue<bool>(SHOW_STATS_REGION) );

        prefState.insertValue<QString>(Util::PREFERENCES, m_state.toString());
        prefState.insertValue<QString>( Settings::SETTINGS, m_settings->getStateString(sessionId, type) );
        result = prefState.toString();
    }
    else if ( type == SNAPSHOT_LAYOUT ){
        result = m_linkImpl->getStateString(getIndex(), getSnapType( type ));
    }
    return result;
}


QString Statistics::_getStatType( const QString& userType ) const {
    QString recognizedType;
    int result = QString::compare( userType, STATS_IMAGE, Qt::CaseInsensitive );
    if ( result == 0 ){
        recognizedType = STATS_IMAGE;
    }
    else {
        result = QString::compare( userType, STATS_REGION, Qt::CaseInsensitive );
        recognizedType = STATS_REGION;
    }
    return recognizedType;
}


void Statistics::_initializeCallbacks(){

    addCommandCallback( "moveStat", [=] (const QString & /*cmd*/,
                                        const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = {Util::TYPE, TO, FROM };
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString typeStr = dataValues[Util::TYPE];
            QString toStr = dataValues[TO];
            QString fromStr = dataValues[FROM];

            bool validTo = false;
            int toIndex = toStr.toInt(&validTo );
            bool validFrom = false;
            int fromIndex = fromStr.toInt( &validFrom );
            QString result;
            if ( validTo && validFrom ){
                result = moveStat( fromIndex, toIndex, typeStr );
            }
            else {
                result = "Original & destination index must be integers for a statistics move: "+params;
            }
            Util::commandPostProcess( result );
            return result;
        });

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

    addCommandCallback( "setStatVisible", [=] (const QString & /*cmd*/,
                                    const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = {Util::VISIBLE, Util::NAME, Util::TYPE };
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString showStatStr = dataValues[Util::VISIBLE];
            bool validBool = false;
            bool showStat = Util::toBool( showStatStr, &validBool );
            QString result;
            if ( validBool ){
                QString statName = dataValues[Util::NAME];
                QString statType = dataValues[Util::TYPE];
                result = setStatVisible( showStat, statName, statType );
            }
            else {
                result = "Stat visible must be true/false: "+params;
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

    //Image labeels
    QList<Carta::Lib::StatInfo::StatType> imageStats = Carta::Lib::StatInfo::getStatTypesImage();
    int imageStatCount = imageStats.size();
    m_state.insertArray( STATS_IMAGE, imageStatCount);
    for ( int i = 0; i < imageStatCount; i++ ){

        QString label = Carta::Lib::StatInfo::toString( imageStats[i] );
        _initializeLabel( STATS_IMAGE, i, label, true );
    }

    //Region labels
    QList<Carta::Lib::StatInfo::StatType> regionStats = Carta::Lib::StatInfo::getStatTypesRegion();
    int regionStatCount = regionStats.size();
    m_state.insertArray( STATS_REGION, regionStatCount );
    for ( int i = 0; i < regionStatCount; i++ ){
        bool visible = false;
        if ( regionStats[i] == Carta::Lib::StatInfo::StatType::Sum ||
                regionStats[i] == Carta::Lib::StatInfo::StatType::FluxDensity ||
                regionStats[i] == Carta::Lib::StatInfo::StatType::Mean ||
                regionStats[i] == Carta::Lib::StatInfo::StatType::RMS ||
                regionStats[i] == Carta::Lib::StatInfo::StatType::Sigma ||
                regionStats[i] == Carta::Lib::StatInfo::StatType::SumSq ||
                regionStats[i] == Carta::Lib::StatInfo::StatType::Min ||
                regionStats[i] == Carta::Lib::StatInfo::StatType::Max ){
            visible = true;
        }
        QString label = Carta::Lib::StatInfo::toString( regionStats[i] );
        _initializeLabel( STATS_REGION, i, label, visible );
    }
    m_state.flushState();
}

void Statistics::_initializeLabel( const QString& arrayName, int arrayIndex,
        const QString& label, bool visible ){
    QString shapeLookup = Carta::State::UtilState::getLookup( arrayName, arrayIndex );
    QString shapeLookupLabel = Carta::State::UtilState::getLookup( shapeLookup, LABEL );
    QString shapeLookupVisible = Carta::State::UtilState::getLookup( shapeLookup, Util::VISIBLE );
    m_state.insertValue<QString>( shapeLookupLabel, label);
    m_state.insertValue<bool>( shapeLookupVisible, visible );
}


bool Statistics::isLinked( const QString& linkId ) const {
    bool linked = false;
    CartaObject* obj = m_linkImpl->searchLinks( linkId );
    if ( obj != nullptr ){
        linked = true;
    }
    return linked;
}


QString Statistics::moveStat( int fromIndex, int toIndex, const QString& type ){
    QString result;
    QString statType = _getStatType( type );
    if ( statType.isEmpty() ){
        result = "Unrecognized type of statistics: "+statType;
    }
    else {
        int statCount = m_state.getArraySize( statType );
        if ( fromIndex >= statCount || toIndex >= statCount ){
            result = "Move indices must be less than: "+ QString::number( statCount );
        }
        else if ( fromIndex < 0  || toIndex < 0 ){
            result = "Move indices must be nonnegative";
        }
        else {
            QString sourceLookup = Carta::State::UtilState::getLookup( statType, fromIndex );
            QString sourceObject = m_state.toString( sourceLookup );
            if ( fromIndex < toIndex ){
                //Move i+1 forward to i position.
               for ( int i = fromIndex; i <toIndex; i++ ){
                   QString moveLookup = Carta::State::UtilState::getLookup( statType, i+1 );
                   QString moveObj = m_state.toString( moveLookup );
                   QString prevLookup = Carta::State::UtilState::getLookup( statType, i );
                   m_state.setObject( prevLookup, moveObj );
               }
           }
           else {
               //Move all the widgets back one to make room for the taget.
               for ( int i = fromIndex; i >= toIndex; i-- ){
                   QString moveLookup = Carta::State::UtilState::getLookup( statType, i-1 );
                   QString moveObj = m_state.toString( moveLookup );
                   QString backLookup = Carta::State::UtilState::getLookup( statType, i );
                   m_state.setObject( backLookup, moveObj );
               }
           }
           QString destLookup = Carta::State::UtilState::getLookup( statType, toIndex );
           m_state.setObject( destLookup, sourceObject );
           m_state.flushState();
        }
    }
    return result;
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

void Statistics::resetState( const QString& state ){
    StateInterface restoredState( "");
    restoredState.setState( state );

    QString settingStr = restoredState.getValue<QString>(Settings::SETTINGS);
    m_settings->resetStateString( settingStr );

    QString prefStr = restoredState.getValue<QString>(Util::PREFERENCES);
    m_state.setState( prefStr );
    bool showImageStats = restoredState.getValue<bool>(SHOW_STATS_IMAGE);
    m_state.setValue<bool>(SHOW_STATS_IMAGE, showImageStats );
    m_state.setValue<bool>(SHOW_STATS_REGION, restoredState.getValue<bool>(SHOW_STATS_REGION));
    m_state.setValue<bool>( "flush", true );
    m_state.flushState();
    m_state.setValue<bool>("flush", false );
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

QString Statistics::setStatVisible( bool showStat, const QString& statName,
        const QString& type ) {
    QString result;
    QString statType = _getStatType( type );
    if ( statType.isEmpty() ){
        result = "Unrecognized type of statistics: "+statType;
    }
    int statCount = m_state.getArraySize( statType );
    bool found = false;
    for ( int i = 0; i < statCount; i++ ){
        QString lookup = Carta::State::UtilState::getLookup( statType, i );
        QString nameLookup = Carta::State::UtilState::getLookup( lookup, LABEL );
        QString name = m_state.getValue<QString>( nameLookup );
        if ( name == statName ){
            found = true;
            QString visibleLookup = Carta::State::UtilState::getLookup( lookup, Util::VISIBLE );
            bool statVisible = m_state.getValue<bool>( visibleLookup );
            if ( statVisible != showStat ){
                m_state.setValue<bool>( visibleLookup, showStat );
                m_state.flushState();
            }
            break;
        }
    }
    if ( !found ){
        result = "Unrecognized statistic: "+statName;
    }
    return result;
}


void Statistics::_updateStatistics( Controller* controller, Carta::Lib::AxisInfo::KnownType /*type*/  ){
    if ( controller != nullptr ){

        int selectedIndex = controller->getSelectImageIndex();
        m_stateData.setValue<int>(SELECTED_INDEX, selectedIndex );

        std::vector< std::shared_ptr<Carta::Lib::Image::ImageInterface> > dataSources =
                controller->getImages();


        std::vector<std::shared_ptr<Region> > coreRegions;
        std::shared_ptr<RegionControls> regionControls = controller->getRegionControls();
        if ( regionControls ){
        	coreRegions = regionControls->getRegions();
        }
        int regionCount = coreRegions.size();
        std::vector<std::shared_ptr<Carta::Lib::Regions::RegionBase> > regions;
        for ( int i = 0; i < regionCount; i++ ){
            regions.push_back( coreRegions[i]->getModel() );
        }

        std::vector<int> frameIndices = controller->getImageSlice();

        int sourceCount = dataSources.size();
        if ( sourceCount > 0 ){
            auto result = Globals::instance()-> pluginManager()
                         -> prepare <Carta::Lib::Hooks::ImageStatisticsHook>(dataSources, regions, frameIndices);
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
                        QString objLookup = UtilState::getLookup( arrayLookup, k );
                        QList<QString> existingKeys = m_stateData.getMemberNames( objLookup );
                        int keyCount = data[i][k].size();
                        for ( int j = 0; j < keyCount; j++ ){
                            QString label = data[i][k][j].getLabel();
                            QString lookup = UtilState::getLookup( objLookup, label );
                            m_stateData.insertValue<QString>( lookup, data[i][k][j].getValue() );
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
        //No statistics
        else {
            m_stateData.resizeArray( STATS, 0 );
            m_stateData.flushState();
        }

    }
}


Statistics::~Statistics(){

}
}
}
