#include "Data/Statistics.h"
#include "Data/Controller.h"
//#include "Data/DataSource.h"
#include "Data/LinkableImpl.h"
//#include "Data/Util.h"

#include <QtCore/QDebug>


using namespace std;

namespace Carta {

namespace Data {

class Statistics::Factory : public CartaObjectFactory {

public:

    CartaObject * create (const QString & path, const QString & id)
    {
        return new Statistics (path, id);
    }
};



const QString Statistics::CLASS_NAME = "Statistics";
bool Statistics::m_registered =
    ObjectManager::objectManager()->registerClass (CLASS_NAME,
                                                   new Statistics::Factory());

Statistics::Statistics( const QString& path, const QString& id ) :
        CartaObject( CLASS_NAME, path, id),
        m_linkImpl( new LinkableImpl( &m_state) ){
    _initializeCallbacks();
     _initializeState();
}

bool Statistics::addLink( CartaObject* cartaObject ){
    Controller* target = dynamic_cast<Controller*>( cartaObject);
    bool objAdded = false;
    if ( target != nullptr){
        objAdded = m_linkImpl->addLink( target );
    }
    else {
        qWarning() << "Statistics: unrecognized link type";
    }
    return objAdded;
}

bool Statistics::removeLink( CartaObject* cartaObject ){
    bool objRemoved = m_linkImpl->removeLink( cartaObject );
    return objRemoved;
}

QList<QString> Statistics::getLinks() const {
    return m_linkImpl->getLinkIds();
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
