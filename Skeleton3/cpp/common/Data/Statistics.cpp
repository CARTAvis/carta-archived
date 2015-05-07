#include "Data/Statistics.h"
#include "Data/Controller.h"
//#include "Data/DataSource.h"
#include "Data/LinkableImpl.h"
//#include "Data/Util.h"

#include <QtCore/QDebug>


using namespace std;

namespace Carta {

namespace Data {

class Statistics::Factory : public Carta::State::CartaObjectFactory {

public:

    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new Statistics (path, id);
    }
};

const QString Statistics::CLASS_NAME = "Statistics";
bool Statistics::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass (CLASS_NAME,
                                                   new Statistics::Factory());

Statistics::Statistics( const QString& path, const QString& id ) :
        CartaObject( CLASS_NAME, path, id),
        m_linkImpl( new LinkableImpl( path) ){
    _initializeCallbacks();
     _initializeState();
}

QString Statistics::addLink( Carta::State::CartaObject* cartaObject ){
    Controller* target = dynamic_cast<Controller*>( cartaObject);
    QString result;
    if ( target != nullptr){
        m_linkImpl->addLink( target );
    }
    else {
        result = "Statistics only supports linking to images";
    }
    return result;
}

QString Statistics::getStateString( const QString& /*sessionId*/, SnapshotType type ) const{
    QString result("");
    if ( type == SNAPSHOT_PREFERENCES ){
        result = m_state.toString();
    }
    else if ( type == SNAPSHOT_LAYOUT ){
        result = m_linkImpl->getStateString(getIndex(), getType( type));
    }
    return result;
}

QString Statistics::removeLink( Carta::State::CartaObject* cartaObject ){
    QString result;
    bool objRemoved = m_linkImpl->removeLink( cartaObject );
    if ( !objRemoved ){
        result = "Link did not exist.";
    }
    return result;
}


void Statistics::clear(){
    m_linkImpl->clear();
}



void Statistics::_initializeCallbacks(){
}

void Statistics::_initializeState(){
}


Statistics::~Statistics(){
    clear();
}

}
}
