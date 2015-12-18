
#include "Data/LinkableImpl.h"
#include "Data/Image/Controller.h"
#include "Data/Image/DataSource.h"
#include "Data/Error/ErrorManager.h"
#include "Data/Util.h"

#include "CartaLib/Hooks/ImageStatisticsHook.h"
#include "CartaLib/RegionInfo.h"

#include "State/UtilState.h"

#include <QDebug>
#include "Statistics.h"

#include "Globals.h"

namespace Carta {

namespace Data {

const QString Statistics::CLASS_NAME = "Statistics";
const QString Statistics::STATS = "stats";

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
            m_linkImpl( new LinkableImpl( path )){
    _initializeDefaultState();
}


QString Statistics::addLink( CartaObject*  target){
    Controller* controller = dynamic_cast<Controller*>(target);
    bool linkAdded = false;
    QString result;
    if ( controller != nullptr ){
        if ( !m_controllerLinked ){
            linkAdded = m_linkImpl->addLink( controller );
            if ( linkAdded ){
                connect(controller, SIGNAL(dataChanged(Controller*)), this , SLOT(_updateStatistics(Controller*)));
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


void Statistics::_initializeDefaultState(){
    m_state.insertArray( STATS, 0 );
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
            m_state.resizeArray( STATS, 0 );
        }
    }
    else {
       result = "Statistics was unable to remove link only image links are supported";
    }
    return result;
}


void Statistics::_updateStatistics( Controller* controller ){
    if ( controller != nullptr ){
        std::shared_ptr<Carta::Lib::Image::ImageInterface> dataSource = controller->getDataSource();
        std::vector<Carta::Lib::RegionInfo> regions = controller->getRegions();
        if ( dataSource ) {
            auto result = Globals::instance()-> pluginManager()
                 -> prepare <Carta::Lib::Hooks::ImageStatisticsHook>(dataSource, regions);
            auto lam = [=] ( const Carta::Lib::Hooks::ImageStatisticsHook::ResultType &data ) {
                int dataCount = data.size();
                m_state.resizeArray( STATS, dataCount );
                for ( int i = 0; i < dataCount; i++ ){
                    QList<QString> keys = data[ i ].keys();
                    QString objLookup = UtilState::getLookup( STATS, i );
                    QList<QString> existingKeys = m_state.getMemberNames( objLookup );
                    int keyCount = keys.size();
                    for ( int j = 0; j < keyCount; j++ ){
                        QString lookup = UtilState::getLookup( objLookup, keys[j] );
                        m_state.insertValue<QString>( lookup, data[i][keys[j]] );
                    }
                }
                m_state.flushState();
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
